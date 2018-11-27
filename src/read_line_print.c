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
		++pos->i;
		++pos->len;
		move_right(pos, IN_MEMORY);
		write(STDOUT_FILENO, &rb, rr);
	}
	else	//middle of line
	{
		shift_letters(line, pos);
		line[pos->i] = (char)rb;
		++pos->len;
		
		int j = pos->i;
		while (j > 0)
		{
			move_left(pos, ON_SCREEN);
			--j;
		}
		
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		j = 0;
		while (j < pos->len)
		{
			move_right(pos, IN_MEMORY);
			write(STDOUT_FILENO, &line[j], 1);
			++j;
		}
		++pos->i;
		while (j > pos->i)
		{
			move_left(pos, ON_SCREEN);
			--j;
		}
	}
}
