/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/08 13:09:40 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <ctime>

Client::Client() //set fd=-1??
{

}

Client::Client(int listenerFd, int clientFd) : _listenerFd(listenerFd), _clientFd(clientFd)
{
	lastActivity = std::time(NULL);
}

Client ::~Client()
{

}

int Client::getListenerFd()
{
	return(_listenerFd);
}
