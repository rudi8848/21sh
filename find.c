#include "21sh.h"

int			ft_path_fitting(t_process *cmd, char **p_arr)
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
		if (access(tmp, F_OK) == OK)
		{	
			if (access(tmp, X_OK) != OK)
			{
				ft_printf("%s: permission denied\n", tmp);
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
		if (tmp)
			free(tmp);
		i++;
	}
	ft_printf("Command %s: not found\n", cmd->argv[0]);
	free_arr(p_arr);
	return (0);
}

int			ft_find(t_process *commands)
{
	char	**path_arr;
	char	*env_path;

	if (!commands->argv[0])
		return (0);
	if (access(commands->argv[0], F_OK) != OK)
	{
		if (!(env_path = get_copy_env("PATH", OK)))
			return (0);
		path_arr = ft_strsplit(env_path, ':');
		if (!path_arr)
			return (0);
		return (ft_path_fitting(commands, path_arr));
	}
	else if (access(commands->argv[0], X_OK) != OK)
	{
		ft_printf("%s: permission denied\n", commands->argv[0]);
		return (0);
	}
	return (1);
}