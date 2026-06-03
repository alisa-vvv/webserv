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
