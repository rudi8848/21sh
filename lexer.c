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
	i = len-1;
	buf[len] = '\0';
	while (i > -1)
	{
		buf[i] = (value % 10) + '0';
		value = value / 10;
		--i;
	}
	return len;
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
		line[j] == '\n' || line[j] == '\t' || line[j] == ' ')
			break;
		var[vname_len] = line[j];
		vname_len++;
		j++;
	}
	var[vname_len+1] = '\0';
	if ((ptr = getenv(var + 1)) != NULL)
	{
		ft_strcpy(var, get_copy_env(var + 1, MUTE));
		word = ft_strcat(word, var);
		*wordn += ft_strlen(var);
	}
	if (vname_len)
		*i += vname_len-1;
	
}

void	read_more(char *line, int i, char *prompt)
{
	ft_printf("%s", prompt);
	cbreak_settings();
	read_line(line, i);
	ft_restore();
}

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_state state = PLANE;
	//int c;
	size_t wordn = 0;

	while (line[*i])
	{
		if (state == PLANE)
		{
			if (line[*i] == ';')
			{
				(*i)++;
				return T_SEMI;
			}
			else if (line[*i] == '&')
			{
				(*i)++;
				return T_BG;
			}
			else if (line[*i] == '|')
			{
				(*i)++;
				return T_PIPE;
			}
			else if (line[*i] == '<')
			{
				(*i)++;
					if (line[*i] == '<')
					{
						(*i)++;
						return T_HRDOC;
					}
				return T_LESS;
			}					
			else if (line[*i] == '\n')
			{
				(*i)++;
				return T_NLINE;
			}
			else if (line[*i] == ' ' || line[*i] == '\t')
				(*i)++;
			else if (line[*i] == '>')
			{
				state = GGREAT;
				(*i)++;
				continue;
			}
			else if (line[*i] == '\'')
			{
				state = INQUOTE;
				(*i)++;
				continue;
			}
			else if (line[*i] == '"')
			{
				state = INDQUOTE;
				(*i)++;
				continue;
			}
			else
			{
				state = INWORD;
				if (line[*i] == '\\' && line[(*i) + 1])			//		BEGINNING of the word
				{
					(*i)++;
					if (line[*i] == '\n' && !line[(*i) + 1])
						read_more(&line[(*i) + 1], (*i)+1, ">");
					else 
						if (!store_char(word, maxword, line[*i], &wordn))
						return T_ERROR;
					
					(*i)++;
					continue;
				}
				else if (line[*i] == '$')
				{
					insert_variable(line, word, i, &wordn);
					(*i)++;
				}
				else
				{
					if (!store_char(word, maxword, line[*i], &wordn))
						return T_ERROR;
						(*i)++;
				}
				continue;
			}
		}
		else if (state == GGREAT)
		{
			if (line[*i] == '>')
			{
				(*i)++;
				return T_GGREAT;
			}
			return T_GREAT;
		}
		else if (state == INQUOTE)
		{
			 if (line[*i] == '\'')
			{
				(*i)++;
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				return T_WORD;
			}
			else
			{
				if (line[(*i) + 1] == '\0')
					read_more(&line[(*i) + 1], (*i) + 1, "quote> ");
				if (!store_char(word, maxword, line[*i], &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
		else if (state == INDQUOTE)
		{
			if (line[*i] == '\\' && line[(*i) + 1])
			{
				(*i)++;
				if (line[*i] == '\n' && !line[(*i) + 1])
						read_more(&line[(*i) + 1], (*i)+1, ">");
				else
				{
					if (!store_char(word, maxword, line[*i], &wordn))
						return T_ERROR;
				}
				(*i)++;
			}
			else if (line[*i] == '$')
			{
				insert_variable(line, word, i, &wordn);
				(*i)++;
			}
			else if (line[*i] == '"')
			{
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				(*i)++;
				return T_WORD;
			}
			else
			{
				if (line[(*i) + 1] == '\0')
					read_more(&line[(*i) + 1], (*i) + 1, "dquote> ");
				if (!store_char(word, maxword, line[*i], &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
		else if (state == INWORD)
		{			
			if (line[*i] == '\\' && line[(*i) + 1])
			{
				(*i)++;
				if (line[*i] == '\n' && !line[(*i) + 1])
					read_more(&line[(*i) + 1], (*i)+1, ">");
				else 
					if (!store_char(word, maxword, line[*i], &wordn))
						return T_ERROR;
				(*i)++;
				continue;
			}
			
			else if (line[*i] == '$')
			{
					insert_variable(line, word, i, &wordn);
					(*i)++;
			}
			else if ( line[*i] == ';' || line[*i]== '&' || line[*i]== '|' || line[*i]== '<'
				|| line[*i]== '>' || line[*i]== '\n' || line[*i]== '\t' || line[*i]== ' ')
			{
				if (!store_char(word, maxword, '\0', &wordn))
					return T_ERROR;
				return T_WORD;
			}
			else
			{
				if (!store_char(word, maxword, line[*i], &wordn))
					return T_ERROR;
				(*i)++;
			}
		}
	}
	return T_EOF;
}

