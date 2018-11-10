/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_history.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:36:58 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 12:55:44 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

void	history_next(char *line, t_cpos *pos, int *cmd)
{
	if (g_hstr_nb && (*cmd < (g_hstr_nb - 1)))
	{
		move_to_border(K_HOME, line, pos);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		ft_bzero(line, MAXLINE);
		init_position(pos, 1, line);
		++(*cmd);
		ft_strcpy(line, g_history[*cmd]);
		pos->len = ft_strlen(line);
		pos->i = pos->len - 1;
		write(STDOUT_FILENO, line, pos->len);
		pos->height = cmd_height(pos, line);
		move_to_border(K_END, line, pos);
	}
	else if (g_hstr_nb && (*cmd == (g_hstr_nb - 1)))
	{
		move_to_border(K_HOME, line, pos);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		ft_bzero(line, MAXLINE);
		init_position(pos, 1, line);
		++(*cmd);
		tputs(tgetstr("bl", NULL), 0, ft_iputchar);
	}
	else
		tputs(tgetstr("bl", NULL), 0, ft_iputchar);
}

void	history_prev(char *line, t_cpos *pos, int *cmd)
{
	if (g_hstr_nb && *cmd)
	{
		move_to_border(K_HOME, line, pos);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		ft_bzero(line, MAXLINE);
		init_position(pos, 1, line);
		if (*cmd > 0)
		{
			--(*cmd);
			ft_strcpy(line, g_history[*cmd]);
		}
		pos->len = ft_strlen(line);
		pos->i = pos->len - 1;
		write(STDOUT_FILENO, line, pos->len);
		pos->height = cmd_height(pos, line);
		move_to_border(K_END, line, pos);
	}
	else
		tputs(tgetstr("bl", NULL), 0, ft_iputchar);
}

void	move_history(uint64_t rb, char *line, t_cpos *pos, int *cmd)
{
	reset_selection(pos, line);
	if (rb == K_DOWN)
		history_next(line, pos, cmd);
	else if (rb == K_UP)
		history_prev(line, pos, cmd);
}
