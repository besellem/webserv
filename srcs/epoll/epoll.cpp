/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/21 18:18:19 by kaye             ###   ########.fr       */
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
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
}

void	Epoll::updateEvents(int & sockFd, int const & events, bool const & modify) {
	struct kevent	evts[EP_EVENTS];
	int				nEvts = 0;

	if (events & kReadEvent)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_READ, EV_ADD, 0, 0, (void *)(intptr_t)sockFd); // defautl: event is enable, if there have some bug, try : EV_ADD | EV_ENABLE
	else if (modify == true)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_READ, EV_DELETE, 0, 0, (void *)(intptr_t)sockFd);

	
	if (events & kWriteEvent)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_WRITE, EV_ADD, 0, 0, (void *)(intptr_t)sockFd); // defautl: event is enable, if there have some bug, try : EV_ADD | EV_ENABLE
	else if (modify == true)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *)(intptr_t)sockFd);

	// debug msg
	// std::cout << (modify == true ? "[MODIFY] fd: [" : "[ADD] fd: [") << sockFd << "] events: [" << ((events & kReadEvent) ? "Read" : "Write") << "]." << std::endl;

	if (kevent(_epollFd, evts, nEvts, NULL, 0, NULL) < 0)
		errorExit("kevent failed");
}

void	Epoll::handleAccept(void) {
	int newSocket = _sock.socketAccept();

	if (newSocket < 0)
		errorExit("accept failed");

	// debug msg
	// socklen_t len = sizeof(sockaddr_in);
	// int r = getpeername(newSocket, (sockaddr*)&_sock.getAddr(), &len);
	// if (r < 0)
	// 	errorExit("getpeername failed");
	// printf("accept a connection from %s\n", inet_ntoa(_sock.getAddr().sin_addr));

	_sock.setNonBlock(newSocket);
	updateEvents(newSocket, kReadEvent|kWriteEvent, false);
}

void	Epoll::handleRead(int & sockFd) {
	char header[30000] = {0};
	recv(sockFd, header, 30000, 0);
	std::cout << "handleRead:\n" << header << std::endl;
	close(sockFd);
}

void	Epoll::handleWrite(int & sockFd) {
	const std::string			const_path = "./www/index.html";

	_sock.parse(sockFd, const_path);
	updateEvents(sockFd, kReadEvent, true);
}

void	Epoll::serverLoop(int const & waitMs) {
	timespec	timeout;
	timeout.tv_sec = waitMs / 1000;
	timeout.tv_nsec = (waitMs % 1000) * 1000 * 1000;

	const int kMaxEvents = 20;
	struct kevent	activeEvs[kMaxEvents];

	int kevt = kevent(_epollFd, NULL, 0, activeEvs, kMaxEvents, &timeout);
	std::cout << "epoll_wait return: [" << kevt << "]" << std::endl;

	for (int i = 0; i < kevt; i++) {
		int sockFd = (int)(intptr_t)activeEvs[i].udata;
		int events = activeEvs[i].filter;
		if (events == EVFILT_READ) {
			if (sockFd == _sock.getServerFd())
				handleAccept();
			else
				handleRead(sockFd);
		}
		else if (events == EVFILT_WRITE)
			handleWrite(sockFd);
		else
			errorExit("Unknow event");
	}
}

/** @brief private function */

void	Epoll::errorExit(const std::string &str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

_END_NS_WEBSERV