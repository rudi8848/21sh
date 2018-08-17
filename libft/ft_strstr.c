/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 14:36:35 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/22 11:48:13 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strstr(const char *haystack, const char *needle)
{
	size_t		i;
	size_t		j;
	char		*ptr;
	size_t		nlen;

	i = 0;
	nlen = ft_strlen(needle);
	ptr = (char*)haystack;
	if (nlen == 0)
		return (ptr);
	while (haystack[i])
	{
		j = 0;
		while (needle[j] == haystack[i + j])
		{
			if (j == nlen - 1)
				return (ptr + i);
			j++;
		}
		i++;
	}
	return (NULL);
}
