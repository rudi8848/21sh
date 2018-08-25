#include "21sh.h"
#include <strings.h>
#include <string.h>
/*
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
*/
static int store_char(char *word, size_t maxword, int c, size_t *np);
//static pid_t	invoke(int argc, char *argv[], int srcfd, const char *srcfile, int dstfd, const char * dstfile, t_bool append, t_bool bcgrnd, int closefd);

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
					ft_printf(">dquote ");
					cbreak_settings();
					read_line(&line[(*i)+1]);
					ft_restore();
					//ft_printf("[LINE]: %s\n", line);
					//continue;
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


static int store_char(char *word, size_t maxword, int c, size_t *np)
{
	//printf("---> %s: %s, %zu, %i, %zu\n", __FUNCTION__, word, maxword, c, *np);
	if (*np < maxword)
	{
		word[(*np)++] = c;
		//printf("%s\n", word);
		return 1;
	}
	else
		return 0;
}


//-------------------------------------------------------------------------------
/*
#define MAXARG 50
#define MAXFNAME 500
#define MAXWORD 500
static t_token command(pid_t *wpid, t_bool makepipe, int *pipefdp)
{
	t_token token;
	t_token term;

	int argc;
	int srcfd;
	int dstfd;
	int pid;
	int pfd[2] = {-1, 1};

	char *argv[MAXARG];
	char srcfile[MAXFNAME] = "";
	char dstfile[MAXFNAME] = "";
	char word[MAXWORD];

	t_bool append;

	argc = 0;
	srcfd = STDIN_FILENO;
	dstfd = STDOUT_FILENO;

	while (TRUE)
	{
		switch (token = ft_gettoken(word, sizeof(word)))
		{
			case T_WORD:
			{
				if (argc >= MAXARG - 1)
				{
					fprintf(stderr, "To many args\n");
					continue;
				}
				if ((argv[argc] = malloc(strlen(word) + 1)) == NULL)
				{
					fprintf(stderr, "Out of arg memory\n");
					continue;
				}
				strcpy(argv[argc], word);
				argc++;
				continue;
			}
			case T_LESS:
			{
				if (makepipe)
				{
					fprintf(stderr, "Extra <\n");
					break;
				}
				srcfd = -1;
				continue;
			}
			case T_GREAT:
			case T_GGREAT:
			{
				if (dstfd != STDOUT_FILENO)
				{
					fprintf(stderr, "Extra > or >>\n");
					break;
				}
				dstfd = -1;
				append = token == T_GGREAT;
				continue;
			}
			case T_PIPE:
			case T_BG:
			case T_SEMI:
			case T_NLINE:
			{
				argv[argc] = NULL;
				if (token == T_PIPE)
				{
					if (dstfd != STDOUT_FILENO)
					{
						fprintf(stderr, "> of >> conflicts with |\n");
						break;
					}
					term = command(wpid, TRUE, &dstfd);
					if (term == T_ERROR)
						return T_ERROR;
				}
				else
					term = token;
				if (makepipe)
				{
					if (pipe(pfd) == ERROR)
					{
						perror("pipe");
						//exit(EXIT_FAILURE);
						return T_ERROR;
					}
					*pipefdp = pfd[1];
					srcfd = pfd[0];
				}
				if (pid = invoke(argc, argv, srcfd, srcfile, dstfd, dstfile,append, term == T_BG, pfd[1]) == ERROR)
				{
					perror("invoke");
					//exit(EXIT_FAILURE);
					return T_ERROR;
				}
				if (token != T_PIPE)
					*wpid = pid;
				if (argc == 0 && (token != T_NLINE || srcfd > 1))
					fprintf(stderr, "Missing command\n");
				while (--argc >= 0)
					free(argv[argc]);
				return term;
			}
			case T_EOF:
				exit(EXIT_SUCCESS);
			case T_ERROR:
				return T_ERROR;
		}
	}
}

*/

