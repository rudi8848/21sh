/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_help.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 13:13:45 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:13:50 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	ft_restore(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_saved);
}

void	free_job(t_job *j)
{
	int			i;
	t_process	*pprev;

	i = 0;
	if (j)
	{
		while (j->first_process)
		{
			pprev = j->first_process;
			j->first_process = j->first_process->next;
			i = 0;
			while (pprev->argv[i])
			{
				free(pprev->argv[i]);
				i++;
			}
			free(pprev);
		}
		free(j);
		j = NULL;
	}
}

void	ft_exit(void)
{
	ft_restore();
	exit(EXIT_SUCCESS);
}

int		ft_iputchar(int c)
{
	return (write(1, &c, 1));
}

void	check_history_capacity(void)
{
	if (g_hstr_nb == MAXHSTR - 1)
	{
		ft_printf("History file is full\n");
		ft_exit();
	}
}
