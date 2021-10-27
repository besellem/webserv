/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:41:15 by besellem          #+#    #+#             */
/*   Updated: 2021/10/27 13:42:15 by besellem         ###   ########.fr       */
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

	if (delim[0] != '\0')
	{
		while ((pos = tmp.find(delim)) != std::string::npos)
		{
			line = tmp.substr(0, pos);
			v.push_back(line);
			tmp.erase(0, pos + delim.length());
		}
	}
	v.push_back(tmp);
	return v;
}

/* Most optimized way of checking if a file exist */
bool	is_valid_path(const std::string& path)
{
	struct stat	buf;
	int			_s = stat(path.c_str(), &buf);
	// std::cout << "is_valid_path(" << path << ") : " << _s << std::endl;
	return (0 == _s);
}

_END_NS_WEBSERV
