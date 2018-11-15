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

#include "../includes/shell.h"

int			check_built(char *cmd)
{
	int		i;
	char	*built[BUILT];

	i = 0;
	built[B_ECHO] = "echo";
	built[B_CD] = "cd";
	built[B_SETENV] = "setenv";
	built[B_UNSETENV] = "unsetenv";
	built[B_ENV] = "env";
	built[B_HRDOC] = "heredoc";
	built[B_EXIT] = "exit";
	built[B_JOBS] = "jobs";
	built[B_BG] = "bg";
	built[B_FG] = "fg";
	built[B_KILLJ] = "killj";
	built[B_END] = NULL;
	while (built[i])
	{
		if (ft_strequ(cmd, built[i]))
			return (i);
		i++;
	}
	return (-1);
}

static void	ft_set_builtins(t_pfb *built_tab)
{
	built_tab[B_ECHO] = &ft_echo;
	built_tab[B_CD] = &ft_cd;
	built_tab[B_SETENV] = &ft_setenv;
	built_tab[B_UNSETENV] = &ft_unsetenv;
	built_tab[B_ENV] = &ft_env;
	built_tab[B_HRDOC] = &ft_heredoc;
	built_tab[B_EXIT] = &b_exit;
	built_tab[B_JOBS] = &ft_jobs;
	built_tab[B_BG] = &ft_bg;
	built_tab[B_FG] = &ft_fg;
	built_tab[B_KILLJ] = &ft_killj;
	built_tab[B_END] = NULL;
}

void		ft_built_exe(char **args, t_built cmd, int infile, int outfile)
{
	static t_pfb	*built_tab = NULL;
	t_pfb			ft_run;

	ft_run = NULL;
	if (!built_tab)
	{
		built_tab = (t_pfb*)ft_memalloc(sizeof(t_pfb) * BUILT);
		if (!built_tab)
		{
			ft_printf("Error\n");
			return ;
		}
		ft_set_builtins(built_tab);
	}
	ft_run = built_tab[cmd];
	ft_run(args, infile, outfile);
}
