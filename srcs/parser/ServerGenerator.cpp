/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:19 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 17:05:37 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"

ServerGenerator::ServerGenerator() { this->_sate = START; }

ServerGenerator::~ServerGenerator() {}

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

Server*	ServerGenerator::operator[](size_t i) {
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
	if (tokens.size() != 1 || state != START)
		throw WebServer::ParsingError();
	gen._servers.push_back(new Server());
	this->_state = NEW_SERVER;
}

/* if the key 'location' is well placed, add a new location to the server */

void	ServerGenerator::newLocation(Server *server, const tokens_type &tok) {
    if (this->_state != SERVER_OPEN)
        throw WebServer::ParsingError();
    server->newLocation(tok);
    this->_state = NEW_LOCATION;
}

/* if the directive is well placed, add it to the server */

void	ServerGenerator::newDirective(Server *server, const tokens_type &tok) {
    if (this->_state == SERVER_OPEN)
        server->newDirective(tok);
    else if (this->_state == LOCATION_OPEN)
        server->newLocationDirective(tok);
    else
        throw WebServer::ParsingError();
    this->_state = NEW_LOCATION;
}

/* check if the '{' is well placed */

void	ServerGenerator::openBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
		throw WebServer::ParsingError();

	if (this->_state == NEW_SERVER)
        this->_state = SERVER_OPEN;
    else if (this->_state == NEW_LOCATION)
        this->_state = LOCATION_OPEN;
    else
        throw WebServer::ParsingError();
}

/* check if the '}' is well placed */

void	ServerGenerator::closeBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
		throw WebServer::ParsingError();

    if (this->_state == LOCATION_OPEN)
        this->_state = NEW_SERVER
    else if (this->_state == SERVER_OPEN || this->_state == LOCATION_CLOSE)
        this->_state = START;
    else
        throw WebServer::ParsingError();
}

