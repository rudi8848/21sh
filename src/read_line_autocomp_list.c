/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_autocomp_list.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/07 14:52:57 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 14:52:59 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void		push_compl(t_compl **head, char *name)
{
	t_compl	*tmp;

	if (!*head)
		return ;
	if (!(tmp = (t_compl*)ft_memalloc(sizeof(t_compl))))
	{
		ft_putstr_fd("Out of memory\n", STDERR_FILENO);
		return ;
	}
	tmp->name = ft_strdup(name);
	tmp->active = 0;
	tmp->next = *head;
	*head = tmp;
}

void		clear_compl(t_compl **head)
{
	t_compl *prev;

	while (*head)
	{
		prev = *head;
		*head = (*head)->next;
		if (prev->name)
		{
			free(prev->name);
			prev->name = NULL;
		}
		free(prev);
		prev = NULL;
	}
	if (*head)
		free(*head);
	*head = NULL;
}
