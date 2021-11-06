/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:41:15 by besellem          #+#    #+#             */
/*   Updated: 2021/11/06 21:39:00 by adbenoit         ###   ########.fr       */
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
	std::string					tmp(s);
	std::vector<std::string>	v;
	std::string					line;
	size_t						pos = tmp.find(delim);

	if (delim[0] != '\0' && !s.empty())
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

/* Returns 1 if the string is an ip address.
Otherwise, returns 0. */
bool	ft_isIpAddress(const std::string &str)
{
	size_t 		x;
	size_t 		n = 0;
	std::string	tmp = "";
	
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]) && str[i] != '.')
			return 0;
		if (std::isdigit(str[i]))
			tmp += str[i];
		if (str[i] == '.' || i == str.size() - 1)
		{
			++n;
			std::stringstream(tmp) >> x;
			if (x > 255 || n > 4)
				return 0;
			tmp = "";
		}
	}
	if (n != 4)
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

/* Joins all the strings of vect separate by sep.
Returns the new string */

std::string vectorJoin(const std::vector<std::string> &vect, char sep)
{
    if (vect.empty())
        return std::string("");
    std::string str = vect[0];
    for (size_t i = 1; i < vect.size(); i++)
    {
        str += sep;
        str += vect[i];
    }
    return str;
}


/* Most optimized way of checking if a file exist */
bool	is_valid_path(const std::string& path)
{
	struct stat	buf;
	int			_s = stat(path.c_str(), &buf);
	// std::cout << "is_valid_path(" << path << ") : " << _s << std::endl;
	return (0 == _s);
}

/* Check if a path is a directory */
bool	ft_isDirectory(const std::string& path)
{
	struct stat	statBuf;
	
	stat(path.c_str(), &statBuf);
	if (S_ISDIR(statBuf.st_mode))
		return 1;
	return 0;
}

/* Returns the content of a file */
std::string	getFileContent(const std::string& file)
{
	std::string		content;
	std::ifstream	ifs(file, std::ios::in);

	std::string		gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof())
				break ;
			content += "\n";
		} while (true);
	}
	ifs.close();
	return content;
}

/* Returns the size of a file */
ssize_t		getFileLength(const std::string& file)
{
	if (ft_isDirectory(file))
		return 0;
	
	std::ifstream	ifs(file, std::ios::binary | std::ios::ate);

	if (ifs.is_open())
	{
		ssize_t	size = ifs.tellg();
		ifs.close();
		return size;
	}
	return SYSCALL_ERR; // may want to throw an error or something
}

_END_NS_WEBSERV
