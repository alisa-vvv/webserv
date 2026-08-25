/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   utils.hpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/08/25 17:14:36 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/25 17:17:40 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//#include "Client.hpp"
//#include "Http.hpp"

class Client;
class Http;

void	createErrorResponse(Client& client, Http& http);
