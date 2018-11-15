#include "shell.h"

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

void	clear_compl(t_compl **head)
{
	t_compl *prev;
	
	while (*head)
	{
		prev = *head;
		*head = (*head)->next;
		free(prev);
		prev = NULL;
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

	if (!pos->autolen)
	{
		pos->autolen = ft_strlen(begin);
		pos->autostart = pos->i - pos->autolen;
	}
	str = find_active(pos->autocompl);
	if (str)
	{
		pos->i = pos->autostart + pos->autolen;
		pos->len = pos->autostart + pos->autolen;
		cursor_to_i(pos);
		
		ft_strclr(&line[pos->autostart]);
		ft_strcpy(&line[pos->autostart], str);
		tputs(tgetstr("sc", NULL), 0, ft_iputchar);
		tputs(tgetstr("cd", NULL), 0, ft_iputchar);
		write(STDOUT_FILENO, &line[pos->autostart + pos->autolen], ft_strlen(line) + ft_strlen(str));
		tputs(tgetstr("rc", NULL), 0, ft_iputchar);
		pos->len += ft_strlen(str) - pos->autolen;
		move_to_border(K_END, line, pos);
	}
}
