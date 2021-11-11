/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/11/11 16:18:20 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


_BEGIN_NS_WEBSERV

Socket::Socket(void) :
	_port(0),
	_serverFd(SYSCALL_ERR),
	_addrLen(sizeof(sockaddr_in))
{
	std::memset(&_addr, 0, sizeof(_addr));
}

Socket::Socket(const std::vector<Server *> serv) :
	_server_blocks(serv),
	_port(serv[0]->port()),
	_addrLen(sizeof(sockaddr_in))
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (SYSCALL_ERR == _serverFd)
		errorExit("socket init");
	_addr.sin_family = AF_INET;
	if ((_addr.sin_addr.s_addr = inet_addr(serv[0]->ip().c_str())) == (in_addr_t)SYSCALL_ERR)
		errorExit("socket address");
	_addr.sin_port = htons(_port);
	std::memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

	int	optval = 1;
	if (SYSCALL_ERR == setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) // can rebind
	{
		close(_serverFd);
		errorExit("set opt");
	}
	setNonBlock(_serverFd);
}

Socket::Socket(const Socket &x)
{ *this = x; }

Socket::~Socket(void)
{}

Socket&		Socket::operator=(const Socket &x)
{
	if (this == &x)
		return *this;
	_server_blocks = x._server_blocks;
	_port = x._port;
	_serverFd = x._serverFd;
	_addrLen = x._addrLen;
	std::memcpy(&_addr, &x._addr, sizeof(sockaddr_in));
	return *this;
}

/** @brief public function */

short			Socket::getPort(void)     const { return _port; }
int				Socket::getServerFd(void) const { return _serverFd; }
sockaddr_in		Socket::getAddr(void)     const { return _addr; }
size_t			Socket::getAddrLen(void)  const { return _addrLen; }

const Server*	Socket::getServer(void)	const { return _server_blocks[0]; }

const Server*	Socket::getServer(const std::string &name) const {
	for (size_t i = 0; i < _server_blocks.size(); i++)
	{
		for (size_t j = 0; j < _server_blocks[i]->name().size(); j++)
			if (_server_blocks[i]->name()[j] == name)
				return _server_blocks[i];
	}

	return _server_blocks[0];
}

void	Socket::startSocket(void)
{
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

void	Socket::setNonBlock(int & fd)
{
	if (SYSCALL_ERR == fcntl(fd, F_SETFL, O_NONBLOCK))
	{
		std::cout << "Error: set non block" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/*
** Read the socket's http request
**
** @return a `enum e_read' value
*/
int		Socket::readHttpRequest(Request *request, int socket_fd)
{
	int	ret;

	while (true)
	{
		request->getHeader().resetBuffer();
		ret = recv(socket_fd, request->getHeader().buf, sizeof(request->getHeader().buf), 0);
		if (SYSCALL_ERR == ret)
		{
			// return READ_FAIL; // false
			break ;
		}
		else if (0 == ret)
		{
			std::cout << "Client disconnected" << std::endl;
			break ;
		}
		else
		{
			request->getHeader().buf[ret] = '\0';
			request->getHeader().content += request->getHeader().buf;
		}
	}

	if (DEBUG)
	{
		std::cout << "++++++++++++++ REQUEST +++++++++++++++\n" << std::endl;
		std::cout << request->getHeader().content << std::endl;
		std::cout << "\n++++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
	}

	if (0 == ret)
		return READ_DISCONNECT;
	else
		return READ_OK;
}

/*
** Parse the http request
**
** @return a `enum e_resolve' value
*/
int		Socket::resolveHttpRequest(Request *request)
{
	/* the buffer is empty, therefore the header was also empty */
	if (request->getHeader().content.empty())
		return RESOLVE_EMPTY;
	
	vector_type				buffer = split_string(request->getHeader().content, NEW_LINE);
	vector_type::iterator	line = buffer.begin();

	/*
	** Parse first line of request. Must be formatted like so:
	**   GET /index.html HTTP/1.1
	*/
	if (request->setRequestFirstLine(*line++) == false)
		return RESOLVE_FAIL;

	if (DEBUG)
	{
		std::cout << "Path            : [" S_CYAN << request->getHeader().uri    << S_NONE "]\n";
		std::cout << "Contructed Path : [" S_CYAN << request->getConstructPath() << S_NONE "]\n";
	}

	// set query string
	size_t pos = request->getHeader().uri.find("?");
	if (pos != std::string::npos)
		request->getHeader().queryString = request->getHeader().uri.substr(pos + 1);

	/* map each line of the header */
	for ( ; line != buffer.end() && !(*line).empty(); ++line)
	{
		request->setHeaderData(*line);
	}
	std::string name = request->getHeader().data["Host"][0];
	// request->setServer(getServer(name));
	
	request->setChunked();
	request->setContent();

	return RESOLVE_OK;
}

/*
** Construct the htttp response and send it to the server
**
** @return a `enum e_send' value
*/
int		Socket::sendHttpResponse(Request* request, int socket_fd)
{
	Response		response(request);
	std::string		toSend;
	
	if (!is_valid_path(request->getConstructPath()))
		response.setStatus(404);
	
	response.setContent(getFileContent(request->getConstructPath()));
	if (response.getCgiStatus() == 0)
		return SEND_FAIL;
	response.setHeader();
	
	toSend =  response.getHeader();
	toSend += NEW_LINE;
	toSend += response.getContent();

	// -- Send to server --
	if (SYSCALL_ERR == send(socket_fd, toSend.c_str(), toSend.length(), 0))
		return SEND_FAIL;

	if (DEBUG)
	{
		std::cout << "------------- RESPONSE ---------------" << std::endl;
		std::cout << toSend.c_str() << std::endl;
		std::cout << "--------------------------------------" << std::endl << std::endl;
	}

	return SEND_OK;
}

/** @brief private function */

void	Socket::errorExit(const std::string& str) const
{
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	Socket::bindStep(const int& serverFd, const sockaddr_in& addr)
{
	if (SYSCALL_ERR == bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)))
		errorExit("bind step");
}

void	Socket::listenStep(const int& serverFd)
{
	if (SYSCALL_ERR == listen(serverFd, SOMAXCONN))
		errorExit("listen step");
}

_END_NS_WEBSERV
