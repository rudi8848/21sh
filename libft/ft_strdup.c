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

#include "includes/libft.h"

char			*ft_strdup(const char *src)
{
	char		*dest;
	size_t		i;

	i = 0;
	dest = (char*)malloc((ft_strlen(src) + 1) * sizeof(char));
	if (!dest)
		return (NULL);
	if (src == NULL)
		return (0);
	while (i <= ft_strlen(src))
	{
		dest[i] = src[i];
		i++;
	}
	return (dest);
}
