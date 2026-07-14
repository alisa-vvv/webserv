/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   Timer.hpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/07/14 12:40:17 by avaliull            #+#    #+#           */
/*   Updated: 2026/07/14 12:51:35 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <chrono>

#define DEFAULT_TIMEOUT_S_CGI 30

using std::chrono::time_point;
using std::chrono::system_clock;

bool	checkTimeout(const time_point<system_clock> timer, int time);
