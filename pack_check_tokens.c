/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pack_check_tokens.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:35:19 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:35:20 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	pack_less(t_job *j, t_process *p, char *line, t_pack *pack)
{
	if (pack->makepipe)
	{
		ft_printf("Extra <\n");
		return (remove_invalid_job(j, p, pack));
	}
	if (ft_gettoken(line, &pack->i, j->srcfile, sizeof(j->srcfile)) != T_WORD)
	{
		ft_printf("Illegal <\n");
		return (remove_invalid_job(j, p, pack));
	}
	j->in_fd = -1;
	return (CONTINUE);
}

static int	pack_heredoc(t_job *j, t_process *p, char *line, t_pack *pack)
{
	t_process *prev;

	prev = ft_memalloc(sizeof(t_process));
	if (ft_gettoken(line, &pack->i, pack->word, sizeof(pack->word)) != T_WORD)
	{
		ft_printf("Error near << \n");
		return (remove_invalid_job(j, p, pack));
	}
	prev->argv[0] = ft_strdup("heredoc");
	prev->argv[1] = ft_strjoin(pack->word, "\n");
	if (p == j->first_process)
	{
		prev->next = p;
		j->first_process = prev;
	}
	else
	{
		ft_printf("Wrong input for heredoc\n");
		return (remove_invalid_job(j, p, pack));
	}
	return (CONTINUE);
}

int			pack_word(t_process *p, t_pack *pack)
{
	if (pack->argc >= MAXARG - 1)
	{
		ft_printf("Too many args\n");
		return (CONTINUE);
	}
	if (!(p->argv[pack->argc] = ft_strdup(pack->word)))
	{
		ft_printf("Out of memory\n");
		return (CONTINUE);
	}
	++pack->argc;
	return (CONTINUE);
}

int			check_tokens(t_job **j, t_process **p, char *line, t_pack *pack)
{
	int ret;

	if (pack->token == T_LESS)
	{
		if (pack_less(*j, *p, line, pack) != CONTINUE)
			return (0);
	}
	else if (pack->token == T_HRDOC)
	{
		if (pack_heredoc(*j, *p, line, pack) != CONTINUE)
			return (0);
	}
	else if (pack->token == T_GREAT || pack->token == T_GGREAT)
	{
		if (pack_great(*j, *p, line, pack) != CONTINUE)
			return (0);
	}
	else if (pack->token == T_PIPE || pack->token == T_BG ||
		pack->token == T_SEMI || pack->token == T_NLINE)
	{
		if ((ret = pack_end(j, p, line, pack)) != CONTINUE)
			return (ret);
	}
	return (CONTINUE);
}
