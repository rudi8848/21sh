/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:18:49 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:18:53 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static t_token	is_great(char *line, int *i)
{
	if (line[*i] == '>')
	{
		++(*i);
		return (T_GGREAT);
	}
	return (T_GREAT);
}

static int		manage_qoutes(char *line, int *i, t_lex *lex)
{
	int ret;

	if (lex->state == INQUOTE)
	{
		if ((ret = quote_tokens(line, i, lex)) == RETURN)
			return (RETURN);
	}
	else if (lex->state == INDQUOTE)
	{
		if ((ret = dquote_tokens(line, i, lex)) == RETURN)
			return (RETURN);
	}
	return (0);
}

static void		init_lex(t_lex *lex, char *word, size_t maxword)
{
	lex->state = PLANE;
	lex->wordn = 0;
	lex->word = word;
	lex->maxword = maxword;
}

t_token			ft_gettoken(char *line, int *i, char *word, size_t maxword)
{
	t_lex	lex;
	int		ret;

	init_lex(&lex, word, maxword);
	while (line[*i])
	{
		if (lex.state == PLANE || lex.state == INWORD)
		{
			if ((ret = manage_plane_inword(line, i, &lex)) == RETURN)
				return (lex.ret_token);
			else if (ret == CONTINUE)
				continue;
		}
		else if (lex.state == GGREAT)
			return (is_great(line, i));
		else if (lex.state == INQUOTE || lex.state == INDQUOTE)
		{
			if ((ret = manage_qoutes(line, i, &lex)) == RETURN)
				return (lex.ret_token);
		}
	}
	return (T_EOF);
}
