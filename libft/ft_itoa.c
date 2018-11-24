/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/06 15:45:06 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/13 10:53:05 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int		ft_len(int nbr)
{
	int			i;
	unsigned	nb;

	i = 0;
	if (nbr < 0)
		nb = -nbr;
	else if (nbr == 0)
		return (1);
	else
		nb = nbr;
	while (nb > 0)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

char			*ft_itoa(int value)
{
	char		*nbr;
	int			neg;
	int			len;
	unsigned	res;

	len = ft_len(value);
	res = (value < 0) ? -value : value;
	neg = (value < 0) ? 1 : 0;
	nbr = (char*)malloc(sizeof(char) * len + neg + 1);
	if (!nbr)
		return (NULL);
	nbr[len + neg] = '\0';
	while (len-- > 0)
	{
		nbr[len + neg] = (res % 10) + '0';
		res = res / 10;
	}
	if (neg)
		nbr[0] = '-';
	return (nbr);
}
