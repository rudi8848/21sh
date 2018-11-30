/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:03:04 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:03:05 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	plane_inword_backslash(char *line, int *i, t_lex *lex)
{
	++(*i);
	if (line[*i] == '\n' && !line[(*i) + 1])
		read_more(&line[(*i) + 1], (*i) + 1, ">", NULL);
	else
	{
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
	}
	++(*i);
	return (CONTINUE);
}

int			plane_inword(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\\' && line[(*i) + 1])
		return (plane_inword_backslash(line, i, lex));
	else if (line[*i] == '$')
	{
		insert_variable(line, lex->word, i, &lex->wordn);
		++(*i);
	}
	else
	{
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
		++(*i);
	}
	return (CONTINUE);
}

static int	word_tokens_bs(char *line, int *i, t_lex *lex)
{
	++(*i);
	if (line[*i] == '\n' && !line[(*i) + 1])
		read_more(&line[(*i) + 1], (*i) + 1, ">", NULL);
	else if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
	{
		lex->ret_token = T_ERROR;
		return (RETURN);
	}
	++(*i);
	return (CONTINUE);
}

static int	word_tokens_end(t_lex *lex)
{
	if (!store_char(lex->word, lex->maxword, '\0', &lex->wordn))
	{
		lex->ret_token = T_ERROR;
		return (RETURN);
	}
	lex->ret_token = T_WORD;
	return (RETURN);
}

int			word_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\\' && line[(*i) + 1])
		return (word_tokens_bs(line, i, lex));
	else if (line[*i] == '$')
	{
		insert_variable(line, lex->word, i, &lex->wordn);
		++(*i);
	}
	else if (line[*i] == ';' || line[*i] == '&' || line[*i] == '|'
			|| line[*i] == '<' || line[*i] == '>' || line[*i] == '\n'
			|| line[*i] == '\t' || line[*i] == ' ')
		return (word_tokens_end(lex));
	else
	{
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
		++(*i);
	}
	return (0);
}
