#include "21sh.h"
/*
int		check_built(char *cmd)
{
	int		i;
	char	*built[BUILT];

	i = 0;
	built[ECHO] = "echo";
	built[CD] = "cd";
	built[SETENV] = "setenv";
	built[UNSETENV] = "unsetenv";
	built[ENV] = "env";
	built[EXIT] = "exit";
	built[END] = NULL;
	while (built[i])
	{
		if (ft_strequ(cmd, built[i]))
			return (i);
		i++;
	}
	return (-1);
}

void	ft_set_builtins(t_pfb *built_tab)
{
	built_tab[ECHO] = &ft_echo;
	built_tab[CD] = &ft_cd;
	built_tab[SETENV] = &ft_setenv;
	built_tab[UNSETENV] = &ft_unsetenv;
	built_tab[ENV] = &ft_env;
	built_tab[EXIT] = &ft_exit;
	built_tab[END] = NULL;
}
*/

int		ft_echo(t_process *p)
{
	int			i;

	i = 1;
	while (p->argv[i])
	{
		ft_putstr_fd(p->argv[i], p->out_fd);
		if (p->argv[i + 1])
			ft_putchar_fd(' ', p->out_fd);
		i++;
	}
	ft_putchar_fd('\n', p->out_fd);
	return (0);
}

int		ft_cd(char **args)
{
	int		ret;
	int		i;
	char	*new;
	char	*ptr;
	char	*old;

	i = 0;
	ptr = NULL;
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

int		ft_setenv(char **args)
{
	int		size;
	int		i;
	char	*str;
	int		len;

	i = 0;
	if ((str = ft_check_args(args)))
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

int		ft_unsetenv(char **args)
{
	int		i;
	int		len;
	int		find;

	find = 0;
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
/*
int		ft_env(char **args)
{
	char		**envp_cp;
	char		**ptr;
	t_process	*cmd;

	cmd = NULL;
	if (ft_strequ(args[1], "-i"))
	{
		envp_cp = (char **)ft_memalloc(sizeof(char*) * 2);
		envp_cp[0] = ft_strjoin("PATH=", get_copy_env("PATH", MUTE));
		envp_cp[1] = NULL;
		ptr = ft_cp_array(args + 2);
		//push(&cmd, ptr);
		if (ptr)
			//executor(cmd, &envp_cp);
		free(envp_cp[0]);
		free(envp_cp);
	}
	else
		ft_print_env(args);
	return (0);
}*/