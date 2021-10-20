/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 20:25:06 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 15:34:52 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Server::Server() {}

Server::~Server() {
    for (size_t i = 0; i < this->_locations.size(); i++)
        delete this->_locations[i];
    this->_locations.clear();
}

Server::Server(const Server &x) { *this = x; }

Server& Server::operator=(const Server &x) {
    if (this == &x)
        return *this;
	this->_port = x._port;
    this->_name = x._name;
    this->_errorPages = x._errorPages;
	this->_cliMaxSize = x._cliMaxSize;
    this->_locations = x._locations;
    return *this;
}

/*
**  Setters
*/

void	Server::setName(const std::vector<std::string> &tok) {
    this->_name = tok[0];
}

void	Server::setPort(const std::vector<std::string> &tok) {
    this->_port = std::atoi(tok[0].c_str());
}

void	Server::setErrorPages(const std::vector<std::string> &tok) {
    for (std::vector<std::string>::const_iterator it = tok.begin(); it != tok.end() - 1; it++)
        this->_errorPages.insert(std::make_pair(std::atoi(it->c_str()), *(tok.end() - 1)));
}

void	Server::setCliMaxSize(const std::vector<std::string> &tok) {
    this->_cliMaxSize = std::atoi(tok[0].c_str());
}

void	Server::newLocation(const std::vector<std::string> &tok) {
    if (tok.size() != 2)
        throw WebServer::ParsingError();
    t_location* loc = new t_location;
    loc->path = tok[1];
    this->_locations.push_back(loc);
}

void	Server::setLocationMethods(const std::vector<std::string> &tok)
{
    std::string methods[] = {"GET", "POST", "DELETE"};
    t_location  *loc      = this->_locations.back();

    for(std::vector<std::string>::const_iterator it = tok.begin() + 1; it != tok.end(); it++)
    {
        for(size_t i = 0; i < 3; i++)
            if (*it == methods[i])
                loc->methods.push_back(methods[i]);      
    }
}

void	Server::newLocationDirective(const std::vector<std::string> &tok)
{
    t_location* loc = this->_locations.back();
    std::string* atrr[] = {&loc->redirection, &loc->root, &loc->index};
    std::string directives[] = {"return", "root" "index"};

    if (tok[0] == "autoindex")
        loc->autoindex = (tok.size() != 2 || tok[1] != "ON") ? 0 : 1;
    else if (tok[0] == "allow")
        this->setLocationMethods(tok);
    else
    {
        for (size_t i = 0; i < 4; i++)
            if (tok[0] == directives[i])
                *(atrr[i]) = tok[1];
    }
}

void	Server::newDirective(const std::vector<std::string> &tokens)
{
	if (tokens.empty())
		return ;
	static method_function   method_ptr[4] = {&Server::setPort,
		&Server::setName, &Server::setErrorPages,
		&Server::setCliMaxSize};
	static std::string  directives[] = {"listen", "server_name",
			"error_pages", "cli_max_size"};

	for (int i = 0; i < 4; i++)
	{
		if (tokens[0] == directives[i])
		{
			method_function func = method_ptr[i];
			(this->*func)(tokens);
		}
	}
}

std::vector<t_location *>	Server::getLocations() const {
    return this->_locations;
}
