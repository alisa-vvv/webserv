/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserTEST.hpp                                    :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/03 14:29:55 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 14:35:47 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "configParser.hpp"
#include <vector>

void		TEST_print_one_token(t_config_token& token, size_t token_idx);
void		TEST_print_tokens(std::vector<t_config_token> tokens);
std::string	TEST_state_to_str(e_state_label	state);
