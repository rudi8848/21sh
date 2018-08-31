#include "21sh.h"

char	*ft_set_new_path(char **args)
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

char	**ft_cp_array(char **src)
{
	char	**dest;
	int		i;
	int		n;

	i = 0;
	n = 0;
	while (src[n] != NULL)
		n++;
	dest = (char **)ft_memalloc(sizeof(char *) * (n + 1));
	if (!dest)
		return (NULL);
	while (i < n)
	{
		dest[i] = ft_strdup(src[i]);
		i++;
	}
	dest[n] = NULL;
	return (dest);
}

int		ft_print_env(char **args, int infile, int outfile)
{
	int i;

	i = 0;
	while (g_envp[i] != NULL)
	{
		ft_putstr_fd(g_envp[i], outfile);
		ft_putstr_fd("\n", outfile);
		i++;
	}
	return (0);
}

int		ft_check_dir(char *name)
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

void		free_arr(char **array)
{
	int	i;

	i = 0;
	while (array[i] != NULL)
	{
		free(array[i]);
		i++;
	}
	free(array);
	array = NULL;
}