//--------------------------------------------------------------------------------
/*
static t_bool	entry_sig(void)
{
	if (sigaction(SIGINT, &entry_int, NULL) == -1)
		perror("sigaction");
	if (sigaction(SIGQUIT, &entry_quit, NULL) == -1)
		perror("sigaction");
	
	return TRUE;
}
*/
/*
void	display_status(pid_t pid, int status)
{
	
	if (pid != 0)
		printf("Process %ld: ", (long)pid);
	if (WIFEXITED(status))
		printf("Exit value %d\n", WEXITSTATUS(status));
	else
	{
		
		char *desc;
		char *signame = get_macrostr("signal", WTERMSIG(status), &desc);
		if (desc[0] == '?')
			desc = signame;
		if (signame[0] == '?')
			printf("Signal #%d", WTERMSIG(status));
		else
			printf("%s", desc);
			
		if (WCOREDUMP(status))
			printf(" - core dumped");
		if (WIFSTOPPED(status))
			printf(" (stopped)");
		printf("\n");
	}
	
}
*/
/*
static void	fd_check(void)
{
	int fd;
	t_bool ok = TRUE;

	for (fd = 3; fd < 20; fd++)
		if (fcntl(fd, F_GETFL) != -1 )
		{
			ok = FALSE;
			fprintf(stderr, "*** fd %d is open ***\n", fd);
		}
	if (!ok)
		_exit(EXIT_FAILURE);
}
*/
/*
static t_bool	wait_and_display(pid_t pid)
{
	pid_t wpid;
	int status;

	do
	{
		if (wpid = waitpid(-1, &status, 0) == -1)
			perror("waitpid");
		display_status(wpid, status);
	} while (wpid != pid);
	return TRUE;
}

static t_bool	builtin(int argc, char *argv[], int srcfd, int dstfd)
{
	char *path;

	if (!strcmp(argv[0], "cd") || !strcmp(argv[0], "echo"))
	{
		printf("BUILTIN %s\n", argv[0]);
	}
	else
		return FALSE;
	if (srcfd != STDIN_FILENO || dstfd != STDOUT_FILENO)
		fprintf(stderr, "Illegal redirection or pipeline\n");	
	return TRUE;
}

static void	redirect(int srcfd, const char *srcfile, int dstfd, const char *dstfile, t_bool append, t_bool bckgrnd)
{
	int flags;

	if (srcfd == STDIN_FILENO && bckgrnd)
	{
		srcfile = "/dev/null";
		srcfd = -1;
	}
	if (srcfile[0] != '\0')
	{
		if ((srcfd = open(srcfile, O_RDONLY, 0)) == -1 )
				perror("open");
	}
	if (dup2(srcfd, STDIN_FILENO) == -1)
		perror("dup2");
	if (srcfd != STDIN_FILENO)
	{
		if (close(srcfd) == -1)
			perror("close");
	}
	if (dstfile[0] != '\0')
	{
		flags = O_WRONLY | O_CREAT;
		if (append)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		if ((dstfd = open(dstfile, flags, flags)) == -1)
			perror("open");
	}
	if (dup2(dstfd, STDOUT_FILENO) == -1)
		perror("dup2");
	if (dstfd != STDOUT_FILENO)
	{
		if (close(dstfd) == -1)
			perror("close");
	}
	fd_check();
	return;
}


static pid_t	invoke(int argc, char *argv[], int srcfd, const char *srcfile, int dstfd, const char * dstfile, t_bool append, t_bool bckgrnd, int closefd)
{
	pid_t pid;
	char *cmdname;
	char *cmdpath;

	if (argc == 0 || builtin(argc, argv, srcfd, dstfd))
		return 0;

	switch (pid = fork())
	{
		case -1:
		{
			fprintf(stderr, "Can't create new process\n");
			return 0;
		}
		case 0:
		{
			if (closefd != -1)
			{
				if (close(closefd) == -1)
				{
					perror("close");
					exit(EXIT_FAILURE);
				}
			}
//			if (!bckgrnd)
//			{
//				if (!entry_sig())
//					perror("entry_sig");
//			}
			redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
			cmdname = strchr(argv[0], '/');
			if (cmdname == NULL)
				cmdname = argv[0];
			else
				cmdname++;
			cmdpath = argv[0];
			argv[0] = cmdname;
			execvp(cmdpath, argv);
			fprintf(stderr, "Can't execute %s\n", cmdpath);
			_exit(EXIT_FAILURE);
		}
		//parent
		if (srcfd > STDOUT_FILENO)
		{
			if (close(srcfd) == -1)
			{
				perror("close");
			}
		}
		if (dstfd > STDOUT_FILENO)
		{
			if (close(dstfd) == -1)
				perror("close");
		}
		if (bckgrnd)
			printf("%ld\n", (long)pid);
		return pid;
	}
}
*/
//--------------------------------------------------------------------------------
/*
int main(void)
{
	pid_t pid;
	t_token term = T_NLINE;

//	ignore_sig();

	while (TRUE)
	{
		if (term == T_NLINE)
			printf("prompt @ ");
		term = command(&pid, FALSE, NULL);

		if (term == T_ERROR)
		{
			fprintf(stderr, "Bad command\n");
			term = T_NLINE;
		}
		if (term != T_BG && pid > 0)
			wait_and_display(pid);
		fd_check();
	}
	
	char word[200];
	int t;
	bzero(word, 200);

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
	

	//printf("%d\n", ft_gettoken(word, sizeof(word)));
	
		return 0;
}*/
