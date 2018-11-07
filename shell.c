#include "21sh.h"
#include <term.h>
#include <errno.h>


struct termios saved;
extern char **environ;
int	g_hstr_fd = -1;

void    ft_restore()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

void	free_job(t_job *j)
{
	int i = 0;
	t_process *pprev;
	if (j)
	{
		while (j->first_process)
		{
			pprev = j->first_process;
			j->first_process = j->first_process->next;
			i = 0;
			while (pprev->argv[i])
			{
				free(pprev->argv[i]);
				i++;
			}
			free(pprev);
		}
		free(j);
		j = NULL;
	}
}

void    ft_exit(void)
{
    ft_restore();
    exit(EXIT_SUCCESS);
}

int ft_iputchar(int c)
{
    return(write(1, &c, 1));
}

int	type_prompt()
{
	char	*user;
	char	*pwd;
	char	*home;
	int		len;
	char	*tmp;
	int totallen = 0;

	tmp = NULL;
	user = get_copy_env("LOGNAME", MUTE);
	pwd = get_current_wd();
	home = get_copy_env("HOME",  MUTE);
	if (home)
	{
		len = ft_strlen(home);
		if (ft_strnequ(home, pwd, len))
		{
			home = "~";
			tmp = ft_strsub(pwd, len, ft_strlen(pwd) - len);
		}
	}
	else
	{
		home = "";
		tmp = pwd;
	}
	totallen = ft_strlen(user) + ft_strlen(home) + ft_strlen(tmp) + 5;
	ft_printf("%s%s: %s%s%s>%s ", RED, user, BLUE, home, tmp, RESET);
	if (tmp != pwd)
		ft_strdel(&tmp);
	return totallen;
}

int     cbreak_settings()
{
	struct termios changed;

    changed = saved;
    changed.c_lflag &= ~(ICANON | ECHO| ISIG);
    changed.c_cc[VMIN] = 1;
    changed.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed) == -1)
    {
        ft_putstr_fd("Cannot set terminal attributes\n", STDERR_FILENO);
        ft_exit();
    }
    return 0;
}

void 	init_terminal()
{
	char buf[MAXWORD];

	if ((tcgetattr(STDOUT_FILENO, &saved)) == -1)
    {
        ft_putstr_fd("Cannot get terminal settings\n", STDERR_FILENO);
        exit(EXIT_FAILURE);
    }
    char *name = NULL;
    if (!(name = getenv("TERM")))
    {
        ft_printf("TERM is not set\n");
        exit(EXIT_FAILURE);
    }
    if (tgetent(buf, name) < 1)
    {
        ft_putstr_fd("Error at tgetent\n", STDERR_FILENO);
        exit(EXIT_FAILURE);
    }
    //copy_env();
}

void	sig_tstp_handler(int signum)
{
	if (signum == SIGTSTP)
	{
		
		pid_t cur;

		cur = tcgetpgrp(shell_terminal);
		if (cur != shell_pgid)
		{

			kill(cur, SIGTSTP);
			tcsetpgrp(shell_terminal, shell_pgid);
			tcsetattr(STDOUT_FILENO, TCSAFLUSH, &saved);
		}
	}
	signal(SIGTSTP, sig_tstp_handler);
}

void	set_stopsignals(sig_t func)
{
	//ft_printf("---> %s\n",__FUNCTION__);
	signal(SIGINT, func);
	signal(SIGQUIT, func);
	signal(SIGTSTP, sig_tstp_handler);
	signal(SIGTERM, func);
	signal(SIGTTIN, func);
	signal(SIGTTOU, func);
	signal(SIGCHLD, SIG_DFL);
	//signal(SIGCHLD, func);
}

t_job	*find_job(pid_t pgid)
{
	t_job	*j;
	for (j = first_job; j; j = j->next)
	{
		if (j->pgid == pgid)
			return j;
	}
	return NULL;
}

int	job_is_stopped(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & (STOPPED)))
			return 0;
		p = p->next;
	}
	return 1;
}

