/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 12:35:04 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 12:51:42 by tcakir-y         ###   ########.fr       */
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

#include "cgi_exec.hpp"
#include <sys/types.h>
#include <netdb.h>

#include "configParser.hpp"

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
	int										_listenerFd;	// the server/listening socket
		int									_port;
		uint32_t							_ip_addr;
		struct sockaddr_in					_address;		// address of the socket
		std::vector<const cfg_server_t *>	_serverConfigs;
		
		public:
		Listener();
		~Listener();

		eListenerError		setup(void);
		eListenerError		_createSocket(void);
		eListenerError		_setSocketOptions();
		eListenerError		_setNonBlocking(int fd);
		eListenerError		_setAddress();
		eListenerError		_bindSocket(void);
		eListenerError		_listenSocket(void);
		
		void				setPort(int port);
		int					getPort() const;
		
		void				setIpAddr(uint32_t ip_addr);
		uint32_t			getIpAddr() const;
		
		int					getListenerFd() const;
		void				setListenerFd(int fd);

		const cfg_server_t	*getServerConfig(int i) const;
		void				addServerConfig(const cfg_server_t *config);
		size_t				getConfigCount() const { return _serverConfigs.size(); };
		
		void				closeSocket();

		//test
		void			printPortNumber();
		int				_printSocketName();
};

#endif
