/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:41:15 by besellem          #+#    #+#             */
/*   Updated: 2021/10/26 22:50:23 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

_BEGIN_NS_WEBSERV

/* Returns the extension of a file */
std::string	getExtension(const std::string& fileName)
{
	size_t pos = fileName.rfind(".");
	if (pos == std::string::npos)
		return std::string("");
	std::string	ext = fileName.substr(pos, fileName.size());
	return ext;
	
}

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
			if (!tmp.empty())
				v.push_back(line);
			tmp.erase(0, pos + delim.length());
		}
	}
	if (!tmp.empty())
		v.push_back(tmp);
	return v;
}

/* Returns 1 if the string is numeric.
Otherwise, returns 0. */
bool	ft_isNumeric(const std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (std::isdigit(str[i]) == 0)
			return 0;
	return 1;
}

/* Cuts str from delimiter.
Returns the new string. */
std::string    ft_strcut(const std::string& str, char delimiter)
{
	size_t pos = str.find(delimiter);
	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
}

/* Cuts vector from delimiter.
Returns the new vector. */
std::vector<std::string>	ft_vectorcut(const std::vector<std::string>& vect, char delimiter)
{
	size_t pos = 0;
	std::vector<std::string> newVect(vect);
	std::vector<std::string>::const_iterator it;

	for(it = newVect.begin(); it != newVect.end(); it++, pos++)
	{
		if (*it != ft_strcut(*it, delimiter))
		{
			newVect[pos] = ft_strcut(*it, delimiter);
			if (!newVect[pos].empty())
				it++;
			break ;
		}
	}
	newVect.erase(it, newVect.end());
	return newVect;
}

_END_NS_WEBSERV
