/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:41:18 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:41:54 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		ft_heredoc(char **argv, int infile, int outfile)
{
	char	line[MAXLINE];
	int		res;
	char	word[MAXWORD];
	int 	i;
	t_token	token;

	ft_bzero(line, MAXLINE);
	res = 0;
	if (infile < -1)
		ft_printf("Wrong input file");
	while (!res)
	{
		read_more(line, 1, "heredoc> ", argv[1]);
		i = 0;
		ft_bzero(word, sizeof(word));
		token = T_WORD;
		while (token != T_NLINE && token != T_EOF && token != T_ERROR)
		{
			dprintf(4, ">\t %s, token: [%d]\n", &line[i], token);
			token = ft_gettoken(line, &i, word, sizeof(word));
		}
		res = ft_strequ(line, argv[1]);
		if (!res)
			ft_putstr_fd(line, outfile);
	}
	return (0);
}
