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
#include <optional>

void		TEST_print_one_token(t_config_token& token, size_t token_idx);
void		TEST_print_tokens(std::vector<t_config_token> tokens);
std::string	TEST_state_to_str(e_state_label	state);
void	printParserDebug(
	const std::string& filled_what,
	const std::string& variable_name,
	const bool print_debug,
	const std::optional<std::string>& string_arg,
	const std::optional<long long int> int_arg,
	const std::optional<size_t> uint_arg
);
void	printYellowDebug(void);
