/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   utils.cpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/08/25 17:17:57 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/25 17:18:50 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Client.hpp"
#include "Http.hpp"

void	createErrorResponse(Client& client, Http& http) {
	http.setResponseCode(HTTP_REQUEST_TIMEOUT);
	http.handleErrorResponse();
	http.buildResponseString();
	client.setResponse(http.getResponseString());
}
