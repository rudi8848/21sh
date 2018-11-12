/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_setenv.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:48:55 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:48:57 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int			env_size(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
		++i;
	return (i);
}

static int	ft_check_dir(char *name)
{
	int				ret;
	struct stat		buf;

	ret = lstat(name, &buf);
	if (ret >= 0)
	{
		if (S_ISDIR(buf.st_mode))
			return (1);
	}
	else
		ft_printf("cannot access %s\n", name);
	return (0);
}

static int	ft_env_rewrite(char *str, int size)
{
	char	**new_envp;
	int		i;

	i = 0;
	new_envp = (char**)ft_memalloc(sizeof(char*) * (size + 2));
	if (!new_envp)
		return (ft_printf("Cannot allocate memory\n"));
	while (i < size)
	{
		new_envp[i] = ft_strdup(g_envp[i]);
		i++;
	}
	new_envp[size] = ft_strdup(str);
	free(str);
	if (new_envp[size] == NULL)
		return (printf("Cannot set env\n"));
	new_envp[size + 1] = NULL;
	free_arr(g_envp);
	g_envp = new_envp;
	return (0);
}

static char	*ft_check_args(char **args, int infile, int outfile)
{
	char	*var;
	char	*str;

	if (!args[1] || (args[2] && args[3]) || infile < -1 || outfile < -1)
	{
		ft_printf("setenv: Wrong number of arguments\n");
		return (NULL);
	}
	if ((ft_strequ(args[1], "HOME") || ft_strequ(args[1], "PWD")
		|| ft_strequ(args[1], "OLDPWD")) && args[2])
	{
		if (!ft_check_dir(args[2]))
			return (NULL);
	}
	str = ft_strjoin(args[1], "=");
	if (args[2])
	{
		var = ft_strjoin(str, args[2]);
		free(str);
		str = ft_strdup(var);
		free(var);
	}
	return (str);
}

int			ft_setenv(char **args, int infile, int outfile)
{
	int		size;
	int		i;
	char	*str;
	int		len;

	i = 0;
	if ((str = ft_check_args(args, infile, outfile)))
	{
		size = env_size(g_envp);
		len = ft_strlen(args[1]);
		while (i < size)
		{
			if (ft_strnequ(args[1], g_envp[i], len)
				&& (g_envp[i])[len] == '=')
			{
				free(g_envp[i]);
				g_envp[i] = ft_strdup(str);
				free(str);
				return (0);
			}
			i++;
		}
		return (ft_env_rewrite(str, size));
	}
	return (0);
}
