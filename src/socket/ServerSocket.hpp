/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 21:29:33 by tutku             #+#    #+#             */
/*   Updated: 2026/05/12 14:06:55 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <iostream>
#include <unistd.h>		// close
#include <sys/socket.h>	// socket, AF_INET, SOCK_STREAM
#include <netinet/in.h>	// sockaddr_in
#include <cerrno>		// errno
#include <cstring>		// std::strerror
#include <fcntl.h>


#define SUCCESS 0
#define ERROR -1
#define BACKLOG 10		//how many pending connections queue can hold

class ServerSocket
{
	private:
		int _fd;
		struct sockaddr_in _address; //address of the socket

	public:
		ServerSocket();
		ServerSocket &operator=(const ServerSocket &other);
		ServerSocket(const ServerSocket &other);
		~ServerSocket();

		int setup(void);
		int createSocket(void);
		int bindSocket(void);
		int listenSocket(void);
		int get_fd();
		struct sockaddr_in &getAddress();
};

void *ft_memset(void *s, int c, size_t n);

#endif

/* note:
struct sockaddr_in _address;

ft_memset(&_address, 0, sizeof(_address));
_address.sin_family = AF_INET;
_address.sin_port = htons(8080);
//IP address of the machine on which the server is running
_address.sin_addr.s_addr = INADDR_ANY;

// htons() which converts a port number to network byte order

//INADDR_ANY; //listens all available IPs
*/