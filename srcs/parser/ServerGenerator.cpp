/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:19 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 12:13:09 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerGenerator.hpp"

ServerGenerator::ServerGenerator() { this->_state = START; }

ServerGenerator::~ServerGenerator() {
    for (size_t i = 0; i < this->_servers.size(); i++)
        delete this->_servers[i];
    this->_servers.clear();
}

ServerGenerator::ServerGenerator(const ServerGenerator &x) {
    *this = x;
}

ServerGenerator&	ServerGenerator::operator=(const ServerGenerator &x) {
    if (&x == this)
        return *this;
    this->_servers = x._servers;
    this->_state = x._state;
    
    return *this;
}

/*
**  Element access
*/

Server*	ServerGenerator::operator[](int i) const {
    return this->_servers[i];
}

Server* ServerGenerator::lastServer() const {
    return this->_servers.back();
}

/*
**  Modifiers
*/

/* if the key 'server' is well placed, add a new location to the generator */

void	ServerGenerator::newServer(const tokens_type &tok) {
	if (tok.size() != 1 || this->_state != START)
        return ;
		// throw WebServer::ParsingError();
	this->_servers.push_back(new Server());
	this->_state = NEW_SERVER;
}

/* if the key 'location' is well placed, add a new location to the server */

void	ServerGenerator::newLocation(Server *server, const tokens_type &tok) {
    if (this->_state != IN_SERVER)
        return ;
        // throw WebServer::ParsingError();
    server->newLocation(tok);
    this->_state = NEW_LOCATION;
}

/* if the directive is well placed, add it to the server */

void	ServerGenerator::newDirective(Server *server, const tokens_type &tok) {
    if (this->_state == IN_SERVER)
        server->newDirective(tok);
    else if (this->_state == IN_LOCATION)
        server->newLocationDirective(tok);
    else
        return ;
        // throw WebServer::ParsingError();
}

/* check if the '{' is well placed */

void	ServerGenerator::openBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
        return ;
		// throw WebServer::ParsingError();

	if (this->_state == NEW_SERVER || this->_state == NEW_LOCATION)
        ++this->_state;
    else
        return ;
        // throw WebServer::ParsingError();
}

/* check if the '}' is well placed */

void	ServerGenerator::closeBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
        return ;
		// throw WebServer::ParsingError();

    if (this->_state == IN_LOCATION)
        this->_state = NEW_SERVER;
    else if (this->_state == IN_SERVER)
        this->_state = START;
    else
        return ;
        // throw WebServer::ParsingError();
}

std::ostream& operator<<(std::ostream& os, const ServerGenerator& config) {
    for (size_t i = 0; i < config._servers.size(); i++)
    {
        os << "server\n" << "{" << std::endl;
        for (size_t j = 0; j < config[i]->getLocations().size(); j++)
        {
            os << "\tlocation\n" << "\t{" << std::endl;
        }
        
    }
    return os;
}
