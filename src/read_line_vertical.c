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

#include "shell.h"

void	jump_up(t_cpos *pos)
{
	if (pos->i + pos->prompt_len > pos->width)
	{
		int j = 0;
		while (pos->i > 0 && j < pos->width)
		{
			--pos->i;
			move_left(pos, ON_SCREEN);
			++j;
		}
	}
}

void	jump_down(t_cpos *pos)
{
	if (pos->len + pos->prompt_len > pos->width)
	{
		int j = 0;
		while (pos->i < pos->len && j < pos->width)
		{
			++pos->i;
			move_right(pos, ON_SCREEN);
			++j;
		}
	}
}

void	ft_jump_vertical(uint64_t rb, char *line, t_cpos *pos)
{
	reset_selection(pos, line);
	//pos->height = cmd_height(pos, line);
	if (!line)
		return;
	if (rb == K_CTRL_UP)
		jump_up(pos);
	else if (rb == K_CTRL_DOWN)
		jump_down(pos);
}