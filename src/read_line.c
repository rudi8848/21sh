/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:14:37 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 14:45:50 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		check_key(char *line, t_cpos *pos, uint64_t rb, int *cmd)
{
	if (rb == K_RIGHT || rb == K_LEFT)
		ft_move(rb, line, pos);
	else if (rb == K_DOWN || rb == K_UP)
		move_history(rb, line, pos, cmd);
	else if (rb == K_BSPACE || rb == K_DELETE || rb == K_CTRL_D)
		return (delete_char(rb, line, pos));
	else if (rb == K_HOME || rb == K_END)
		move_to_border(rb, line, pos);
	else if (rb == K_ESC)
		ft_exit();
	else if (rb == K_CTRL_UP || rb == K_CTRL_DOWN)
		ft_jump_vertical(rb, line, pos);
	else if (rb == K_CTRL_RIGHT || rb == K_CTRL_LEFT)
		ft_jump(rb, line, pos);
	else if (rb == K_SHFT_L || rb == K_SHFT_R)
		ft_highlight(rb, line, pos);
	else if (rb == K_ALT_C || rb == K_ALT_V || rb == K_ALT_X)
		ft_copy_paste(rb, line, pos);
	else if (rb == K_TAB)
		ft_autocomplete(line, pos);
	return (CONTINUE);
}

void	end_line(char *line, uint64_t rb, t_cpos *pos)
{
	reset_selection(pos, line);
	move_to_border(K_END, line, pos);
	ft_printf("\n");
	if (rb == K_CTRL_C)
		ft_bzero(line, MAXLINE);
	else
		(line[pos->len] = '\n');
}

void	read_line(char *line, int start, char *eol)
{
	dprintf(4, ">\t->\t%s\t<-\n", __FUNCTION__);
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
	init_position(&pos, start, line, eol);
	rb = 0;
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
		dprintf(4, ">\t->\t%s [%llu], ret: %i\t<-\n", __FUNCTION__, rb, rr);
		if (ft_isprint(rb))
			print(line, &pos, rb, rr);
		else if (rb == K_CTRL_C || rb == K_ENTER)
			return (end_line(line, rb, &pos));
		else if (check_key(line, &pos, rb, &cmd) == RETURN)
			return ;
		else
			line[pos.len] = '\n';
		rb = 0;
		
	}
	dprintf(4, ">\t->\t%s ret: %i\t<-\n", __FUNCTION__, rr);
	if (rr < 0)
	{
		perror("read");
		exit(1);
	}
	return ;
}
