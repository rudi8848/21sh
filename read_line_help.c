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

void	get_curpos(t_cpos *pos)
{
	char	buf[21];
	int		ret;

	ret = 0;
	memset(buf, 0, 21);
	write(0, "\033[6n", 4);
	if ((ret = read(0, buf, 20)) < 0)
		ft_printf("Cannot get current position\n");
	pos->curx = atoi(strrchr(buf, ';') + 1);
	pos->cury = atoi(&buf[2]) - 1;
}

void	reset_selection(t_cpos *pos, char *line)
{
	t_cpos tmp;

	tmp = *pos;
	tputs(tgetstr("me", NULL), 0, ft_iputchar);
	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	move_to_border(K_HOME, line, &tmp);
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line, pos->len);
	pos->selection = 0;
	pos->first = -1;
	pos->last = -1;
	tputs(tgetstr("rc", NULL), 0, ft_iputchar);
	if (pos->autocompl)
		clear_compl(&pos->autocompl);
	if (pos->bgn)
	{
		free(pos->bgn);
		pos->bgn = NULL;
	}
	pos->autostart = 0;
	pos->autolen = 0;
	pos->is_auto = 0;
}

void	init_position(t_cpos *pos, int start, char *line)
{
	get_curpos(pos);
	pos->startline = start;
	pos->width = ft_get_width();
	pos->curln = 0;
	pos->height = 1;
	pos->len = 0;
	pos->i = 0;
	pos->autocompl = NULL;
	pos->bgn = NULL;
	reset_selection(pos, line);
	if (start)
		pos->start = pos->curx;
}

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
}
