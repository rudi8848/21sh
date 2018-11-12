#include "21sh.h"
#include <term.h>
#include <errno.h>

int	g_hstr_fd = -1;

void	ft_restore(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

void	free_job(t_job *j)
{
	int i;
	t_process *pprev;

	i = 0;
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

void	ft_exit(void)
{
	ft_restore();
	exit(EXIT_SUCCESS);
}

int ft_iputchar(int c)
{
    return (write(1, &c, 1));
}

void	find_home(char **home, char **pwd, char **tmp)
{
	int		len;

	len = ft_strlen(*home);
	if (ft_strnequ(*home, *pwd, len))
	{
		*home = "~";
		*tmp = ft_strsub(*pwd, len, ft_strlen(*pwd) - len);
	}
}

int		type_prompt()
{
	char	*user;
	char	*pwd;
	char	*home;
	char	*tmp;
	int totallen;

	tmp = NULL;
	user = get_copy_env("LOGNAME", MUTE);
	pwd = get_current_wd();
	home = get_copy_env("HOME",  MUTE);
	if (home)
		find_home(&home, &pwd, &tmp);
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

int		cbreak_settings()
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

void	init_terminal()
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
	signal(SIGINT, func);
	signal(SIGQUIT, func);
	signal(SIGTSTP, sig_tstp_handler);
	signal(SIGTERM, func);
	signal(SIGTTIN, func);
	signal(SIGTTOU, func);
	signal(SIGCHLD, SIG_DFL);
}

int		job_is_stopped(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & (STOPPED)))
			return (0);
		p = p->next;
	}
	return (1);
}

int		job_is_completed(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & COMPLETED))
			return (0);
		p = p->next;
	}
	return (1);
}

void	close_process_files(t_launch *launch, int errfile)
{
	if (launch->infile != STDIN_FILENO)
	{
		dup2(launch->infile, STDIN_FILENO);
		close(launch->infile);
	}
	if (launch->outfile != STDOUT_FILENO)
	{
		dup2(launch->outfile, STDOUT_FILENO);
		close(launch->outfile);
	}
	if (errfile != STDERR_FILENO)
	{
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}
}

void	launch_process(t_launch *launch, pid_t pgid, int errfile, int foreground)
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
	close_process_files(launch, errfile);
	if (execve(launch->p->argv[0], launch->p->argv, g_envp) < 0)
		perror("execve");
	exit(EXIT_FAILURE);
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

void	check_process_status(t_process *p, pid_t pid, int status)
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
}

int		check_process(t_job **j, t_process *p, pid_t pid, int status)
{
	while (p)
	{
		if (p->pid == pid)
		{
			check_process_status(p, pid, status);
			return (NORM);
		}
		p = p->next;
	}
	*j = (*j)->next;
	return (ERROR);
}

