/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/06/02 14:38:27 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>

/*
client obj answers:

	What data does this client have?
	What state is this client in?
	Should I read from it?
	Should I write to it?
	Should I close it?
*/
class Client
{
	private:

	public:
		int		fd;
		time_t	lastActivity;
		Client();
		Client(int fd);
		~Client();
		
};

#endif
