/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:40:02 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:40:04 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	get_line(char *line, int infile)
{
	char *str;

	if (g_shell_is_interactive)
	{
		if (g_hstr_fd == -1)
			g_hstr_fd = open(g_history_file, O_RDWR | O_CREAT |
				O_APPEND, S_IRWXU);
		cbreak_settings();
		dprintf(4, ">\tgetting new line\n");
		read_line(&line[0], 0, NULL);
		dprintf(4, ">\tgot line: %s\n", line);
		ft_restore();
	}
	else
	{
		ft_bzero(line, MAXLINE);
		str = NULL;
		if (get_next_line(infile, &str) > 0)
		{
			ft_strcpy(line, str);
			line[ft_strlen(str)] = '\n';
			free(str);
		}
		else
			exit(close(infile));
	}
}
/*
static void	restore_fd(void)
{
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	if (dup2(g_cin, STDIN_FILENO) < 0
		|| dup2(g_cout, STDOUT_FILENO) < 0
		|| dup2(g_cerr, STDERR_FILENO) < 0)
		perror("21sh: restore file descriptors");
}
*/
static void	run_comand(char *line)
{
	t_job *ptr;

	if (g_shell_is_interactive)
	{
		set_stopsignals(SIG_IGN);
		append_history(line);
	}
	ptr = g_first_job;
	while (ptr)
	{
		if (!ptr->foreground && ptr->nbr)
			do_job_notification();
		else if (!ptr->nbr)
			launch_job(ptr, ptr->foreground);
		//restore_fd();
		ptr = ptr->next;
	}
	do_job_notification();
	if (g_shell_is_interactive)
	{
		close(g_hstr_fd);
		g_hstr_fd = -1;
	}
}

static void	check_args(int argc, char **argv, int *infile)
{
	if (argc == 1)
		init_shell();
	else if (argc == 2)
	{
		g_shell_is_interactive = 0;
		*infile = open(argv[1], O_RDONLY);
		if (*infile < 0)
		{
			perror("21sh: Cannot open input file");
			exit(EXIT_FAILURE);
		}
		copy_env();
	}
	else
	{
		ft_putstr_fd("21sh: Unexpected argument\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

int			main(int argc, char **argv)
{
	char	line[MAXLINE];
	t_job	*j;
	int		infile;
int ret;
	g_envp = NULL;
	g_hstr_fd = -1;
	check_args(argc, argv, &infile);
	g_first_job = NULL;
	if (g_shell_is_interactive)
		init_history();
	while (1)
	{
		j = (t_job*)ft_memalloc(sizeof(t_job));
		if (!j)
		{
			perror("ft_memalloc");
			return (1);
		}
		dprintf(4, ">\tstart\n");
		get_line(line, infile);
		dprintf(4, ">\tgot line: %s\n", line);
		if (is_empty(line))
		{
			dprintf(4, ">\tline is empty\n");
			free(j);
		}
		else if ((ret = pack_args(line, j)))
		{
			dprintf(4, ">\targs packed\n");
			run_comand(line);
		}
		else if (!ret)
		{
			dprintf(4, ">\targs NOT packed\n");
		}
	}
	return (0);
}
