/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_dquotes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:13:40 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:13:42 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

int			quote_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\'')
	{
		++(*i);
		if (!store_char(lex->word, lex->maxword, '\0', &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
		lex->ret_token = T_WORD;
		return (RETURN);
	}
	else
	{
		if (line[(*i) + 1] == '\0')
			read_more(&line[(*i) + 1], (*i) + 1, "quote> ");
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
		++(*i);
	}
	return (0);
}

static int	dquote_tokens_bs(char *line, int *i, t_lex *lex)
{
	++(*i);
	if (line[*i] == '\n' && !line[(*i) + 1])
		read_more(&line[(*i) + 1], (*i) + 1, ">");
	else
	{
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
	}
	++(*i);
	return (0);
}

static int	dquote_tokens_end(int *i, t_lex *lex)
{
	if (!store_char(lex->word, lex->maxword, '\0', &lex->wordn))
	{
		lex->ret_token = T_ERROR;
		return (RETURN);
	}
	++(*i);
	lex->ret_token = T_WORD;
	return (RETURN);
}

static int	dquote_tokens_store(char *line, int *i, t_lex *lex)
{
	if (line[(*i) + 1] == '\0')
		read_more(&line[(*i) + 1], (*i) + 1, "dquote> ");
	if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
	{
		lex->ret_token = T_ERROR;
		return (RETURN);
	}
	++(*i);
	return (0);
}

int			dquote_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\\' && line[(*i) + 1])
	{
		if (dquote_tokens_bs(line, i, lex) == RETURN)
			return (RETURN);
	}
	else if (line[*i] == '$')
	{
		insert_variable(line, lex->word, i, &lex->wordn);
		++(*i);
	}
	else if (line[*i] == '"')
		return (dquote_tokens_end(i, lex));
	else
	{
		if (dquote_tokens_store(line, i, lex) == RETURN)
			return (RETURN);
	}
	return (0);
}
