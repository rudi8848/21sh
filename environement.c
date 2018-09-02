#include "21sh.h"

char	*get_copy_env(char *needle, int warning)
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
		i++;
	}
	if (!res)
	{
		if (!warning)
			ft_printf("Environement element %s not found\n", needle);
		return (NULL);
	}
	return (res);
}

int		env_size(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
		i++;
	return (i);
}

char	*get_current_wd(void)
{
	char	buf[2048 + 1];
	char	*dest;

	dest = getcwd(buf, 2048);
	return (dest);
}

char	*ft_path_substitute(char *path)
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

void	ft_move_env(int i, int find)
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
			i++;
		}
	}
}

void	copy_env(void)
{
	extern char		**environ;
	int				size;
	char			**copy;
	int				i;
	int				shlvl;

	i = 0;
	size = env_size(environ);
	copy = (char**)ft_memalloc(sizeof(char*) * size + 1);
	if (!copy)
	{
		ft_printf("Error: cannot copy environement\n");
		exit(0);
	}
	while (environ[i])
	{
		copy[i] = ft_strdup(environ[i]);
		i++;
	}
	copy[i] = NULL;
	g_envp = copy;
	shlvl = (ft_atoi(get_copy_env("SHLVL", MUTE))) + 1;
	
	char *args[4];
	args[0] = "";
	args[1] = "SHLVL";
	args[2] = ft_itoa(shlvl);
	args[3] = NULL;
	ft_setenv(args, -1, -1);
}