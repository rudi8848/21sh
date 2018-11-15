/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_signals.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:55:01 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:55:04 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

void	sig_tstp_handler(int signum)
{
	pid_t cur;

	if (signum == SIGTSTP)
	{
		cur = tcgetpgrp(g_shell_terminal);
		if (cur != g_shell_pgid)
		{
			kill(cur, SIGTSTP);
			tcsetpgrp(g_shell_terminal, g_shell_pgid);
			tcsetattr(STDOUT_FILENO, TCSAFLUSH, &g_saved);
		}
	}
	signal(SIGTSTP, sig_tstp_handler);
}

void	set_stopsignals(sig_t func)
{
	signal(SIGINT, func);
	signal(SIGQUIT, func);
	signal(SIGTSTP, sig_tstp_handler);
	signal(SIGTERM, func);
	signal(SIGTTIN, func);
	signal(SIGTTOU, func);
	signal(SIGCHLD, SIG_DFL);
}
