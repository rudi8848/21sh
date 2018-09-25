#include "21sh.h"
#include <term.h>
#include <errno.h>

#define MAXHSTR 4096
struct termios saved;
extern char **environ;

//===========================================

	
	char	*g_history[MAXHSTR];
	int	g_hstr_nb;
	int	g_hstr_fd = -1;
//===========================================

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
				//ft_printf("delete [%s]\n", pprev->argv[i]);
				free(pprev->argv[i]);
				i++;
			}
			free(pprev);
		}
		//free(j);
		//j = NULL;
	}
}

void    ft_exit(void)
{
    ft_restore();
    //system("leaks a.out");
    exit(EXIT_SUCCESS);
}

int ft_iputchar(int c)
{
    return(write(1, &c, 1));
}
/*
void    ft_prompt(void)
{
    write(STDOUT_FILENO, "# ", 2);
}*/

void	type_prompt()
{
	char	*user;
	char	*pwd;
	char	*home;
	int		len;
	char	*tmp;

	user = get_copy_env("LOGNAME", MUTE);
	pwd = get_current_wd();
	home = get_copy_env("HOME",  MUTE);
	if (home)
		len = ft_strlen(home);
	if (ft_strnequ(home, pwd, len))
	{
		home = "~";
		tmp = ft_strsub(pwd, len, ft_strlen(pwd) - len);
	}
	else
	{
		home = "";
		tmp = pwd;
	}
	ft_printf("%s%s: %s%s%s>%s ", RED, user, BLUE, home, tmp, RESET);
	if (tmp != pwd)
		ft_strdel(&tmp);
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

void    read_line(char *line, int start)
{
    
    int         rr;
    uint64_t    rb;
    int j;
	char *str;
    //static char cp_buf[MAXLINE];
    
	int n;

    rb = 0;
    int len = 0;
    int i = 0;
   int cmd;

   //if (g_hstr_nb)
	   cmd = g_hstr_nb;

   
    if (!start)
	    type_prompt();
	ft_bzero(line, MAXLINE - start);
    while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
    {
        //ft_printf("\n-> %lld\n", rb);
        if (rb == K_RIGHT)
        {
            if (i < len)
            {
                i++;
                TERM_CRS_RIGHT
            }
        }
        else if (rb == K_CTRL_RIGHT)
        {
        	 if (i < len)
        	 {
        	 	j = i;
        	 	if (line[j] && line[j] != ' ' && line[j] != '\t')		//start in word
        	 	{
        	 		while(line[j] && line[j] != ' ' && line[j] != '\t')	//go to next space
        	 			j++;
        	 	}
        	 	if (line[j] && (line[j] == ' ' || line[j] == '\t'))		//we're in space
        	 	{
        	 		while (line[j] && (line[j] == ' ' || line[j] == '\t'))	//go to next not space
        	 			j++;
        	 	}
        	 	if (j - i >= 0)
        		tputs(tgoto(tgetstr("RI", NULL), 0, j - i), 0, ft_iputchar);
        	 	i = j;
        	 }
        }
        else if (rb == K_LEFT )
        {
            if (i > 0)
            {   i--;
                TERM_CRS_LEFT
            }
        }
        else if (rb == K_CTRL_LEFT)
        {
        	j = i;
			if (j && !line[j])	// if cursor stands at the end of line
        		j--;
        	if (j && line[j] != ' ' && line[j] != '\t' && line[j - 1] != ' ' && line[j - 1] != '\t' )	//if we are in word
        	{
        		while(j && (line[j] != ' ' && line[j] != '\t'))
        			j--;
        		if (line[j] == ' ' || line[j] == '\t')
        				j++;
        	}
        	else if (j && line[j] != ' ' && line[j] != '\t' && (line[j-1] == ' ' || line[j-1] == '\t'))
        		j--;
        	if (j && (line[j] == ' ' || line[j] == '\t'))
        	{
        		while(j && (line[j] == ' ' || line[j] == '\t'))
        			j--;
        		if (j && line[j] != ' ' && line[j] != '\t')	//if we are in word
	        	{
	        		while(j)
	        		{
	        			if (line[j] == ' ' || line[j] == '\t')
	        			{
	        				j++;
	        				break;
	        			}
	        			j--;
	        		}
	        	}
        	}
        	if (i - j >= 0)
        	tputs(tgoto(tgetstr("LE", NULL), 0, i - j), 0, ft_iputchar);
        	i = j;
        }
        else if (rb == K_ENTER)
        {
            line[len] = '\n';
            ft_printf("\n");
            //ft_printf("\n[%s]\nlen[%i], i[%i] {%i}\n", line, len, i, n);
            return;
        }
        else if (ft_isprint(rb))
        {
            if (len + 1 + start  == MAXLINE)
            {
                TERM_BELL          // bell
                ft_printf("\nLine is too long\n");
                return;
            }
            write(STDOUT_FILENO, &rb, rr);
            tputs(tgetstr("im", NULL), 0, ft_iputchar);          //insert mode
            if (line[i])   //if it's at the middle of line
            {
                j = len + 1;
                while (j > i)
                {
                    line[j] = line[j - 1];
                    j--;
                }            
                tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
                tputs(tgetstr("ce", NULL), 0, ft_iputchar);      // delete end of line
                write(STDOUT_FILENO, line + i + 1, len);
                tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
            }
            
           		line[i] = (char)rb;
                len++;
            if (i < len)
                i++;
            tputs(tgetstr("ei", NULL), 0, ft_iputchar);          //end of insertion mode
        }
        else if (rb == K_DOWN)
        {
        	//	here will be history navigation
   			if (g_hstr_nb)
   			{
	        	ft_bzero(line, MAXLINE);
	        	tputs(tgoto(tgetstr("LE", NULL), 0, i - 0), 0, ft_iputchar);
	        	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	        	tputs(tgetstr("ce", NULL), 0, ft_iputchar);      // delete end of line
				
				ft_strcpy(line, g_history[cmd]);
				if (cmd < g_hstr_nb -1)
					cmd++;
	            len = ft_strlen(line);
	            i = len;
		        ft_printf("%s",line);
		        tputs(tgetstr("rc", NULL), 0, ft_iputchar); 
		        tputs(tgoto(tgetstr("RI", NULL), 0, i), 0, ft_iputchar);
        	}
            //TERM_BELL         // bell
        }
        else if (rb == K_UP)
        {
        	if (g_hstr_nb)
        	{
	        	ft_bzero(line, MAXLINE);
	           	tputs(tgoto(tgetstr("LE", NULL), 0, i - 0), 0, ft_iputchar);
	        	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	        	tputs(tgetstr("ce", NULL), 0, ft_iputchar);      // delete end of line
	            //----------------------------------
	        	if (cmd)
	        	{
					cmd--;
					ft_strcpy(line, g_history[cmd]);
	        	}
	        				
	            //----------------------------------
	            len = ft_strlen(line);
	            i = len;
	        	ft_printf("%s",line);
	            tputs(tgetstr("rc", NULL), 0, ft_iputchar); 
	        	tputs(tgoto(tgetstr("RI", NULL), 0, i), 0, ft_iputchar);
        	}
           //TERM_BELL          // bell
        }
        else if (rb == K_HOME)
        {
        	if (i)
        	{
        		TERM_HOME	
        		i = 0;
        	}
        }
        else if (rb == K_END)
        {
        	if (i < len)
        	{
        		TERM_END	
        		i = len;
        	}
        }
        else if (rb == K_CTRL_C)
        {
        	ft_printf("\n");
        	ft_bzero(line, MAXLINE);
        	return;
        }
        else if (rb == K_DELETE || rb == K_CTRL_D)	// CTRL + D the same
        {
            if (len > 0 && i >= 0 && i < len)
            {
                TERM_DEL     // turn off deleting mode
                len--;
                j = i;
                while (line[j])
                {
                    line[j] = line[j + 1];
                    j++;
                }
            }
        }
        else if (rb == K_BSPACE)
        {
            if (i > 0)
            {
                i--;
                len--;
                j = i;
                while (line[j])
                {
                    line[j] = line[j + 1];
                    j++;
                }
                TERM_BACK
            }
        }
        
        else if (rb == K_ESC)
            ft_exit();
        rb = 0;
    }
    line[i] = '\n';
    return;
}

void 	init_terminal()
{
	//ft_printf("---> %s\n",__FUNCTION__);
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
    copy_env();
}

void	set_stopsignals(sig_t func)
{
//	ft_printf("---> %s\n",__FUNCTION__);
	signal(SIGINT, func);
	signal(SIGQUIT, func);
	signal(SIGTSTP, func);
	signal(SIGTERM, func);
	signal(SIGTTIN, func);
	signal(SIGTTOU, func);
	signal(SIGCHLD, func);
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
//	ft_printf("--> %s\n", __FUNCTION__);
	t_process	*p;

	for (p = j->first_process; p; p = p->next)
		if (!(p->state & (STOPPED | COMPLETED)))
			return 0;
	return 1;
}

int	job_is_completed(t_job *j)
{
//	ft_printf("--> %s\n", __FUNCTION__);
	t_process	*p;
	for (p = j->first_process; p; p = p->next)
		if (!(p->state & COMPLETED))
			return 0;
	return 1;
	
}

void	launch_process(t_process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground)
{
//	ft_printf("--> %s\n", __FUNCTION__);
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
//	ft_printf("--> %s\n", __FUNCTION__);
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
//	printf("---> %s, pid = %d\n", __FUNCTION__, pid);
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
			//	printf("p->pid: [%d], p->argv: [%s]\n", p->pid, p->argv[0]);
				if (p->pid == pid)
				{
					p->status = status;
					if (WIFSTOPPED(status))
						p->state |= STOPPED;
					else
					{
						p->state |= COMPLETED;
						if (WIFSIGNALED(status))
							fprintf(stderr, "%d: Terminated by signal %d.\n", (int)pid, WTERMSIG(p->status));
					}
					return 0;
				}
				p = p->next;
			}
			j = j->next;
		}
		fprintf(stderr, "No child process %d.\n", pid);
				return -1;
		/*
		for (j = first_job; j; j = j->next)
			for (p = j->first_process; p; p = p->next)
				if (p->pid == pid)
				{
					p->status = status;
					if (WIFSTOPPED(status))
						p->state |= STOPPED;
					else
					{
						p->state |= COMPLETED;
						if (WIFSIGNALED(status))
							fprintf(stderr, "%d: Terminated by signal %d.\n", (int)pid, WTERMSIG(p->status));
					}
					return 0;
				}
			fprintf(stderr, "No child process %d.\n", pid);
			return -1;
			*/
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
//	ft_printf("--> %s\n", __FUNCTION__);
	int	status;
	pid_t	pid;

	status = 0;

	pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
