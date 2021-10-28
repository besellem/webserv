/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/28 19:02:48 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

/** @brief public function */

Epoll::Epoll(void) : _sock() {}
Epoll::~Epoll(void) {
	delete [] _chlist;
	delete [] _evlist;
}

Epoll::Epoll(std::vector<Socket> const & multiSock) :
_multiSock(multiSock),
_epollFd(-1) {
	_nListenEvent = multiSock.size();
	_chlist = new struct kevent[_nListenEvent];
	_evlist = new struct kevent[_nListenEvent];
}

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	int i = 0;
	for (std::vector<Socket>::iterator it = _multiSock.begin(); it != _multiSock.end(); it++, i++) {
		int sockFd = it->getServerFd();

		EV_SET(&_chlist[i], sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
	}

	int kevt = kevent(_epollFd, _chlist, _nListenEvent, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("epoll start failed!");
}

void	Epoll::addEvents(int const & sockFd) {
	EV_SET(_chlist, sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
}

void	Epoll::deleteEvents(int const & sockFd) {
	EV_SET(_chlist, sockFd, EVFILT_READ, EV_DELETE, 0, 0, 0);
}

void	Epoll::clientConnect(int & fd, int const & serverFd) {
	int sockFd = serverFd;

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	fd = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);

	if (fd < 0)
		errorExit("accept failed");

	std::cout << "accept fd: " << fd << std::endl;

	_sock.setNonBlock(fd);

	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "]" << "\n" << std::endl;
}

void	Epoll::readCase(int & fd, Socket & sock) {
	// debug msg
	std::cout << S_RED "Reading ..." S_NONE << "\n" << std::endl;

	sock.readHttpRequest(fd);			
	try {
		sock.resolveHttpRequest();
	}
	catch (std::exception &e) {
		EXCEPT_WARNING;
		return ;
	}
	sock.sendHttpResponse(fd);
}

void	Epoll::serverLoop(void) {
	for(;;) {
		int kevt = kevent(_epollFd, NULL, 0, _evlist, _nListenEvent, NULL);
		if (kevt < 0)
			errorExit("kevent failed in loop");

		if (kevt > 0)
			// debug msg
			std::cout << "---\nStar: Num of request: [" S_GREEN << kevt << S_NONE "]" << "\n---\n" << std::endl;

		Socket tmp;

		for (int i = 0; i < kevt; i++) {
			struct kevent currentEvt = _evlist[i];
			int currentSocket = -1;
			int currentFd = currentEvt.ident;

			std::cout << "currFd: " << currentFd << std::endl;

			for (int j = 0; j < _nListenEvent; j++) {
				if (currentFd == _multiSock[j].getServerFd()) {
					clientConnect(currentSocket, _multiSock[j].getServerFd());
					tmp = _multiSock[j];

					readCase(currentSocket, tmp);

					// debug msg
					std::cout << S_RED "closing ..." S_NONE << "\n" << std::endl;
					
					close(currentSocket);
				}
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
