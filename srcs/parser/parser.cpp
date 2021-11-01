/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/01 16:46:31 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

/* Breaks a string into a sequence of zero or
more nonempty tokens delimiter by tabulation or space
Returns the vector of tokens. */
Server::tokens_type    getTokens(std::string line)
{
	Server::tokens_type tokens;

	size_t pos = line.find(' ');
	pos = (line.find('\t') < pos) ? line.find('\t') : pos;
	while (pos != std::string::npos)
	{
		std::string str = line.substr(0, pos);
		if (!str.empty())
			tokens.push_back(str);
		line.erase(0, pos + 1);
		pos = line.find(' ');
		pos = (line.find('\t') < pos) ? line.find('\t') : pos;
	}
	if (line[0])
		tokens.push_back(line);
	return tokens;
}

/* Fills the servers with line's data */
void	parse_line(ServerGenerator& servers, std::string line)
{	
	line = ft_strcut(line, '#');
	Server::tokens_type tok = getTokens(line);
	if (!tok.empty())
	{
		if (tok[0] == "server")
			servers.newServer(tok);
		else if (tok[0] == "location")
			servers.newLocation(servers.last(), tok);
		else if (tok[0] == "{")
			servers.openBlock(tok);
		else if (tok[0] == "}")
			servers.closeBlock(tok);
		else
		{
			tok = ft_vectorcut(tok, ';');
			if (servers.size() == 0)
				throw WebServer::ParsingError();
			servers.newDirective(servers.last(), tok);
		}
	}
}

/* Reads the config file and parse it line by line */
void	WebServer::parse(const std::string &config_file)
{
	if (getExtension(config_file) != ".conf")
		throw ParsingError();
	std::fstream os;
	std::string line;
	os.open(config_file);
	if (!os)
		throw ParsingError();
	while (std::getline(os, line))
		parse_line(this->_servers, line);
	if (this->_servers.state() != START)
		throw ParsingError();
	os.close();
	if (DEBUG)
	{
		std::cout << "============== Parsing ===============" << std::endl;
		std::cout << this->_servers;
        std::cout << "======================================" << std::endl;
	}
}

_END_NS_WEBSERV
