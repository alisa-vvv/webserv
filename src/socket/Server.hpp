/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/06/04 11:16:39 by tcakir-y         ###   ########.fr       */
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
#include <ctime>

#define SUCCESS 0
#define ERROR -1
#define BACKLOG 128 // how many pending connections queue can hold

class Server
{
private:
	int _port;
	uint32_t _host;
	struct sockaddr_in _address; // address of the socket

	int _fd;								// the server/listening socket
	// std::vector<int> _fds;
	std::vector<struct pollfd> _pollFds;	// the list poll() watches
	std::map<int, Client> _clients;			// client state, found by client fd

	int _createSocket(void);
	int _setNonBlocking(int fd);
	int _setSocketOptions();
	int _setAddress();
	int _bindSocket(void);
	int _listenSocket(void);
	int _initPollEvent(void);
	void _addFdToPoll(int fd);
	int _accept(int serverListenFd);
	void _closeClientFd(int fd);
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
	void printPollInfo(int i);
	int _printSocketName();
};

void *ft_memset(void *s, int c, size_t n);

#endif

/*
_fd
  the listening/server socket

clientFd
  one connected client socket, returned by accept()

_pollFds vector -> who should I watch?, tells who is ready
  list of all fds poll() should watch
  pollfds has both:
	server fd  -> used for accept()
	client fds -> used for recv()/send()

_clients map -> what do I know about this client?
  actual per-client data/state, looked up by fd
*/

/*
server fd  + POLLIN = accept()
		a new client is waiting to connect

client fd  + POLLIN = recv()
		this connected client sent data, so recv() can read it
*/
