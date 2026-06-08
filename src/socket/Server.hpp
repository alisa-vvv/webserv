/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/06/08 14:41:54 by tutku            ###   ########.fr       */
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

enum eServerError
{
	SERVER_OK = 0,
	SERVER_CREATESOCK_ERR,
	SERVER_SETSOCKOPT_ERR,
	SERVER_SETNONBLOCKING_ERR,
	SERVER_SETADDRESS_ERR,
	SERVER_BIND_ERR,
	SERVER_LISTEN_ERR,
	SERVER_ACCEPT_ERR,
	SERVER_POLL_ERR
};

class Server
{
private:
	int _port;
	uint32_t _host;
	struct sockaddr_in _address; // address of the socket

	int _fd;								// the server/listening socket
	// std::vector<int> _listenFds;
	std::vector<struct pollfd> _pollFds;	// the list poll() watches
	std::map<int, Client> _clients;			// client state, found by client fd

	eServerError _createSocket(void);
	eServerError _setNonBlocking(int fd);
	eServerError _setSocketOptions();
	eServerError _setAddress();
	eServerError _bindSocket(void);
	eServerError _listenSocket(void);
	eServerError _initPollEvent(void);
	void _addFdToPoll(int fd);
	eServerError _accept(int serverListenFd);
	void _closeClientFd(int fd);
	Server(const Server &other);
	Server &operator=(const Server &other);

public:
	Server();
	Server(uint32_t host, int port);
	~Server();
	eServerError setup(void);
	eServerError run(void);
	int get_fd() const;
	void closeSocket();
	
	//test
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
