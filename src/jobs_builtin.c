/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:43:34 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:47:04 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

int			ft_bg(char **args, int infile, int outfile)
{
	t_job	*j;
	int		nbr;

	if (infile < -1 || outfile < -1)
		ft_printf("Wrong input or output file\n");
	if (args[1] && args[1][0] == '%')
	{
		nbr = ft_atoi(&args[1][1]);
		j = g_first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break ;
			j = j->next;
		}
		if (j)
		{
			continue_job(j, 0);
			set_stopsignals(SIG_DFL);
		}
	}
	return (0);
}

int			ft_fg(char **args, int infile, int outfile)
{
	t_job	*j;
	int		nbr;

	if (infile < -1 || outfile < -1)
		ft_printf("Wrong input or output file\n");
	if (args[1] && args[1][0] == '%')
	{
		nbr = ft_atoi(&args[1][1]);
		j = g_first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break ;
			j = j->next;
		}
		if (j)
		{
			continue_job(j, 1);
			set_stopsignals(SIG_DFL);
		}
	}
	return (0);
}

int			ft_killj(char **args, int infile, int outfile)
{
	t_job	*j;
	int		nbr;

	if (infile < -1 || outfile < -1)
		ft_printf("Wrong input or output file\n");
	if (args[1] && args[1][0] == '%')
	{
		nbr = ft_atoi(&args[1][1]);
		j = g_first_job;
		while (j)
		{
			if (nbr == j->nbr)
				break ;
			j = j->next;
		}
		if (j)
			kill(-j->pgid, SIGKILL);
	}
	return (0);
}

static void	print_processes(t_job *j, int fd)
{
	int			i;
	t_process	*p;

	p = j->first_process;
	while (p)
	{
		i = 0;
		while (p->argv[i])
		{
			ft_putchar_fd(' ', fd);
			ft_putstr_fd(p->argv[i], fd);
			ft_putchar_fd(' ', fd);
			i++;
		}
		p = p->next;
	}
}

int			ft_jobs(char **argv, int infile, int outfile)
{
	t_job	*j;
	char	*status_str;

	j = g_first_job;
	if (argv || infile >= -1)
		while (j)
		{
			if (job_is_completed(j))
			{
				j = j->next;
				continue;
			}
			else if (job_is_stopped(j))
				status_str = "stopped";
			else
				status_str = "running";
			ft_putchar_fd('[', outfile);
			ft_putnbr_fd(j->nbr, outfile);
			ft_putstr_fd("] ", outfile);
			ft_putstr_fd(status_str, outfile);
			print_processes(j, outfile);
			ft_putchar_fd('\n', outfile);
			j = j->next;
		}
	return (0);
}
