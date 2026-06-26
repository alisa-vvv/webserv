/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/06/25 12:38:11 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <ctime>

Client::Client() //set fd=-1??
{

}

Client::Client(int listenerFd, int clientFd) : listenerFd(listenerFd), clientFd(clientFd)
{
	lastActivity = std::time(NULL);
}

Client ::~Client()
{

}
