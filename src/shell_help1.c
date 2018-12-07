/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_help1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/07 15:21:31 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 15:21:33 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		cbreak_settings(void)
{
	struct termios changed;

	changed = g_saved;
	changed.c_lflag &= ~(ICANON | ECHO | ISIG);
	changed.c_cc[VMIN] = 1;
	changed.c_cc[VTIME] = 0;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed) == -1)
	{
		ft_putstr_fd("21sh: Cannot set terminal attributes\n", STDERR_FILENO);
		ft_exit();
	}
	return (0);
}

int		is_empty(char *line)
{
	size_t i;

	i = 0;
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\n' && line[i] != '\t')
			return (0);
		++i;
	}
	return (1);
}
