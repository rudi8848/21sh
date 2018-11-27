/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:14:37 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 11:18:24 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	check_key(char *line, t_cpos *pos, uint64_t rb, int *cmd)
{
	if (rb == K_RIGHT || rb == K_LEFT)
		ft_move(rb, line, pos);
	if (!cmd)
		return;
	else if (rb == K_DOWN || rb == K_UP)
		move_history(rb, line, pos, cmd);
	else if (rb == K_BSPACE || rb == K_DELETE || rb == K_CTRL_D)
		delete_char(rb, line, pos);
	else if (rb == K_HOME || rb == K_END)
		move_to_border(rb, line, pos);
	else if (rb == K_ESC)
		ft_exit();
	else if (rb == K_CTRL_UP || rb == K_CTRL_DOWN)
		ft_jump_vertical(rb, line, pos);
	else if (rb == K_CTRL_RIGHT || rb == K_CTRL_LEFT)
		ft_jump(rb, line, pos);/*
	else if (rb == K_SHFT_L || rb == K_SHFT_R)
		ft_highlight(rb, line, pos);
	else if (rb == K_ALT_C || rb == K_ALT_V || rb == K_ALT_X)
		ft_copy_paste(rb, line, pos);*/
	else if (rb == K_TAB)
		ft_autocomplete(line, pos);
		
}

void	end_line(char *line, uint64_t rb, t_cpos *pos)
{
	//reset_selection(pos, line);
	move_to_border(K_END, line, pos);
	ft_printf("\n");
	if (rb == K_CTRL_C)
		ft_bzero(line, MAXLINE);
	else
		(line[pos->len] = '\n');
}

void	read_line(char *line, int start)
{
	int			rr;
	uint64_t	rb;
	t_cpos		pos;
	int			cmd;

	cmd = g_hstr_nb;
	if (start == 0)
		pos.prompt_len = type_prompt();
	else
		pos.prompt_len = 0;
	ft_bzero(line, MAXLINE - start);
	init_position(&pos, start, line);
	rb = 0;
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
		if (ft_isprint(rb))
			print(line, &pos, rb, rr);
		else if (rb == K_CTRL_C || rb == K_ENTER)
			return (end_line(line, rb, &pos));
		else
			check_key(line, &pos, rb, &cmd);
		rb = 0;
	}
	return ;
}
