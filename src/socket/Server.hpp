/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/05/28 15:44:18 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>		// close
#include <sys/socket.h> // socket, AF_INET, SOCK_STREAM
#include <netinet/in.h> // sockaddr_in
#include <cerrno>		// errno
#include <cstring>		// std::strerror
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <map>

#define SUCCESS 0
#define ERROR -1
#define BACKLOG 128 // how many pending connections queue can hold

class Server
{
private:
	int _port;
	uint32_t _host;
	struct sockaddr_in _address; // address of the socket

	int _fd;
	// std::vector<int> _fds;
	std::vector<struct pollfd> _pollFds; //stores the fds you want poll() to watch
	std::map<int, Client>      _clients;

	int _createSocket(void);
	int _setNonBlocking(int fd);
	int _setSocketOptions();
	int _setAddress();
	int _bindSocket(void);
	int _listenSocket(void);
	int _initPollEvent(void);
	int _accept(void);
	Server(const Server &other);
	Server &operator=(const Server &other);

public:
	Server();
	Server(uint32_t host, int port);
	~Server();
	int setup(void);
	int get_fd() const;
	void closeSocket();
	void printPortNumber();
};

void *ft_memset(void *s, int c, size_t n);

#endif