int		mark_process_status(pid_t pid, int status)
{
	t_job		*j;
	t_process	*p;

	if (pid > 0)
	{
		j = first_job;
		while (j)
		{
			p = j->first_process;
			if (check_process(&j, p, pid, status) == NORM)
				return (0);
		}
		ft_putstr_fd("No clild process ", STDERR_FILENO);
		ft_putnbr_fd((int)pid, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
				return (-1);
	}
	else if (pid == 0 || errno == ECHILD)
		return (-1);
	else
	{
		perror("waitpid");
		return (-1);
	}
}

void	update_status(void)
{
	int		status;
	pid_t	pid;
	int		res;

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
	int		status;
	pid_t	pid;
	int		res;

	status = 0;
	res = 0;
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

void	notify_jobs(t_job **j, t_job **jlast, t_job **jnext)
{
	while(*j)
	{
		*jnext = (*j)->next;
		if (job_is_completed(*j))
		{
			if (*jlast)
				(*jlast)->next = *jnext;
			else
				first_job = *jnext;
			free_job(*j);
			*j = NULL;
		}
		else if (job_is_stopped(*j) && !(*j)->notified)
		{
			(*j)->notified = 1;
			*jlast = *j;
		}
		else
			*jlast = *j;
		*j = *jnext;
	}
}

void	do_job_notification(void)
{
	t_job		*j;
	t_job		*jlast;
	t_job		*jnext;

	update_status();
	jlast = NULL;

	j = first_job;
	notify_jobs(&j, &jlast, &jnext);
}

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

void	check_fd(t_job *j)
{
	if (j->in_fd != STDIN_FILENO)
		close(j->in_fd);
	if (j->out_fd != STDOUT_FILENO)
		close(j->out_fd);
	if (j->err_fd > 2)
		close(j->err_fd);
}

void	set_job_number(t_job *j)
{
	t_job *ptr;

	j->nbr = 1;
	ptr = first_job;
	while (ptr)
	{
		if (j != ptr && j->nbr >= ptr->nbr)
			j->nbr++;
		ptr = ptr->next;
	}
}

void	init_launch(t_launch *launch, int *mypipe)
{
	launch->infile = -1;
	launch->outfile = -1;
	launch->pid = 0;
	launch->ret = -1;
	launch->p = NULL;
	mypipe[0] = -1;
	mypipe[1] = -1;
}

int		set_outfile(t_job *j, t_launch *launch, int *mypipe)
{
	if (j->out_fd == -1 && (j->out_fd = open(j->dstfile, j->flags, FILE_PERM )) == -1)
	{
		perror("open");
		return (ERROR);
	}
	if (launch->p->next)
	{
		if (pipe(mypipe) < 0)
		{
			perror("pipe");
			ft_exit();
		}
		launch->outfile = mypipe[1];
	}
	else
		launch->outfile = j->out_fd;
	return (NORM);
}

void	do_builtin(t_job *j, t_launch *launch, int *mypipe)
{
	launch->p->state |= COMPLETED;
	ft_built_exe(launch->p->argv, launch->ret, launch->infile, launch->outfile);
	if (launch->infile != j->in_fd)
		close(launch->infile);
	if (launch->outfile != j->out_fd)
		close(launch->outfile);
	launch->infile = mypipe[0];
	launch->p = launch->p->next;
}

void	make_child_process(t_job *j, t_launch *launch, int foreground)
{
	launch->pid = fork();
	if (launch->pid == CHILD)
		launch_process(launch, j->pgid, j->err_fd, foreground);
	else if (launch->pid == ERROR)
	{
		perror("fork");
		ft_exit();
	}
	else
	{
		launch->p->pid = launch->pid;
		if (shell_is_interactive)
		{
			if (!j->pgid)
				j->pgid = launch->pid;
			setpgid(launch->pid, j->pgid);
		}	
	}
}

void	switch_files(t_job *j, t_launch *launch, int *mypipe)
{
	if (launch->infile != j->in_fd)
		close(launch->infile);
	if (launch->outfile != j->out_fd)
		close(launch->outfile);
	launch->infile = mypipe[0];
	launch->p = launch->p->next;
}

int		check_files(t_job *j, t_launch *launch)
{
	if (j->in_fd == -1 && (j->in_fd = open(j->srcfile, O_RDONLY)) == -1)
	{
		perror("open");
		return (ERROR);
	}
	if (j->err_fd == -1 && (j->err_fd = open(j->errfile, j->flags, FILE_PERM)) == -1)
	{
		perror("open");
		return (ERROR);
	}
	launch->infile = j->in_fd;
	launch->p = j->first_process;
	return (NORM);
}

int		run_process(t_job *j, t_launch *launch, int *mypipe, int foreground)
{
	if (set_outfile(j, launch, mypipe) != NORM)
			return (RETURN);
	if ((launch->ret = check_built(launch->p->argv[0])) >= 0)
	{
		do_builtin(j, launch, mypipe);
		return (CONTINUE);
	}
	else
	{
		if (!ft_find(launch->p))
		{
			launch->p->state |= COMPLETED;
			return (RETURN);
		}
	make_child_process(j, launch, foreground);
	}
	return (NORM);
}

void	launch_job(t_job *j, int foreground)
{
	int			mypipe[2];
	t_launch	launch;
	int			ret;

	init_launch(&launch, mypipe);
	if (check_files(j, &launch) != NORM)
		return ;
	set_job_number(j);
	while (launch.p)
	{
		if ((ret = run_process(j, &launch, mypipe, foreground)) == RETURN)
			return ;
		else if (ret == CONTINUE)
			continue ;
		switch_files(j, &launch, mypipe);
	}
	check_fd(j);
	if (!shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j, 0);
	else
		put_job_in_background(j, 0);
}

void	init_shell(void)
{
	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);

	if (shell_is_interactive)
	{
		while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgid(0)))
		{
			fprintf(stderr, "shell is not interactive\n");
			kill(-shell_pgid, SIGTTIN);
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
** static void fd_check(void)
** {
**     int fd;
**     bool ok = true;
** 
**     for (fd = 3; fd < 20; fd++)
**         if (fcntl(fd, F_GETFL) != -1 || errno != EBADF) {
**             ok = false;
**             fprintf(stderr, "*** fd %d is open ***\n", fd);
**         }
**     if (!ok)
**         _exit(EXIT_FAILURE);
**}
*/

void	init_history(void)
{
	int		i;
	char	*str;

	i = 0;
	str = NULL;
	g_hstr_fd = open(".history", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
	if (g_hstr_fd < 0)
	{
		ft_printf("Error: Cannot open/create history file\n");
		return ;
	}
	while (get_next_line(g_hstr_fd, &str) > 0)
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

void	check_args(int argc, char **argv, int *infile)
{
	if (argc == 1)
		init_shell();
	else if (argc == 2)
	{
		shell_is_interactive = 0;
		*infile = open(argv[1], O_RDONLY);
		if (*infile < 0)
		{
			perror("Cannot open input file");
			exit(EXIT_FAILURE);
		}
		copy_env();
	}
	else
	{
		ft_putstr_fd("Unexpected argument\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

void	get_line(char *line, int infile)
{
	char *str;

	if (shell_is_interactive)
	{
		if (g_hstr_fd == -1)
			g_hstr_fd = open(".history", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
		cbreak_settings();
		read_line(&line[0], 0);
		ft_restore();
	}
	else
	{
		ft_bzero(line, MAXLINE);
		str = NULL;
		if (get_next_line(infile, &str) > 0)
		{
			ft_strcpy(line, str);
			line[ft_strlen(str)] = '\n';
			free(str);
		}
		else
			exit(close(infile));
	}
}

void	append_history(char *line)
{
	check_history_capacity();
	ft_putstr_fd(line, g_hstr_fd);
	line[ft_strlen(line) - 1] = 0;
	g_history[g_hstr_nb] = ft_strdup(line);
	g_hstr_nb++;
}

void	run_comand(char *line)
{
	t_job *ptr;

	if (shell_is_interactive)
	{
		set_stopsignals(SIG_IGN);
		append_history(line);
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

int		main(int argc, char **argv)
{
	char	line[MAXLINE];
	t_job	*j;
	int		infile;

	g_envp = NULL;
	check_args(argc, argv, &infile);
	first_job = NULL;
	if (shell_is_interactive)
		init_history();
	while (1)
	{
		j = (t_job*)ft_memalloc(sizeof(t_job));
		if (!j)
		{
			perror("ft_memalloc");
			return (1);
		}
		get_line(line, infile);
		if (line[0] != '\n' && pack_args(line, j))
			run_comand(line);
	}
	return (0);
}
