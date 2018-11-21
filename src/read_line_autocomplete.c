/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_autocomplete.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/15 10:47:46 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/15 11:57:45 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

static void	find_start(char *line, int *j)
{
	if (!line[*j])
		--(*j);
	while (*j > 0)
	{
		if (line[*j] == ' ' || line[*j] == '\n'
				|| line[*j] == '\\' || line[*j] == '|' || line[*j] == '/'
				|| line[*j] == '&' || line[*j] == '$' || line[*j] == '>'
				|| line[*j] == '<' || line[*j] == ',')
		{
			++(*j);
			break ;
		}
		--(*j);
	}
}

static int	is_first_word(char *line, t_cpos *pos, char **begin)
{
	int j;
	int b;

	if (pos->i > 0)
	{
		j = pos->i;
		find_start(line, &j);
		if (!(*begin = ft_strsub(line, j, pos->i)) || !ft_strlen(*begin))
			return (ERROR);
	}
	b = 0;
	while (line[b] && b < j)
	{
		if (line[b] != ' ' && line[b] != '\n'
				&& line[b] != '\\' && line[b] != '|' && line[b] != '/'
				&& line[b] != '&' && line[b] != '$' && line[b] != '>'
				&& line[b] != '<' && line[b] != ',')
			return (0);
		++b;
	}
	return (1);
}

static int	search_vars(char *line, t_cpos *pos, t_compl **head, char **begin)
{
	int ret;
	int first;

	if ((first = is_first_word(line, pos, begin) == 1))
	{
		ret = read_path(head, *begin);
		ret += ft_read_dir(head, get_current_wd(), *begin, 1);
	}
	else if (first == ERROR)
		ret = 0;
	else
		ret = ft_read_dir(head, get_current_wd(), *begin, 0);
	return (ret);
}

static int	check_result(t_compl **head, char **begin, t_cpos *pos, int ret)
{
	if (ret < 1)
	{
		free(*head);
		*head = NULL;
		pos->autocompl = NULL;
		if (*begin)
		{
			free(*begin);
			*begin = NULL;
		}
		return (RETURN);
	}
	pos->autocompl = *head;
	*head = NULL;
	if (pos->bgn)
		free(pos->bgn);
	if (*begin)
	{
		pos->bgn = ft_strdup(*begin);
		free(*begin);
		*begin = NULL;
	}
	return (0);
}

void		ft_autocomplete(char *line, t_cpos *pos)
{
	t_compl	*head;
	t_compl	*ptr;
	char	*begin;
	int		ret;

	begin = NULL;
	if (!pos->is_auto)
	{
		if (!(head = (t_compl*)ft_memalloc(sizeof(t_compl))))
		{
			ft_putstr_fd("Out of memory\n", STDERR_FILENO);
			return ;
		}
		ptr = head;
		head->next = NULL;
		if (pos->len == 0)
			ret = read_path(&head, "");
		else
			ret = search_vars(line, pos, &head, &begin);
		if (check_result(&head, &begin, pos, ret) == RETURN)
			return ;
		pos->autocompl = sort_list(head, pos->autocompl);
		free(ptr);
	}
	complete(line, pos, pos->bgn);
}
