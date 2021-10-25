/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 12:22:15 by besellem         ###   ########.fr       */
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
		EV_SET(&evts[nEvts++], sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)(intptr_t)sockFd); // defautl: event is enable, if there have some bug, try : EV_ADD | EV_ENABLE
	else if (modify == true)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_READ, EV_DELETE, 0, 0, (void *)(intptr_t)sockFd);
	
	if (events & kWriteEvent)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void *)(intptr_t)sockFd); // defautl: event is enable, if there have some bug, try : EV_ADD | EV_ENABLE
	else if (modify == true)
		EV_SET(&evts[nEvts++], sockFd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *)(intptr_t)sockFd);

	// debug msg
	std::cout << "\n" << (modify == true ? "[" S_GREEN "MODIFY" S_NONE "] fd: [" S_GREEN : "[" S_GREEN "ADD" S_NONE "] fd: [" S_GREEN) << sockFd << S_NONE "] events: [" S_GREEN << ((events & kReadEvent) ? "Read" S_NONE : "NOTHING" S_NONE) << "] & [" S_GREEN << ((events & kWriteEvent) ? "Write" S_NONE "]" : "NOTHING" S_NONE "]") << "\n" << std::endl;

	if (kevent(_epollFd, evts, nEvts, NULL, 0, NULL) < 0)
		errorExit("kevent failed");
	// kevent(_epollFd, evts, nEvts, NULL, 0, NULL);
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
	std::cout << "Client Connected!: form: [" S_GREEN << inet_ntoa(_sock.getAddr().sin_addr) << S_NONE "]:[" S_GREEN << ntohs(_sock.getAddr().sin_port) << S_NONE "]" << std::endl;
}

// void	Epoll::handleRead(int & sockFd) {
// 	// char header[30000] = {0};
// 	// recv(sockFd, header, 30000, 0);
// 	// std::cout << "\nFrom client(receive):\n" S_GREEN << header << S_NONE << std::endl;
// 	// close(sockFd);

// 	// updateEvents(sockFd, kWriteEvent, false);
// }

// void	Epoll::handleWrite(int & sockFd) {
// 	const std::string			const_path = "./www/index.html";

// 	_sock.parse(sockFd, const_path);
// 	close(sockFd);
// 	// updateEvents(sockFd, kReadEvent, true);
	
// 	// debug msg
// 	std::cout << "\nSend successfully:\n" << std::endl;
// }

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
