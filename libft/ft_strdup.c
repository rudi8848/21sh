/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 13:54:14 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 13:55:28 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char			*ft_strdup(const char *src)
{
	char		*dest;
	size_t		i;
	size_t		len;

	i = 0;
	len = ft_strlen(src);
	dest = (char*)ft_memalloc((len + 1) * sizeof(char));
	if (!dest || !src)
		return (NULL);
	while (i < len)
	{
		dest[i] = src[i];
		++i;
	}
	return (dest);
}
