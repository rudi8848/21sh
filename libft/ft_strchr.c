/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 14:21:16 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 14:22:11 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char	*ft_strchr(const char *str, int c)
{
	char			*ptr;
	unsigned char	find;
	int				i;

	i = 0;
	ptr = (char*)str;
	find = (unsigned char)c;
	if (find == '\0')
		return (ptr + ft_strlen(str));
	else
	{
		while (str[i])
		{
			if (ptr[i] == find)
				return (ptr + i);
			i++;
		}
	}
	return (NULL);
}
