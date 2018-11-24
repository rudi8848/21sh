/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_copy_paste.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/10 11:18:57 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/10 11:21:33 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	ft_copy(char *line, t_cpos *pos, char **buf)
{
	int	len;

	len = pos->last - pos->first;
	if (*buf)
		free(*buf);
	*buf = ft_strsub(line, pos->first, len);
}

void	ft_paste(char *line, t_cpos *pos, char **buf)
{
	char	*tmp;
	int		dif;

	if (!*buf)
		return ;
	if (pos->len + ft_strlen(*buf) >= MAXLINE)
	{
		ft_printf("line is too long\n");
		return ;
	}
	dif = ft_strlen(*buf);
	tmp = ft_strsub(line, pos->i, pos->len - pos->i);
	ft_strcpy(&line[pos->i], *buf);
	pos->i += dif;
	pos->len += dif;
	ft_strcpy(&line[pos->i], tmp);
	pos->height = cmd_height(pos, line);
	cursor_to_i(pos);
	reset_selection(pos, line);
	free(tmp);
	free(*buf);
	*buf = NULL;
}

void	ft_cut(char *line, t_cpos *pos, char **buf)
{
	char	*tmp;
	int		len;

	len = pos->last - pos->first;
	if (*buf)
		free(*buf);
	*buf = ft_strsub(line, pos->first, len);
	tmp = ft_strsub(line, pos->last, pos->len - pos->last);
	ft_bzero(&line[pos->first], pos->len);
	ft_strcpy(&line[pos->first], tmp);
	pos->len -= ft_strlen(*buf);
	pos->i -= ft_strlen(*buf);
	cursor_to_i(pos);
	reset_selection(pos, line);
	free(tmp);
}

void	ft_copy_paste(uint64_t rb, char *line, t_cpos *pos)
{
	static char *buf = NULL;

	if (rb == K_ALT_C)
		ft_copy(line, pos, &buf);
	else if (rb == K_ALT_V)
		ft_paste(line, pos, &buf);
	else if (rb == K_ALT_X)
		ft_cut(line, pos, &buf);
}
