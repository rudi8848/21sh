/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_move_border.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 12:39:15 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:45:40 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

void	move_to_home(t_cpos *pos)
{
	if (pos->i)
	{
		pos->i = 0;
		pos->curx = pos->start;
		pos->cury -= pos->curln;
		pos->curln = 0;
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
	}
}

void	move_to_end(t_cpos *pos)
{
	if (pos->i < pos->len)
	{
		pos->i = pos->len;
		if (pos->height == 1)
		{
			pos->curx = pos->start + pos->len;
			tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
						(*pos).cury), 0, ft_iputchar);
		}
		else
		{
			if (pos->curln < pos->height)
			{
				pos->cury += pos->height - pos->curln - 1;
				pos->curln = pos->height - 1;
			}
			pos->curx = (pos->len - (pos->width - pos->start + 1))
				% (pos->width) + 1;
			if (pos->curx == 0)
				pos->curx = 1;
			tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
		}
	}
}

void	move_to_border(uint64_t direction, char *line, t_cpos *pos)
{
	pos->height = cmd_height(pos, line);
	if (direction == K_HOME)
		move_to_home(pos);
	if (direction == K_END)
		move_to_end(pos);
}
