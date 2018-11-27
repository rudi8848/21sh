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

#include "shell.h"

void	move_right(t_cpos *pos, int mode)
{
	
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

void	move_left(t_cpos *pos, int mode)
{
		if (pos->curx == 0 && pos->cury > 0)
		{
			pos->curx = pos->width - 1;
			--pos->cury;
			if (mode == ON_SCREEN)
			{
				tputs(tgetstr("up", NULL), 1, ft_iputchar);
				tputs(tgoto(tgetstr("ch", NULL), 1, pos->width -1),1, ft_iputchar);
			}
		}
		else if (pos->curx > 0)
		{
			--pos->curx;
			if (mode == ON_SCREEN)
				tputs(tgetstr("le", NULL), 1, ft_iputchar);
		}
}

void	ft_move(uint64_t direction, char *line, t_cpos *pos)
{
//	reset_selection(pos, line);
	if (!line)
		return;
	if (direction == K_RIGHT)
	{
		if (pos->i < pos->len)
		{
			++pos->i;
			move_right(pos, ON_SCREEN);
		}
	}
	else if (direction == K_LEFT)
		if (pos->i > 0)
		{
			--pos->i;
			move_left(pos, ON_SCREEN);
		}
}
