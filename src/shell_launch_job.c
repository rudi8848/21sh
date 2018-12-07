/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_launch_job.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:47:13 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 12:47:15 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	set_outfile(t_job *j, t_launch *launch, int *mypipe)
{
	if (j->out_fd == -1 && (j->out_fd = open(j->dstfile,
					j->flags, FILE_PERM)) == -1)
	{
		perror("21sh: open");
		return (ERROR);
	}
	if (launch->p->next)
	{
		if (pipe(mypipe) < 0)
		{
			perror("21sh: pipe");
			ft_exit();
		}
		launch->outfile = mypipe[1];
	}
	else
		launch->outfile = j->out_fd;
	return (NORM);
}

static void	switch_files(t_job *j, t_launch *launch, int *mypipe)
{
	if (launch->infile != j->in_fd)
		close(launch->infile);
	if (launch->outfile != j->out_fd)
		close(launch->outfile);
	launch->infile = mypipe[0];
	launch->p = launch->p->next;
}

static int	check_files(t_job *j, t_launch *launch)
{
	if (j->in_fd == -1 && (j->in_fd = open(j->srcfile, O_RDONLY)) == -1)
	{
		perror("21sh: open");
		return (ERROR);
	}
	if (j->err_fd == -1 && (j->err_fd = open(j->errfile,
					j->flags, FILE_PERM)) == -1)
	{
		perror("21sh: open");
		return (ERROR);
	}
	launch->infile = j->in_fd;
	launch->p = j->first_process;
	return (NORM);
}

static int	run_process(t_job *j, t_launch *launch, int *mypipe, int foreground)
{
	if (set_outfile(j, launch, mypipe) != NORM)
		return (RETURN);
	if ((launch->ret = check_built(launch->p->argv)) >= 0)
	{
		do_builtin(j, launch, mypipe);
		return (CONTINUE);
	}
	else
	{
		if (!ft_find(launch->p))
		{
			launch->p->state |= COMPLETED;
			return (RETURN);
		}
		make_child_process(j, launch, foreground);
	}
	return (NORM);
}

void		launch_job(t_job *j, int foreground)
{
	int			mypipe[2];
	t_launch	launch;
	int			ret;

	init_launch(&launch, mypipe);
	if (check_files(j, &launch) != NORM)
		return ;
	set_job_number(j);
	while (launch.p)
	{
		if ((ret = run_process(j, &launch, mypipe, foreground)) == RETURN)
			return ;
		else if (ret == CONTINUE)
			continue ;
		switch_files(j, &launch, mypipe);
	}
	check_fd(j);
	if (!g_shell_is_interactive)
		wait_for_job(j);
	else if (foreground)
		put_job_in_foreground(j, 0);
	else
		put_job_in_background(j, 0);
}
