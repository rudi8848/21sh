#include "21sh.h"
#include <ctype.h>
#include <strings.h>

# define K_LEFT		4479771
# define K_RIGHT	4414235
# define K_UP		4283163
# define K_DOWN		4348699

//# define K_ENTER	13	//if raw mode
# define K_ENTER	10
# define K_TAB		9
# define K_SPACE 	32
# define K_ESC		27
# define K_DELETE	2117294875L
# define K_BSPACE	127


#define MAXLINE 500

struct termios saved;
struct termios changed;

void	ft_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
	exit(EXIT_SUCCESS);
}

int ft_putchar(int c)
{
	return(write(1, &c, 1));
}

static void		ft_int_handler(int signum)
{
	ft_exit();
}



void			ft_set_signals(void)
{
	//signal(SIGTSTP, ft_tstp_handler);
	//signal(SIGCONT, ft_cont_handler);
	signal(SIGINT, ft_int_handler);
	signal(SIGTERM, ft_int_handler);
	signal(SIGABRT, ft_int_handler);
	signal(SIGQUIT, ft_int_handler);
	//signal(SIGWINCH, ft_winch_handler);
}

int 	raw_settings(void)
{
	changed = saved;

	changed.c_cflag &= ~(CSIZE | PARENB);
	changed.c_cflag |= CS8;
	changed.c_iflag &= ~(INLCR | ICRNL | ISTRIP | INPCK | IXON | BRKINT);
	changed.c_oflag &= ~OPOST;
	changed.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);

	changed.c_cc[VMIN] = 1;
	changed.c_cc[VTIME] = 0;


	//int ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed) == -1)
	{
		perror(__FUNCTION__);
		ft_exit();
	}
	return 0;
}

int 	cbreak_settings()
{
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

void	ft_prompt(void)
{
	write(STDOUT_FILENO, "# ", 2);
}

int		read_loop(void)
{
	char line[MAXLINE];
	int			rr;
	uint64_t	rb;
	int j;
	//char c;

	bzero(line, MAXLINE);
	rb = 0;
	int len = 0;
	int i = 0;
	ft_prompt();
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
		if (len + 1 == MAXLINE)
		{
			printf("Line is too long\n");
			ft_exit();
		}
		if (rb == K_RIGHT)
		{
			if (i < len)
			{
				i++;
				tputs(tgetstr("nd", NULL), 0, ft_putchar);
			}
		}
		else if (rb == K_LEFT )
		{
			if (i > 0)
			{	i--;
				tputs(tgetstr("le", NULL), 0, ft_putchar);
			}
		}
		else if (rb == K_ENTER)
			{
				//i--; index points to next character
				printf("\n");
				//передавать в лексер сo '\n'???
				if (len)
				{
					printf("[GOT:] %s [len = %d, index = %d]\n", line, len, i);
					bzero(line, len);
					i = 0;
					len = 0;
				}
				ft_prompt();
			}
		else if (isprint(rb))
		{
			write(STDOUT_FILENO, &rb, rr);
			tputs(tgetstr("im", NULL), 0, ft_putchar);
			if (line[i + 1])	//if it's at the middle of line
			{
					j = len + 1;
					while (j > i)
					{
						line[j] = line[j - 1];
						j--;
					}
					//and insert in termcap
				
				//tputs(tgetstr("mi", NULL), 0, ft_putchar);
				//tputs(tgetstr("ic", NULL), 0, ft_putchar);
				
				tputs(tgetstr("ip", NULL), 0, ft_putchar);
			

			}
			
			line[i] = (char)rb;

			if (i <= len)
			{
				i++;
				len++;
			}
			tputs(tgetstr("ei", NULL), 0, ft_putchar);
		}
		else if (rb == K_DOWN)
		{

		}
		else if (rb == K_UP)
		{

		}
		else if (rb == K_DELETE)
		{
			if (len > 0 && i >= 0 && i < len)
			{
				tputs(tgetstr("dm", NULL), 0, ft_putchar);		//turn on deleting mode
				tputs(tgetstr("dc", NULL), 0, ft_putchar);		//delete 1 char on cursor position
				tputs(tgetstr("ed", NULL), 0, ft_putchar);		// turn off deleting mode
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
				tputs(tgetstr("le", NULL), 0, ft_putchar);	//1 position to left
				tputs(tgetstr("dm", NULL), 0, ft_putchar);	//turn on deleting mode
				tputs(tgetstr("dc", NULL), 0, ft_putchar);	//delete 1 char on cursor position
				tputs(tgetstr("ed", NULL), 0, ft_putchar);	// turn off deleting mode
			}
		}
		
		else if (rb == K_ESC)
			ft_exit();
		//ft_print_forward(g_attr.args);
		rb = 0;
	}
	return 0;
}



int	main(void)
{
	int ret = 0;
	ft_set_signals();
	//printf("<%s>\n", ctermid(NULL));
	ret = tcgetattr(STDOUT_FILENO, &saved);
	if (ret == -1)
	{
		perror(__FUNCTION__);
		return 1;
	}
//-----------------------------------------------------------
	char *name = NULL;
	if (!(name = getenv("TERM")))
	{
		perror("getenv");
		exit(0);
	}
	if (tgetent(NULL, name) < 1)
	{
		perror("tgetent");
		exit(0);
	}
	//ret = tputs(tgetstr("cl", NULL), 0, ft_putchar);
	//printf("[%d]\n", ret);
	//-----------------------------------------------------------

	//raw_settings();
	cbreak_settings();
	read_loop();

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
	return 0;
}