//	ft_printf("---> %s, pid = %d\n", __FUNCTION__, pid);
	while (!mark_process_status(pid, status))
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
	/*
	do
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
	while (!mark_process_status(pid, status));
	*/
}

void	wait_for_job(t_job *j)
{
//	ft_printf("--> %s\n", __FUNCTION__);
	int	status;
	pid_t	pid;

	status = 0;

	pid = waitpid(WAIT_ANY, &status, WUNTRACED);
	//pid = waitpid(-j->pgid, &status, WUNTRACED);
//	ft_printf("---> %s, pid = %d\n", __FUNCTION__, pid);
		while (mark_process_status(pid, status) == 0 && !job_is_stopped(j)
			&& !job_is_completed(j))
			pid = waitpid(WAIT_ANY, &status, WUNTRACED);
	/*
	do
		pid = waitpid(WAIT_ANY, &status, WUNTRACED);
	while (mark_process_status(pid, status) == 0 && !job_is_stopped(j)
			&& !job_is_completed(j));
			*/
}

void	format_job_info(t_job *j, const char *status)
{
	//fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
	ft_putnbr_fd(j->pgid, STDERR_FILENO);
	ft_putstr_fd(" : ", STDERR_FILENO);
	ft_putstr_fd(status, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
}

void	do_job_notification(void)
{
//	ft_printf("--> %s\n", __FUNCTION__);
	t_job		*j;
	t_job		*jlast;
	t_job		*jnext;
	//t_process	*p;

	update_status();
	jlast = NULL;
	//for (j = first_job; j; j = jnext)
	j = first_job;
	while(j)
	{
		
		jnext = j->next;
		if (job_is_completed(j))
		{
			format_job_info(j, "completed");
			if (jlast)
				jlast->next = jnext;
			else
				first_job = jnext;
			//ft_printf("--> free j: %s ", j->first_process->argv[0]);
			free_job(j);
			j = NULL;
		}
		else if (job_is_stopped(j) && !j->notified)
		{
			format_job_info(j, "stopped");
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
	//ft_printf("---> %s\n",__FUNCTION__);
	t_process	*p;
	pid_t		pid;
	int		mypipe[2] = {-1, -1};
	int		infile = -1;
	int		outfile = -1;
	int ret;

	if (j->in_fd == -1 && (j->in_fd = open(j->srcfile, O_RDONLY)) == -1)
	{
		perror("open");
		return ;
	}
	infile = j->in_fd;
	p = j->first_process;
	while(p)
	{
		if (j->out_fd == -1 && (j->out_fd = open(j->dstfile, j->flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH )) == -1)
		{
			perror("open");
			return ;
		}
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

		if ((ret = check_built(p->argv[0])) >= 0)
		{
			ft_built_exe(p->argv, ret, infile, outfile);
			p->state |= COMPLETED;
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
				exit(1);
			}
			else
			{
				p->pid = pid;
				if (shell_is_interactive)
				{
					if (!j->pgid)			//first process sets pgid for new group
						j->pgid = pid;
					setpgid(pid, j->pgid);
					//ft_printf("j->pgid:[%d], pid:[%d]\n", j->pgid, pid);
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
	format_job_info(j, "launched");

	if (!shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j, 0);
	else
		put_job_in_background(j, 0);
}

void 	print_jobs()
{
		int i;
	t_job *j = first_job;
	t_process *p;
	printf("\n+++ PRINT +++\n");
	while (j)
	{
		ft_printf("%s\n", j->notified ? "notified" : "not notified");
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
			printf("state: %s\n", p->state & COMPLETED ? "completed" : "not completed");
			printf("-----------\n");
			p = p->next;
		}
		printf("SRC[%s][%d],DST[%s][%d], pgid[%d]\n", j->srcfile, j->in_fd, j->dstfile, j->out_fd, j->pgid);
		printf("===========\n");
		j = j->next;
	}
	printf("+++ DONE +++\n");
}
void	chld_handler(int signum)
{
//	ft_printf("---> %s\n",__FUNCTION__);
	if (signum == SIGCHLD)
		do_job_notification();
}

void	init_shell(void)
{
	//ft_printf("---> %s\n",__FUNCTION__);
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
		signal(SIGCHLD, chld_handler);
		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0)
		{
			perror("Couldn't put the shell in it's own group");
			exit(1);
		}

		tcsetpgrp(shell_terminal, shell_pgid);
		init_terminal();
	}
	//else
		//read commands from file, recognise comments
	//		fprintf(stderr, "shell is not interactive\n");
}

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
//	ft_printf("history: [%d]\n", i);
	g_hstr_nb = i;
}

void	print_history(void)
{
	int i = 0;
	printf("nbr history: [%d]\n", g_hstr_nb);
	while (i < g_hstr_nb)
	{
		printf("[%d]: %s\n", i, g_history[i]);
		i++;
	}
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
	//ft_printf("---> %s\n",__FUNCTION__);
	char line[MAXLINE];
	t_job *j;
	t_job *ptr;
	
	int infile;
	g_envp = NULL;
	if (argc == 1)
		init_shell();
	else
	{
		shell_is_interactive = 0;
		infile = open(argv[1], O_RDONLY);
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
			
			char *str = NULL;
			if (get_next_line(infile, &str) > 0)
				ft_strcpy(line, str);
			else
			{
				close(infile);
				ft_exit();
			}
		}
		//ft_printf("[%s]", line);
		if (line[0] != '\n' && pack_args(line, j))
		{
			if (shell_is_interactive)
			{
				check_history_capacity();
				ft_putstr_fd(line, g_hstr_fd);
				line[ft_strlen(line) - 1] = 0;
				g_history[g_hstr_nb] = ft_strdup(line);
				g_hstr_nb++;
			}
			ptr = first_job;
			while (ptr)
			{	
				launch_job(ptr, ptr->foreground);
				ptr = ptr->next;
			}
			//print_jobs();
			if (shell_is_interactive)
			do_job_notification();	// <--- in jobs 
//			print_jobs();
			if (shell_is_interactive)
			{
				close(g_hstr_fd);
				g_hstr_fd = -1;
				fd_check();
			}
			//print_history();
		}
	}
//system("leaks test");

	return 0;
}
/*

		*	edit few lines ctrl+UP, ctrl+DOWN 
		*	rewrite cursor according to winsize + SIGWINCH
		*	history		!!! need to remove last command when max DOWN
		*	copy/paste
		*	2>&-, 2>file
		*	non-interactive mode has to read line (GNL) from main(argv[1])
		*	jobs builtins (%, %%, bg, fg, jobs)
		*	tab

		-	pipes with builtins
		-	group for builtins? they're not separated process
		-	ctrl+c
		-	pipe heredoc
		-	ctrl + left/right
*/
