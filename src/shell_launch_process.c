/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_launch_process.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:52:13 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:15:10 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	close_process_files(t_launch *launch, int errfile)
{
	if (launch->infile != STDIN_FILENO)
	{
		dup2(launch->infile, STDIN_FILENO);
		close(launch->infile);
	}
	if (launch->outfile != STDOUT_FILENO)
	{
		dup2(launch->outfile, STDOUT_FILENO);
		close(launch->outfile);
	}
	if (errfile != STDERR_FILENO)
	{
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}
}

void		launch_process(t_launch *launch, pid_t pgid, int errfile, int fg)
{
	pid_t	pid;

	if (g_shell_is_interactive)
	{
		pid = getpid();
		if (pgid == 0)
			pgid = pid;
		setpgid(pid, pgid);
		if (fg)
			tcsetpgrp(g_shell_terminal, pgid);
		set_stopsignals(SIG_DFL);
		signal(SIGTSTP, sig_tstp_handler);
	}
	close_process_files(launch, errfile);
	if (execve(launch->p->argv[0], launch->p->argv, g_envp) < 0)
		perror("execve");
	exit(EXIT_FAILURE);
}