int	job_is_completed(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & COMPLETED))
			return 0;
		p = p->next;
	}
	return 1;
}

void	launch_process(t_process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground)
{
	pid_t	pid;

	if (shell_is_interactive)
	{
		pid = getpid();
		if (pgid == 0)
			pgid = pid;
		setpgid(pid, pgid);
		if (foreground)
			tcsetpgrp(shell_terminal, pgid);
		set_stopsignals(SIG_DFL);
		signal(SIGTSTP,sig_tstp_handler);
	}
	if (infile != STDIN_FILENO)
	{
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if (outfile != STDOUT_FILENO)
	{
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
	if (errfile != STDERR_FILENO)
	{
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}
	if (execve(p->argv[0], p->argv, g_envp) < 0)
		perror("execve");

	exit(1);
}

void	put_job_in_foreground(t_job *j, int cont)
{
	tcsetpgrp(shell_terminal, j->pgid);
	if (cont)
	{
		tcsetattr(shell_terminal, TCSAFLUSH, &j->tmodes);
		if (kill( -j->pgid, SIGCONT) < 0)
			perror("kill(SIGCONT)");
	}

	wait_for_job(j);

	tcsetpgrp(shell_terminal, shell_pgid);
	tcgetattr(shell_terminal, &j->tmodes);
	tcsetattr(shell_terminal, TCSAFLUSH, &saved);
}

void	put_job_in_background(t_job *j, int cont)
{
	if (cont)
	{
		if (kill( -j->pgid, SIGCONT) < 0)
			perror("kill(SIGCONT)");
	}
}

int	mark_process_status(pid_t pid, int status)
{
	t_job		*j;
	t_process	*p;

	if (pid > 0)
	{
		j = first_job;
		while (j)
		{
			p = j->first_process;
			while (p)
			{
				if (p->pid == pid)
				{
					p->status = status;
					if (WIFSTOPPED(status))
						p->state |= STOPPED;
					else
					{
						p->state |= COMPLETED;
						if (WIFSIGNALED(status))
							{
								ft_putnbr_fd((int)pid, STDERR_FILENO);
								ft_putstr_fd(": Terminated by signal ", STDERR_FILENO);
								ft_putnbr_fd(WTERMSIG(p->status), STDERR_FILENO);
								ft_putchar_fd('\n', STDERR_FILENO);
							}
					}
					return 0;
				}
				p = p->next;
			}
			j = j->next;
		}
		fprintf(stderr, "No child process %d.\n", pid);
				return -1;
	}
	else if (pid == 0 || errno == ECHILD)
		return -1;
	else
	{
		perror("waitpid");
		return -1;
	}
}

void	update_status(void)
{
	int status;
	pid_t	pid;
	int res;
	
	status = 0;
	res = 0;
	while (res == 0)
	{
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
		res = mark_process_status(pid, status);
	}
}

void	wait_for_job(t_job *j)
{
	int	status;
	pid_t	pid;

	status = 0;
	int res = 0;
	while (!job_is_stopped(j) && res == 0 && !job_is_completed(j))
	{
		pid = waitpid(-j->pgid, &status, WUNTRACED);
		res = mark_process_status(pid, status);
	}
}

void	format_job_info(t_job *j, const char *status)
{
	ft_putnbr_fd(j->pgid, STDERR_FILENO);
	ft_putstr_fd(" : ", STDERR_FILENO);
	ft_putstr_fd(status, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
}

void	do_job_notification(void)
{
	t_job		*j;
	t_job		*jlast;
	t_job		*jnext;

	update_status();
	jlast = NULL;

	j = first_job;
	while(j)
	{
		
		jnext = j->next;
		if (job_is_completed(j))
		{
			if (jlast)
				jlast->next = jnext;
			else
				first_job = jnext;
			free_job(j);
			j = NULL;
		}
		else if (job_is_stopped(j) && !j->notified)
		{
			j->notified = 1;
			jlast = j;
		}
		else
			jlast = j;
		j = jnext;
	}
}
//-----------------------------------------------
void	mark_job_as_running(t_job *j)
{
	t_process	*p;

	for (p = j->first_process; p; p = p->next)
		p->state &= ~STOPPED;
	j->notified = 0;
}

void	continue_job(t_job *j, int foreground)
{
	mark_job_as_running(j);
	if (foreground)
		put_job_in_foreground(j, 1);
	else
		put_job_in_background(j, 1);
}
//-----------------------------------------------
void	launch_job(t_job *j, int foreground)
{
	t_process	*p;
	pid_t		pid = 0;
	int		mypipe[2] = {-1, -1};
	int		infile = -1;
	int		outfile = -1;
	int ret;

	ret = -1;
	if (j->in_fd == -1 && (j->in_fd = open(j->srcfile, O_RDONLY)) == -1)
	{
		perror("open");
		return ;
	}
	if (j->err_fd == -1 && (j->err_fd = open(j->errfile, j->flags, FILE_PERM)) == -1)
	{
		perror("open");
		return ;
	}
	infile = j->in_fd;
	p = j->first_process;
	//------- Job number ---
	j->nbr = 1;
	t_job *ptr = first_job;
	while (ptr)
	{
		if (j != ptr && j->nbr >= ptr->nbr)
			j->nbr++;
		ptr = ptr->next;
	}
	//----------------------

	while(p)
	{
		if (j->out_fd == -1 && (j->out_fd = open(j->dstfile, j->flags, FILE_PERM )) == -1)
		{
			perror("open");
			return ;
		}
//------------------------------------ set outfile
		if (p->next)
		{
			if (pipe(mypipe) < 0)
			{
				perror("pipe");
				exit(1);
			}
			outfile = mypipe[1];
		}
		else
			outfile = j->out_fd;
//------------------------------------
		if ((ret = check_built(p->argv[0])) >= 0)
		{
			p->state |= COMPLETED;
			ft_built_exe(p->argv, ret, infile, outfile);
		if (infile != j->in_fd)
			close(infile);
		
		if (outfile != j->out_fd)
			close(outfile);
		
		infile = mypipe[0];
			p = p->next;
			continue;
		}
			else
		{	
			if (!ft_find(p))	//	<--- here need to close all fd
			{
				p->state |= COMPLETED;
				return;
			}

			pid = fork();
			if (pid == CHILD)
				launch_process(p, j->pgid, infile, outfile, j->err_fd, foreground);
			else if (pid == ERROR)
			{
				perror("fork");
				ft_exit();
			}
			else
			{
				p->pid = pid;
				if (shell_is_interactive)
				{
					if (!j->pgid)			//first process sets pgid for new group
						j->pgid = pid;
					setpgid(pid, j->pgid);
				}	
			}
		}		//END not built
		if (infile != j->in_fd)
			close(infile);
		
		if (outfile != j->out_fd)
			close(outfile);
		
		infile = mypipe[0];
		p = p->next;
	}	// END loop


	if (j->in_fd != STDIN_FILENO)
			close(j->in_fd);
	if (j->out_fd != STDOUT_FILENO)
			close(j->out_fd);
	if (j->err_fd > 2)
		close(j->err_fd);
	//format_job_info(j, "launched");

	if (!shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j, 0);
	else
		put_job_in_background(j, 0);
}
/*
void 	print_jobs()
{
		int i;
	t_job *j = first_job;
	t_process *p;
	printf("\n+++ PRINT +++\n");
	while (j)
	{
		ft_printf("%s, %s\n", j->foreground ? "fg" : "bg",j->notified ? "notified" : "not notified");
		p = j->first_process;
		while (p)
		{
			i = 0;

			while (p->argv[i])
			{
				printf("[%d] %s\n",i, p->argv[i]);
//				ft_printf("src: [%s][%d], dst: [%s][%d]\n", j->srcfile, j->in_fd, j->);
				i++;
			}
			printf("state: %s\n",  p->state & COMPLETED ? "completed" : "not completed");
			printf("-----------\n");
			p = p->next;
		}
		printf("[[%d]] SRC[%s][%d],DST[%s][%d], pgid[%d]\n",j->nbr, j->srcfile,
		 j->in_fd, j->dstfile, j->out_fd, j->pgid);
		printf("===========\n");
		j = j->next;
	}
	printf("+++ DONE +++\n");
}
*/
void	init_shell(void)
{
	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);

	if (shell_is_interactive)
	{
		while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgid(0)))
		{
			fprintf(stderr, "shell is not interactive\n");
			kill( - shell_pgid, SIGTTIN);
		}

		set_stopsignals(SIG_IGN);
		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0)
		{
			perror("Couldn't put the shell in it's own group");
			exit(1);
		}

		tcsetpgrp(shell_terminal, shell_pgid);
		init_terminal();
	}
	copy_env();
}
/*
static void fd_check(void)
{
     int fd;
     bool ok = true;
 
     for (fd = 3; fd < 20; fd++)
         if (fcntl(fd, F_GETFL) != -1 || errno != EBADF) {
             ok = false;
             fprintf(stderr, "*** fd %d is open ***\n", fd);
         }
     if (!ok)
         _exit(EXIT_FAILURE);
}
*/
void	init_history(void)
{
	int i = 0;
	char *str;

	str = NULL;
	while(get_next_line(g_hstr_fd, &str) > 0)
	{
		g_history[i] = ft_strdup(str);
		free(str);
		i++;
	}
	if (str)
		free(str);
	g_hstr_nb = i;
}

