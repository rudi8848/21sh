/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_autocomp_read.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/15 11:58:38 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/15 11:58:41 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	is_directory(char *name)
{
	int				ret;
	struct stat		buf;

	ret = lstat(name, &buf);
	if (ret >= 0)
	{
		if (S_ISDIR(buf.st_mode))
			return (1);
	}
	return (0);
}

static int	check_name(t_compl **head, char *name, int len, char dironly)
{
	if (!dironly)
	{
		if (len == 0 && name[0] == '.')
			return (0);
		push_compl(head, name);
		return (1);
	}
	else
	{
		if (is_directory(name))
		{
			push_compl(head, name);
			return (1);
		}
	}
	return (0);
}

int			ft_read_dir(t_compl **head, char *name, char *begin, char dironly)
{
	struct dirent	*info;
	DIR				*dirp;
	size_t			len;
	int				ret;

	len = ft_strlen(begin);
	dirp = opendir(name);
	ret = 0;
	if (dirp == NULL)
		return (0);
	while ((info = readdir(dirp)))
	{
		if (len == 0 || ft_strnequ(info->d_name, begin, len))
			ret += check_name(head, info->d_name, len, dironly);
	}
	closedir(dirp);
	return (ret);
}

int			read_path(t_compl **head, char *begin)
{
	char	**path_arr;
	char	*env_path;
	int		i;
	int		ret;

	i = 0;
	ret = 0;
	if (!(env_path = get_copy_env("PATH", NORM)))
		return (0);
	if (!(path_arr = ft_strsplit(env_path, ':')))
		return (0);
	while (path_arr[i])
	{
		ret += ft_read_dir(head, path_arr[i], begin, 0);
		++i;
	}
	free_arr(path_arr);
	return (ret);
}
