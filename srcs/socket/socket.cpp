/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/24 16:43:12 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

_BEGIN_NS_WEBSERV

Socket::Socket(void) {}
Socket::~Socket(void) {}

Socket::Socket(const short& port) :
	_port(port),
	_addrLen(sizeof(sockaddr_in))
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		errorExit("socket init");

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(port);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));
}

/** @brief public function */

short		Socket::getPort(void)     const { return _port; }
int			Socket::getServerFd(void) const { return _serverFd; }
sockaddr_in	Socket::getAddr(void)     const { return _addr; }
size_t		Socket::getAddrLen(void)  const { return _addrLen; }

void	Socket::startUp(void)
{
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

/** @brief socket tools */
void	Socket::setNonBlock(int & fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
		std::cout << "Error: set non block" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int		Socket::socketAccept(void) {
	int socketFd = accept(_serverFd, (struct sockaddr *)&_addr, (socklen_t *)&_addrLen);
	
	if (socketFd < 0) {
		std::cout << "Error: accept" << std::endl;
		exit(EXIT_FAILURE);
	}
	return socketFd;
}

void	Socket::_parse_wrapper(const char* http_header)
{
	// std::map<std::string, std::string>
	std::string		head(http_header);
	std::string		content;

	do
	{
		size_t pos = head.find('\n');
		if (pos == std::string::npos)
			break ;
		// head.substr(0, );
	} while (true);
}

// -- in process --
void	Socket::parse(int skt, const char* http_header)
{
	getParsing(skt, http_header);
}

// TO REMOVE
void	Socket::parse(int skt, const std::string& path)
{
	getParsing(skt, path);
}

/** @brief private function */

void	Socket::errorExit(const std::string& str) const
{
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	Socket::bindStep(const int& serverFd, const sockaddr_in& addr)
{
	if (bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		errorExit("bind step");
}

void	Socket::listenStep(const int& serverFd)
{
	if (listen(serverFd, 20) < 0)
		errorExit("listen step");
}

// -- in process --
void	Socket::getParsing(int skt, const char* http_header)
{
	std::string		content = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";

	std::ifstream	ifs(http_header, std::ios::binary | std::ios::ate);
	if (ifs.is_open())
	{
		int	fileSize = ifs.tellg();
		ifs.close();
		content += std::to_string(fileSize);
		content += "\n\n";
	}
	else
		std::cout << "open file error: for found size of file" << std::endl;

	ifs.open(http_header, std::ios::in);
	std::string	gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof() == true)
				break ;
			content += "\n";
		} while (true);
		send(skt, content.c_str(), content.length(), 0);
	}
	else
		std::cout << "open file error: for get content";
	ifs.close();
}

// TO REMOVE
void	Socket::getParsing(int skt, const std::string& path)
{
	std::string		content = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";

	std::ifstream	ifs(path, std::ios::binary | std::ios::ate);
	if (ifs.is_open())
	{
		int	fileSize = ifs.tellg();
		ifs.close();
		content += std::to_string(fileSize);
		content += "\n\n";
	}
	else
		std::cout << "open file error: for found size of file" << std::endl;

	ifs.open(path, std::ios::in);
	std::string	gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof() == true)
				break ;
			content += "\n";
		} while (true);
		send(skt, content.c_str(), content.length(), 0);
	}
	else
		std::cout << "open file error: for get content";
	ifs.close();
}

_END_NS_WEBSERV