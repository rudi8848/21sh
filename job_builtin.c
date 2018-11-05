#include "21sh.h"

void	print_processes(t_job *j, int fd)
{
	int i;
	t_process *p;

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

int		ft_jobs(char **argv, int infile, int outfile)
{
	t_job *j;
	char *status_str;
	int i;
	t_process *p;

	j = first_job;
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