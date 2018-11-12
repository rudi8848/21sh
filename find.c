/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:38:58 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:39:46 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	check_access(t_process *cmd, char *tmp, char **p_arr)
{
	if (access(tmp, X_OK) != NORM)
	{
		ft_putstr_fd(tmp, STDERR_FILENO);
		ft_putstr_fd(": permission denied\n", STDERR_FILENO);
		free_arr(p_arr);
		free(tmp);
		return (0);
	}
	free(cmd->argv[0]);
	cmd->argv[0] = ft_strdup(tmp);
	free_arr(p_arr);
	free(tmp);
	return (1);
}

static int	ft_path_fitting(t_process *cmd, char **p_arr)
{
	char	*tmp;
	char	*tmp1;
	int		i;

	i = 0;
	while (p_arr[i] != NULL)
	{
		tmp1 = ft_strjoin(p_arr[i], "/");
		tmp = ft_strjoin(tmp1, cmd->argv[0]);
		free(tmp1);
		if (access(tmp, F_OK) == NORM)
			return (check_access(cmd, tmp, p_arr));
		if (tmp)
			free(tmp);
		++i;
	}
	ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
	free_arr(p_arr);
	return (0);
}

int			ft_find(t_process *commands)
{
	char	**path_arr;
	char	*env_path;

	if (!commands->argv[0])
		return (0);
	if (access(commands->argv[0], F_OK) != NORM)
	{
		if (!(env_path = get_copy_env("PATH", NORM)))
			return (0);
		path_arr = ft_strsplit(env_path, ':');
		if (!path_arr)
			return (0);
		return (ft_path_fitting(commands, path_arr));
	}
	else if (access(commands->argv[0], X_OK) != NORM)
	{
		ft_putstr_fd(commands->argv[0], STDERR_FILENO);
		ft_putstr_fd(": permission denied\n", STDERR_FILENO);
		return (0);
	}
	return (1);
}
