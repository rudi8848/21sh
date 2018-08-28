#include "21sh.h"
#include <term.h>
#include <errno.h>

#define MAXLINE 500

# define K_LEFT     4479771
# define K_RIGHT    4414235
# define K_UP       4283163
# define K_DOWN     4348699
# define K_CTRL_LEFT     74995417045787
# define K_CTRL_RIGHT    73895905418011

# define K_ENTER    10
# define K_TAB      9
# define K_SPACE    32
# define K_ESC      27
# define K_DELETE   2117294875L
# define K_BSPACE   127
# define K_HOME		4741915
# define K_END		4610843
# define K_ALT_C	42947		//for copy
# define K_ALT_V	10127586	//for paste

# define TERM_BELL	tputs(tgetstr("bl", tbuf), 0, ft_iputchar);
# define TERM_BACK	tputs(tgetstr("le", tbuf), 0, ft_iputchar);\
                	tputs(tgetstr("dm", tbuf), 0, ft_iputchar);\
                	tputs(tgetstr("dc", tbuf), 0, ft_iputchar);\
                	tputs(tgetstr("ed", tbuf), 0, ft_iputchar);

# define TERM_DEL	tputs(tgetstr("dm", tbuf), 0, ft_iputchar);\
                	tputs(tgetstr("dc", tbuf), 0, ft_iputchar);\
                	tputs(tgetstr("ed", tbuf), 0, ft_iputchar); 

# define TERM_END tputs(tgoto(tgetstr("RI", tbuf), 0, len - i), 0, ft_iputchar);
# define TERM_HOME tputs(tgoto(tgetstr("LE", tbuf), 0, i), 0, ft_iputchar);
# define TERM_CRS_RIGHT tputs(tgetstr("nd", tbuf), 0, ft_iputchar);
# define TERM_CRS_LEFT	tputs(tgetstr("le", tbuf), 0, ft_iputchar);

struct termios saved;
extern char **environ;

void    ft_restore()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

void	free_job(t_job *j)
{
	t_job *jprev;
	t_process *pprev;
	int i;
	while (j)
	{
		jprev = j;
		j = j->next;
		while (jprev->first_process)
		{
			pprev = jprev->first_process;
			jprev->first_process = jprev->first_process->next;
			i = 0;
			while (pprev->argv[i])
			{
				free(pprev->argv[i]);
				i++;
			}
			free(pprev);
		}
		
		free(jprev);
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

void    ft_prompt(void)
{
    write(STDOUT_FILENO, "# ", 2);
}
/*

static void     ft_int_handler(int signum)
{
    ft_exit();
}

void            ft_set_signals(void)
{
    //signal(SIGTSTP, ft_tstp_handler);
    //signal(SIGCONT, ft_cont_handler);
    signal(SIGINT, ft_int_handler);
    signal(SIGTERM, ft_int_handler);
    signal(SIGABRT, ft_int_handler);
    signal(SIGQUIT, ft_int_handler);
    //signal(SIGWINCH, ft_winch_handler);
}
*/
int     cbreak_settings()
{
	struct termios changed;

    changed = saved;
    changed.c_lflag &= ~(ICANON | ECHO);
    changed.c_cc[VMIN] = 1;
    changed.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed) == -1)
    {
        perror(__FUNCTION__);
        ft_exit();
    }

    return 0;
}

