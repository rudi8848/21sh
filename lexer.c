#include "21sh.h"
#include <strings.h>
#include <string.h>

static int store_char(char *word, size_t maxword, int c, size_t *np)
{
	if (*np < maxword)
	{
		word[(*np)++] = c;
		return (1);
	}
	else
		return (0);
}

size_t		ft_len(int nbr)
{
	size_t			i;
	
	i = 0;
	if (nbr == 0)
		return (1);
	while (nbr != 0)
	{
		nbr /= 10;
		++i;
	}
	return (i);
}

size_t	ft_itoa_buf(int value, char *buf)
{
	size_t			len;
	int i;

	len = ft_len(value);
	i = len - 1;
	buf[len] = '\0';
	while (i > -1)
	{
		buf[i] = (value % 10) + '0';
		value = value / 10;
		--i;
	}
	return (len);
}

static	void insert_variable(char *line, char *word, int *i, size_t *wordn)
{
	int j = *i;
	int vname_len = 0;
	char var[256];
	char *ptr = &var[0];

	if (line[j + 1] == '$')
	{
		vname_len = ft_itoa_buf(getpid(), &word[*wordn]);
		*wordn += vname_len;
		*i += 1;
		return;
	}
	while (line[j] != '\0') 
	{
		if (line[j] == ';' || line[j] == '&' ||
		line[j] == '|' || line[j] == '<' || line[j] == '>' ||
		line[j] == '\n' || line[j] == '\t' || line[j] == ' ' ||
		line[j] == '\"' || line[j] == '\'')
			break ;
		var[vname_len] = line[j];
		++vname_len;
		++j;
	}
	var[vname_len + 1] = '\0';
	if ((ptr = get_copy_env(var + 1, MUTE)) != NULL)
	{
		ft_strcpy(var, ptr);
		word = ft_strcat(word, var);
		*wordn += ft_strlen(var);
	}
	if (vname_len)
		*i += vname_len - 1;
}

void	read_more(char *line, int i, char *prompt)
{
	ft_printf("%s", prompt);
	cbreak_settings();
	read_line(line, i);
	ft_restore();
}

static t_token plane_ret(char *line, int *i)
{
	if (line[*i] == ';')
	{
		++(*i);
		return (T_SEMI);
	}
	else if (line[*i] == '&')
	{
		++(*i);
		return (T_BG);
	}
	else if (line[*i] == '|')
	{
		++(*i);
		return (T_PIPE);
	}
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
	else if (line[*i] == '\n')
	{
		++(*i);
		return (T_NLINE);
	}
	return (T_EOF);
}

void plane_cont(char *line, int *i, t_state *state)
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

static t_token is_great(char *line, int *i)
{
	if (line[*i] == '>')
	{
		++(*i);
		return (T_GGREAT);
	}
	return (T_GREAT);
}

int plane_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == ';' || line[*i] == '&' || line[*i] == '|' || line[*i] == '<' || line[*i] == '\n')
	{
		lex->ret_token =  plane_ret(line, i);
		return (RETURN);
	}
	else if (line[*i] == ' ' || line[*i] == '\t')
	{
		++(*i);
		return (0);
	}
	else if (line[*i] == '>' || line[*i] == '\'' || line[*i] == '"')
	{
		plane_cont(line, i, &lex->state);
		return (CONTINUE);
	}
	else
	{
		lex->state = INWORD;
		if (line[*i] == '\\' && line[(*i) + 1])			//		BEGINNING of the word
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
			return (CONTINUE);

		}
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
	return (0);
}

int quote_tokens(char *line, int *i, t_lex *lex)
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

int dquote_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\\' && line[(*i) + 1])
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
	}
	else if (line[*i] == '$')
	{
		insert_variable(line, lex->word, i, &lex->wordn);
		++(*i);
	}
	else if (line[*i] == '"')
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
	else
	{
		if (line[(*i) + 1] == '\0')
			read_more(&line[(*i) + 1], (*i) + 1, "dquote> ");
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
			{
				lex->ret_token = T_ERROR;
				return (RETURN);
			}
		++(*i);
	}
	return (0);
}

int word_tokens(char *line, int *i, t_lex *lex)
{
	if (line[*i] == '\\' && line[(*i) + 1])
	{
		++(*i);
		if (line[*i] == '\n' && !line[(*i) + 1])
			read_more(&line[(*i) + 1], (*i) + 1, ">");
		else 
			if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
			{
					lex->ret_token = T_ERROR;
					return (RETURN);
			}
		++(*i);
		return (CONTINUE);
	}
	else if (line[*i] == '$')
	{
		insert_variable(line, lex->word, i, &lex->wordn);
		++(*i);
	}
	else if ( line[*i] == ';' || line[*i] == '&' || line[*i] == '|' || line[*i] == '<'
		|| line[*i] == '>' || line[*i] == '\n' || line[*i] == '\t' || line[*i] == ' ')
	{
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
		if (!store_char(lex->word, lex->maxword, line[*i], &lex->wordn))
		{	
			lex->ret_token = T_ERROR;
			return (RETURN);
		}
		++(*i);
	}
	return (0);
}

int manage_qoutes(char *line, int *i, t_lex *lex)
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
	return 0;
}

int manage_plane_inword(char *line, int *i, t_lex *lex)
{
	int ret;

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
	return 0;
}

void	init_lex(t_lex *lex, char *word, size_t maxword)
{
	lex->state = PLANE;
	lex->wordn = 0;
	lex->word = word;
	lex->maxword = maxword;
}

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword)
{
	t_lex lex;
	int ret;

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
			return is_great(line, i);
		else if (lex.state == INQUOTE || lex.state == INDQUOTE)
		{
			if ((ret = manage_qoutes(line, i, &lex)) == RETURN)
				return (lex.ret_token);
		}
	}
	return (T_EOF);
}
