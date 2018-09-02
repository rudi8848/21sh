#include "21sh.h"

int		ft_heredoc(char **argv, int infile, int outfile)
{
	char line[MAXLINE];

	ft_bzero(line, MAXLINE);
	read_more(line, 1, "heredoc> ");
	ft_putstr_fd(line, outfile);
	//ft_bzero(line, MAXLINE);
	while(ft_strcmp(line, argv[1]))
	{

		read_more(line, 1, "heredoc> ");
		ft_putstr_fd(line, outfile);
		ft_bzero(line, MAXLINE);
	}
	return 0;
}