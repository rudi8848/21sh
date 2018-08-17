/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:08:34 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/07 09:13:49 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

char			*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*fresh;
	size_t			i;
	size_t			len;

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
				fresh[i] = f(i, s[i]);
				i++;
			}
			fresh[i] = '\0';
			return (fresh);
		}
	}
	return (NULL);
}
