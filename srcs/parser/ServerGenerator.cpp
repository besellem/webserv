/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:19 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 17:00:26 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
		throw WebServer::ParsingError();
	this->_servers.push_back(new Server());
	this->_state = NEW_SERVER;
}

/* if the key 'location' is well placed, add a new location to the server */

void	ServerGenerator::newLocation(Server *server, const tokens_type &tok) {
    if (this->_state != IN_SERVER)
        throw WebServer::ParsingError();
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
        throw WebServer::ParsingError();
}

/* check if the '{' is well placed */

void	ServerGenerator::openBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
		throw WebServer::ParsingError();

	if (this->_state == NEW_SERVER || this->_state == NEW_LOCATION)
        ++this->_state;
    else
        throw WebServer::ParsingError();
}

/* check if the '}' is well placed */

void	ServerGenerator::closeBlock(const tokens_type &tokens)
{
	if (tokens.size() != 1)
		throw WebServer::ParsingError();

    if (this->_state == IN_LOCATION)
        this->_state = IN_SERVER;
    else if (this->_state == IN_SERVER)
        this->_state = START;
    else
        throw WebServer::ParsingError();
}

std::ostream& operator<<(std::ostream& os, const ServerGenerator& config) {
    std::string loc_directives[] = {"return", "root", "index"};
    for (size_t i = 0; i < config._servers.size(); i++)
    {
        os << "server\n" << "{" << std::endl;
        if (config._servers[i]->getPort() != -1)
            os << "\tlisten " << config._servers[i]->getPort() << std::endl;
        if (!config._servers[i]->getName().empty())
            os << "\tserver_name " << config._servers[i]->getName() << std::endl;
        if (!config._servers[i]->getErrorPages().empty())
        {
            for (std::map<int, std::string>::const_iterator it = config._servers[i]->getErrorPages().begin(); it != config._servers[i]->getErrorPages().end(); it++)
                os << "\terror_page " << it->first << " " << it->second << std::endl;
        }
        if (config._servers[i]->getClimaxSize() != -1)
            os << "\tcli_max_size " << config._servers[i]->getClimaxSize() << std::endl;
        for (size_t j = 0; j < config[i]->getLocations().size(); j++)
        {
            t_location* loc = config[i]->getLocations()[j];
            std::string* loc_atrr[] = {&loc->redirection, &loc->root, &loc->index};
            os << "\tlocation " << loc->path << std::endl << "\t{" << std::endl;
            if (loc->autoindex == 1)
                os << "\t\tautoindex on" << std::endl;
            if (!loc->methods.empty())
            {
                os << "\t\tallow";
                for (size_t k = 0; k < loc->methods.size(); k++)
                    os << " " << loc->methods[k];
                os << std::endl;
            }
            for (size_t k = 0; k < 3; k++)
                if (!(*loc_atrr)[k].empty())
                    os << "\t\t" << loc_directives[k] << " " << (*loc_atrr)[k] << std::endl;
            
                os << "\t}" << std::endl;
        }
        os << "}" << std::endl;
    }
    return os;
}
