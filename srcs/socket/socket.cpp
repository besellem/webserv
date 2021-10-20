/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 13:20:53 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

webserv::Socket::Socket(void) {}
webserv::Socket::~Socket(void) {}

webserv::Socket::Socket(short const & port) :
	_port(port),
	_addrLen(sizeof(sockaddr_in)) {

	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		errorExit("socket init");

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(port);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));
}

/** @brief public function */

short		webserv::Socket::getPort(void) { return _port; }
int			webserv::Socket::getServerFd(void) { return _serverFd; }
sockaddr_in	webserv::Socket::getAddr(void) { return _addr; }
size_t 		webserv::Socket::getAddrLen(void) { return _addrLen; }

void		webserv::Socket::startUp(void) {
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

void		webserv::Socket::parsing(int skt, std::string path) {
	getParsing(skt, path);
}

/** @brief private function */

void	webserv::Socket::errorExit(std::string const & str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	webserv::Socket::bindStep(int const & serverFd, sockaddr_in const & addr) {
	if (bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		errorExit("bind step");
}

void	webserv::Socket::listenStep(int const & serverFd) {
	if (listen(serverFd, 20) < 0)
		errorExit("listen step");
}

void	webserv::Socket::getParsing(int skt, std::string path) {
	std::string				content = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";

	std::ifstream ifs(path, std::ios::binary | std::ios::ate);
	if (ifs.is_open()) {
		int fileSize = ifs.tellg();
		content += std::to_string(fileSize);
		content += "\n\n";
		ifs.close();
	}
	else
		std::cout << "open file error: for found size of file";

	ifs.open(path, std::ios::in);
	std::string		gline;
	if (ifs.is_open()) {
		do {
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof() == true)
				break ;
			content += "\n";
		} while (true);
		::write(skt, content.c_str(), content.length());
	}
	else
		std::cout << "open file error: for get content";
	ifs.close();
}