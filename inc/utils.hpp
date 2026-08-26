/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   utils.hpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/08/25 17:14:36 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/25 17:45:43 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Client;
class Http;

void	createErrorResponse(Client& client, Http& http, const int& error_code);
