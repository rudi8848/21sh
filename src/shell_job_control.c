/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_job_control.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:00:49 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:14:08 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

int			job_is_stopped(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & (STOPPED)))
			return (0);
		p = p->next;
	}
	return (1);
}

int			job_is_completed(t_job *j)
{
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		if (!(p->state & COMPLETED))
			return (0);
		p = p->next;
	}
	return (1);
}

static void	notify_jobs(t_job **j, t_job **jlast, t_job **jnext)
{
	while (*j)
	{
		*jnext = (*j)->next;
		if (job_is_completed(*j))
		{
			if (*jlast)
				(*jlast)->next = *jnext;
			else
				g_first_job = *jnext;
			free_job(*j);
			*j = NULL;
		}
		else if (job_is_stopped(*j) && !(*j)->notified)
		{
			(*j)->notified = 1;
			*jlast = *j;
		}
		else
			*jlast = *j;
		*j = *jnext;
	}
}

void		do_job_notification(void)
{
	t_job		*j;
	t_job		*jlast;
	t_job		*jnext;

	update_status();
	jlast = NULL;
	j = g_first_job;
	notify_jobs(&j, &jlast, &jnext);
}
