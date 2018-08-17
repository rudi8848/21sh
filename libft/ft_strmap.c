/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 15:27:59 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 15:29:18 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strmap(char const *s, char (*f)(char))
{
	char		*fresh;
	size_t		i;
	size_t		len;

	i = 0;
	if (s && f)
	{
		len = ft_strlen(s);
		fresh = (char*)malloc(sizeof(char) * (len + 1));
		if (!fresh)
			return (NULL);
		else
		{
			while (i < len)
			{
				fresh[i] = f(s[i]);
				i++;
			}
			fresh[i] = '\0';
			return (fresh);
		}
	}
	return (NULL);
}
