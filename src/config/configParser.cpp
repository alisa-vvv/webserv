/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:50:42 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParserTEST.hpp"
#include "configParsingInfo.hpp"
#include <fstream>
#include <vector>

bool	tokenIsAlpha(t_config_token& token) {
	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (false);
	return (true);
}

//bool fillServerField(
//	const ParsingInfo parsing_info,
//	const size_t& block_start_idx,
//	const std::vector<t_config_token>& tokens
//) {
//	for (size_t i = block_start_idx; i < tokens.size(); i++) {
//
//	}
//	return (true);
//}

bool	matchBlockName(
	const std::vector<std::string> allowed_strings,
	const t_config_token& token
) {
	for (size_t i = 0; i < allowed_strings.size(); i++) {
		if (token.val == allowed_strings.at(i))
	  		return (true);
	}
	return (false);
}

Config	tokensToConfig(const ParsingInfo parsing_info,
			const std::vector<t_config_token>&	tokens) {
	Config	config;

	for (size_t i = 0; i < tokens.size(); i++) {
		const t_config_token&	cur_token = tokens.at(i);
		if (cur_token.type == BLOCK_NAME) {
			if (!matchBlockName(parsing_info.depth0_valid_block_names, cur_token)) {
				// exception, error, w/e
				return (config);
			}
		}
	}
	return (config);
}

int	parseConfig([[maybe_unused]] const ParsingInfo parsing_info) {
	std::ifstream				config_file(CONFIG_PATH_TEST);
	std::vector<t_config_token>	tokens;

	tokens = tokenize(config_file);
	if (evaluateTokens(tokens) == 1)
		return (1);
	else
		TEST_print_tokens(tokens);

	//Config config = tokensToConfig(parsing_info, tokens);
	return (0);
}
