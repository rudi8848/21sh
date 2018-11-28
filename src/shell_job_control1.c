/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_job_control1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:09:17 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:14:44 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void		put_job_in_foreground(t_job *j, int cont)
{
	tcsetpgrp(g_shell_terminal, j->pgid);
	if (cont)
	{
		tcsetattr(g_shell_terminal, TCSAFLUSH, &j->tmodes);
		if (kill(-j->pgid, SIGCONT) < 0)
			perror("kill(SIGCONT)");
	}
	wait_for_job(j);
	tcsetpgrp(g_shell_terminal, g_shell_pgid);
	tcgetattr(g_shell_terminal, &j->tmodes);
	tcsetattr(g_shell_terminal, TCSAFLUSH, &g_saved);
}

void		put_job_in_background(t_job *j, int cont)
{
	if (cont)
	{
		if (kill(-j->pgid, SIGCONT) < 0)
			perror("kill(SIGCONT)");
	}
}

void		wait_for_job(t_job *j)
{
	int		status;
	pid_t	pid;
	int		res;
	t_process	*last;

	last = j->first_process;
	while (last->next)
		last = last->next;

	if (last->state | COMPLETED && (!job_is_completed(j) || !job_is_stopped(j)))
	{
		t_process *tmp = j->first_process;
		while (tmp != last)
		{
			if (!(tmp->state | COMPLETED) || !(tmp->state | STOPPED))
			kill(tmp->pid, SIGTERM);
			tmp = tmp->next;
		}	
		pid = waitpid(last->pid, &status, WUNTRACED);
		mark_process_status(pid, status);
		return;
	}
	status = 0;
	res = 0;
	while (!job_is_stopped(j) && res == 0 && !job_is_completed(j))
	{
		pid = waitpid(-j->pgid, &status, WUNTRACED);
		res = mark_process_status(pid, status);
	}
}

static void	mark_job_as_running(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		p->state &= ~STOPPED;
		p = p->next;
	}
	j->notified = 0;
}

void		continue_job(t_job *j, int foreground)
{
	mark_job_as_running(j);
	if (foreground)
		put_job_in_foreground(j, 1);
	else
		put_job_in_background(j, 1);
}
