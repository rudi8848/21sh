#include "21sh.h"

void	print_processes(t_job *j/*, int fd*/)
{
	int i;
	t_process *p;

	p = j->first_process;
		while (p)
		{
			i = 0;
			while (p->argv[i])
			{
				ft_printf(" %s ", p->argv[i]/*, j->argv[i + 1] ? ' ' : '\n'*/);
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
		ft_printf("[%d] %s", j->nbr, status_str);
		print_processes(j);
		ft_printf("\n");
		j = j->next;
	}
	return 0;
}