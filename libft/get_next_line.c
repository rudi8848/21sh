/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/25 12:50:56 by gvynogra          #+#    #+#             */
/*   Updated: 2017/12/25 13:02:20 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static t_list		*fd_search(t_list **head, int fd)
{
	t_list			*tmp;

	tmp = *head;
	while (tmp)
	{
		if ((size_t)fd == tmp->content_size)
			return (tmp);
		tmp = tmp->next;
	}
	tmp = ft_lstnew("", fd);
	ft_lstadd(head, tmp);
	tmp = (*head);
	return (tmp);
}

static int			read_file(int const fd, void **data)
{
	char			buff[BUFF_SIZE + 1];
	int				read_bytes;
	char			*new_buff;

	new_buff = NULL;
	read_bytes = read(fd, buff, BUFF_SIZE);
	if (read_bytes > 0)
	{
		buff[read_bytes] = '\0';
		new_buff = ft_strjoin(*data, buff);
		if (!new_buff)
			return (-1);
		free(*data);
		*data = new_buff;
	}
	return (read_bytes);
}

static int			cut_content(t_list **file, char **buff)
{
	int				bytes;

	bytes = 0;
	*buff = ft_strchr((*file)->content, '\n');
	while (*buff == NULL)
	{
		bytes = read_file((*file)->content_size, &(*file)->content);
		if (bytes == 0)
		{
			if ((*buff = ft_strchr((char*)(*file)->content, '\0'))
					== (char*)(*file)->content)
				return (0);
		}
		else if (bytes < 0)
			return (-1);
		else
			*buff = ft_strchr((*file)->content, '\n');
	}
	return (1);
}

int					get_next_line(int const fd, char **line)
{
	static t_list	*file;
	t_list			*tmp;
	char			*remainder;
	int				ret;

	if ((!line || !(*line = ft_strnew(1))) || (fd < 0) ||
		((read(fd, 0, 0)) < 0))
		return (-1);
	tmp = fd_search(&file, fd);
	if ((!tmp->content && !((tmp->content = ft_memalloc(sizeof(char))))))
		return (-1);
	ret = cut_content(&tmp, &remainder);
	if (ret != 1)
		return (ret);
	free(*line);
	if (!(*line = ft_strsub(tmp->content, 0, remainder - (char*)tmp->content)))
		return (-1);
	remainder = ft_strdup(remainder + 1);
	free(tmp->content);
	tmp->content = remainder;
	return (1);
}
