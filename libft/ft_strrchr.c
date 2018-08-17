/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 14:28:27 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 14:30:08 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strrchr(const char *str, int c)
{
	char			*ptr;
	unsigned char	find;
	int				i;

	i = ft_strlen(str);
	ptr = (char*)str;
	find = (unsigned char)c;
	while (i >= 0)
	{
		if (ptr[i] == find)
			return (ptr + i);
		i--;
	}
	return (NULL);
}
