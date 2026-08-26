/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   AllowedCgi.cpp                                          :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/08/26 12:46:27 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/26 13:22:53 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "AllowedCgi.hpp"

const std::map<std::string, e_cgi_extension>	AllowedCgi::ext_strings {
	{ ".py", CGI_EXT_PY },
	{ ".bla", CGI_EXT_BLA },
};

