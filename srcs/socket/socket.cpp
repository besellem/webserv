/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/11/05 14:30:01 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


_BEGIN_NS_WEBSERV

Socket::Socket(void) :
	_port(0),
	_serverFd(SYSCALL_ERR),
	_addrLen(sizeof(sockaddr_in))
{}

Socket::~Socket(void)
{}

Socket::Socket(const Server *serv) :
	_server_block(serv),
	_port(serv->port()),
	_addrLen(sizeof(sockaddr_in))
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (SYSCALL_ERR == _serverFd)
		errorExit("socket init");
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

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

Socket&		Socket::operator=(const Socket &x)
{
	if (this == &x)
		return *this;
	_server_block = x._server_block;
	_port = x._port;
	_serverFd = x._serverFd;
	_addrLen = x._addrLen;
	memcpy(&_addr, &x._addr, sizeof(sockaddr_in));
	return *this;
}

/** @brief public function */

short			Socket::getPort(void)     const { return _port; }
int				Socket::getServerFd(void) const { return _serverFd; }
const Server*	Socket::getServer(void)   const { return _server_block; }
sockaddr_in		Socket::getAddr(void)     const { return _addr; }
size_t			Socket::getAddrLen(void)  const { return _addrLen; }

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

/* Read the socket's http request */
void	Socket::readHttpRequest(Request *request, int socket_fd)
{
	int	ret;

	while (true)
	{
		request->getHeader().resetBuffer();
		ret = recv(socket_fd, request->getHeader().buf, sizeof(request->getHeader().buf), 0);
		if (SYSCALL_ERR == ret)
		{
			break ;
			// if (errno == EAGAIN || errno == EWOULDBLOCK)
			// 	break;
			// else
			// 	errorExit("read http request");
		}
		else if (0 == ret)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
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
		// write(STDOUT_FILENO, request->getHeader().content.c_str(), request->getHeader().content.length());
		std::cout << "\n++++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
	}
}

/* Parse the http request */
void	Socket::resolveHttpRequest(Request *request)
{
	/* the buffer is empty, therefore the header was also empty */
	if (request->getHeader().content.empty())
		return ;
	
	vector_type				buffer = split_string(request->getHeader().content, NEW_LINE);
	vector_type::iterator	line = buffer.begin();

	/*
	** Parse first line of request. Must be formatted like so:
	**   GET /index.html HTTP/1.1
	*/
	vector_type				first_line = split_string(*line, " ");
	if (first_line.size() != 3)
		throw HttpHeader::HttpBadRequestError();
	
	request->getHeader().request_method = first_line[0];
	request->getHeader().uri = first_line[1];
	request->setConstructPath();
	++line;

	if (DEBUG)
	{
		std::cout << "Path            : [" S_CYAN << request->getHeader().uri << S_NONE "]\n";
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

	request->setChunked();
	request->setContent();

	/* there's some more info to parse */
	// if (line != buffer.end() && (*line).empty())
	// {
	// 	++line; // first line is always empty
		
	// 	std::cout << S_RED "-> afterward" S_NONE << std::endl;

	// 	if (request->getHeader().chunked)
	// 	{
	// 		for ( ; line != buffer.end(); ++line)
	// 		{
	// 			// std::cout << "[" << std::stoul(*line, NULL, 16) << "]" << std::endl;
	// 			PRINT(*line);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for ( ; line != buffer.end(); ++line)
	// 		{
	// 			PRINT(*line);
	// 		}
	// 	}
	// 	std::cout << S_RED "-> end" S_NONE << std::endl;
	// }
}

/* Construct the htttp response and send it to the server */
void		Socket::sendHttpResponse(Request* request, int socket_fd)
{
	Response		response(request);
	std::string		toSend;
	
	if (!is_valid_path(request->getConstructPath()))
		response.setStatus(404);
	response.setContent(getFileContent(request->getConstructPath()));
	response.setHeader();
	
	toSend =  response.getHeader();
	toSend += NEW_LINE;
	toSend += response.getContent();

	// -- Send to server --
	send(socket_fd, toSend.c_str(), toSend.length(), 0);

	if (DEBUG)
	{
		std::cout << "------------- RESPONSE ---------------" << std::endl;
		std::cout << toSend.c_str() << std::endl;
		std::cout << "--------------------------------------" << std::endl << std::endl;
	}
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
