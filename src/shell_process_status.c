/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_job_status.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:58:19 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:58:21 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

static void	check_process_status(t_process *p, pid_t pid, int status)
{
	p->status = status;
	if (WIFSTOPPED(status))
		p->state |= STOPPED;
	else
	{
		p->state |= COMPLETED;
		if (WIFSIGNALED(status))
		{
			ft_putnbr_fd((int)pid, STDERR_FILENO);
			ft_putstr_fd(": Terminated by signal ", STDERR_FILENO);
			ft_putnbr_fd(WTERMSIG(p->status), STDERR_FILENO);
			ft_putchar_fd('\n', STDERR_FILENO);
		}
	}
}

static int	check_process(t_job **j, t_process *p, pid_t pid, int status)
{
	while (p)
	{
		if (p->pid == pid)
		{
			check_process_status(p, pid, status);
			return (NORM);
		}
		p = p->next;
	}
	*j = (*j)->next;
	return (ERROR);
}

int			mark_process_status(pid_t pid, int status)
{
	t_job		*j;
	t_process	*p;

	if (pid > 0)
	{
		j = g_first_job;
		while (j)
		{
			p = j->first_process;
			if (check_process(&j, p, pid, status) == NORM)
				return (0);
		}
		ft_putstr_fd("No clild process ", STDERR_FILENO);
		ft_putnbr_fd((int)pid, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (-1);
	}
	else if (pid == 0 || errno == ECHILD)
		return (-1);
	else
	{
		perror("waitpid");
		return (-1);
	}
}

void		update_status(void)
{
	int		status;
	pid_t	pid;
	int		res;

	status = 0;
	res = 0;
	while (res == 0)
	{
		pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
		res = mark_process_status(pid, status);
	}
}
