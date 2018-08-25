#include "21sh.h"

int	pack_args(char *line, t_job **first_job)
{
	ft_printf("---> %s\n", __FUNCTION__);
	t_token	token;
	char word[MAXWORD];
	int append;
	int pfd[2] = {-1, -1};
	int argc;
	t_job *j = *first_job;
	t_process *p = j->first_process;

	argc = 0;
	j->in_fd = STDIN_FILENO;
	j->out_fd = STDOUT_FILENO;
	j->err_fd = STDERR_FILENO;

	p->in_fd = j->in_fd;
	p->out_fd = j->out_fd;
	
	int makepipe = 0;
	int i = 0;
	while (1)
	{

//		ft_printf("--> j[%p], p[%p]\n", j, p);
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
			strcpy(p->argv[argc], word);
			//ft_printf("> [%d] : [%s]\n", argc, p->argv[argc]);
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
			if (ft_gettoken(line, &i, j->srcfile, sizeof(j->srcfile)) != T_WORD)
			{
				ft_printf("Illegal <\n");
				return 0;
			}
			j->in_fd = -1;
			continue;
		}
		else if (token == T_GREAT || token == T_GGREAT)
		{
			if (j->out_fd != STDOUT_FILENO)
			{
				ft_printf("Extra > or >>\n");
				return 0;
			}
			if (ft_gettoken(line, &i, j->dstfile, sizeof(j->dstfile)) != T_WORD)
			{
				ft_printf("Illegal > or >>\n");
				return 0;
			}
			j->out_fd = -1;
			append = token == T_GGREAT;
			continue;
		}
		else if (token == T_PIPE || token == T_BG || token == T_SEMI || token == T_NLINE)
		{
			p->argv[argc] = NULL;
			if (token == T_PIPE)
			{
				if (j->out_fd != STDOUT_FILENO)
				{
					ft_printf("> or >> conflicts with |\n");
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
				//new job
				//ft_printf("SEMI\n");
				j->next = (t_job*)ft_memalloc(sizeof(t_job));
				p->next = NULL;
				j = j->next;
				j->first_process = (t_process*)ft_memalloc(sizeof(t_process));
				p = j->first_process;
				argc = 0;
				continue;
			}
			if (makepipe)
			{
				if (pipe(pfd) == -1)
				{
					perror("pipe()");
					return 0;
				}
					p->in_fd = pfd[0];
					p->out_fd = pfd[1];
			}
			if (argc == 0 && (token != T_NLINE || j->in_fd > 1))
			{
				ft_printf("missing command\n");
				return 0;
			}
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
