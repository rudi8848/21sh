#include "shell.h"
#include <dirent.h>
/*
void	ft_read_info(char *prefix, char *name)
{
	char *path;
	path = ft_strjoin(prefix, name);
	ft_printf("%s\t", ft_strrchr(path, '/') + 1);
}
*/
void	ft_read_dir(char *name, char *begin)
{
	struct dirent	*info;
	DIR		*dirp;
//	char		*prefix;
	//ft_printf(">>>\t %s:\t\n", name);
	dirp = opendir(name);
	if (dirp == NULL)
	{
//		perror("cannot open directory");
		return;
	}
	while ((info = readdir(dirp)))
	{
		if (info->d_name[0] == '.')
			continue;
	//	ft_read_info(prefix, info->d_name);
		if (ft_strnequ(info->d_name, begin, ft_strlen(begin)))
			ft_printf("%s\t", info->d_name);
	}
//	ft_printf("\n\t------\n");
	closedir(dirp);
}

void	ft_autocomplete(char *line, t_cpos *pos)
{
//	static int is_pressed = 0;
if (pos == NULL || line == NULL)
{
	ft_printf("error\n");
	return;
}
	char **path_arr;
	char *env_path;
	//char *tmp = NULL;
	char *word = NULL;
	
	int j = 0;
	if (pos->i > 0)
	{
		j = pos->i;
		if (!line[j])
			--j;
		while (j && line[j] != ' ')
			--j;
		word = ft_strsub(line, j, pos->i);
	}
	if (!(env_path = get_copy_env("PATH", NORM)))
		return ;
//	ft_printf("PATH: %s\n", env_path);
	if (!(path_arr = ft_strsplit(env_path, ':')))
		return ;
//	char *ptr = path_arr[0];
//	ft_printf("PTR: %s\n", ptr);
	int i = 0;
	while(path_arr[i])
	{
		ft_read_dir(path_arr[i], word);
		++i;
	}
}
