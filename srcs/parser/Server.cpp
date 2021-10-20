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

Server::Server() : _port(-1), _cliMaxSize(-1) {}

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
**  Getters
*/

const int&	Server::getPort() const {
    return this->_port ;
}
const std::string&	Server::getName() const {
    return this->_name ;
}
const std::map<int, std::string>&	Server::getErrorPages() const {
    return this->_errorPages ;
}
const int&	Server::getClimaxSize() const {
    return this->_cliMaxSize ;
}
const std::vector<t_location *>&	Server::getLocations() const {
    return this->_locations;
}

/*
**  Setters
*/

void	Server::setName(const tokens_type &tok) {
     if (tok.size() != 2)
        throw WebServer::ParsingError();
    this->_name = tok[1];
}

void	Server::setPort(const tokens_type &tok) {
     if (tok.size() != 2)
        throw WebServer::ParsingError();
    if (!ft_isNumeric(tok[1]))
        throw WebServer::ParsingError();
    std::stringstream(tok[1]) >> this->_port;
    if (this->_port < 0 || this->_port > 65535)
        throw WebServer::ParsingError();
}

void	Server::setErrorPages(const tokens_type &tok) {
     if (tok.size() < 3)
        throw WebServer::ParsingError();
    for (tokens_type::const_iterator it = tok.begin() + 1; it != tok.end() - 1; it++)
    {
        int error_code;
        if (!ft_isNumeric(*it))
            throw WebServer::ParsingError();
        std::stringstream(*it) >> error_code;
        if (this->_errorPages.find(error_code) != this->_errorPages.end())
            this->_errorPages.erase(error_code);
        this->_errorPages.insert(std::make_pair(error_code, *(tok.end() - 1)));
    }
}

void	Server::setCliMaxSize(const tokens_type &tok) {
     if (tok.size() != 2)
        throw WebServer::ParsingError();
    if (!ft_isNumeric(tok[1]))
        throw WebServer::ParsingError();
    std::stringstream(tok[1]) >> this->_cliMaxSize;
    if (this->_cliMaxSize < 0)
        throw WebServer::ParsingError();
}

void	Server::newLocation(const tokens_type &tok) {
    if (tok.size() != 2)
        throw WebServer::ParsingError();
    t_location* loc = new t_location;
    loc->path = tok[1];
    loc->autoindex = 0;
    this->_locations.push_back(loc);
}

void	Server::setLocationMethods(const tokens_type &tok)
{
    std::string methods[] = {"GET", "POST", "DELETE"};
    t_location  *loc      = this->_locations.back();

    for(tokens_type::const_iterator it = tok.begin() + 1; it != tok.end(); it++)
    {
        size_t i;
        for(i = 0; i < 3; i++)
            if (*it == methods[i])
            {
                loc->methods.push_back(methods[i]);
                break ;
            }
        if (i == 3)
            throw WebServer::ParsingError();
    }
}

void	Server::newLocationDirective(const tokens_type &tok)
{
    t_location* loc = this->_locations.back();
    std::string* atrr[] = {&loc->redirection, &loc->root, &loc->index};
    std::string directives[] = {"return", "root", "index"};

    if (tok.size() < 2)
        throw WebServer::ParsingError();
    if (tok[0] == "autoindex")
        loc->autoindex = (tok.size() != 2 || tok[1] != "ON") ? 0 : 1;
    else if (tok[0] == "allow")
        this->setLocationMethods(tok);
    else
    {
        if (tok.size() != 2)
            throw WebServer::ParsingError();
        for (size_t i = 0; i < 3; i++)
        {
            if (tok[0] == directives[i])
            {
                *(atrr[i]) = tok[1];
                return ;
            }
        }
        throw WebServer::ParsingError();
    }
}

void	Server::newDirective(const tokens_type &tokens)
{
	if (tokens.empty())
		return ;
	static method_function   method_ptr[4] = {&Server::setPort,
		&Server::setName, &Server::setErrorPages,
		&Server::setCliMaxSize};
	static std::string  directives[] = {"listen", "server_name",
			"error_page", "cli_max_size"};

	for (int i = 0; i < 4; i++)
	{
		if (tokens[0] == directives[i])
		{
			method_function func = method_ptr[i];
			(this->*func)(tokens);
            return ;
		}
	}
    throw WebServer::ParsingError();
}
