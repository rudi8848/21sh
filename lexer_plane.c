/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_plane.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:58:29 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:58:31 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static t_token		plane_simple_ret(char *line, int *i)
{
	t_token ret;

	if (line[*i] == ';')
		ret = T_SEMI;
	else if (line[*i] == '&')
		ret = T_BG;
	else if (line[*i] == '|')
		ret = T_PIPE;
	else
		ret = T_NLINE;
	++(*i);
	return (ret);
}

static t_token		plane_ret(char *line, int *i)
{
	if (line[*i] == ';' || line[*i] == '&' ||
		line[*i] == '|' || line[*i] == '\n')
		return (plane_simple_ret(line, i));
	else if (line[*i] == '<')
	{
		++(*i);
		if (line[*i] == '<')
		{
			++(*i);
			return (T_HRDOC);
		}
		return (T_LESS);
	}
	return (T_EOF);
}

static void			plane_cont(char *line, int *i, t_state *state)
{
	if (line[*i] == '>')
	{
		*state = GGREAT;
		++(*i);
	}
	else if (line[*i] == '\'')
	{
		*state = INQUOTE;
		++(*i);
	}
	else if (line[*i] == '"')
	{
		*state = INDQUOTE;
		++(*i);
	}
}

static int			plane_tokens(char *line, int *i, t_lex *lex)
{
	int	ret;

	if (line[*i] == ';' || line[*i] == '&' || line[*i] == '|'
		|| line[*i] == '<' || line[*i] == '\n')
	{
		lex->ret_token = plane_ret(line, i);
		return (RETURN);
	}
	else if (line[*i] == ' ' || line[*i] == '\t')
		++(*i);
	else if (line[*i] == '>' || line[*i] == '\'' || line[*i] == '"')
	{
		plane_cont(line, i, &lex->state);
		return (CONTINUE);
	}
	else
	{
		lex->state = INWORD;
		if ((ret = plane_inword(line, i, lex)) == RETURN)
			return (RETURN);
		else if (ret == CONTINUE)
			return (CONTINUE);
	}
	return (0);
}

int					manage_plane_inword(char *line, int *i, t_lex *lex)
{
	int	ret;

	if (lex->state == PLANE)
	{
		if ((ret = plane_tokens(line, i, lex)) == RETURN)
			return (RETURN);
		else if (ret == CONTINUE)
			return (CONTINUE);
	}
	else if (lex->state == INWORD)
	{
		if ((ret = word_tokens(line, i, lex)) == RETURN)
			return (RETURN);
		else if (ret == CONTINUE)
			return (CONTINUE);
	}
	return (0);
}
