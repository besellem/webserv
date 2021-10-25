/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 15:27:46 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

/** @brief public function */

Epoll::Epoll(void) : _sock(0) {}
Epoll::~Epoll(void) {}

Epoll::Epoll(Socket const & sock) :
	_sock(sock),
	_epollFd(-1) {}

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	int sockFd = _sock.getServerFd();
	addEvents(sockFd);

	int kevt = kevent(_epollFd, _chlist, 1, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("epoll start failed!");
}

void	Epoll::addEvents(int const & sockFd) {
	EV_SET(_chlist, sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
}

void	Epoll::deleteEvents(int const & sockFd) {
	EV_SET(_chlist, sockFd, EVFILT_READ, EV_DELETE, 0, 0, 0);
}

void	Epoll::clientConnect(int & fd) {
	int sockFd = _sock.getServerFd();
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	fd = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);

	if (fd < 0)
		errorExit("accept failed");

	_sock.setNonBlock(fd);
	addEvents(fd);

	int kevt = kevent(_epollFd, _chlist, 1, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("kevent failed in clientConnect");

	// debug msg
	std::cout << "Client Connected!: form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr) << S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "]" << std::endl;
}

void	Epoll::clientDisconnect(int const & fd) {
	deleteEvents(fd);
	
	int kevt = kevent(_epollFd, _chlist, 1, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("kevent failed in clientConnect");

	close(fd);
}

void	Epoll::readCase(struct kevent & event) {
	_sock.readHttpRequest(event.ident);			
	try {
		_sock.resolveHttpRequest();
	}
	catch (std::exception &e) {
		EXCEPT_WARNING;
		event.flags |= EV_EOF;
		return ;
	}
	_sock.sendHttpResponse(event.ident);
	event.flags |= EV_EOF;
}

void	Epoll::eofCase(struct kevent & event) {
	clientDisconnect(event.ident);
}

void	Epoll::serverLoop(void) {
	int kevt = kevent(_epollFd, NULL, 0, _evlist, maxEvent, NULL);
	if (kevt < 0)
		errorExit("kevent failed in loop");
	
	// debug msg
	std::cout << "Num of request: [" S_GREEN << kevt << S_NONE "]" << std::endl;

	for (int i = 0; i < kevt; i++) {
		struct kevent currentEvt = _evlist[i];
		int currentSocket = -1;
		int currentFd = currentEvt.ident;
	
		if (currentFd == _sock.getServerFd()) {
			clientConnect(currentSocket);
		}
		else {
			if (currentEvt.flags & EVFILT_READ) {
				std::cout << "READ CASE\n"; 
				readCase(currentEvt);
			}
			if (currentEvt.flags & EV_EOF) {
				std::cout << "EOF CASE\n";
				eofCase(currentEvt);
			}
		}
	}
}

/** @brief private function */

void	Epoll::errorExit(const std::string &str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

_END_NS_WEBSERV
