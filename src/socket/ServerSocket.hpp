/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 21:29:33 by tutku             #+#    #+#             */
/*   Updated: 2026/05/10 22:33:22 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

#define SUCCESS 0
#define ERROR -1

class ServerSocket
{
	private:
		int _fd;
		struct sockaddr_in _address; //address of the socket

	public:
		ServerSocket();
		~ServerSocket();
		ServerSocket &operator=(const ServerSocket &other);
		ServerSocket(const ServerSocket &other);

		int setup(void);
		int createSocket(void);
		int bindSocket(void);
		int get_fd();
		struct sockaddr_in getAddress();
};

#endif
