/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 18:12:55 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

_INLINE_NAMESPACE::Socket::Socket(void)  {}
_INLINE_NAMESPACE::Socket::~Socket(void) {}

_INLINE_NAMESPACE::Socket::Socket(const short& port) :
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

short		_INLINE_NAMESPACE::Socket::getPort(void)     const { return _port; }
int			_INLINE_NAMESPACE::Socket::getServerFd(void) const { return _serverFd; }
sockaddr_in	_INLINE_NAMESPACE::Socket::getAddr(void)     const { return _addr; }
size_t		_INLINE_NAMESPACE::Socket::getAddrLen(void)  const { return _addrLen; }

void	_INLINE_NAMESPACE::Socket::startUp(void)
{
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

void	_INLINE_NAMESPACE::Socket::_parse_wrapper(const char* http_header)
{
	// std::map<std::string, std::string>
	std::string		head(http_header);
	std::string		content;

	do
	{
		size_t pos = head.find('\n');
		if (pos == std::string::npos)
			break ;
		head.substr(0, );
	} while (true);
}

void	_INLINE_NAMESPACE::Socket::parse(int skt, const char* http_header)
{
	getParsing(skt, http_header);
}

/** @brief private function */

void	_INLINE_NAMESPACE::Socket::errorExit(const std::string& str) const
{
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	_INLINE_NAMESPACE::Socket::bindStep(const int& serverFd, const sockaddr_in& addr)
{
	if (bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		errorExit("bind step");
}

void	_INLINE_NAMESPACE::Socket::listenStep(const int& serverFd)
{
	if (listen(serverFd, 20) < 0)
		errorExit("listen step");
}

void	_INLINE_NAMESPACE::Socket::getParsing(int skt, const char* http_header)
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
