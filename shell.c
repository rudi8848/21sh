#include "21sh.h"

int		main(void)
{
	t_job *first_job;


	if ((first_job = (t_job*)ft_memalloc(sizeof(t_job))) == NULL)
	{
		perror("ft_memmaloc");
		return 1;
	}	
	else
		ft_printf("OK\n");
	free(first_job);
	return 0;
}