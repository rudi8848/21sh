/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_move_border.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/07 15:08:55 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 15:08:57 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	move_to_home(t_cpos *pos)
{
	if (pos->i)
	{
		while (pos->i > 0)
		{
			move_left(pos, ON_SCREEN);
			--pos->i;
		}
	}
}

void	move_to_end(t_cpos *pos)
{
	if (pos->i < pos->len)
	{
		while (pos->i < pos->len)
		{
			move_right(pos, ON_SCREEN);
			++pos->i;
		}
	}
}

void	move_to_border(uint64_t direction, char *line, t_cpos *pos)
{
	if (!line)
		return ;
	if (direction == K_HOME)
		move_to_home(pos);
	if (direction == K_END)
		move_to_end(pos);
}
