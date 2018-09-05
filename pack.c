#include "21sh.h"

void	init_job(t_job *j)
{
	j->in_fd = STDIN_FILENO;
	j->out_fd = STDOUT_FILENO;
	j->err_fd = STDERR_FILENO;
	j->foreground = 1;
	ft_bzero(j->srcfile, sizeof(j->srcfile));
	ft_bzero(j->dstfile, sizeof(j->dstfile));
	j->flags = O_WRONLY | O_CREAT | O_CLOEXEC;
}

int	pack_args(char *line, t_job **first_job)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_token	token;
	char word[MAXWORD];
	//int flags = O_WRONLY | O_CREAT;
	int argc;
	t_job *j = *first_job;
	t_process *p = j->first_process;

	argc = 0;

	init_job(j);
	int makepipe = 0;
	int i = 0;

	while (1)
	{
		ft_bzero(word, sizeof(word));
		token = ft_gettoken(line, &i, word, sizeof(word));

		if (token == T_WORD)
		{
			if (argc >= MAXARG-1)
			{
				ft_printf("Too many args\n");
				continue;
			}
			if (!(p->argv[argc] = (char*)ft_memalloc(ft_strlen(word) + 1)))
			{
				ft_printf("Out of memory\n");
				continue;
			}
			ft_strcpy(p->argv[argc], word);
			argc++;
			continue;
		}
		else if (token == T_LESS)
		{
			if (makepipe)
			{
				ft_printf("Extra <\n");
				return 0;
			}
			if (ft_gettoken(line, &i, j->srcfile, sizeof(j->srcfile)) != T_WORD)	// OR BG
			{
				ft_printf("Illegal <\n");
				return 0;
			}
			j->in_fd = -1;
			continue;
		}
		else if (token == T_HRDOC)
		{
			if (ft_gettoken(line, &i, word, sizeof(word)) != T_WORD)
			{
				ft_printf("Error near << \n");
				return 0;
			}
			t_process *prev = ft_memalloc(sizeof(t_process));
			prev->argv[0] = ft_strdup("heredoc");
			prev->argv[1] = ft_strjoin(word, "\n");
			if (p == j->first_process)
			{
				prev->next = p;
				j->first_process = prev;
			}
			else
			{
				ft_printf("Wrong input for heredoc\n");
				return 0;
			}
			continue;
		}
		else if (token == T_GREAT || token == T_GGREAT)
		{
			if (j->out_fd != STDOUT_FILENO)
			{
				ft_printf("\nExtra > or >>\n");
				return 0;
			}
			if (ft_gettoken(line, &i, j->dstfile, sizeof(j->dstfile)) != T_WORD)
			{
				ft_printf("\nIllegal > or >>\n");
				return 0;
			}
			j->out_fd = -1;
			if ( token == T_GGREAT)
				j->flags |= O_APPEND;
			else
				j->flags |= O_TRUNC;
			continue;
		}
		else if (token == T_PIPE || token == T_BG || token == T_SEMI || token == T_NLINE)
		{
			p->argv[argc] = NULL;
			if (token == T_BG)
			{
				j->foreground = 0;
				continue;
			}
			if (token == T_PIPE)
			{
				if (j->out_fd != STDOUT_FILENO)
				{
					ft_printf("\n> or >> conflicts with |\n");
					return 0;
				}
				//one simple command ends, need to create next process and pipe()
				p->next  = (t_process*)ft_memalloc(sizeof(t_process));
				p = p->next;
				//ft_printf("PIPE\n");
				makepipe = 1;
				argc = 0;
				continue;
			}
			if (token == T_SEMI)
			{
				if (line[i] && line[i] != '\n')
				{
					j->next = (t_job*)ft_memalloc(sizeof(t_job));
					p->next = NULL;
					j = j->next;
					j->first_process = (t_process*)ft_memalloc(sizeof(t_process));
					p = j->first_process;
					argc = 0;
					init_job(j);
					continue;
				}
				else
					return 1;
			}
			if (argc == 0 && (token != T_NLINE || j->in_fd /*> 1*/ != -1))
			{
				ft_printf("Missing command\n");
				return 0;
			}
			else if (argc == 0 && token == T_NLINE)
				return 0;
			return 1;

		}
		else if (token == T_EOF)
		{
			return 0;
		}
		else if (token == T_ERROR)
		{
			return 0;	
		}

	}
}
