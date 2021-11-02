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

_BEGIN_NS_WEBSERV


Server::Server() : _port(8000), _cliMaxSize(-1) {}

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
const int&							Server::port()              const { return this->_port; }
const std::vector<std::string>&		Server::name()              const { return this->_name; }
const std::map<int, std::string>&	Server::errorPages()        const { return this->_errorPages; }
const int&							Server::cliMaxSize()        const { return this->_cliMaxSize; }
const t_location&	                Server::locations(int i)    const { return *(this->_locations[i]); }
const std::vector<t_location *>&	Server::locations(void)     const { return this->_locations; }
size_t  Server::nLoc()									        const { return this->_locations.size(); }

/*
**  Setters
*/

void	Server::setName(const tokens_type &tok) {
    this->_name.assign(tok.begin() + 1, tok.end());
}

void	Server::setPort(const tokens_type &tok) {
     if (tok.size() != 2)
        throw WebServer::ParsingError();
    if (!ft_isNumeric(tok[1]))
        throw WebServer::ParsingError();
    std::stringstream(tok[1]) >> this->_port;
    if (this->_port > std::numeric_limits<unsigned short>().max())
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
        this->_errorPages[error_code] = ROOT_PATH + std::string("/") + tok.back();
    }
}

void	Server::setCliMaxSize(const tokens_type &tok) {
     if (tok.size() != 2)
        throw WebServer::ParsingError();
    if (!ft_isNumeric(tok[1]))
        throw WebServer::ParsingError();
    std::stringstream(tok[1]) >> this->_cliMaxSize;
}

void	Server::setCgi(t_location  *loc, const tokens_type &tok) {
    loc->cgi.assign(tok.begin() + 1, tok.end());
}

void	Server::setIndex(t_location  *loc, const tokens_type &tok) {
    loc->index.assign(tok.begin() + 1, tok.end());
}

void	Server::setAutoIndex(t_location  *loc, const tokens_type &tok) {
    if (tok.size() != 2)
        throw WebServer::ParsingError();
    if (tok[1] == "on")
        loc->autoindex = ON;
    else if (tok[1] != "off")
        throw WebServer::ParsingError();
}

void	Server::setRedirection(t_location  *loc, const tokens_type &tok) {
    if (tok.size() != 2)
        throw WebServer::ParsingError();
    loc->redirection = tok[1];
}

void	Server::setRoot(t_location  *loc, const tokens_type &tok) {
    if (tok.size() != 2)
        throw WebServer::ParsingError();
    loc->root = tok[1];
}

void	Server::setMethods(t_location  *loc, const tokens_type &tok)
{
    std::string methods[] = {"GET", "POST", "DELETE"};

    for(tokens_type::const_iterator it = tok.begin() + 1; it != tok.end(); it++)
    {
        size_t i;
        for(i = 0; i < 3; i++)
        {
            if (*it == methods[i])
            {
                bool exist = 0;
                for(tokens_type::const_iterator it1 = loc->methods.begin(); it1 != loc->methods.end(); it1++)
                    if (*it1 == methods[i])
                        exist = 1;
                if (!exist)
                    loc->methods.push_back(methods[i]);
                break ;
            }
        }
        if (i == 3)
            throw WebServer::ParsingError();
    }
}

/*
**  Modifiers
*/

/* Adds a new location to the server */
void	Server::newLocation(const tokens_type &tok) {
	if (tok.size() != 2 || tok[1].find(';') != std::string::npos)
		throw WebServer::ParsingError();
	t_location* loc = new t_location;
	loc->path = tok[1];
	loc->methods.push_back("GET");
	loc->autoindex = OFF;
	this->_locations.push_back(loc);
}

/* Adds the new directive to the location */
void	Server::newLocationDirective(const tokens_type &tok)
{
    std::string directives[] = {"allow", "rewrite", "root",
            "index", "autoindex", "cgi_pass"};
	static method_function1   method_ptr[] = {&Server::setMethods,
		    &Server::setRedirection, &Server::setRoot, &Server::setIndex,
            &Server::setAutoIndex, &Server::setCgi,};
    t_location* loc = this->_locations.back();

    if (tok.size() < 2)
        throw WebServer::ParsingError();
	for (size_t i = 0; i < 6; i++)
	{
		if (tok[0] == directives[i])
		{
			method_function1 func = method_ptr[i];
			(this->*func)(loc, tok);
            return ;
		}
	}
    throw WebServer::ParsingError();
}

/* Adds the new directive to the server */
void	Server::newDirective(const tokens_type &tokens)
{
	if (tokens.empty())
		return ;
	static method_function   method_ptr[] = {&Server::setPort,
            &Server::setName, &Server::setErrorPages, &Server::setCliMaxSize};
	static std::string  directives[] = {"listen", "server_name",
			"error_page", "client_max_body_size"};

	for (size_t i = 0; i < 4; i++)
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

/* Display location block like the config file */
std::ostream& operator<<(std::ostream& os, const t_location& loc)
{
    std::string directives[] = {"rewrite", "root"};
    const std::string* loc_atrr[] = {&loc.redirection, &loc.root};
    os << "\tlocation " << loc.path << std::endl << "\t{" << std::endl;
    if (loc.autoindex == ON)
        os << "\t\tautoindex on" << std::endl;
    if (!loc.methods.empty())
    {
        os << "\t\tallow";
        for (size_t k = 0; k < loc.methods.size(); k++)
            os << " " << loc.methods[k];
        os << std::endl;
    }
    if (!loc.cgi.empty())
    {
        os << "\t\tcgi";
        for (size_t k = 0; k < loc.cgi.size(); k++)
            os << " " << loc.cgi[k];
        os << std::endl;
    }
    if (!loc.index.empty())
    {
        os << "\t\tindex";
        for (size_t k = 0; k < loc.index.size(); k++)
            os << " " << loc.index[k];
        os << std::endl;
    }
    for (size_t k = 0; k < 2; k++)
        if (!(*loc_atrr)[k].empty())
            os << "\t\t" << directives[k] << " " << (*loc_atrr)[k] << std::endl;
    os << "\t}" << std::endl;
    return os;
}

/* Display server block like the config file */
std::ostream& operator<<(std::ostream& os, const Server& server)
{
    os << "server\n" << "{" << std::endl;
    if (server.port() != -1)
        os << "\tlisten " << server.port() << std::endl;
    if (!server.name().empty())
    {
        os << "\tserver_name";
        for (std::vector<std::string>::const_iterator it = server.name().begin();
            it != server.name().end(); it++)
            os << "  " << *it;
        os << std::endl;
    }
    if (!server.errorPages().empty())
    {
        for (std::map<int, std::string>::const_iterator it = server.errorPages().begin();
            it != server.errorPages().end(); it++)
            os << "\terror_page " << it->first << " " << it->second << std::endl;
    }
    if (server.cliMaxSize() != -1)
        os << "\tclient_max_body_size " << server.cliMaxSize() << std::endl;
    for (size_t j = 0; j < server.nLoc(); j++)
        os << server.locations(j);
    os << "}" << std::endl;
    return os;
}


_END_NS_WEBSERV
