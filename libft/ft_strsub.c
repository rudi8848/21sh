/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsub.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 15:36:40 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 15:37:07 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char	*ft_strsub(char const *s, unsigned int start, size_t len)
{
	char		*sub;
	size_t		i;

	i = 0;
	if (s)
	{
		sub = ft_strnew(len);
		if (!sub)
			return (NULL);
		while (i < len)
		{
			sub[i] = s[start];
			i++;
			start++;
		}
		return (sub);
	}
	return (NULL);
}
