/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help_func.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 16:20:20 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 16:20:29 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char	**ft_cp_array(char **src)
{
	char	**dest;
	int		i;
	int		n;

	i = 0;
	n = 0;
	while (src[n] != NULL)
		n++;
	dest = (char **)ft_memalloc(sizeof(char *) * (n + 1));
	if (!dest)
		return (NULL);
	while (i < n)
	{
		dest[i] = ft_strdup(src[i]);
		i++;
	}
	dest[n] = NULL;
	return (dest);
}

void	free_arr(char **array)
{
	int	i;

	i = 0;
	while (array[i] != NULL)
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	array = NULL;
}

char	*get_current_wd(void)
{
	char	buf[2048 + 1];
	char	*dest;

	dest = getcwd(buf, 2048);
	return (dest);
}
