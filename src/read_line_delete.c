/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_delete.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:21:55 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 11:27:21 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	delete_from_screen(char *line, t_cpos *pos)
{
	int j;

	j = pos->i;
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	while (j < pos->len)
	{
		write(STDOUT_FILENO, &line[j], 1);
		move_right(pos, IN_MEMORY);
		++j;
	}
	while (j > pos->i)
	{
		move_left(pos, ON_SCREEN);
		--j;
	}
}

static void	delete_front(char *line, t_cpos *pos)
{
	int j;

	if (pos->len > 0 && pos->i >= 0 && pos->i < pos->len)
	{
		pos->len--;
		
		j = pos->i;
		while (line[j])
		{
			line[j] = line[j + 1];
			++j;
		}
		delete_from_screen(line, pos);
	}
}

static void	delete_back(char *line, t_cpos *pos)
{
	int j;

	if (pos->i > 0)
	{
		pos->i--;
		pos->len--;
	
		j = pos->i;
		while (line[j])
		{
			line[j] = line[j + 1];
			++j;
		}
		move_left(pos, ON_SCREEN);
		delete_from_screen(line, pos);
	}
}

void	delete_char(uint64_t rb, char *line, t_cpos *pos)
{
	reset_selection(pos, line);
	if (rb == K_CTRL_D && is_empty(line) && !pos->startline)
		ft_exit();
	else if (rb == K_CTRL_D && is_empty(line) && pos->startline && pos->eol)
		ft_strcpy(line, pos->eol);
	if (rb == K_DELETE || rb == K_CTRL_D)
		delete_front(line, pos);
	else if (rb == K_BSPACE)
		delete_back(line, pos);
}
