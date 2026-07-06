/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/06 20:10:11 by tutku            ###   ########.fr       */
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
