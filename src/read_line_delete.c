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

#include "../includes/shell.h"

void	delete_from_screen(char *line, t_cpos *pos, char offset)
{
	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - offset,
			(*pos).cury), 0, ft_iputchar);
	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line + pos->i, pos->len);
	pos->height = cmd_height(pos, line);
	tputs(tgetstr("rc", NULL), 0, ft_iputchar);
}

void	delete_front(char *line, t_cpos *pos)
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
		delete_from_screen(line, pos, 1);
	}
}

void	delete_back(char *line, t_cpos *pos)
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
		if (pos->curx > 1)
		{
			delete_from_screen(line, pos, 2);
			--(*pos).curx;
		}
		else if (pos->curx <= 1 && pos->curln)
		{
			pos->curx = pos->width;
			--(*pos).cury;
			--(*pos).curln;
			delete_from_screen(line, pos, 1);
		}
	}
}

void	delete_char(uint64_t rb, char *line, t_cpos *pos)
{
	reset_selection(pos, line);
	pos->height = cmd_height(pos, line);
	if (rb == K_DELETE || rb == K_CTRL_D)
		delete_front(line, pos);
	else if (rb == K_BSPACE)
		delete_back(line, pos);
}
