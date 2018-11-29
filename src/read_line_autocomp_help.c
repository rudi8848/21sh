/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_autocomp_help.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/15 10:45:36 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/15 10:47:24 by gvynogra         ###   ########.fr       */
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

static void	set_active(t_compl *head, t_compl *ptr, char **str)
{
	if (ptr->name)
		*str = ptr->name;
	ptr->active = 0;
	if (ptr->next)
		ptr->next->active = 1;
	else
		head->active = 1;
}

static char	*find_active(t_compl *head)
{
	t_compl *ptr;
	char	*str;
	int		find;

	find = 0;
	str = NULL;
	ptr = head;
	while (ptr)
	{
		if (ptr->active)
		{
			find = 1;
			set_active(head, ptr, &str);
			if (str)
				return (str);
		}
		ptr = ptr->next;
	}
	if (!find)
	{
		str = head->name;
		head->active = 1;
	}
	return (str);
}

void		complete(char *line, t_cpos *pos, char *begin)
{
	char	*str;

	if (!pos->is_auto)
	{
		pos->autolen = ft_strlen(begin);
		pos->autostart = pos->i - pos->autolen;
		pos->is_auto = 1;
	}
	str = find_active(pos->autocompl);
	if (str)
	{
		pos->len = pos->autostart + pos->autolen;
		while (pos->i != pos->len)
		{
			move_left(pos, ON_SCREEN);
			--pos->i;
		}
		ft_strclr(&line[pos->autostart]);
		ft_strcpy(&line[pos->autostart], str);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		pos->len += ft_strlen(str) - pos->autolen;
		while (pos->i < pos->len)
		{
			write(STDOUT_FILENO, &line[pos->i], 1);
			move_right(pos, IN_MEMORY);
			++pos->i;
		}
	}
}
