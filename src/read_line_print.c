#include "shell.h"

void	move_right(t_cpos *pos, int mode);
void	move_left(t_cpos *pos, int mode);


void	shift_letters(char *line, t_cpos *pos)
{
	int j;

	j = pos->len + 1;
	while (j > pos->i)
	{
		line[j] = line[j - 1];
		--j;
	}
}
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
	if (!line[pos->i])	//end of line
	{
		line[pos->i] = (char)rb;
		//++pos->i;
		++pos->len;
		move_right(pos, IN_MEMORY);
		write(STDOUT_FILENO, &rb, rr);
	}
	else
	{
		shift_letters(line, pos);
		line[pos->i] = (char)rb;
		++pos->len;
		//tputs(tgetstr("im", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, &rb, rr);
		//tputs(tgetstr("ei", NULL), 0, ft_iputchar);
		move_right(pos, IN_MEMORY);
		if (pos->len > pos->width - pos->prompt_len)
		{
			tputs(tgetstr("sc", NULL), 0, ft_iputchar);
			tputs(tgetstr("cd", NULL), 0, ft_iputchar);
			write(STDOUT_FILENO, &line[pos->i], pos->len - pos->i);
			tputs(tgetstr("rc", NULL), 0, ft_iputchar);
		}

	}

}
