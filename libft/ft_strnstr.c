/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 14:39:59 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 14:41:55 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strnstr(const char *haystack,
		const char *needle, size_t len)
{
	int		i;
	int		j;
	char	*ptr;
	int		nlen;

	i = 0;
	j = 0;
	ptr = (char*)haystack;
	nlen = ft_strlen(needle);
	if (nlen == 0)
		return (ptr);
	while (haystack[i] && len)
	{
		if (needle[j] == haystack[i])
			j++;
		if (j == nlen)
			return (ptr + (i - nlen + 1));
		i++;
		len--;
	}
	return (NULL);
}
