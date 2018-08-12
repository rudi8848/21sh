#include "21sh.h"
#include <ctype.h>

# define K_LEFT		4479771
# define K_RIGHT	4414235
# define K_UP		4283163
# define K_DOWN		4348699

# define K_ENTER	13
//# define K_ENTER	10
# define K_TAB		9
# define K_SPACE 	32
# define K_ESC		27
# define K_DELETE	2117294875L
# define K_BSPACE	127


struct termios saved;
struct termios changed;

void	ft_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
	exit(EXIT_SUCCESS);
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

int 	change_settings(void)
{
	changed = saved;

	changed.c_cflag &= ~(CSIZE | PARENB);
	changed.c_cflag |= CS8;
	changed.c_iflag &= ~(INLCR | ICRNL | ISTRIP | INPCK | IXON | BRKINT);
	changed.c_oflag &= ~OPOST;
	changed.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);

	changed.c_cc[VMIN] = 1;
	changed.c_cc[VTIME] = 0;


	int ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed);
	if (ret == -1)
	{
		perror(__FUNCTION__);
		exit(0);
	}
	return 0;
}

int		read_loop(void)
{
int			rr;
	uint64_t	rb;
	//t_dslist	*ptr;

	//ptr = g_attr.args;
	rb = 0;
	//ft_print_forward(ptr);
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
		if (rb == K_RIGHT || rb == K_LEFT)
			//ft_move_h(&ptr, rb);
			printf("LEFT RIGHT\n");
		else if (isprint(rb))
			write(STDOUT_FILENO, &rb, rr);
		else if (rb == K_DOWN || rb == K_UP)
			//ft_move_v(&ptr, rb);
			printf("UP DOWN\n");
		//else if (rb == K_SPACE)
		//	ft_switch_mode(ptr, M_SLCT);
		else if (rb == K_DELETE || rb == K_BSPACE)
		//	ft_delete_active(&ptr);
			printf("DELETE\n");
		else if (rb == K_ENTER)
			//return (ft_print_selected(g_attr.args));
			printf("ENTER\n");
		else if (rb == K_ESC)
			//ft_exit();
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
	ret = tcgetattr(STDOUT_FILENO, &saved);
	if (ret == -1)
	{
		perror(__FUNCTION__);
		return 1;
	}

	change_settings();

	read_loop();

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
	return 0;
}