/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:41:15 by besellem          #+#    #+#             */
/*   Updated: 2021/10/22 17:48:44 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

_BEGIN_NS_WEBSERV

/* Split a string by a delimiter converting it into a vector */
std::vector<std::string>	split_string(const std::string& s, const std::string& delim)
{
	std::string					tmp = s;
	std::vector<std::string>	v;
	std::string					line;
	size_t						pos = tmp.find(delim);

	while ((pos = tmp.find(delim)) != std::string::npos)
	{
		line = tmp.substr(0, pos);
		v.push_back(line);
		tmp.erase(0, pos + delim.length());
	}
	v.push_back(tmp);
	return v;
}

_END_NS_WEBSERV
