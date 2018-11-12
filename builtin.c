/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:17:07 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:18:08 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		ft_echo(char **argv, int infile, int outfile)
{
	int			i;
	int			opt;

	opt = 0;
	i = 1;
	if (infile < -1)
		ft_printf("Wrong input file\n");
	while (argv[i])
	{
		if (ft_strequ(argv[i], "-n"))
		{
			opt = 1;
			++i;
			continue;
		}
		ft_putstr_fd(argv[i], outfile);
		if (argv[i + 1])
			ft_putchar_fd(' ', outfile);
		++i;
	}
	if (!opt)
		ft_putchar_fd('\n', outfile);
	return (0);
}

int		b_exit(char **args, int infile, int outfile)
{
	if (infile < -1 || outfile < -1 || !args)
		ft_printf("Wrong input or output file\n");
	ft_exit();
	return (0);
}
