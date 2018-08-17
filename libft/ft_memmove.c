/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 13:30:35 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/06 13:32:03 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n)
{
	unsigned char	*ptr_d;
	unsigned char	*ptr_s;

	ptr_d = (unsigned char*)dst;
	ptr_s = (unsigned char*)src;
	if (ptr_s < ptr_d)
	{
		while (n--)
			ptr_d[n] = ptr_s[n];
	}
	else
		ft_memcpy(ptr_d, ptr_s, n);
	return (ptr_d);
}
