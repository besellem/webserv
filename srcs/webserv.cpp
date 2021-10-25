/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 05:59:50 by besellem          #+#    #+#             */
/*   Updated: 2021/10/25 12:16:44 by adbenoit         ###   ########.fr       */
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
WebServer&	WebServer::operator=(const WebServer& x)
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

size_t				WebServer::serverSize(void) const
{ return this->_servers.size(); }

const Server&		WebServer::getServer(int i) const
{ return *(this->_servers[i]); }

void				WebServer::createServers(void)
{
	const size_t	server_size = _servers.size();
	Socket			cur;

	// create a socket for each server declared in the config file
	_socks = new Socket[server_size];
	for (size_t i = 0; i < server_size; ++i)
	{
		// init each socket with each server's port
		_socks[i] = Socket(_servers[i]->port());
		// std::cout << i << " here\n"; // add a cout -> no seg

		cur = _socks[i];
		cur.startSocket();
		webserv::Epoll _epoll(cur);
		int fd = cur.getServerFd();

		_epoll.updateEvents(fd);
		while (true) // <------------- ??? 
		{
			_epoll.serverLoop(1);
		}
	} // for each server
}


WebServer::ParsingError::ParsingError() {}

const char*			WebServer::ParsingError::what() const throw()
{ return "Config File Error"; }


_END_NS_WEBSERV
