/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_print.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 12:46:02 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:53:37 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

void	shift_letters(char *line, t_cpos *pos)
{
	int j;

	j = pos->len + 1;
	while (j > pos->i)
	{
		line[j] = line[j - 1];
		--j;
	}
	pos->height = cmd_height(pos, line);
}

void	find_position(char *line, t_cpos *pos)
{
	if (!pos->curln && pos->len > pos->width - pos->start)
	{
		tputs(tgetstr("sc", NULL), 0, ft_iputchar);
		tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury + 1), 0, ft_iputchar);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, line + pos->width - pos->start, pos->len);
		tputs(tgetstr("rc", NULL), 0, ft_iputchar);
	}
	else if (pos->curln && pos->len > pos->width - pos->start
			+ pos->width * pos->curln)
	{
		tputs(tgetstr("sc", NULL), 0, ft_iputchar);
		tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury + 1), 0, ft_iputchar);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, line + pos->width - pos->start +
				pos->width * pos->curln, pos->len);
		tputs(tgetstr("rc", NULL), 0, ft_iputchar);
	}
}

void	line_break(t_cpos *pos)
{
	++(*pos).cury;
	++(*pos).curln;
	++(*pos).height;
	pos->curx = 0;
	tputs(tgetstr("do", NULL), 0, ft_iputchar);
	tputs(tgetstr("cr", NULL), 0, ft_iputchar);
}

void	print(char *line, t_cpos *pos, uint64_t rb, int rr)
{
	reset_selection(pos, line);
	if (pos->len + 1 + pos->startline == MAXLINE)
	{
		tputs(tgetstr("bl", NULL), 0, ft_iputchar);
		ft_printf("\nLine is too long\n");
		return ;
	}
	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
				(*pos).cury), 0, ft_iputchar);
	tputs(tgetstr("im", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, &rb, rr);
	find_position(line, pos);
	if (line[pos->i])
		shift_letters(line, pos);
	if (pos->curx == pos->width)
		line_break(pos);
	if (pos->curx < pos->width)
		++(*pos).curx;
	line[pos->i] = (char)rb;
	++(*pos).len;
	if (pos->i < pos->len)
		++(*pos).i;
	tputs(tgetstr("ei", NULL), 0, ft_iputchar);
}
