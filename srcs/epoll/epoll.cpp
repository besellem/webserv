/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 13:51:11 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

/** @brief static val */

short const	Epoll::kReadEvent = 1;
short const	Epoll::kWriteEvent = 2;

/** @brief public function */

Epoll::Epoll(void) : _sock(0) {}
Epoll::~Epoll(void) {}

Epoll::Epoll(Socket const & sock) : _sock(sock) {
	// _epollFd = kqueue();
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
}

void	Epoll::updateEvents(int & sockFd) {
	EV_SET(_chlist, sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
}

void	Epoll::handleAccept(int & newSocket) {
	int sockFd = _sock.getServerFd();
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	newSocket = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);

	if (newSocket < 0)
		errorExit("accept failed");

	// debug msg
	std::cout << "Client Connected!: form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr) << S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "]" << std::endl;
}

void	Epoll::serverLoop(int const & waitMs) {
	(void)waitMs;

	const int kMaxEvents = 20;
	int kevt = kevent(_epollFd, _chlist, 1, _evlist, kMaxEvents, NULL);
	
	// debug msg
	std::cout << "Num of request: [" S_GREEN << kevt << S_NONE "]" << std::endl;

	if (_evlist[0].flags & EV_EOF)
			errorExit("EV_EOF");

	for (int i = 0; i < kevt; i++) {

		int sockFd = _evlist[i].ident;
	
		if (_evlist[i].flags & EV_ERROR)
			errorExit("EV_ERROR");
		if (sockFd == _sock.getServerFd()) {

			int newSocket = -1;
			handleAccept(newSocket);
			_sock.readHttpRequest(newSocket);
			
			try
			{
				_sock.resolveHttpRequest();
			}
			catch (std::exception &e)
			{
				EXCEPT_WARNING;
				continue ;
			}
			_sock.sendHttpResponse(newSocket);
			close(newSocket);
		}
	}
}

/** @brief private function */

void	Epoll::errorExit(const std::string &str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

_END_NS_WEBSERV
