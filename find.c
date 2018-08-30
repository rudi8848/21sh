#include "21sh.h"

int			ft_path_fitting(t_process *cmd, char **p_arr, int f)
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
		f = access(tmp, X_OK);
		if (f == OK)
		{
			free(cmd->argv[0]);
			cmd->argv[0] = ft_strdup(tmp);
			free_arr(p_arr);
			free(tmp);
			return (1);
		}
		if (tmp)
			free(tmp);
		i++;
	}
	free_arr(p_arr);
	return (0);
}

int			ft_find(t_process *commands)
{
	char	**path_arr;
	int		find;
	char	*env_path;

	if (!commands->argv[0])
		return (0);
	if ((find = access(commands->argv[0], X_OK)) != OK)
	{
		env_path = get_copy_env("PATH", OK);
		if (!env_path)
			return (0);
		path_arr = ft_strsplit(env_path, ':');
		if (!path_arr)
			return (0);
		if (!ft_path_fitting(commands, path_arr, find))
		{
			ft_printf("Command %s: not found\n", commands->argv[0]);
			return (0);
		}
	}
	return (1);
}