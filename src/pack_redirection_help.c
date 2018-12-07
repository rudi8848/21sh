/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pack_redirection_help.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:40:46 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 14:45:21 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	close_output_nbr(t_job *j, t_process *p, t_pack *pack, int nbr)
{
	if (nbr == j->in_fd)
	{
		j->in_fd = -1;
		ft_strcpy(j->srcfile, "/dev/null");
		ft_bzero(j->dstfile, sizeof(j->dstfile));
	}
	else if (nbr == j->out_fd)
	{
		j->out_fd = -1;
		ft_bzero(j->dstfile, sizeof(j->dstfile));
		ft_strcpy(j->dstfile, "/dev/null");
	}
	else if (nbr == j->err_fd)
	{
		j->err_fd = -1;
		ft_strcpy(j->errfile, "/dev/null");
		ft_bzero(j->dstfile, sizeof(j->dstfile));
	}
	else
	{
		ft_printf("Bad file descriptor: %d\n", nbr);
		return (remove_invalid_job(j, p, pack));
	}
	return (CONTINUE);
}

int			close_output(t_job *j, t_process *p, t_pack *pack)
{
	int nbr;
	int ret;
	int del;

	if ((del = is_digit_only(p->argv[pack->argc - 1])))
		nbr = ft_atoi(p->argv[pack->argc - 1]);
	else
		nbr = 1;
	if ((ret = close_output_nbr(j, p, pack, nbr)) != CONTINUE)
		return (ret);
	if (del)
	{
		--pack->argc;
		free(p->argv[pack->argc]);
		p->argv[pack->argc] = NULL;
	}
	return (CONTINUE);
}

int			is_digit_only(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		++i;
	}
	return (1);
}
