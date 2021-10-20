/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 18:48:29 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	ft_isNumeric(const std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (std::isdigit(str[i]) == 0)
			return 0;
	return 1;
}

/* Cuts str from delimiter.
Returns the new string */

std::string    ft_strcut(const std::string& str, char delimiter)
{
	size_t pos = str.find(delimiter);
	if (pos == std::string::npos)
		return str;
	return str.substr(0, pos);
}

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

std::vector<std::string>    getTokens(std::string line)
{
	std::vector<std::string> tokens;

	size_t pos = line.find(' ');
	pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;
	while (pos != std::string::npos)
	{
		std::string str = line.substr(0, pos);
		if (!str.empty())
			tokens.push_back(str);
		line.erase(0, pos + 1);
		pos = line.find(' ');
		pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;
	}
	if (line[0])
		tokens.push_back(line);
	
	return tokens;
}

void	WebServer::parse(const std::string config_file)
{
	std::cout << "=== Parsing ===\n";
	int pos = config_file.rfind(".conf");
	if (pos != (int)config_file.size() - 5)
		throw ParsingError();
	std::fstream os;
	std::string line;
	os.open(config_file);
	while (std::getline(os, line))
	{
		line = ft_strcut(line, '#');
		ServerGenerator::tokens_type tok = getTokens(line);
		if (!tok.empty())
		{
			if (tok[0] == "server")
				this->_config.newServer(tok);
			else if (tok[0] == "location")
				this->_config.newLocation(this->_config.lastServer(), tok);
			else if (tok[0] == "{")
				this->_config.openBlock(tok);
			else if (tok[0] == "}")
				this->_config.closeBlock(tok);
			else
			{
				tok = ft_vectorcut(tok, ';');
				this->_config.newDirective(this->_config.lastServer(), tok);
			}
		}
	}
	if (this->_config._state != START)
		throw ParsingError();
	os.close();
	std::cout << "\n## ServerGenerator :\n\n" << this->_config;
}
