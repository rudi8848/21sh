#include "21sh.h"

int		ft_heredoc(char **argv, int infile, int outfile)
{
	char line[MAXLINE];
	int res;

	ft_bzero(line, MAXLINE);
	res = 0;
	if (infile < -1)
		ft_printf("Wrong input file");
	while(!res)
	{
		read_more(line, 1, "heredoc> ");
		res = ft_strequ(line, argv[1]);
		if (!res)
			ft_putstr_fd(line, outfile);
	}
	return 0;
}