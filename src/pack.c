/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pack.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:50:56 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:52:06 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void		init_job(t_job *j)
{
	j->in_fd = STDIN_FILENO;
	j->out_fd = STDOUT_FILENO;
	j->err_fd = STDERR_FILENO;
	j->foreground = 1;
	j->nbr = 0;
	j->next = NULL;
	if (g_shell_is_interactive)
	{
		if ((tcgetattr(STDOUT_FILENO, &j->tmodes)) == -1)
		{
			ft_putstr_fd("21sh: Cannot get terminal settings\n", STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
	}
	ft_bzero(j->srcfile, sizeof(j->srcfile));
	ft_bzero(j->dstfile, sizeof(j->dstfile));
	j->flags = O_WRONLY | O_CREAT | O_CLOEXEC;
	if (!(j->first_process = (t_process*)ft_memalloc(sizeof(t_process))))
	{
		ft_putstr_fd("21sh: Memory error\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

static void	push_back(t_job *elem)
{
	t_job *ptr;

	if (!g_first_job)
	{
		g_first_job = elem;
		return ;
	}
	ptr = g_first_job;
	while (ptr->next)
		ptr = ptr->next;
	ptr->next = elem;
}

static void	init_pack(t_pack *pack)
{
	pack->i = 0;
	pack->argc = 0;
	pack->makepipe = 0;
}

int			remove_invalid_job(t_job *j, t_process *p, t_pack *pack)
{
	t_job *prev;

	p->argv[pack->argc] = NULL;
	if (j == g_first_job)
		g_first_job = NULL;
	else
	{
		prev = g_first_job;
		while (prev->next != j)
			prev = prev->next;
		prev->next = j->next;
	}
	free_job(j);
	return (0);
}

int			pack_args(char *line, t_job *j)
{
	t_process	*p;
	t_pack		pack;
	int			ret;

	init_job(j);
	push_back(j);
	init_pack(&pack);
	p = j->first_process;
	while (1)
	{
		ft_bzero(pack.word, sizeof(pack.word));
		pack.token = ft_gettoken(line, &pack.i, pack.word, sizeof(pack.word));
		if (pack.token == T_WORD)
			pack_word(p, &pack);
		else if ((ret = check_tokens(&j, &p, line, &pack)) != CONTINUE)
			return (ret);
		else if (pack.token == T_EOF || pack.token == T_ERROR)
			return (remove_invalid_job(j, p, &pack));
		continue ;
	}
}
