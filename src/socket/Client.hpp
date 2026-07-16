/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 01:21:49 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include "RcvBuffer.hpp"
#include "ConfigParser.hpp"

class Client
{
	private:
		int							_listenerFd;
		int							_clientFd;
		RcvBuffer					_rcvBuffer;
		std::vector<cfg_server_t *>	_serverConfigs;
		time_t						_lastActivity;

	public:
		Client();
		Client(int listenerFd, int clientFd);
		~Client();

		time_t		getLastActivity();
		int			getListenerFd();
		RcvBuffer&	getRcvBuffer();

		void		setLastActivity(time_t lastActivity);
		void		setState();
		void		updateLastActivity();
};

#endif
