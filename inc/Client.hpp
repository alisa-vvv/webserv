/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 13:42:26 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include "RcvBuffer.hpp"
#include "configParser.hpp"

class Listener;

class Client
{
	private:
		int							_listenerFd;
		int							_clientFd;
		RcvBuffer					_rcvBuffer;
		time_t						_lastActivity;
		const Listener				*_listener;

	public:
		Client();
		Client(const Listener *listener, int clientFd);
		~Client();

		time_t			getLastActivity();
		int				getListenerFd();
		RcvBuffer&		getRcvBuffer();
		const Listener*	getListenerClass() const;

		void			setLastActivity(time_t lastActivity);
		void			setState();
		void			updateLastActivity();
};

#endif
