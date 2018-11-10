/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_highlight.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:31:28 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 11:36:07 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

void	reprint(char *line, t_cpos *pos)
{
	int x;
	int y;
	int i;

	x = pos->curx;
	y = pos->cury;
	i = pos->i;
	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	move_to_border(K_HOME, line, pos);
	pos->i = i;
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line, pos->first);
	pos->curx += pos->first;
	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1, (*pos).cury),
			0, ft_iputchar);
	tputs(tgetstr("mr", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line + pos->first, pos->last - pos->first);
	pos->curx += pos->last - pos->first;
	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
				(*pos).cury), 0, ft_iputchar);
	tputs(tgetstr("me", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line + pos->last, pos->len - pos->last);
	tputs(tgetstr("rc", NULL), 0, ft_iputchar);
	pos->curx = x;
	pos->cury = y;
}

void	select_left(char *line, t_cpos *pos)
{
	if (pos->i > 0)
	{
		if (pos->last < pos->i)
			pos->last = pos->i;
		--(*pos).i;
		--(*pos).curx;
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
		if ((*pos).first == -1 || (*pos).first > pos->i)
			(*pos).first = (*pos).i;
		reprint(line, pos);
	}
}

void	select_right(char *line, t_cpos *pos)
{
	if (pos->i < pos->len)
	{
		if (pos->first > pos->i || pos->first == -1)
			pos->first = pos->i;
		++(*pos).i;
		++(*pos).curx;
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
		if (pos->last < pos->i || pos->last == -1)
			pos->last = pos->i;
		reprint(line, pos);
	}
}

void	ft_highlight(uint64_t rb, char *line, t_cpos *pos)
{
	if (rb == K_SHFT_L)
		select_left(line, pos);
	else if (rb == K_SHFT_R)
		select_right(line, pos);
}
