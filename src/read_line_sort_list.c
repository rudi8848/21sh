/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_sort_list.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/21 13:25:03 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/21 13:25:11 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

t_compl			*sort_list(t_compl *a, t_compl *head)
{
	t_compl			*b;
	t_compl			*c;

	while (head != NULL)
	{
		if (head->next != NULL)
			b = head;
		else
			return (a);
		head = head->next;
		if (a == NULL || ft_strcmp(b->name, a->name) < 0)
		{
			b->next = a;
			a = b;
		}
		else
		{
			c = a;
			while (c->next && !(ft_strcmp(b->name, c->next->name) < 0))
				c = c->next;
			b->next = c->next;
			c->next = b;
		}
	}
	return (a);
}