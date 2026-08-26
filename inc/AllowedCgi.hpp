/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   AllowedCgi.hpp                                          :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/08/26 13:07:39 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/26 14:47:22 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

typedef enum	e_cgi_extension {
	CGI_EXT_PY,
	CGI_EXT_BLA,
	CGI_EXT_BAD,
	CGI_EXT_UNKNOWN,
}	e_cgi_extension;

class	AllowedCgi {
public:
	static const	std::map<std::string, e_cgi_extension>	ext_strings;
	static const	std::map<e_cgi_extension, std::string>	executables;

	inline e_cgi_extension	match(std::string ext_token) const {
		if (ext_strings.find(ext_token) == ext_strings.end()) {
			return(CGI_EXT_UNKNOWN);
		}
		return (ext_strings.at(ext_token));
	}
};
