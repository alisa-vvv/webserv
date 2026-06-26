/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 12:35:04 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/06/25 17:22:34 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
#define LISTENER_HPP

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
#include <csignal>

#define SUCCESS 0
#define ERROR -1
#define BACKLOG 128 // how many pending connections queue can hold

enum eListenerError
{
	LISTENER_OK = 0,
	LISTENER_CREATESOCK_ERR,
	LISTENER_SETSOCKOPT_ERR,
	LISTENER_SETNONBLOCKING_ERR,
	LISTENER_SETADDRESS_ERR,
	LISTENER_BIND_ERR,
	LISTENER_LISTEN_ERR,
	LISTENER_ACCEPT_ERR,
	LISTENER_POLL_ERR,
	SIGACTION_ERR	// TODO: check if needed in here
};

class Listener
{
	private:
		int							_port;
		uint32_t					_ip_addr;
		int							_listenerFd;
		struct sockaddr_in			_address;	// address of the socket
		int							_fd;		// the server/listening socket
		std::vector<struct pollfd>	_pollFds;	// the list poll() watches

	public:
		Listener();
		~Listener();

		void			_buildListener();
		eListenerError	setup(void);
		eListenerError	_createSocket(void);
		eListenerError	_setSocketOptions();
		eListenerError	_setNonBlocking(int fd);
		eListenerError	_setAddress();
		eListenerError	_bindSocket(void);
		eListenerError	_listenSocket(void);

		void			setPort(int port);
		int				getPort() const;
		void			setIpAddr(uint32_t ip_addr);
		uint32_t		getIpAddr() const;
		int				getListenerFd();
		void			setListenerFd(int fd);

		//test
		void			printPortNumber();
		void			printPollInfo(int i);
		int				_printSocketName();
};

#endif