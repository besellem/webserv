/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 05:59:50 by besellem          #+#    #+#             */
/*   Updated: 2021/11/10 15:42:37 by adbenoit         ###   ########.fr       */
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

size_t			WebServer::countSocket() const
{
	size_t	count = _servers.size();
	for (size_t i = 0; i < _servers.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			if (_servers[i]->ip() == _servers[j]->ip() && _servers[i]->port() == _servers[j]->port())
			{
				--count;
				break ;
			}
		}
	}
	
	return count;
}

std::vector<Server *> WebServer::getSocketConfigs(size_t& index)
{
	std::vector<Server *> configs;
	bool firstOccur = 0;

	// is index the first config of this socket config
	while(!firstOccur)	
	{
		firstOccur = 1;
		for (size_t j = 0; j < index; j++)
		{
			if (_servers[index]->ip() == _servers[j]->ip() && _servers[index]->port() == _servers[j]->port())
			{
				firstOccur = 0;
				++index;
				break ;
			}
		}
	}
	
	configs.push_back(_servers[index]);
	
	// get the others configs for this socket
	for (size_t i = index + 1; i < _servers.size(); i++)
	{
		if (_servers[index]->ip() == _servers[i]->ip() && _servers[index]->port() == _servers[i]->port())
			configs.push_back(_servers[i]);
	}
	
	return configs;
}

void			WebServer::createServers(void)
{
	const size_t	n = countSocket();
	Socket			cur;

	// create a socket for each server declared in the config file
	_socks = new Socket[n];

	for (size_t i = 0; i < n; ++i)
	{
		/** @brief init server */
		// _socks[i] = Socket(_servers[i]);
		_socks[i] = Socket(getSocketConfigs(i));
		_socks[i].startSocket();
	}

	/** @brief start server */
	Epoll	_epoll(_socks, n);
	_epoll.startEpoll();
}

WebServer::ParsingError::ParsingError() {}

const char*		WebServer::ParsingError::what() const throw()
{ return "file not served by webserv"; }


_END_NS_WEBSERV
