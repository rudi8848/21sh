/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:37:05 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:21:42 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	ft_change_env(char *new, char *old)
{
	char	*arr[4];

	new = ft_strdup(get_current_wd());
	arr[0] = "setenv";
	arr[1] = "PWD";
	arr[2] = new;
	arr[3] = NULL;
	if (get_copy_env("PWD", MUTE))
		ft_unsetenv(arr, -1, -1);
	ft_setenv(arr, -1, -1);
	arr[1] = "OLDPWD";
	arr[2] = old;
	if (get_copy_env("OLDPWD", MUTE))
		ft_unsetenv(arr, -1, -1);
	ft_setenv(arr, -1, -1);
	free(new);
}

static char	*ft_path_substitute(char *path)
{
	char	*dest;
	char	*tmp;

	dest = NULL;
	tmp = NULL;
	if (!path || path[0] == '~' || ft_strequ(path, "--"))
	{
		if (get_copy_env("HOME", OK))
			tmp = ft_strdup(get_copy_env("HOME", MUTE));
		if (!path || !path[1] || ft_strequ(path, "--"))
			return (tmp);
		dest = ft_strjoin(tmp, path + 1);
		free(tmp);
	}
	return (dest);
}

static char	*ft_set_new_path(char **args)
{
	char	*new;

	if (!args[1] || args[1][0] == '~' || ft_strequ(args[1], "--"))
		new = ft_path_substitute(args[1]);
	else if (args[1][0] == '-')
	{
		if (!get_copy_env("OLDPWD", OK))
			return (NULL);
		new = ft_strdup(get_copy_env("OLDPWD", MUTE));
	}
	else
		new = ft_strdup(args[1]);
	return (new);
}

int			ft_cd(char **args, int infile, int outfile)
{
	int		ret;
	char	*new;
	char	*old;

	if (infile < -1 || outfile < -1)
		ft_printf("Wrong input or output file\n");
	if (ft_strequ(".", args[1]))
		return (0);
	new = ft_set_new_path(args);
	old = ft_strdup(get_current_wd());
	ret = chdir(new);
	free(new);
	if (ret == OK)
		ft_change_env(new, old);
	else
		ft_printf("Cannot change directory\n");
	free(old);
	return (ret);
}
