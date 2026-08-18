/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 13:00:51 by avaliull          #+#    #+#             */
/*   Updated: 2026/08/18 11:30:41 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <optional>
#include "Http.hpp"
#include "Client.hpp"

std::optional<cgi_t>	executeCGI(
	Client& client,
	std::map<int, cgi_t>&	background_cgis
);
std::tuple<int, size_t>	checkBackgroundCGIs(
	const Client& client,
	std::map<int, cgi_t>&	background_cgis
);
int	checkCgiDone(
	cgi_t&	cgi
);