void	check_history_capacity(void)
{
	if (g_hstr_nb == MAXHSTR - 1)
	{
		ft_printf("MAXHISTORY!!!\n");
		ft_exit();
	}
}

int	main(int argc, char **argv)
{
	char line[MAXLINE];
	t_job *j;
	t_job *ptr;
	
	int infile;
	g_envp = NULL;
	if (argc == 1)
		init_shell();
	else if (argc == 2)
	{
		shell_is_interactive = 0;
		infile = open(argv[1], O_RDONLY);
		copy_env();
	}
	else
	{
		ft_putstr_fd("Unexpected argument\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
	first_job = NULL;
	if (shell_is_interactive)
	{
		g_hstr_fd = open(".history", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
		init_history();
	}
	while (1)
	{
		j = (t_job*)ft_memalloc(sizeof(t_job));
		if (!j)
		{
			perror("ft_memalloc");
			return 1;
		}
		if (shell_is_interactive)
		{
			if (g_hstr_fd == -1)
				g_hstr_fd= open(".history", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
			cbreak_settings();
			read_line(&line[0], 0);
			ft_restore();
		}
		else
		{
			ft_bzero(line, MAXLINE);
			char *str = NULL;
			if (get_next_line(infile, &str) > 0)
			{
				ft_strcpy(line, str);
				line[ft_strlen(str)] = '\n';
				free(str);
			}
			else
			{
				close(infile);
				exit(0);
			}
		}
		if (line[0] != '\n' && pack_args(line, j))
		{
			if (shell_is_interactive)
			{
				set_stopsignals(SIG_IGN);
				check_history_capacity();
				ft_putstr_fd(line, g_hstr_fd);
				line[ft_strlen(line) - 1] = 0;
				g_history[g_hstr_nb] = ft_strdup(line);
				g_hstr_nb++;
			}
			ptr = first_job;
			while (ptr)
			{
				if (!ptr->foreground && ptr->nbr)
					do_job_notification();
				else if (!ptr->nbr)
					launch_job(ptr, ptr->foreground);
				ptr = ptr->next;

			}
			do_job_notification();
			if (shell_is_interactive)
			{
				close(g_hstr_fd);
				g_hstr_fd = -1;
			}
		}
	}
	return 0;
}
