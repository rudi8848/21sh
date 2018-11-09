#include "21sh.h"

void	init_job(t_job *j)
{
	j->in_fd = STDIN_FILENO;
	j->out_fd = STDOUT_FILENO;
	j->err_fd = STDERR_FILENO;
	j->foreground = 1;
	j->nbr = 0;
	j->next = NULL;
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

void	init_pack(t_pack *pack)
{
	pack->i = 0;
	pack->argc = 0;
	pack->makepipe = 0;
}

int remove_invalid_job(t_job *j, t_process *p, t_pack *pack)
{
	t_job *prev;

	p->argv[pack->argc] = NULL;
	if (j == first_job)
		first_job = NULL;
	else
	{
		prev = first_job;
		while(prev->next != j)
			prev = prev->next;
		prev->next = j->next;
	}
	free_job(j);
	return 0;
}


int pack_word(t_process *p, t_pack *pack)
{
	int ret;

	if (pack->argc >= MAXARG - 1)
	{
		ft_printf("Too many args\n");
		ret = CONTINUE;
	}
	if (!(p->argv[pack->argc] = ft_strdup(pack->word)))
	{
		ft_printf("Out of memory\n");
		ret = CONTINUE;
	}
	++pack->argc;
	return CONTINUE;
}

int pack_less(t_job *j, t_process *p, char *line, t_pack *pack)
{
	if (pack->makepipe)
	{
		ft_printf("Extra <\n");
		return remove_invalid_job(j, p, pack);
	}
	if (ft_gettoken(line, &pack->i, j->srcfile, sizeof(j->srcfile)) != T_WORD)	// OR BG
	{
		ft_printf("Illegal <\n");
		return remove_invalid_job(j, p, pack);
	}
	j->in_fd = -1;
	return CONTINUE;
}

int pack_heredoc(t_job *j, t_process *p, char *line, t_pack *pack)
{
	if (ft_gettoken(line, &pack->i, pack->word, sizeof(pack->word)) != T_WORD)
	{
		ft_printf("Error near << \n");
		return remove_invalid_job(j, p, pack);
	}
	t_process *prev = ft_memalloc(sizeof(t_process));
	prev->argv[0] = ft_strdup("heredoc");
	prev->argv[1] = ft_strjoin(pack->word, "\n");
	if (p == j->first_process)
	{
		prev->next = p;
		j->first_process = prev;
	}
	else
	{
		ft_printf("Wrong input for heredoc\n");
		return remove_invalid_job(j, p, pack);
	}
	return CONTINUE;
}

int pack_great(t_job *j, t_process *p, char *line, t_pack *pack)
{
// --- check errors
	if (j->out_fd != STDOUT_FILENO)
	{
		ft_printf("\nExtra > or >>\n");
		return remove_invalid_job(j, p, pack);
	}
	if (!pack->argc)
	{
		ft_printf("Error\n");
		return remove_invalid_job(j, p, pack);
	}
	// --- if next is not name of file
	if ((pack->tkn = ft_gettoken(line, &pack->i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
	{
		//	--- redirection fd to fd or close
		if (pack->tkn == T_BG)
		{
			if ((pack->tkn = ft_gettoken(line, &pack->i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
			{
				ft_printf("ERROR\n");
					return remove_invalid_job(j, p, pack);
			}
			if (ft_strequ("-", j->dstfile))
			{
				int nbr;
				if (ft_isdigit(p->argv[pack->argc - 1][0]))
					nbr = ft_atoi(p->argv[pack->argc - 1]);
				else
					nbr = 1;
				if (nbr == j->in_fd)
				{
					;//j->in_fd = -1;
					//ft_strcpy(j->srcfile, "/dev/null");
				}
				else if (nbr == j->out_fd)
				{
					j->out_fd = -1;
					ft_strcpy(j->dstfile, "/dev/null");
				}
				else if (nbr == j->err_fd)
				{
					j->err_fd = -1;
					ft_strcpy(j->errfile, "/dev/null");
				}
				else
				{
					ft_printf("Bad file descriptor: %d\n", nbr);
					return remove_invalid_job(j, p, pack);
				}
				if (ft_isdigit(p->argv[pack->argc - 1][0]))
				{
					--pack->argc;
					free(p->argv[pack->argc]);
					p->argv[pack->argc] = NULL;
				}
			}
			else if (ft_isdigit(j->dstfile[0]))
			{
				//j->out_fd = ft_atoi(j->dstfile);
				if (ft_isdigit(p->argv[pack->argc - 1][0]))
				{
					int nbr = ft_atoi(p->argv[pack->argc - 1]);
					if (nbr == j->in_fd)
						;//j->in_fd = ft_atoi(j->dstfile);
					else if (nbr == j->out_fd)
						j->out_fd = ft_atoi(j->dstfile);
					else if (nbr == j->err_fd)
						j->err_fd = ft_atoi(j->dstfile);
					else
					{
						ft_printf("Bad file descriptor: %d\n", nbr);
						return remove_invalid_job(j, p, pack);
					}
					--pack->argc;
					free(p->argv[pack->argc]);
					p->argv[pack->argc] = NULL;
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
				return remove_invalid_job(j, p, pack);
			}
			return CONTINUE;
		}
		else
			ft_printf("\nIllegal > or >>\n");
			return remove_invalid_job(j, p, pack);
			}
		if (ft_isdigit(p->argv[pack->argc - 1][0]))
			{
				int nbr = ft_atoi(p->argv[pack->argc - 1]);
				if (nbr == j->in_fd)
					j->in_fd = open(j->dstfile, j->flags, FILE_PERM);
				else if (nbr == j->out_fd)
					j->out_fd = -1;
				else if (nbr == j->err_fd)
					j->err_fd = open(j->dstfile, j->flags, FILE_PERM);
				else
				{
					ft_printf("Bad file descriptor: %d\n", nbr);
					return remove_invalid_job(j, p, pack);
				}
				--pack->argc;
				free(p->argv[pack->argc]);
				p->argv[pack->argc] = NULL;
			}
			//-----------
			j->out_fd = -1;
			if ( pack->token == T_GGREAT)
				j->flags |= O_APPEND;
			else
				j->flags |= O_TRUNC;
			return CONTINUE;
}

int pack_end(t_job *j, t_process *p, char *line, t_pack *pack)
{
	p->argv[pack->argc] = NULL;
	if (pack->token == T_BG)
	{
		j->foreground = 0;
		return CONTINUE;
	}
	if (pack->token == T_PIPE)
	{
		if (j->out_fd != STDOUT_FILENO)
			return remove_invalid_job(j, p, pack);
		p->next  = (t_process*)ft_memalloc(sizeof(t_process));
		p = p->next;
		pack->makepipe = 1;
		pack->argc = 0;
		return CONTINUE;
	}
	if (pack->token == T_SEMI)
	{
		if (line[pack->i] && line[pack->i] != '\n')
		{
			j->next = (t_job*)ft_memalloc(sizeof(t_job));
			p->next = NULL;
			j = j->next;
			pack->argc = 0;
			init_job(j);
			p = j->first_process;
			pack->makepipe = 0;
			return CONTINUE;
		}
		else
			return 1;
	}
	if (pack->argc == 0 && (pack->token != T_NLINE || j->in_fd != -1))
	{
		ft_printf("Missing command\n");
		return remove_invalid_job(j, p, pack);
	}
	else if (pack->argc == 0 && pack->token == T_NLINE)
		return remove_invalid_job(j, p, pack);
	return 1;
}

int	pack_args(char *line, t_job *j)
{
	/*
	t_token	token;
	t_token	tkn;
	char word[MAXWORD];
	int argc;
	int makepipe;
	int i;
	*/
	t_process *p;
	t_pack	pack;
	int ret;
/*
	i = 0;
	argc = 0;
	makepipe = 0;
*/
	init_job(j);
	push_back(j);
	init_pack(&pack);
	p = j->first_process;
	while (1)
	{
		ft_bzero(pack.word, sizeof(pack.word));
		pack.token = ft_gettoken(line, &pack.i, pack.word, sizeof(pack.word));
//--- T_WORD
		if (pack.token == T_WORD)
		{
			pack_word(p, &pack);
			continue;
		}
//----------------------------------
//--- T_LESS
		else if (pack.token == T_LESS)
		{
			/*
			if (pack.makepipe)
			{
				ft_printf("Extra <\n");
				return remove_invalid_job(j, p, &pack);
			}
			if (ft_gettoken(line, &pack.i, j->srcfile, sizeof(j->srcfile)) != T_WORD)	// OR BG
			{
				ft_printf("Illegal <\n");
				return remove_invalid_job(j, p, &pack);
			}
			j->in_fd = -1;
			continue;
			*/
			if (pack_less(j, p, line, &pack) != CONTINUE)
				return (0);
			continue;
		}
//----------------------------------
//--- T_HRDOC
		else if (pack.token == T_HRDOC)
		{
			/*
			if (ft_gettoken(line, &pack.i, pack.word, sizeof(pack.word)) != T_WORD)
			{
				ft_printf("Error near << \n");
				return remove_invalid_job(j, p, &pack);
			}
			t_process *prev = ft_memalloc(sizeof(t_process));
			prev->argv[0] = ft_strdup("heredoc");
			prev->argv[1] = ft_strjoin(pack.word, "\n");
			if (p == j->first_process)
			{
				prev->next = p;
				j->first_process = prev;
			}
			else
			{
				ft_printf("Wrong input for heredoc\n");
				return remove_invalid_job(j, p, &pack);
			}
			*/
			if (pack_heredoc(j, p, line, &pack) != CONTINUE)
				return (0);
			continue;
		}
//----------------------------------
//--- T_GREAT || T_GGREAT
		else if (pack.token == T_GREAT || pack.token == T_GGREAT)
		{
			/*
			// --- check errors
			if (j->out_fd != STDOUT_FILENO)
			{
				ft_printf("\nExtra > or >>\n");
				//	CLEAN ALL
				return remove_invalid_job(j, p, &pack);
			}
			if (!pack.argc)
			{
				ft_printf("Error\n");
				return remove_invalid_job(j, p, &pack);
			}
			// --- if next is not name of file
			if ((pack.tkn = ft_gettoken(line, &pack.i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
			{
				//	--- redirection fd to fd or close
				if (pack.tkn == T_BG)
				{
						if ((pack.tkn = ft_gettoken(line, &pack.i, j->dstfile, sizeof(j->dstfile))) != T_WORD)
						{
							ft_printf("ERROR\n");
							return remove_invalid_job(j, p, &pack);
						}
						if (ft_strequ("-", j->dstfile))
						{
							int nbr;
							if (ft_isdigit(p->argv[pack.argc - 1][0]))
								nbr = ft_atoi(p->argv[pack.argc - 1]);
							else
								nbr = 1;
							if (nbr == j->in_fd)
							{
								;//j->in_fd = -1;
								//ft_strcpy(j->srcfile, "/dev/null");
							}
							else if (nbr == j->out_fd)
							{
								j->out_fd = -1;
								ft_strcpy(j->dstfile, "/dev/null");
							}
							else if (nbr == j->err_fd)
							{
								j->err_fd = -1;
								ft_strcpy(j->errfile, "/dev/null");
							}
							else
							{
								ft_printf("Bad file descriptor: %d\n", nbr);
								return remove_invalid_job(j, p, &pack);
							}
							if (ft_isdigit(p->argv[pack.argc - 1][0]))
							{
								--pack.argc;
								free(p->argv[pack.argc]);
								p->argv[pack.argc] = NULL;
							}

						}
						else if (ft_isdigit(j->dstfile[0]))
						{
							//j->out_fd = ft_atoi(j->dstfile);
							if (ft_isdigit(p->argv[pack.argc - 1][0]))
							{
								int nbr = ft_atoi(p->argv[pack.argc - 1]);
								if (nbr == j->in_fd)
									;//j->in_fd = ft_atoi(j->dstfile);
								else if (nbr == j->out_fd)
									j->out_fd = ft_atoi(j->dstfile);
								else if (nbr == j->err_fd)
									j->err_fd = ft_atoi(j->dstfile);
								else
								{
									ft_printf("Bad file descriptor: %d\n", nbr);
									return remove_invalid_job(j, p, &pack);
								}
								--pack.argc;
								free(p->argv[pack.argc]);
								p->argv[pack.argc] = NULL;
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
							return remove_invalid_job(j, p, &pack);
						}
					continue;
				}
				else
				ft_printf("\nIllegal > or >>\n");
				return remove_invalid_job(j, p, &pack);
				

			
			//-----------
			if (ft_isdigit(p->argv[pack.argc - 1][0]))
			{
				int nbr = ft_atoi(p->argv[pack.argc - 1]);
				if (nbr == j->in_fd)
					j->in_fd = open(j->dstfile, j->flags, FILE_PERM);
				else if (nbr == j->out_fd)
					j->out_fd = -1;
				else if (nbr == j->err_fd)
					j->err_fd = open(j->dstfile, j->flags, FILE_PERM);
				else
				{
					ft_printf("Bad file descriptor: %d\n", nbr);
					return remove_invalid_job(j, p, &pack);
				}
				--pack.argc;
				free(p->argv[pack.argc]);
				p->argv[pack.argc] = NULL;
			}
			//-----------
			j->out_fd = -1;
			if ( pack.token == T_GGREAT)
				j->flags |= O_APPEND;
			else
				j->flags |= O_TRUNC;
			continue;
		}*/
			if (pack_great(j, p, line, &pack) != CONTINUE)
					return (0);
				continue;
			}
//----------------------------------
//--- T_PIPE || T_BG || T_SEMI || T_NLINE
		else if (pack.token == T_PIPE || pack.token == T_BG || pack.token == T_SEMI || pack.token == T_NLINE)
		{
			/*
			p->argv[pack.argc] = NULL;
			if (pack.token == T_BG)
			{
				j->foreground = 0;
				continue;
			}
			if (pack.token == T_PIPE)
			{
				if (j->out_fd != STDOUT_FILENO)
					return remove_invalid_job(j, p, &pack);
				p->next  = (t_process*)ft_memalloc(sizeof(t_process));
				p = p->next;
				pack.makepipe = 1;
				pack.argc = 0;
				continue;
			}
			if (pack.token == T_SEMI)
			{
				if (line[pack.i] && line[pack.i] != '\n')
				{
					j->next = (t_job*)ft_memalloc(sizeof(t_job));
					p->next = NULL;
					j = j->next;
					pack.argc = 0;
					init_job(j);
					p = j->first_process;
					pack.makepipe = 0;
					continue;
				}
				else
					return 1;
			}
			if (pack.argc == 0 && (pack.token != T_NLINE || j->in_fd != -1))
			{
				ft_printf("Missing command\n");
				return remove_invalid_job(j, p, &pack);
			}
			else if (pack.argc == 0 && pack.token == T_NLINE)
				return remove_invalid_job(j, p, &pack);
			return 1;
			*/
			if ((ret = pack_end(j, p, line, &pack)) != CONTINUE)
					return (ret);
				continue;

		}
//----------------------------------
		else if (pack.token == T_EOF || pack.token == T_ERROR)
			return remove_invalid_job(j, p, &pack);
//----------------------------------
	}
}
