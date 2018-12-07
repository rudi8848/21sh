/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:41:18 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 14:44:43 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	replace_str(char *line)
{
	char	word[MAXWORD];
	int		i;
	size_t	wordn;
	char	tail[MAXWORD];

	wordn = 0;
	i = 0;
	ft_bzero(word, sizeof(word));
	ft_bzero(tail, sizeof(tail));
	insert_variable(line, word, &i, &wordn);
	ft_strcpy(tail, &line[i + 1]);
	ft_bzero(line, ft_strlen(line));
	ft_strcpy(line, word);
	ft_strcpy(&line[wordn], tail);
}

int			ft_heredoc(char **argv, int infile, int outfile)
{
	char	line[MAXLINE];
	int		res;
	char	*ptr;

	ft_bzero(line, MAXLINE);
	res = 0;
	if (infile < -1)
		ft_printf("Wrong input file");
	while (!res)
	{
		read_more(line, 1, "heredoc> ", argv[1]);
		if ((ptr = ft_strchr(line, '$')))
			replace_str(ptr);
		res = ft_strequ(line, argv[1]);
		if (!res)
			ft_putstr_fd(line, outfile);
	}
	return (0);
}
