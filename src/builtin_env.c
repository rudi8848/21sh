/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:26:38 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:18:59 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	ft_move_env(int i, int find)
{
	if (find)
	{
		while (g_envp[i] != NULL)
		{
			if (g_envp[i] != NULL)
				free(g_envp[i]);
			if (!g_envp[i + 1])
				g_envp[i] = NULL;
			else
				g_envp[i] = ft_strdup(g_envp[i + 1]);
			++i;
		}
	}
}

int			ft_unsetenv(char **args, int infile, int outfile)
{
	int		i;
	int		len;
	int		find;

	find = 0;
	if (infile < -1 || outfile < -1)
		ft_printf("Wrong input or output file\n");
	if (args[1])
	{
		len = ft_strlen(args[1]);
		i = 0;
		while (g_envp[i] != NULL)
		{
			if (ft_strnequ(args[1], g_envp[i], len)
				&& (g_envp[i])[len] == '=')
			{
				find = 1;
				break ;
			}
			i++;
		}
		ft_move_env(i, find);
	}
	return (0);
}

static void	ft_print_env(int outfile)
{
	int i;

	i = 0;
	while (g_envp[i] != NULL)
	{
		ft_putstr_fd(g_envp[i], outfile);
		ft_putstr_fd("\n", outfile);
		i++;
	}
}

int			ft_env(char **args, int infile, int outfile)
{
	if (args || infile >= -1)
		ft_print_env(outfile);
	return (0);
}
