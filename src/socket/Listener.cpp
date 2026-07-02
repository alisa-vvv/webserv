/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 12:34:57 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/02 23:09:46 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

eListenerError Listener::setup(void)
{
	eListenerError err;

	err = this->_createSocket();
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	err = this->_setSocketOptions();
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	err = this->_setNonBlocking(_listenerFd);
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	err = this->_setAddress();
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	err = this->_bindSocket();
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	this->_printSocketName(); //test
	err = this->_listenSocket();
	if (err != LISTENER_OK)
	{
		closeSocket();
		return err;
	}
	return LISTENER_OK;
}

/*
	AF_INET : IPv4 protocol
	SOCK_STREAM: TCP socket
	0: default protocol
*/
eListenerError Listener::_createSocket()
{
	_listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenerFd == ERROR)
	{
		std::cerr << "Error opening socket: " << std::strerror(errno) << std::endl;
		return LISTENER_CREATESOCK_ERR;
	}
	std::cout << "Socket created successfully!" << std::endl;
	return LISTENER_OK;
}

/*
 SO_REUSEADDR option for when you restart the server, dont get:
	Address already in use
*/
eListenerError Listener::_setSocketOptions()
{
	int opt = 1;

	if (setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR)
	{
		std::cerr << "Error setsockopt: " << std::strerror(errno) << std::endl;
		return LISTENER_SETSOCKOPT_ERR;
	}
	return LISTENER_OK;
}

/*
	Sets a file descriptor to non-blocking mode.

	fd:
		The file descriptor to modify. This can be a listening socket,
		client socket, or another fd such as a pipe.

	How it works:
		1. F_GETFL gets the fd's current file status flags.
		2. O_NONBLOCK is added to those flags.
		3. F_SETFL applies the updated flags back to the fd.

	Return:
		SUCCESS on success, ERROR on failure.
*/
eListenerError Listener::_setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == ERROR)
	{
		std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << std::endl;
		return LISTENER_SETNONBLOCKING_ERR;
	}
	int status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		std::cerr << "Error making fd non-blocking: " << std::strerror(errno) << std::endl;
		return LISTENER_SETNONBLOCKING_ERR;
	}
	return LISTENER_OK;
}

eListenerError Listener::_setAddress()
{
	memset(&_address, 0, sizeof(_address));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(_ip_addr);
	return LISTENER_OK;
}

// https://www.linuxhowtos.org/C_C++/socket.htm //TODO: remove later
// reinterpret casting-> Keep the same memory address, 
//    but treat the pointer as a different pointer type.
eListenerError Listener::_bindSocket()
{
	if (bind(_listenerFd, reinterpret_cast<const struct sockaddr *>(&_address), sizeof(_address)) == ERROR)
	{
		std::cerr << "Couldn't bind the port!: " << std::strerror(errno) << std::endl;
		return LISTENER_BIND_ERR;
	}
	std::cout << "Bind successful" << std::endl;
	return LISTENER_OK;
}

/*
int listen(int sockfd, int backlog);

backlog is the max num of connections
	that will be queued while the server is busy
*/
eListenerError Listener::_listenSocket()
{
	if (listen(this->_listenerFd, BACKLOG) == ERROR) //TODO: change fd variable
	{
		std::cerr << "Couldn't listen socket!: " << std::strerror(errno) << std::endl;
		return LISTENER_LISTEN_ERR;
	}
	std::cout << "Listen successful" << std::endl;
	return LISTENER_OK;
}

void Listener::setPort(int port)
{
	this->_port = port;
}

int Listener::getPort() const
{
	return this->_port;
}

void Listener::setIpAddr(uint32_t ip_addr)
{
	this->_ip_addr = ip_addr;
}

uint32_t Listener::getIpAddr() const
{
	return this->_ip_addr;
}

int Listener::getListenerFd() const
{
	return _listenerFd;
}

void Listener::setListenerFd(int fd)
{
	_listenerFd = fd;
}

void Listener::closeSocket()
{
	if (_listenerFd >= 0)
	{
		close(_listenerFd);
		_listenerFd = -1;
	}
}

Listener::Listener() : _listenerFd(-1), _port(0), _ip_addr(0)
{
	std::memset(&_address, 0, sizeof(_address));
}

Listener::~Listener()
{

}
