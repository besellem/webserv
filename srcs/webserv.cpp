/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 05:59:50 by besellem          #+#    #+#             */
/*   Updated: 2021/11/01 15:27:32 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


_BEGIN_NS_WEBSERV

WebServer::WebServer(void) :
	_servers(),
	_socks(nullptr)
{}

WebServer::WebServer(const WebServer& x)
{
	*this = x;
}

WebServer::~WebServer()
{
	if (_socks != nullptr)
	{
		delete [] _socks;
		_socks = nullptr;
	}
}

// TO DO but unused
WebServer&		WebServer::operator=(const WebServer& x)
{
	if (this == &x)
		return *this;
	_servers = x._servers;
	// _socks = x._socks;
	return *this;
}

/*
** Declared in `parser.cpp' :
**
** void		WebServer::parse(const std::string&);
*/

size_t			WebServer::serverSize(void) const
{ return this->_servers.size(); }

const Server&	WebServer::getServer(int i) const
{ return *(this->_servers[i]); }

void			WebServer::createServers(void)
{
	const size_t	server_size = _servers.size();
	Socket			cur;

	// create a socket for each server declared in the config file
	_socks = new Socket[server_size];

	for (size_t i = 0; i < server_size; ++i)
	{
		/** @brief init server */
		_socks[i] = Socket(_servers[i]);
		_socks[i].startSocket();
	}

	/** @brief start server */
	Epoll	_epoll(_socks, server_size);
	_epoll.startEpoll();
}

WebServer::ParsingError::ParsingError() {}

const char*		WebServer::ParsingError::what() const throw()
{ return "Config File Error"; }


_END_NS_WEBSERV
