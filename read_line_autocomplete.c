#include "shell.h"
#include <dirent.h>
/*
	указатель на статический список в стр.поз.
	если нажата не ТАБ - удаляем список
	иначе - подставляем активный
		если нет списка - создаем
		(если нет активного - то первый)
		иначе - передвигаем на след.

*/


void	push_compl(t_compl **head, char *name)
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

void	clear_compl(t_compl *head)
{
	//ft_printf(">>>\t%s\n", __FUNCTION__);
	t_compl *prev;
	
	while (head)
	{
		prev = head;
		head = head->next;
		free(prev);
		prev = NULL;
	}

}

void	print_list(t_compl *head)
{
	t_compl *ptr;

	ptr = head;
	while (ptr)
	{
		if (ptr->name)
			ft_printf("%s\t", ptr->name);
		ptr = ptr->next;
	}
}

int		is_directory(char *name)
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

void	ft_read_dir(t_compl **head, char *name, char *begin, char dironly)
{
	struct dirent	*info;
	DIR		*dirp;
	size_t	len = ft_strlen(begin);

	dirp = opendir(name);
	if (dirp == NULL)
	{
		return;
	}
	while ((info = readdir(dirp)))
	{
		if (info->d_name[0] == '.')
			continue;
		//ft_printf("> begin: %s, file: %s, len: %d\n", begin, info->d_name, len);
		if (len == 0 || ft_strnequ(info->d_name, begin, len))
		{

			if (!dironly)
				push_compl(head, info->d_name);
			else
			{
				if (is_directory(info->d_name))
					push_compl(head, info->d_name);
			}
		}
	}
	closedir(dirp);
}

void	read_path(t_compl **head, char *begin)
{
//--------------------------------------------------- copy PATH
	char **path_arr;
	char *env_path;
	int i = 0;

	if (!(env_path = get_copy_env("PATH", NORM)))
		return ;
	if (!(path_arr = ft_strsplit(env_path, ':')))
		return ;
//--------------------------------------------------- read PATH
	while(path_arr[i])
	{
		ft_read_dir(head, path_arr[i], begin, 0);
		++i;
	}
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
		while (j && line[j] != ' ' && line[j] != '\n'
			&& line[j] != '\\' && line[j] != '|'  && line[j] != '/'
			&& line[j] != '&'  && line[j] != '$'  && line[j] != '>'
			&& line[j] != '<'  && line[j] != ',')
			{
				--j;
			}
		*begin = ft_strsub(line, j, pos->i);
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

char	*find_active(t_compl *head)
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
			if (ptr->name)
				str = ptr->name;
			ptr->active = 0;
			if (ptr->next)
				ptr->next->active = 1;
			else
				head->active = 1;
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

void	complete(char *line, t_cpos *pos, char *begin)
{
	char	*str;
	int		len;
	int		start;

	if (!line || !begin)
		ft_printf("Error\n");
	len = ft_strlen(begin);
	start = pos->i - len;
	
	str = find_active(pos->autocompl);
	if (str)
	{
		
		ft_strclr(&line[start]);
		ft_strcpy(&line[start], str);
		tputs(tgetstr("sc", NULL), 0, ft_iputchar);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, &line[start + len], ft_strlen(line) + ft_strlen(str));
		tputs(tgetstr("rc", NULL), 0, ft_iputchar);
		
		/*ft_printf("> %s\n", str);*/
		pos->i += ft_strlen(str) - len;
		pos->len += ft_strlen(str) - len;
	}
	//ft_printf(">line: %s\n", line);
}

void	ft_autocomplete(char *line, t_cpos *pos)
{
	t_compl *head = NULL;
	char *begin = NULL;

	if (!pos->autocompl)
	{
		if (!(head = (t_compl*)ft_memalloc(sizeof(t_compl))))
		{
			ft_putstr_fd("Out of memory\n", STDERR_FILENO);
			return ;
		}
		if (pos->len == 0)
		{
			//	show PATH
			read_path(&head, "");
		}
		else
		{
			if (is_first_word(line, pos, &begin))
			{
				//show PATH and directories in current dir that fit to begin
				read_path(&head, begin);
				ft_read_dir(&head, get_current_wd(), begin, 1);
			}
			else
			{
				//show all current dir	
				ft_read_dir(&head, get_current_wd(), begin, 0);
			}
		}
		pos->autocompl = head;
		pos->bgn = ft_strdup(begin);
	}
	//ft_printf("> BEGIN: %s\n", pos->bgn);
	complete(line, pos, pos->bgn);
	//print_list(head);
	//clear_compl(head);
}