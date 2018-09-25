#include "21sh.h"

void	init_job(t_job *j)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	j->in_fd = STDIN_FILENO;
	j->out_fd = STDOUT_FILENO;
	j->err_fd = STDERR_FILENO;
	j->foreground = 1;
	if (shell_is_interactive)
	{
		if ((tcgetattr(STDOUT_FILENO, &j->tmodes)) == -1)
	    {
	        ft_putstr_fd("Cannot get terminal settings\n", STDERR_FILENO);
	        exit(EXIT_FAILURE);
	    }
	}
	ft_bzero(j->srcfile, sizeof(j->srcfile));
	ft_bzero(j->dstfile, sizeof(j->dstfile));
	j->flags = O_WRONLY | O_CREAT | O_CLOEXEC;
	if (!(j->first_process = (t_process*)ft_memalloc(sizeof(t_process))))
	{
		ft_printf("Memory error\n");
		exit(EXIT_FAILURE);
	}
}

void	push_back(t_job *elem)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_job *ptr;

	if (!first_job)
	{
		first_job = elem;
		return;
	}
	ptr = first_job;
	while (ptr->next)
		ptr = ptr->next;
	ptr->next = elem;
}

int	pack_args(char *line, t_job *j)
{
	//ft_printf("---> %s\n", __FUNCTION__);
	t_token	token;
	t_token	tkn;
	char word[MAXWORD];
	int argc;
	t_process *p;
	int makepipe = 0;
	int i = 0;
	argc = 0;

	init_job(j);
	push_back(j);
	p = j->first_process;
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
				//	CLEAN ALL
				return 0;
			}
			continue;
		}
		else if (token == T_GREAT || token == T_GGREAT)
		{
			if (j->out_fd != STDOUT_FILENO)
			{
				ft_printf("\nExtra > or >>\n");
				//	CLEAN ALL
				return 0;
			}
			if ((tkn = ft_gettoken(line, &i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
			{
				//	!!!		check file description arrregation here 
				if (tkn == T_BG)
				{
					if ((tkn = ft_gettoken(line, &i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
					{
						ft_printf("ERROR\n");
						return 0;
					}
					if (ft_strequ("-", j->dstfile))
					{
						int nbr = ft_atoi(p->argv[argc - 1]);
						if (nbr == j->in_fd)
							close(j->in_fd);
						else if (nbr == j->out_fd)
							close(j->out_fd);
						else if (nbr == j->err_fd)
							close(j->err_fd);
						else
						{
							ft_printf("Bad file descriptor: %d\n", nbr);
							return 0;
						}
						//ft_printf("<<< need to close %s >>>\n", p->argv[argc - 1]);
					}
					else if (ft_isdigit(j->dstfile[0]))
					{
						//j->out_fd = ft_atoi(j->dstfile);
						if (ft_isdigit(p->argv[argc - 1][0]))
						{
							int nbr = ft_atoi(p->argv[argc - 1]);
							if (nbr == j->in_fd)
								j->in_fd = ft_atoi(j->dstfile);
							else if (nbr == j->out_fd)
								j->out_fd = ft_atoi(j->dstfile);
							else if (nbr == j->err_fd)
								j->err_fd = ft_atoi(j->dstfile);
							else
							{
								ft_printf("Bad file descriptor: %d\n", nbr);
								return 0;
							}
							argc--;
							free(p->argv[argc]);
							p->argv[argc] = NULL;
						}
						else	//	p->argv[argc]	not digit
						{
							j->out_fd = ft_atoi(j->dstfile);
						}
							//	compare atoi(p->argv[argc-1]) with j->in_fd, out, err, if fits - 
							// change it to atoi(j->dstfile), remove p->argv[argc -1] and argc--
							// if p->argv[argc -1] is not number need to redirect STDOUT_FILENO
							// and don't touch argv[argc -1 ] and argc
							//int nbr = ft_atoi(p->argv)
							//ft_printf("<<< %s is redirected to %s >>>\n", p->argv[argc - 1], j->dstfile);
					}
					else
					{
						ft_printf("ERROR\n");
						return 0;
					}
					continue;
				}
				else
				ft_printf("\nIllegal > or >>\n");
				//	CLEAN ALL
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
					//	CLEAN ALL
					return 0;
				}
				p->next  = (t_process*)ft_memalloc(sizeof(t_process));
				p = p->next;
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
					argc = 0;
					init_job(j);
					p = j->first_process;
					makepipe = 0;
					continue;
				}
				else
					return 1;
			}
			if (argc == 0 && (token != T_NLINE || j->in_fd /*> 1*/ != -1))
			{
				ft_printf("Missing command\n");
				//	CLEAN ALL
				return 0;
			}
			else if (argc == 0 && token == T_NLINE)
				//	CLEAN ALL
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
