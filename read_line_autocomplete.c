#include "shell.h"

int	ft_read_dir(t_compl **head, char *name, char *begin, char dironly)
{
	struct dirent	*info;
	DIR		*dirp;
	size_t	len = ft_strlen(begin);
	int ret = 0;

	dirp = opendir(name);
	if (dirp == NULL)
		return 0;
	while ((info = readdir(dirp)))
	{
		if (len == 0 || ft_strnequ(info->d_name, begin, len))
		{

			if (!dironly)
			{
				if (len == 0 && info->d_name[0] == '.')
					continue;
				push_compl(head, info->d_name);
				++ret;
			}
			else
			{
				if (is_directory(info->d_name))
				{
					push_compl(head, info->d_name);
					ret++;
				}
			}
		}
	}
	closedir(dirp);
	return ret;
}

int	read_path(t_compl **head, char *begin)
{
	char **path_arr;
	char *env_path;
	int i = 0;
	int ret = 0;

	if (!(env_path = get_copy_env("PATH", NORM)))
		return 0;
	if (!(path_arr = ft_strsplit(env_path, ':')))
		return 0;
	while(path_arr[i])
	{
		ret += ft_read_dir(head, path_arr[i], begin, 0);
		++i;
	}
	free_arr(path_arr);
	return ret;
}

int		is_first_word(char *line, t_cpos *pos, char **begin)
{
	int j;
	int b;

	if (pos->i > 0)
	{
		j = pos->i;
		if (!line[j])
			--j;
		while (j > 0)
		{
			if (line[j] == ' ' || line[j] == '\n'
			|| line[j] == '\\' || line[j] == '|'  || line[j] == '/'
			|| line[j] == '&'  || line[j] == '$'  || line[j] == '>'
			|| line[j] == '<'  || line[j] == ',')
			{
				++j;
				break;
			}
			--j;
		}
		if (!(*begin = ft_strsub(line, j, pos->i)) || !ft_strlen(*begin))
			return ERROR;
	}
	b = 0;
	while (line[b] && b < j)
	{
		if (line[b] != ' ' && line[b] != '\n'
			&& line[b] != '\\' && line[b] != '|'  && line[b] != '/'
			&& line[b] != '&'  && line[b] != '$'  && line[b] != '>'
			&& line[b] != '<'  && line[b] != ',')
			return (0);
		++b;
	}
	return (1);
}

void	ft_autocomplete(char *line, t_cpos *pos)
{
	t_compl *head = NULL;
	char *begin = NULL;
	int ret = 0;
	int first;

	if (!pos->autocompl)
	{
		if (!(head = (t_compl*)ft_memalloc(sizeof(t_compl))))
		{
			ft_putstr_fd("Out of memory\n", STDERR_FILENO);
			return ;
		}
		head->next = NULL;
		if (pos->len == 0)
			ret = read_path(&head, "");
		else
		{
			if ((first = is_first_word(line, pos, &begin) == 1))
			{
				ret = read_path(&head, begin);
				ret += ft_read_dir(&head, get_current_wd(), begin, 1);
			}
			else if (first == ERROR)
				ret = 0;
			else
				ret = ft_read_dir(&head, get_current_wd(), begin, 0);
		}
		if (!ret)
		{
			free(head);
			head = NULL;
			pos->autocompl = NULL;
			if (begin && ft_strlen(begin))
			{
				free(begin);
				begin = NULL;
			}
			return ;
		}
		else
		{
			pos->autocompl = head;
			pos->bgn = ft_strdup(begin);
		}
	}
	//if (pos->bgn)
		complete(line, pos, pos->bgn);
}