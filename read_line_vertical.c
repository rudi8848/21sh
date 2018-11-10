/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_vertical.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 12:56:49 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:58:55 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

void	jump_up(t_cpos *pos)
{
	if (pos->curln)
	{
		if (pos->curln == 1)
		{
			if (pos->curx < pos->start)
			{
				pos->curx = pos->start;
				pos->i = 0;
			}
		}
		if (pos->i)
			pos->i -= pos->width;
		--(*pos).cury;
		--(*pos).curln;
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
	}
}

void	jump_down(t_cpos *pos)
{
	int tail;

	if (pos->height > 1 && pos->curln < pos->height - 1)
	{
		if (pos->curln == pos->height - 2)
		{
			tail = (pos->len - (pos->width - pos->start)) % pos->width;
			if (pos->curx > tail)
			{
				pos->curx = tail;
				pos->i = pos->len;
			}
		}
		if (pos->i + pos->width < pos->len)
			pos->i += pos->width;
		++(*pos).cury;
		++(*pos).curln;
		tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
					(*pos).cury), 0, ft_iputchar);
	}
}

void	ft_jump_vertical(uint64_t rb, char *line, t_cpos *pos)
{
	reset_selection(pos, line);
	pos->height = cmd_height(pos, line);
	if (rb == K_CTRL_UP)
		jump_up(pos);
	else if (rb == K_CTRL_DOWN)
		jump_down(pos);
}
