/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pack_end.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:46:37 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:46:39 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	pack_pipe(t_job **j, t_process **p, t_pack *pack)
{
	if ((*j)->out_fd != STDOUT_FILENO )
		return (remove_invalid_job(*j, *p, pack));
	(*p)->next = (t_process*)ft_memalloc(sizeof(t_process));
	*p = (*p)->next;
	pack->makepipe = 1;
	pack->argc = 0;
	return (CONTINUE);
}

static int	pack_semicolon(t_job **j, t_process **p, char *line, t_pack *pack)
{
	if (line[pack->i] && line[pack->i] != '\n')
	{
		(*j)->next = (t_job*)ft_memalloc(sizeof(t_job));
		(*p)->next = NULL;
		(*j) = (*j)->next;
		pack->argc = 0;
		init_job(*j);
		*p = (*j)->first_process;
		pack->makepipe = 0;
		return (CONTINUE);
	}
	else
		return (1);
}

int			pack_end(t_job **j, t_process **p, char *line, t_pack *pack)
{
	(*p)->argv[pack->argc] = NULL;
	if (pack->token == T_BG)
	{
		(*j)->foreground = 0;
		return (CONTINUE);
	}
	if (pack->token == T_PIPE)
		return (pack_pipe(j, p, pack));
	if (pack->token == T_SEMI)
		return (pack_semicolon(j, p, line, pack));
	if (pack->argc == 0 && (pack->token != T_NLINE || (*j)->in_fd != -1))
	{
		ft_putstr_fd("Missing command\n", STDERR_FILENO);
		return (remove_invalid_job(*j, *p, pack));
	}
	else if (pack->argc == 0 && pack->token == T_NLINE)
		return (remove_invalid_job(*j, *p, pack));
	return (1);
}
