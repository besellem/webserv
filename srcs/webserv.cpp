/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 05:59:50 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 09:16:25 by besellem         ###   ########.fr       */
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

WebServer&	WebServer::operator=(const WebServer& x)
{
	if (this == &x)
		return *this;
	_servers = x._servers;
	return *this;
}

/*
** Declared in `parser.cpp' :
**
** void		WebServer::parse(const std::string&);
*/

size_t				WebServer::serverSize(void) const
{ return this->_servers.size(); }

const Server&		WebServer::getServer(int i)
{ return *(this->_servers[i]); }

void				WebServer::createServers(void)
{
	const size_t	server_size = _servers.size();
	HttpHeader		header;
	int				sock_fd;
	Socket			cur;

	// create a socket for each server declared in the config file
	_socks = new Socket[server_size];
	for (size_t i = 0; i < server_size; ++i)
	{
		// init each socket with each server's port
		_socks[i] = Socket(_servers[i]->port());

		cur = _socks[i];
		cur.startUp();
		while (true)
		{
			sockaddr_in	addr = cur.getAddr();
			size_t		len = cur.getAddrLen();
			sock_fd = socketAccept(cur.getServerFd(),
								   (struct sockaddr *)&addr,
								   (socklen_t *)&len);

			cur.resolveHttpRequest(sock_fd);
			// header. = resolveHttpRequest(sock_fd);
			// cur.header.data

			close(sock_fd);
		}
	} // for each server
}


const char*			WebServer::ParsingError::what() const throw()
{ return "Config File Error"; }


_END_NS_WEBSERV
