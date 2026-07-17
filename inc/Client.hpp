/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 16:48:17 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include "RcvBuffer.hpp"

class Listener;

class Client
{
	private:
		int				_listenerFd;
		int				_clientFd;
		RcvBuffer		_rcvBuffer;
		time_t			_lastActivity;
		const Listener	*_listener;

		receiveStatus	_recvStatus;
		std::string		_response;
		bool			_responseStatus;


	public:
		Client();
		Client(const Listener *listener, int clientFd);
		~Client();

		time_t			getLastActivity();
		int				getListenerFd();
		RcvBuffer&		getRcvBuffer();
		const Listener*	getListenerClass() const;

		void			setRecvStatus(receiveStatus recvStatus);
		receiveStatus	getRecvStatus() const;

		void			setResponse(std::string response);
		std::string		getResponse() const;
		const char		*updatedResponse(std::string response);

		void			setResponseStatus(bool response);
		bool			getResponseStatus() const;

		int 			getClientFd();

		void			setLastActivity(time_t lastActivity);
		void			updateLastActivity();
};

#endif
