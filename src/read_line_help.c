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

void	reset_selection(t_cpos *pos, char *line)
{
	t_cpos tmp;

	tmp = *pos;
	if (pos->selection)
	{
		tputs(tgetstr("sc", NULL), 0, ft_iputchar);
		tputs(tgetstr("me", NULL), 0, ft_iputchar);
		move_to_border(K_HOME, line, &tmp);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		while (tmp.i < pos->len)
		{
			write(STDOUT_FILENO, &line[tmp.i], 1);
			++tmp.i;
		}
		tputs(tgetstr("rc", NULL), 0, ft_iputchar);
		pos->selection = 0;
		pos->first = -1;
		pos->last = -1;
	}
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

void	init_position(t_cpos *pos, int start, char *line, char *eol)
{
	pos->cury = 0;
	pos->startline = start;
	pos->width = ft_get_width();
	pos->curx = pos->prompt_len % pos->width;
	pos->len = 0;
	pos->i = 0;
	pos->is_auto = 0;
	pos->autocompl = NULL;
	pos->bgn = NULL;
	pos->eol = eol;
	reset_selection(pos, line);
}
