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

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_state state = PLANE;
	int c;
	size_t wordn = 0;

	while ((c = line[(*i)++]) != '\0')
	{
		//ft_printf("loop %s [%d]\n", line, state);
		switch (state)
		{
			case PLANE:
			{
				switch (c)
				{
					case ';':
						return T_SEMI;
					case '&':
						return T_BG;
					case '|':
						return T_PIPE;
					case '<':
						return T_LESS;
					case '\n':
						return T_NLINE;
					case ' ':
					case '\t':
						continue;
					case '>':
					{
						state = GGREAT;
						continue;
					}
					case '"':
					{
						state = INQUOTE;
						continue;
					}
					default:
					{
						state = INWORD;
						if (!store_char(word, maxword, c, &wordn))
							return T_ERROR;
						continue;
					}
				}
			}
			case GGREAT:
			{
				if (c == '>')
					return T_GGREAT;
				(*i)--;
				return T_GREAT;
			}

			case INQUOTE:
			{
				if (c != '\"' && !line[(*i) + 1])
				{
					ft_printf("\ndquote> ");
					cbreak_settings();
					read_line(&line[(*i)+1], *i);
					ft_restore();
				}
				switch (c)
				{
					
					case '\\':
					{
						if ((c = line[*i]) == EOF)		// ???
							c = '\\';
						if (!store_char(word, maxword, c, &wordn))
							return T_ERROR;
						continue;
					}
					case '"':
					{
						if (!store_char(word, maxword, '\0', &wordn))
							return T_ERROR;
						return T_WORD;
					}
					default:
					{
						if (!store_char(word, maxword, c, &wordn))
							return T_ERROR;
						continue;
					}
				}
			}
			case INWORD:
			{
				switch (c)
				{
					case ';':
					case '&':
					case '|':
					case '<':
					case '>':
					case '\n':
					case '\t':
					case ' ':
					(*i)--;
					if (!store_char(word, maxword, '\0', &wordn))
							return T_ERROR;
					return T_WORD;
					default:
					if (!store_char(word, maxword, c, &wordn))
							return T_ERROR;
					continue;
				}
			}
		}
		
	}
	return T_EOF;
}
