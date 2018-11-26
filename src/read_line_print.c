#include "shell.h"



/*void	go_right(t_cpos *pos, int mode)
{
	if (pos->i < pos->len)
	{
		++pos->i;
		if (pos->curx == pos->width - 1)
		{
			pos->curx = 0;
			++pos->cury;
			if (mode == ON_SCREEN)
			{
				tputs(tgetstr("do", NULL), 1, ft_iputchar);
				tputs(tgetstr("cr", NULL), 1, ft_iputchar);
			}
		}
		else if (pos->cury >= 0)
		{
			++pos->curx;
			if (mode == ON_SCREEN)
				tputs(tgetstr("nd", NULL), 1, ft_iputchar);
		}
	}
}
*/
void	print(char *line, t_cpos *pos, uint64_t rb, int rr)
{
	//reset_selection(pos, line);
	if (pos->len + 1 + pos->startline == MAXLINE)
	{
		tputs(tgetstr("bl", NULL), 0, ft_iputchar);
		ft_printf("\nLine is too long\n");
		return ;
	}
	if (pos->i == pos->len)	//end of line
	{
		line[pos->i] = (char)rb;
		write(STDOUT_FILENO, &rb, rr);
		//++pos->i;
		++pos->len;
		go_right(IN_MEMORY);
	}
	else
	{
		shift_letters(line, pos);
		line[pos->i] = (char)rb;
		++pos->len;
		tputs(tgetstr("im", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, &rb, rr);
		tputs(tgetstr("ei", NULL), 0, ft_iputchar);
		go_right(IN_MEMORY);
	}

}