/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pack_redirection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:38:26 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:38:28 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	fd_to_fd(t_job *j, t_process *p, t_pack *pack)
{
	//int		nbr;
	int		left;
	int		right;

	if (is_digit_only(p->argv[pack->argc - 1]))
	{
		left = ft_atoi(p->argv[pack->argc - 1]);
		right = ft_atoi(j->dstfile);
		if (dup2(left, right) == -1)
		{
			ft_printf("21sh: Bad file descriptor: %d\n", left );
			return (remove_invalid_job(j, p, pack));	
		}
		if (left > 2)
			close(left);
	}
	j->out_fd = right;
	/*
		nbr = ft_atoi(p->argv[pack->argc - 1]);
		if (nbr == j->in_fd)
			j->in_fd = ft_atoi(j->dstfile);
		else if (nbr == j->out_fd)
			j->out_fd = ft_atoi(j->dstfile);
		else if (nbr == j->err_fd)
			j->err_fd = ft_atoi(j->dstfile);
		else
		{
			ft_printf("21sh: Bad file descriptor: %d\n", nbr);
			return (remove_invalid_job(j, p, pack));
		}
		--pack->argc;
		free(p->argv[pack->argc]);
		p->argv[pack->argc] = NULL;
	}*/
		--pack->argc;
		free(p->argv[pack->argc]);
		p->argv[pack->argc] = NULL;
	
	//j->out_fd = ft_atoi(j->dstfile);
	return (CONTINUE);
}

static int	check_output_fd(t_job *j, t_process *p, char *line, t_pack *pack)
{
	int ret;

	if ((pack->tkn = ft_gettoken(line, &pack->i, j->dstfile,
					sizeof(j->dstfile))) != T_WORD)
	{
		ft_printf("21sh: Parse error\n");
		return (remove_invalid_job(j, p, pack));
	}
	if (ft_strequ("-", j->dstfile))
	{
		if ((ret = close_output(j, p, pack)) != CONTINUE)
			return (ret);
	}
	else if (is_digit_only(j->dstfile))
		return (fd_to_fd(j, p, pack));
	else
	{
		ft_printf("21sh: Parse error\n");
		return (remove_invalid_job(j, p, pack));
	}
	return (CONTINUE);
}

static int	fd_to_file(t_job *j, t_process *p, t_pack *pack)
{
	int nbr;

	nbr = ft_atoi(p->argv[pack->argc - 1]);
	if (nbr == j->in_fd)
		j->in_fd = open(j->dstfile, j->flags, FILE_PERM);
	else if (nbr == STDOUT_FILENO)
		j->out_fd = -1;
	else if (nbr == j->err_fd)
		j->err_fd = open(j->dstfile, j->flags, FILE_PERM);
	else
	{
		ft_printf("21sh: Bad file descriptor: %d\n", nbr);
		return (remove_invalid_job(j, p, pack));
	}
	--pack->argc;
	free(p->argv[pack->argc]);
	p->argv[pack->argc] = NULL;
	return (CONTINUE);
}

static int	check_redirection(t_job *j, t_process *p, char *line, t_pack *pack)
{
	int		ret;

	if ((pack->tkn = ft_gettoken(line, &pack->i,
					j->dstfile, sizeof(j->dstfile))) != T_WORD)
	{
		if (pack->tkn == T_BG)
		{
			if ((ret = check_output_fd(j, p, line, pack)) != CONTINUE)
				return (ret);
		}
		else
		{
			ft_printf("\n21sh: Illegal > or >>\n");
			return (remove_invalid_job(j, p, pack));
		}
	}
	else
	{
		j->out_fd = -1;
		if (is_digit_only(p->argv[pack->argc - 1]))
			return (fd_to_file(j, p, pack));
	}
	return (CONTINUE);
}

int			pack_great(t_job *j, t_process *p, char *line, t_pack *pack)
{
	if (j->out_fd != STDOUT_FILENO)
	{
		ft_printf("\n21sh: Extra > or >>\n");
		return (remove_invalid_job(j, p, pack));
	}
	if (!pack->argc)
	{
		ft_printf("21sh: Parse error\n");
		return (remove_invalid_job(j, p, pack));
	}
	if (pack->token == T_GGREAT)
		j->flags |= O_APPEND;
	else
		j->flags |= O_TRUNC;
	return (check_redirection(j, p, line, pack));
}
