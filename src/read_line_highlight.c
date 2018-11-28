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

#include "shell.h"

void	reprint(char *line, t_cpos *pos)
{
	int i;

	i = pos->i;
	move_to_border(K_HOME, line, pos);

	tputs(tgetstr("cd", NULL), 0, ft_iputchar);

	while (pos->i < pos->first)
	{
		write(STDOUT_FILENO, &line[pos->i], 1);
		move_right(pos, IN_MEMORY);
		++pos->i;
	}
	tputs(tgetstr("mr", NULL), 0, ft_iputchar);
	while (pos->i < pos->last)
	{
		write(STDOUT_FILENO, &line[pos->i], 1);
		move_right(pos, IN_MEMORY);
		++pos->i;
	}

	tputs(tgetstr("me", NULL), 0, ft_iputchar);
	while (pos->i < pos->len)
	{
		write(STDOUT_FILENO, &line[pos->i], 1);
		move_right(pos, IN_MEMORY);
		++pos->i;
	}
	while (pos->i > i)
	{
		move_left(pos, ON_SCREEN);
		--pos->i;
	}
}

void	select_left(char *line, t_cpos *pos)
{
	if (pos->i > 0)
	{
		if (pos->last < pos->i)
			pos->last = pos->i;
		--pos->i;
		if (pos->first == -1 || pos->first > pos->i)
			pos->first = pos->i;
		move_left(pos, ON_SCREEN);
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
		if (pos->last < pos->i || pos->last == -1)
			pos->last = pos->i;
		move_right(pos, ON_SCREEN);
		reprint(line, pos);
	}
}

void	ft_highlight(uint64_t rb, char *line, t_cpos *pos)
{
	pos->selection = 1;
	if (rb == K_SHFT_L)
		select_left(line, pos);
	else if (rb == K_SHFT_R)
		select_right(line, pos);
}
