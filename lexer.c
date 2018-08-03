#include "21sh.h"

typedef enum {
	T_WORD,
	T_PIPE,
	T_BG,
	T_SEMI,
	T_GREAT,
	T_GGREAT,
	T_LESS,
	T_LLESS,
	T_NLINE,
	T_EOF,
	T_ERROR
} t_token;

typedef enum {
	PLANE,
	GGREAT,
	INQUOTE,
	INWORD
} t_state;

static int store_char(char *word, size_t maxword, int c, size_t *np);

static t_token ft_gettoken(char *word, size_t maxword)
{
	t_state state = PLANE;
	int c;
	size_t wordn = 0;

	while ((c = getchar() != EOF))
	{
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
							fprintf(stderr, "ERROR\n");
						continue;
					}
				}
			}
			case GGREAT:
			{
				if (c == '>')
					return T_GGREAT;
				ungetc(c, stdin);
				return T_GREAT;
			}

			case INQUOTE:
			{
				switch (c)
				{
					case '\\':
					{
						if ((c = getchar()) == EOF)
							c = '\\';
						if (!store_char(word, maxword, c, &wordn))
							fprintf(stderr, "ERROR\n");
						continue;
					}
					case '"':
					{
						if (!store_char(word, maxword, '\0', &wordn))
							fprintf(stderr, "ERROR\n");
						return T_WORD;
					}
					default:
					{
						if (!store_char(word, maxword, c, &wordn))
							fprintf(stderr, "ERROR\n");
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
					ungetc(c, stdin);
					if (!store_char(word, maxword, '\0', &wordn))
							fprintf(stderr, "ERROR\n");
					return T_WORD;
					default:
					if (!store_char(word, maxword, c, &wordn))
							fprintf(stderr, "ERROR\n");
					continue;
				}
			}
		} 
	}
	return T_EOF;
}


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

int main(void)
{
	char word[200];
	int t;

	while (1)
	{
		switch (t = ft_gettoken(word, sizeof(word)))
		{
			case T_WORD:
			{
				printf("[%i] ", t);
				printf("T_WORD %s\n", word);
				break;
			}
			case T_PIPE:
			{
				printf("[%i] ", t);
				printf("T_PIPE\n");
				break;
			}
			case T_BG:
			{
				printf("[%i] ", t);
				printf("T_BG\n");
				break;
			}
			case T_SEMI:
			{
				printf("[%i] ", t);
				printf("T_SEMI\n");
				break;
			}
			case T_GREAT:
			{
				printf("[%i] ", t);
				printf("T_GREAT\n");
				break;
			}
			case T_GGREAT:
			{
				printf("[%i] ", t);
				printf("T_GGREAT\n");
				break;
			}
			case T_LESS:
			{
				printf("[%i] ", t);
				printf("T_LESS\n");
				break;
			}
			case T_LLESS:
			{
				printf("[%i] ", t);
				printf("T_LLESS\n");
				break;
			}
			case T_NLINE:
			{
				printf("[%i] ", t);
				printf("T_NLINE\n");
				break;
			}
			case T_EOF:
			{
				printf("[%i] ", t);
				printf("T_EOF\n");
				exit(0);
			}
			case T_ERROR:
			{
				printf("[%i] ", t);
				printf("T_ERROR\n");
				exit(0);
			}
		}
	}
}