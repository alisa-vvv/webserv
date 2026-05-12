/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:34:42 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/05/12 13:36:52 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

void *ft_memset(void *s, int c, size_t n)
{
	unsigned char *ptr;
	size_t counter;

	ptr = (unsigned char *)s;
	counter = 0;
	while (counter < n)
	{
		ptr[counter] = (unsigned char)c;
		counter++;
	}
	return (s);
}
