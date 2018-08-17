/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdel.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/13 10:28:41 by gvynogra          #+#    #+#             */
/*   Updated: 2017/11/13 10:37:49 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/libft.h"

void	ft_lstdel(t_list **alst, void (*del)(void *, size_t))
{
	t_list	*lst;
	t_list	*nextl;

	lst = *alst;
	while (lst)
	{
		nextl = lst->next;
		del(lst->content, lst->content_size);
		free(lst);
		lst = nextl;
	}
	*alst = NULL;
}
