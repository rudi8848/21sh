#include "21sh.h"
#include <strings.h>
#include <string.h>

static int store_char(char *word, size_t maxword, int c, size_t *np)
{
	if (*np < maxword)
	{
		word[(*np)++] = c;
		return 1;
	}
	else
		return 0;
}

static	void insert_variable(char *line, char *word, int *i, size_t *wordn)
{
	int j = *i;
	int vname_len = 0;
	char var[256];
	char *ptr = &var[0];

	while (line[j] != '\0') 
	{
		if (line[j] == ';' || line[j] == '&' ||
		line[j] == '|' || line[j] == '<' || line[j] == '>' ||
		line[j] == '\n' || line[j] == '\t' || line[j] == ' ')
			break;
		var[vname_len] = line[j];
		vname_len++;
		j++;
	}
	var[vname_len] = '\0';
	if ((ptr = getenv(var + 1)) != NULL)
	{
		ft_strcpy(var, getenv(var + 1));
		word = ft_strcat(word, var);
		*wordn += ft_strlen(var);
	}
	if (vname_len)
		*i += vname_len - 1;
	
}

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_state state = PLANE;
	int c;
	size_t wordn = 0;


	while ((c = line[*i]) != '\0')
	{
		if (state == PLANE)
		{
			if (c == ';')
			{
				(*i)++;
				return T_SEMI;
			}
			else if (c == '&')
			{
				(*i)++;
				return T_BG;
			}
			else if (c == '|')
			{
				(*i)++;
				return T_PIPE;
			}
			else if (c == '<')
			{
				(*i)++;
				return T_LESS;
			}					
			else if (c == '\n')
			{
				(*i)++;
				return T_NLINE;
			}
			else if (c == ' ' || c == '\t')
				(*i)++;
			else if (c == '>')
			{
				state = GGREAT;
				(*i)++;
			}
			else if (c == '\'')
			{
				state = INQUOTE;
				(*i)++;
			}
			else if (c == '"')
			{
				state = INDQUOTE;
				(*i)++;
			}
			else
			{
				state = INWORD;
				if (!store_char(word, maxword, c, &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
		else if (state == GGREAT)
		{
			if (c == '>')
					return T_GGREAT;
				return T_GREAT;
		}
		else if (state == INQUOTE)
		{
			 if (c == '\'')
			{
				(*i)++;
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				return T_WORD;
			}
			else
			{
				if (line[(*i) + 1] == '\0')
				{
					ft_printf("\nquote> ");
					cbreak_settings();
					read_line(&line[(*i) + 1], (*i) + 1);
					ft_restore();
					//ft_printf("[%d][%s]\n", *i, line);
				}
				if (!store_char(word, maxword, c, &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
		else if (state == INDQUOTE)
		{
			if (c == '\\' && line[(*i) + 1])
			{
				(*i)++;
				if (!store_char(word, maxword, line[*i], &wordn))
					return T_ERROR;
				(*i)++;
			}
			else if (c == '$')
			{
				insert_variable(line, word, i, &wordn);
				(*i)++;
			}
			else if (c == '"')
			{
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				(*i)++;
				return T_WORD;
			}
			else
			{
				if (line[(*i) + 1] == '\0')
				{
					ft_printf("\ndquote> ");
					cbreak_settings();
					read_line(&line[(*i) + 1], (*i) + 1);
					ft_restore();
				}
				if (!store_char(word, maxword, c, &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
		else if (state == INWORD)
		{
			if (c == '$')
			{
					insert_variable(line, word, i, &wordn);
					(*i)++;
			}
			else if ( c == ';' || c == '&' || c == '|' || c == '<' || c == '>' || c == '\n' || c == '\t' || c == ' ')
			{
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				return T_WORD;
			}
			else
			{
				if (!store_char(word, maxword, c, &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
	}
	return T_EOF;
}

