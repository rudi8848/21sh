/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_help.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:27:40 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 11:30:45 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		ft_get_width(void)
{
	struct winsize	argp;
	int				ret;

	ret = ioctl(g_shell_terminal, TIOCGWINSZ, &argp);
	if (ret != 0)
	{
		ft_putstr_fd("Cannot get window size\n", STDERR_FILENO);
		return (-1);
	}
	return (argp.ws_col);
}
/*
void	get_curpos(t_cpos *pos)
{
	char	buf[21];
	int		ret;

	ret = 0;
	ft_memset(buf, 0, 21);
	write(STDERR_FILENO, "\033[6n", 4);
	if ((ret = read(STDERR_FILENO, buf, 20)) < 0)
	{
		ft_printf("Cannot get current position\n");
		pos->curx = 0;
		pos->cury = 0;
		return ;
	}
	pos->curx = ft_atoi(ft_strrchr(buf, ';') + 1);
	pos->cury = ft_atoi(&buf[2]) - 1;
}*/
void	reset_selection(t_cpos *pos, char *line)
{
	t_cpos tmp;

	tmp = *pos;
	if (!line)
		return;
	tputs(tgetstr("me", NULL), 0, ft_iputchar);
	//tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	move_to_border(K_HOME, line, &tmp);
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	while (pos->i < pos->len)
	{
		write(STDOUT_FILENO, &line[pos->i], 1);
		++pos->i;
	}
	//write(STDOUT_FILENO, line, pos->len);
	pos->selection = 0;
	pos->first = -1;
	pos->last = -1;
	//tputs(tgetstr("rc", NULL), 0, ft_iputchar);
	if (pos->is_auto)
	{
		if (pos->autocompl)
		{
			clear_compl(&pos->autocompl);
			pos->autocompl = NULL;
		}
		if (pos->bgn)
			ft_strdel(&pos->bgn);
		pos->autostart = 0;
		pos->autolen = 0;
		pos->is_auto = 0;
	}
}

void	init_position(t_cpos *pos, int start, char *line)
{
	//get_curpos(pos);
	pos->cury = 0;
	pos->startline = start;
	pos->width = ft_get_width();
	pos->curx = pos->prompt_len % pos->width;
	pos->curln = 0;
	pos->height = 1;
	pos->len = 0;
	pos->i = 0;
	pos->is_auto = 0;
	pos->autocompl = NULL;
	pos->bgn = NULL;
	reset_selection(pos, line);
	
//		pos->start = pos->prompt_len;
}
/*
int		cmd_height(t_cpos *pos, char *line)
{
	int len;
	int width;
	int height;

	height = 1;
	len = ft_strlen(line);
	width = ft_get_width();
	if (len <= width - pos->start)
		return (height);
	len -= width - pos->start;
	height += len / width;
	if (len % width)
		height++;
	return (height);
}*/