void    read_line(char *line, int start)
{
    
    int         rr;
    uint64_t    rb;
    int j;

    static char cp_buf[MAXLINE];
    bzero(line, MAXLINE - start);
    rb = 0;
    int len = 0;
    int i = 0;
    char buf[MAXWORD];
    char *ptr = &buf[0];
    char **tbuf = &ptr;
    if (!start)
	    ft_prompt();
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
        if (rb == K_CTRL_RIGHT)
        {
        	 if (i < len)
        	 {
        	 	//переместиться к началу следующего слова

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
		//ft_printf("\n CTRL LEFT\n");
        	 if (i)
        	 {
			 int n = i;
        	 	//переместиться к началу предыдущего слова
			if (line[i] == ' ' || line[i] == 't' || line[i]== '\0')
				i--;
			 while (i > 0 && (line[i] == ' ' || line[i] == '\t'))
				i--;
        	 	while (i > 0 && (line[i] != ' ' || line[i] != '\t'))
				i--;
			if (i)
				i++;
			tputs(tgoto(tgetstr("LE", tbuf), 0, n - i), 0, ft_iputchar);
        	 }
        }
        else if (rb == K_ENTER)
        {
            line[len] = '\n';
            //ft_printf("[PASS][%s]\n", line);
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
            tputs(tgetstr("im", tbuf), 0, ft_iputchar);          //insert mode
            if (line[i])   //if it's at the middle of line
            {
                    j = len + 1;
                    while (j > i)
                    {
                        line[j] = line[j - 1];
                        j--;
                    }
                    //and insert in termcap
                
                tputs(tgetstr("sc", tbuf), 0, ft_iputchar);      // save cursor position
                tputs(tgetstr("ce", tbuf), 0, ft_iputchar);      // delete end of line
                write(STDOUT_FILENO, line + i + 1, len);
                tputs(tgetstr("rc", tbuf), 0, ft_iputchar);      // restore cursor position
            }
            
           		line[i] = (char)rb;
                len++;
            if (i < len)
                i++;
            tputs(tgetstr("ei", tbuf), 0, ft_iputchar);          //end of insertion mode
        }
        else if (rb == K_DOWN)
        {
        	//	here will be history navigation
            TERM_BELL         // bell
        }
        else if (rb == K_UP)
        {
        	//	here will be history navigation
           TERM_BELL          // bell
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
        else if (rb == K_DELETE)
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
        perror("cannot get terminal settings");
        exit(EXIT_FAILURE);
    }
    char *name = NULL;
    if (!(name = getenv("TERM")))
    {
        perror("getenv");
        exit(EXIT_FAILURE);
    }
    if (tgetent(buf, name) < 1)
    {
        perror("tgetent");
        exit(EXIT_FAILURE);
    }
}

//===========================================
	pid_t	shell_pgid;
	int		shell_terminal;
	int		shell_is_interactive;
	t_job	*first_job = NULL;
//===========================================


void	set_stopsignals(sig_t func)
{
	//ft_printf("---> %s\n",__FUNCTION__);
	signal(SIGINT, func);
	signal(SIGQUIT, func);
	signal(SIGTSTP, func);
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
	t_process	*p;

	for (p = j->first_process; p; p = p->next)
		if (!(p->state & (STOPPED | COMPLETED)))
			return 0;
	return 1;
}

int	job_is_completed(t_job *j)
{
	t_process	*p;
	for (p = j->first_process; p; p = p->next)
		if (!(p->state & COMPLETED))
			return 0;
	return 1;
	
}

void	launch_process(t_process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground)
{
	//ft_printf("---> %s\n",__FUNCTION__);
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
	
	if (execve(p->argv[0], p->argv, environ) < 0)
		perror("execve");
	exit(1);
}

void	wait_for_job(t_job *j);
void	put_job_in_foreground(t_job *j, int cont)
{
	tcsetpgrp(shell_terminal, j->pgid);

	if (cont)
	{
		tcsetattr(shell_terminal, TCSAFLUSH, &j->tmodes);
		if (kill(- j->pgid, SIGCONT) < 0)
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
		if (kill( - j->pgid, SIGCONT) < 0)
			perror("kill(SIGCONT)");
}

int	mark_process_status(pid_t pid, int status)
{
	t_job		*j;
	t_process	*p;

	if (pid > 0)
	{
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
	int	status;
	pid_t	pid;

	do
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
	while (!mark_process_status(pid, status));
}

void	wait_for_job(t_job *j)
{
	int	status;
	pid_t	pid;

	do
		pid = waitpid(WAIT_ANY, &status, WUNTRACED);
	while (!mark_process_status(pid, status) && !job_is_stopped(j)
			&& !job_is_completed(j));
}

void	format_job_info(t_job *j, const char *status)
{
	fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void	do_job_notification(void)
{
	t_job		*j;
	t_job		*jlast;
	t_job		*jnext;
	t_process	*p;

	update_status();
	jlast = NULL;
	for (j = first_job; j; j = jnext)
	{
		jnext = j->next;
		if (job_is_completed(j))
		{
			format_job_info(j, "completed");
			if (jlast)
				jlast->next = jnext;
			free_job(j);
		}
		else if (job_is_stopped(j) && !j->notified)
		{
			format_job_info(j, "stopped");
			j->notified = 1;
			jlast = j;
		}
		else
			jlast = j;
	}
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

void	launch_job(t_job *j, int foreground)
{
	//ft_printf("---> %s\n",__FUNCTION__);
	t_process	*p;
	pid_t		pid;
	int		mypipe[2] = {-1, -1};
	int		infile = -1;
	int		outfile = -1;

	infile = j->in_fd;
	for(p = j->first_process; p; p = p->next)
	{
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
			outfile = mypipe[1];

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
				if (!j->pgid)
					j->pgid = pid;
				setpgid(pid, j->pgid);
			}
		}
		if (infile != j->in_fd)
			close(infile);
		if (outfile != j->out_fd)
			close(outfile);
		infile = mypipe[0];
	}
	format_job_info(j, "launched");

	if (!shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j, 0);
	else
		put_job_in_background(j, 0);
}

void 	print_jobs(t_job *first_job)
{
		int i;
	t_job *j = first_job;
	t_process *p;
	while (j)
	{
		p = j->first_process;
		while (p)
		{
			i = 0;
			while (p->argv[i])
			{
				printf("[%d] %s\n",i, p->argv[i]);
				i++;
			}
			ft_printf("-----------\n");
			p = p->next;
		}
		ft_printf("===========\n");
		j = j->next;
	}
	ft_printf("OK\n");
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

		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0)
		{
			perror("Couldn't put the shell in it's own group");
			exit(1);
		}

		tcsetpgrp(shell_terminal, shell_pgid);
		init_terminal();
	}
	else
			fprintf(stderr, "shell is not interactive\n");
}

int		main(void)
{
	//ft_printf("---> %s\n",__FUNCTION__);
	char line[MAXLINE];
	t_process *process;

	first_job = (t_job*)ft_memalloc(sizeof(t_job));
	process = (t_process*)ft_memalloc(sizeof(t_process));
	if (!first_job || !process)
	{
		perror("ft_memalloc");
		return 1;
	}
	first_job->first_process = process;

	init_shell();

	cbreak_settings();
	read_line(&line[0], 0);
	ft_restore();
	//ft_printf("\n[GOT:] %s", line);

	if (pack_args(line, &first_job))
		ft_printf("OK\n");
	else
		ft_printf("not valid\n");


print_jobs(first_job);
t_job *j;
j = first_job;
while (j)
{
	launch_job(j, 1);
	do_job_notification();
	j = j->next;
}
free_job(first_job);
//system("leaks test");
/*	
		launch_job(first_job, 1);

	do_job_notification();
	continue_job(first_job, 0);
*/
	return 0;
}
