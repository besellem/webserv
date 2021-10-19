/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 17:12:54 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
	
	return tokens;
}

void	WebServer::parse(const std::string config_file)
{
	int pos = config_file.rfind(".conf");
	if (pos != (int)config_file.size() - 5)
		throw ParsingError();
	std::fstream os;
	std::string line;
	os.open(config_file);
	int i = 10;
	while(i) // trouver le EOF
	{
		std::getline(os, line);
		tokens_type tok = getTokens(line);
		if (!tok.empty())
		{
			// faire un tableau de methodes a la place
			if (tok[0] == "server")
				this->newServer(tok);
			else if (tok[0] == "location")
				this->newLocation(this->lastServer(), tok);
			else if (tok[0] == "{")
				this->openBlock(tok);
			else if (tok[0] == "}")
				this->closeBlock(tok);
			else
				this->newDirective(this->lastServer(), tok);
		}
		--i;
	}
	os.close(config_file);
}
