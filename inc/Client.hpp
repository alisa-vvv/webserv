/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/19 20:00:05 by tutku            ###   ########.fr       */
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
		size_t			_bytesSent;

	public:
		/* ============================ CONSTRUCTORS ============================ */
		Client();
		Client(const Listener *listener, int clientFd);
		~Client();

		/* ============================== GETTERS ============================== */
		time_t				getLastActivity() const;
		receiveStatus		getRecvStatus() const;
		const std::string	&getResponse() const;
		bool				getResponseStatus() const;
		size_t				getBytesSent() const;
		int					getListenerFd() const;
		RcvBuffer			&getRcvBuffer();
		const Listener*		getListenerClass() const;
		int 				getClientFd() const;

		/* ============================== SETTERS ============================== */
		void				setLastActivity(time_t lastActivity);
		void				setRecvStatus(receiveStatus recvStatus);
		void				setResponse(const std::string& response);
		void				setResponseStatus(bool response);
		
		/* ======================== RESPONSE HANDLING ========================= */
		void				updateBytesSent(size_t bytes);
		bool				isResponseComplete() const;

		/* ========================== TIMEOUT HANDLING ========================== */
		void				updateLastActivity();
};

#endif
