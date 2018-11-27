/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_jump.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:52:04 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:02:04 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
/*
void	cursor_to_i(t_cpos *pos)
{
	int first_line;
	int tail;
	int lns;
	int distance;

	first_line = 0;
	tail = 0;
	lns = 0;
	distance = pos->i;
	pos->curx = pos->start;
	if (!pos->curln && pos->start + distance < pos->width)
		pos->curx += distance;
	else
	{
		first_line = pos->width - pos->start + 1;
		tail = (pos->len - first_line) % pos->width;
		pos->curx = tail;
		lns = (pos->len - first_line) / pos->width;
		if (tail)
			++lns;
		pos->curln += lns;
		pos->cury += lns;
	}
	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1,
				(*pos).cury), 0, ft_iputchar);
}
*/
void	jump_right(char *line, t_cpos *pos, int *j)
{
	int		dif;

	if (pos->i < pos->len && pos->curx < pos->width)
	{
		(*j) = pos->i;
		if (line[(*j)] && line[(*j)] != ' ' && line[(*j)] != '\t')
		{
			while (line[(*j)] && line[(*j)] != ' ' && line[(*j)] != '\t')
				++(*j);
		}
		if (line[(*j)] && (line[(*j)] == ' ' || line[(*j)] == '\t'))
		{
			while (line[(*j)] && (line[(*j)] == ' ' || line[(*j)] == '\t'))
				++(*j);
		}
		if ((dif = *j - pos->i) >= 0)
		{
			/*pos->i = *j;
			cursor_to_i(pos);*/
			while (pos->i < *j)
			{
				move_right(pos, ON_SCREEN);
				++pos->i;
			}
		}
	}
}

void	prev_space(char *line, int *j)
{
	if ((*j) > 0 && (line[(*j)] == ' ' || line[(*j)] == '\t'))
	{
		while ((*j) > 0 && (line[(*j)] == ' ' || line[(*j)] == '\t'))
			--(*j);
		if ((*j) > 0 && line[(*j)] != ' ' && line[(*j)] != '\t')
		{
			while ((*j) > 0)
			{
				if (line[(*j)] == ' ' || line[(*j)] == '\t')
				{
					++(*j);
					break ;
				}
				--(*j);
			}
		}
	}
}

void	jump_left(char *line, t_cpos *pos, int *j)
{
	int		dif;

	(*j) = pos->i;
	if ((*j) > 0 && !line[(*j)])
		--(*j);
	if ((*j) > 0 && line[(*j)] != ' ' && line[(*j)] != '\t' && line[(*j) - 1]
			!= ' ' && line[(*j) - 1] != '\t')
	{
		while ((*j) > 0 && (line[(*j)] != ' ' && line[(*j)] != '\t'))
			--(*j);
		if (line[(*j)] == ' ' || line[(*j)] == '\t')
			++(*j);
	}
	else if ((*j) > 0 && line[(*j)] != ' ' && line[(*j)] != '\t'
			&& (line[(*j) - 1] == ' ' || line[(*j) - 1] == '\t'))
		--(*j);
	prev_space(line, j);
	if ((dif = pos->i - *j) >= 0)
	{
		//pos->i = *j;
		//cursor_to_i(pos);
		while (pos->i > *j)
		{
			move_left(pos, ON_SCREEN);
			--pos->i;
		}
	}
}

void	ft_jump(uint64_t rb, char *line, t_cpos *pos)
{
	int		j;

	j = 0;
	//reset_selection(pos, line);
	if (rb == K_CTRL_RIGHT)
		jump_right(line, pos, &j);
	else if (rb == K_CTRL_LEFT)
		jump_left(line, pos, &j);
}
