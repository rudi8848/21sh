/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_move.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 12:37:20 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:38:51 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

void	move_right(t_cpos *pos)
{
	if (pos->i < pos->len)
	{
		++(*pos).i;
		if (pos->curx < pos->width)
			++(*pos).curx;
		else if (pos->curx == pos->width)
		{
			++(*pos).cury;
			++(*pos).curln;
			pos->curx = 1;
		}
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
	}
}

void	move_left(t_cpos *pos)
{
	if (pos->i > 0)
	{
		--(*pos).i;
		if (((*pos).curln == 0 && (*pos).curx > (*pos).start) ||
			(pos->curln > 0 && pos->curx > 1))
			--(*pos).curx;
		else if ((*pos).curln > 0 && (*pos).curx == 1)
		{
			--(*pos).curln;
			--(*pos).cury;
			pos->curx = pos->width;
		}
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
	}
}

void	ft_move(uint64_t direction, char *line, t_cpos *pos)
{
	reset_selection(pos, line);
	if (direction == K_RIGHT)
		move_right(pos);
	else if (direction == K_LEFT)
		move_left(pos);
}
