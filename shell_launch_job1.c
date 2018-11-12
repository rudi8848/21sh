/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_launch_job1.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:50:30 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:50:32 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	check_fd(t_job *j)
{
	if (j->in_fd != STDIN_FILENO)
		close(j->in_fd);
	if (j->out_fd != STDOUT_FILENO)
		close(j->out_fd);
	if (j->err_fd > 2)
		close(j->err_fd);
}

void	set_job_number(t_job *j)
{
	t_job *ptr;

	j->nbr = 1;
	ptr = g_first_job;
	while (ptr)
	{
		if (j != ptr && j->nbr >= ptr->nbr)
			j->nbr++;
		ptr = ptr->next;
	}
}

void	do_builtin(t_job *j, t_launch *launch, int *mypipe)
{
	launch->p->state |= COMPLETED;
	ft_built_exe(launch->p->argv, launch->ret, launch->infile, launch->outfile);
	if (launch->infile != j->in_fd)
		close(launch->infile);
	if (launch->outfile != j->out_fd)
		close(launch->outfile);
	launch->infile = mypipe[0];
	launch->p = launch->p->next;
}

void	make_child_process(t_job *j, t_launch *launch, int foreground)
{
	launch->pid = fork();
	if (launch->pid == CHILD)
		launch_process(launch, j->pgid, j->err_fd, foreground);
	else if (launch->pid == ERROR)
	{
		perror("fork");
		ft_exit();
	}
	else
	{
		launch->p->pid = launch->pid;
		if (g_shell_is_interactive)
		{
			if (!j->pgid)
				j->pgid = launch->pid;
			setpgid(launch->pid, j->pgid);
		}
	}
}
