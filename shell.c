#include "21sh.h"
#include <term.h>

#define MAXLINE 500
/*
	разбираем строку на лексемы, пакуем в t_process->argv

	читаем и записываем по слову, пока не дойдем до не слова или конца строки
*/

# define K_LEFT     4479771
# define K_RIGHT    4414235
# define K_UP       4283163
# define K_DOWN     4348699

# define K_ENTER    10
# define K_TAB      9
# define K_SPACE    32
# define K_ESC      27
# define K_DELETE   2117294875L
# define K_BSPACE   127

struct termios saved;


void    ft_restore()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
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

void    read_line(char *line)
{
    
    int         rr;
    uint64_t    rb;
    int j;

    bzero(line, MAXLINE);
    rb = 0;
    int len = 0;
    int i = 0;
    ft_prompt();
    while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
    {
        
        if (rb == K_RIGHT)
        {
            if (i < len)
            {
                i++;
                tputs(tgetstr("nd", NULL), 0, ft_iputchar);
            }
        }
        else if (rb == K_LEFT )
        {
            if (i > 0)
            {   i--;
                tputs(tgetstr("le", NULL), 0, ft_iputchar);
            }
        }
        else if (rb == K_ENTER)
            {
                line[len] = '\n';
                return;
            }
        else if (ft_isprint(rb))
        {
            if (len + 1 == MAXLINE)
            {
                tputs(tgetstr("bl", NULL), 0, ft_iputchar);          // bell
                printf("\nLine is too long\n");
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
                    //and insert in termcap
                
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
            tputs(tgetstr("bl", NULL), 0, ft_iputchar);          // bell
        }
        else if (rb == K_UP)
        {
        	//	here will be history navigation
            tputs(tgetstr("bl", NULL), 0, ft_iputchar);          // bell
        }
        else if (rb == K_DELETE)
        {
            if (len > 0 && i >= 0 && i < len)
            {
                tputs(tgetstr("dm", NULL), 0, ft_iputchar);      //turn on deleting mode
                tputs(tgetstr("dc", NULL), 0, ft_iputchar);      //delete 1 char on cursor position
                tputs(tgetstr("ed", NULL), 0, ft_iputchar);      // turn off deleting mode
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
                tputs(tgetstr("le", NULL), 0, ft_iputchar);  //1 position to left
                tputs(tgetstr("dm", NULL), 0, ft_iputchar);  //turn on deleting mode
                tputs(tgetstr("dc", NULL), 0, ft_iputchar);  //delete 1 char on cursor position
                tputs(tgetstr("ed", NULL), 0, ft_iputchar);  // turn off deleting mode
            }
        }
        
        else if (rb == K_ESC)
            ft_exit();
        rb = 0;
    }
    line[i] = '\n';
    return;
}

int 	pack_argv(t_process **process, char *line)
{
	return 0;
}

void 	init_terminal()
{
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
    if (tgetent(NULL, name) < 1)
    {
        perror("tgetent");
        exit(EXIT_FAILURE);
    }
}

void	ignore_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
}

void	init_shell(void)
{
	
	pid_t	shell_pgid;
	int	shell_terminal;
	int	shell_is_interactive;

	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);

	if (shell_is_interactive)
	{
		while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgid(0)))
		{
			fprintf(stderr, "shell is not interactive\n");
			kill( - shell_pgid, SIGTTIN);
		}

		ignore_signals();

		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0)
		{
			perror("Couldn't put the shell in it's own group");
			exit(1);
		}

		tcsetpgrp(shell_terminal, shell_pgid);
		init_terminal();
	}
}

int		main(void)
{

	t_process *process;

	char line[MAXLINE];
	process = (t_process*)ft_memalloc(sizeof(t_process));
	if (!process)
	{
		perror("ft_memalloc");
		return 1;
	}
	//--------------------------------------------------------
	
//   	init_terminal();
	init_shell();
	ft_printf("Initialization successfull\n");
    //ft_set_signals();

	cbreak_settings();
	read_line(&line[0]);
	ft_restore();
	ft_printf("\n[GOT:] %s", line);

 //   pack_argv(&process, &line[0]);
/*

	//--------------------------------------------------------
	int i = 0;
	while (process->argv[i])
	{
		write(1, process->argv[i], sizeof(process->argv[i]));
		write(1, "\n", 1);
		i++;
	}
	*/
	return 0;
}
