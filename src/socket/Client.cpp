/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/06/10 23:08:21 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() //set fd=-1??
{

}

Client::Client(int fd) : fd(fd)
{
	lastActivity = std::time(NULL);
}

Client ::~Client()
{

}
