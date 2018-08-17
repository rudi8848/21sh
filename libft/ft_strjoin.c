/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 15:39:21 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 15:40:23 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strjoin(char const *s1, char const *s2)
{
	char		*dest;
	size_t		len;
	size_t		i;
	size_t		j;

	i = 0;
	j = 0;
	if (s1 && s2)
	{
		len = ft_strlen(s1) + ft_strlen(s2);
		dest = (char*)malloc(sizeof(char) * (len + 1));
		if (dest)
		{
			while (s1[i])
			{
				dest[i] = s1[i];
				i++;
			}
			while (s2[j])
				dest[i++] = s2[j++];
			dest[i++] = '\0';
			return (dest);
		}
	}
	return (NULL);
}
