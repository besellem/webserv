/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 12:16:19 by adbenoit         ###   ########.fr       */
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
	int i = 10;
	while (i) // trouver le EOF
	{
		std::cout << "reading file...\n";
		std::getline(os, line);
		ServerGenerator::tokens_type tok = getTokens(line);
		if (!tok.empty())
		{
			std::cout << "setting " << tok[0] << std::endl;
			// faire un tableau de methodes a la place
			if (tok[0] == "server")
				this->_servGen.newServer(tok);
			else if (tok[0] == "location")
				this->_servGen.newLocation(this->_servGen.lastServer(), tok);
			else if (tok[0] == "{")
				this->_servGen.openBlock(tok);
			else if (tok[0] == "}")
				this->_servGen.closeBlock(tok);
			else
				this->_servGen.newDirective(this->_servGen.lastServer(), tok);
		}
		--i;
	}
	os.close();
	std::cout << "\n## ServerGenerator :\n\n" << this->_servGen;
}
