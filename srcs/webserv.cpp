/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 05:59:50 by besellem          #+#    #+#             */
/*   Updated: 2021/10/28 13:22:21 by kaye             ###   ########.fr       */
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

	// std::vector<Socket> multiServ;

	// create a socket for each server declared in the config file
	_socks = new Socket[server_size];

	for (size_t i = 0; i < server_size; ++i)
	{
		// init each socket
		_socks[i] = Socket(_servers[i]);

		cur = _socks[i];
		cur.startSocket();

		// multiServ.push_back(cur);

		Epoll	_epoll(cur);
		_epoll.startEpoll();
		_epoll.serverLoop();
	} // for each server

	// exit(EXIT_SUCCESS);

	// Epoll	_epoll(cur);
	// _epoll.startEpoll();
	// _epoll.serverLoop();
}

WebServer::ParsingError::ParsingError() {}

const char*		WebServer::ParsingError::what() const throw()
{ return "Config File Error"; }


_END_NS_WEBSERV
