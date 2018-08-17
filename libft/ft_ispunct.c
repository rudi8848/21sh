/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ispunct.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/14 17:53:26 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/14 18:08:27 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

int		ft_ispunct(int c)
{
	if (c >= 33 && c <= 39)
		return (1);
	else if (c >= 40 && c <= 47)
		return (1);
	else if (c >= 58 && c <= 63)
		return (1);
	else if (c == 64 || c == 123 || c == 124)
		return (1);
	else if (c >= 91 && c <= 96)
		return (1);
	else
		return (0);
}
