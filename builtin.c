#include "21sh.h"

int		check_built(char *cmd)
{
	int		i;
	char	*built[BUILT];

	i = 0;
	built[B_ECHO] = "echo";
	built[B_CD] = "cd";
	built[B_SETENV] = "setenv";
	built[B_UNSETENV] = "unsetenv";
	built[B_ENV] = "env";
	built[B_HRDOC] = "heredoc";
	built[B_EXIT] = "exit";
	built[B_JOBS] = "jobs";
	built[B_BG] = "bg";
	built[B_FG] = "fg";
	built[B_KILLJ] = "killj";
	built[B_END] = NULL;
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
	built_tab[B_ECHO] = &ft_echo;
	built_tab[B_CD] = &ft_cd;
	built_tab[B_SETENV] = &ft_setenv;
	built_tab[B_UNSETENV] = &ft_unsetenv;
	built_tab[B_ENV] = &ft_env;
	built_tab[B_HRDOC] = &ft_heredoc;
	built_tab[B_EXIT] = &b_exit;
	built_tab[B_JOBS] = &ft_jobs;
	built_tab[B_BG] = &ft_bg;
	built_tab[B_FG] = &ft_fg;
	built_tab[B_KILLJ] = &ft_killj;
	built_tab[B_END] = NULL;
}
void	ft_built_exe(char **args, t_built cmd, int infile, int outfile)
{
	//ft_printf("> %s: in: %d, out: %d\n", __FUNCTION__, infile, outfile);
	static t_pfb	*built_tab = NULL;
	t_pfb			ft_run;

	ft_run = NULL;
	if (!built_tab)
	{
		built_tab = (t_pfb*)ft_memalloc(sizeof(t_pfb) * BUILT);
		if (!built_tab)
		{
			ft_printf("Error\n");
			return ;
		}
		ft_set_builtins(built_tab);
	}
	ft_run = built_tab[cmd];
	ft_run(args, infile, outfile);
}

int		ft_echo(char **argv, int infile, int outfile)
{
	int			i;
	int			opt;

	opt = 0;
	i = 1;
	while (argv[i])
	{
		if (ft_strequ(argv[i], "-n"))
		{
			opt = 1;
			++i;
			continue;
		}
		ft_putstr_fd(argv[i], outfile);
		if (argv[i + 1])
			ft_putchar_fd(' ', outfile);
		++i;
	}
	if (!opt)
		ft_putchar_fd('\n', outfile);
	return (0);
}

int		ft_cd(char **args, int infile, int outfile)
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

int		ft_setenv(char **args, int infile, int outfile)
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

int		ft_unsetenv(char **args, int infile, int outfile)
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

int		ft_env(char **args, int infile, int outfile)
{
	ft_print_env(args, infile, outfile);
	return (0);
}

int		ft_bg(char **args, int infile, int outfile)
{

	t_job *j;
	if (args[1] && args[1][0] == '%')
	{
		int nbr = ft_atoi(&args[1][1]);
		j = first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break;
			j = j->next;
		}
		if (j)
		{
			continue_job(j, 0);
			set_stopsignals(SIG_DFL);
		}
	}
	return 0;
}


int		ft_fg(char **args, int infile, int outfile)
{
	
	t_job *j;
	if (args[1] && args[1][0] == '%')
	{
		int nbr = ft_atoi(&args[1][1]);
		j = first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break;
			j = j->next;
		}
		if (j)
		{
			continue_job(j, 1);
			set_stopsignals(SIG_DFL);
		}
	}
	
	return 0;
}

int		ft_killj(char **args, int infile, int outfile)
{
	
	t_job *j;
	if (args[1] && args[1][0] == '%')
	{
		int nbr = ft_atoi(&args[1][1]);
		j = first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break;
			j = j->next;
		}
		if (j)
			kill(-j->pgid, SIGKILL);
	}
	return 0;
}

int		b_exit(char **args, int infile, int outfile)
{
	ft_exit();
	return 0;
}