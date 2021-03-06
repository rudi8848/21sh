/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:43:36 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:43:38 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	init_launch(t_launch *launch, int *mypipe)
{
	launch->infile = -1;
	launch->outfile = -1;
	launch->pid = 0;
	launch->ret = -1;
	launch->p = NULL;
	mypipe[0] = -1;
	mypipe[1] = -1;
}

void	init_terminal(void)
{
	char	buf[MAXWORD];
	char	*name;

	if ((tcgetattr(STDOUT_FILENO, &g_saved)) == -1)
	{
		ft_putstr_fd("21sh: Cannot get terminal settings\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
	name = NULL;
	if (!(name = getenv("TERM")))
	{
		ft_printf("21sh: TERM is not set\n");
		exit(EXIT_FAILURE);
	}
	if (tgetent(buf, name) < 1)
	{
		ft_putstr_fd("21sh: Error at tgetent\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

void	append_history(char *line)
{
	check_history_capacity();
	ft_putstr_fd(line, g_hstr_fd);
	line[ft_strlen(line) - 1] = 0;
	g_history[g_hstr_nb] = ft_strdup(line);
	g_hstr_nb++;
}

void	init_history(void)
{
	int		i;
	char	*str;

	i = 0;
	str = NULL;
	g_history_file = ft_strjoin(get_current_wd(), "/.history");
	g_hstr_fd = open(g_history_file, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
	if (g_hstr_fd < 0)
	{
		ft_printf("21sh: Cannot open/create history file\n");
		return ;
	}
	while (get_next_line(g_hstr_fd, &str) > 0)
	{
		g_history[i] = ft_strdup(str);
		free(str);
		i++;
	}
	if (str)
		free(str);
	g_hstr_nb = i;
}

void	init_shell(void)
{
	g_shell_terminal = STDIN_FILENO;
	g_shell_is_interactive = isatty(g_shell_terminal);
	if (g_shell_is_interactive)
	{
		while (tcgetpgrp(g_shell_terminal) != (g_shell_pgid = getpgid(0)))
		{
			ft_putstr_fd("21sh: shell is not interactive\n", STDERR_FILENO);
			kill(-g_shell_pgid, SIGTTIN);
		}
		set_stopsignals(SIG_IGN);
		g_shell_pgid = getpid();
		if (setpgid(g_shell_pgid, g_shell_pgid) < 0)
		{
			perror("21sh: Couldn't put the shell in it's own group");
			exit(1);
		}
		tcsetpgrp(g_shell_terminal, g_shell_pgid);
		init_terminal();
	}
	/*
	if ((g_cin = dup2(STDIN_FILENO, 40)) < 0 ||
	(g_cout = dup2(STDOUT_FILENO, 41)) < 0 ||
	(g_cerr = dup2(STDERR_FILENO, 42)) < 0)
		perror("21sh: saving file descriptors");*/
	copy_env();
}
