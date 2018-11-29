/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:32:52 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:43:00 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

const t_bset g_built_tab[BUILT] = {
	{ "echo", &ft_echo },
	{ "cd", &ft_cd },
	{ "setenv", &ft_setenv },
	{ "unsetenv", &ft_unsetenv },
	{ "env", &ft_env },
	{ "heredoc", &ft_heredoc },
	{ "exit", &b_exit },
	{ "jobs", &ft_jobs },
	{ "bg", &ft_bg },
	{ "fg", &ft_fg },
	{ "killj", &ft_killj },
	{ NULL, NULL }
};

int			check_built(char *cmd)
{
	int i;

	i = 0;
	while (g_built_tab[i].command)
	{
		if (ft_strequ(cmd, g_built_tab[i].command))
			return (i);
		++i;
	}
	return (-1);
}

void		ft_built_exe(char **args, t_built cmd, int infile, int outfile)
{
	t_pfb			ft_run;

	ft_run = g_built_tab[cmd].function;
	ft_run(args, infile, outfile);
}
