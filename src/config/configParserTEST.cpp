/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserTEST.cpp                                    :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/03 14:29:13 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 14:33:53 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParserTEST.hpp"
#include <iostream>

void	printYellowDebug(void) {
	std::cout << CLR_YEL << "DEBUG:\n" << CLR_NON;
}

void	printParserDebug(
	const std::string& filled_what,
	const std::string& variable_name,
	const bool print_debug,
	const std::optional<std::string>& string_arg,
	const std::optional<long long int> int_arg,
	const std::optional<size_t> uint_arg
) {
	if (SHOW_CONFIG_PARSER_DEBUG == true) {
		if (print_debug == true)
			printYellowDebug();
		std::cout << CLR_YEL << "Filled " << filled_what << ":\n";
		std::cout << "  ";
		std::cout << CLR_CYA << variable_name;
		std::cout << " = [" << CLR_NON;
		if (string_arg != std::nullopt)
			std::cout << *string_arg;
		if (uint_arg != std::nullopt)
			std::cout << *uint_arg;
		if (int_arg != std::nullopt)
			std::cout << *int_arg;
		std::cout << CLR_CYA << "]" << CLR_NON << std::endl;
	}
}

std::string	TEST_state_to_str(e_state_label	state) {
	switch (state) {
		case FINDING_BLOCK:
			return ("FINDING_BLOCK");
		case FINDING_VALUES:
			return ("FINDING_VALUES");
	}
}

void	TEST_print_one_token(t_config_token& token, size_t token_idx) {
		std::cout << "Tok " << token_idx << ".\n";
		std::cout << "\tTYPE:\t";
		std::cout << CLR_GRN;
		switch (token.type) {
			case EVALUATED:
				std::cout << CLR_CYA <<  "EVALUATED" << CLR_GRN;
				break ;
			case UNDEFINED_TYPE:
				std::cout << CLR_RED <<  "UNDEFINED_TYPE" << CLR_GRN;
				break ;
			case BLOCK_NAME:
				std::cout << "BLOCK_NAME";
				break ;
			case BLOCK_PREFIX:
				std::cout << "BLOCK_PREFIX";
				break ;
			case BRACE_OPEN:
				std::cout << "BRACE_OPEN";
				break ;
			case BRACE_CLOSE:
				std::cout << "BRACE_CLOSE";
				break ;
			case KEY:
				std::cout << "KEY";
				break ;
			case VALUE:
				std::cout << "VALUE";
				break ;
			case SEMICOLON:
				std::cout << "SEMICOLON";
				break ;
			case NEWLINE:
				std::cout << "NEWLINE";
			break ;
		}
		std::cout << CLR_NON;
		std::cout << '\n';
		std::cout << "\tVAL:\t";
		std::cout << CLR_YEL;
		if (token.val == "\n") {
			std::cout << CLR_CYA << "\\n" << CLR_NON;
		}
		else
			std::cout << token.val;
		std::cout << CLR_NON;
		std::cout << '\n';
}

void	TEST_print_tokens(std::vector<t_config_token> tokens) {
	std::cout << "checking tokens, size: \n" << tokens.size() << '\n';
	for (size_t i = 0; i < tokens.size(); i++) {
		TEST_print_one_token(tokens.at(i), i);
	}
}
