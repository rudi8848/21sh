/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 13:27:43 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 13:28:27 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

void	*ft_memccpy(void *dst, const void *src, int c, size_t n)
{
	unsigned char		*ptr_d;
	unsigned char		*ptr_s;
	size_t				i;
	unsigned char		stop;

	i = 0;
	ptr_d = (unsigned char*)dst;
	ptr_s = (unsigned char*)src;
	stop = (unsigned char)c;
	while (i < n)
	{
		ptr_d[i] = ptr_s[i];
		if (ptr_s[i] == stop)
			return (ptr_d + i + 1);
		i++;
	}
	return (NULL);
}
