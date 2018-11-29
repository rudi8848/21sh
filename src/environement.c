/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environement.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:19:09 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:19:36 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char		*get_copy_env(char *needle, int warning)
{
	char	*res;
	size_t	len;
	int		i;

	res = NULL;
	i = 0;
	len = ft_strlen(needle);
	while (g_envp[i] != NULL)
	{
		if (ft_strnequ(needle, g_envp[i], len) && g_envp[i][len] == '=')
		{
			res = g_envp[i] + (len + 1);
			return (res);
		}
		++i;
	}
	if (!res)
	{
		if (!warning)
			ft_printf("21sh: Environement element %s not found\n", needle);
		return (NULL);
	}
	return (res);
}

static void	set_shlvl(void)
{
	int		shlvl;
	char	*nbr;
	char	*args[4];

	shlvl = (ft_atoi(get_copy_env("SHLVL", MUTE))) + 1;
	args[0] = "";
	args[1] = "SHLVL";
	nbr = ft_itoa(shlvl);
	args[2] = nbr;
	args[3] = NULL;
	ft_setenv(args, -1, -1);
	free(nbr);
}

void		copy_env(void)
{
	extern char		**environ;
	int				size;
	char			**copy;
	int				i;

	i = 0;
	size = env_size(environ);
	copy = (char**)ft_memalloc(sizeof(char*) * (size + 1));
	if (!copy)
	{
		ft_printf("21sh: Out of memory\n");
		exit(0);
	}
	while (environ[i])
	{
		copy[i] = ft_strdup(environ[i]);
		++i;
	}
	copy[i] = NULL;
	g_envp = copy;
	set_shlvl();
}
