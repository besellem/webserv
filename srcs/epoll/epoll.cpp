/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/28 18:04:24 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

# define NB false // set kqueue in non-block mode

_BEGIN_NS_WEBSERV

/** @brief public function */

Epoll::Epoll(void) : _sock() {}
Epoll::~Epoll(void) {
	delete [] _chlist;
}

Epoll::Epoll(Socket const & sock) :
	_sock(sock),
	_epollFd(-1) {
		_nListenEvent = 1;
		_chlist = new struct kevent[1];
	}

Epoll::Epoll(std::vector<Socket> const & multiSock) :
_multiSock(multiSock),
_epollFd(-1) {
	_nListenEvent = multiSock.size();
	_chlist = new struct kevent[_nListenEvent];
}

// void	Epoll::startEpoll(void) {
// 	_epollFd = kqueue();
// 	if (_epollFd < 0)
// 		errorExit("epoll create");
	
// 	int sockFd = _sock.getServerFd();
// 	addEvents(sockFd);

// 	// test non block
// 	struct timespec timeout = {0, 0};

// 	int kevt = kevent(_epollFd, _chlist, _nListenEvent, NULL, 0, (NB == true) ? &timeout : NULL);
// 	if (kevt < 0)
// 		errorExit("epoll start failed!");

// 	std::cout << "Epoll begin: " << kevt << std::endl;
// }

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	int i = 0;
	for (std::vector<Socket>::iterator it = _multiSock.begin(); it != _multiSock.end(); it++, i++) {
		int sockFd = it->getServerFd();
		// addEvents(sockFd);

		std::cout << "add socketFd: " << sockFd << std::endl;
		EV_SET(&_chlist[i], sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
	}

	// test non block
	struct timespec timeout = {0, 0};

	int kevt = kevent(_epollFd, _chlist, _nListenEvent, NULL, 0, (NB == true) ? &timeout : NULL);
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
	addEvents(fd);

	// test non block
	struct timespec timeout = {0, 0};

	int kevt = kevent(_epollFd, _chlist, 1, NULL, 0, (NB == true) ? &timeout : NULL);
	if (kevt < 0)
		errorExit("kevent failed in clientConnect");

	// debug msg
	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "]" << "\n" << std::endl;
}

void	Epoll::clientDisconnect(int const & fd) {
	deleteEvents(fd);
	
	// test non block
	struct timespec timeout = {0, 0};

	int kevt = kevent(_epollFd, _chlist, 1, NULL, 0, (NB == true) ? &timeout : NULL);
	if (kevt < 0)
		errorExit("kevent failed in clientConnect");

	close(fd);
}

void	Epoll::readCase(struct kevent & event, Socket & sock) {
	// debug msg
	std::cout << S_RED "Reading ..." S_NONE << "\n" << std::endl;

	sock.readHttpRequest(event.ident);			
	try {
		sock.resolveHttpRequest();
	}
	catch (std::exception &e) {
		EXCEPT_WARNING;
		event.flags |= EV_EOF;
		return ;
	}
	sock.sendHttpResponse(event.ident);
	event.flags |= EV_EOF;
}

void	Epoll::eofCase(struct kevent & event) {
	// debug msg
	std::cout << S_RED "Closing ..." S_NONE << "\n" << std::endl;

	clientDisconnect(event.ident);
}

void	Epoll::serverLoop(void) {
	for(;;) {
		// test non block
		struct timespec timeout = {0, 0};

		int kevt = kevent(_epollFd, NULL, 0, _evlist, maxEvent, (NB == true) ? &timeout : NULL);
		if (kevt < 0)
			errorExit("kevent failed in loop");
		if (kevt > 0)
			// debug msg
			std::cout << "---\nStar: Num of request: [" S_GREEN << kevt << S_NONE "]" << "\n---\n" << std::endl;

		for (int i = 0; i < kevt; i++) {
			struct kevent currentEvt = _evlist[i];
			int currentSocket = -1;
			int currentFd = currentEvt.ident;
		
			for (int j = 0; j < _nListenEvent; j++) {
				if (currentFd == _multiSock[j].getServerFd())
					clientConnect(currentSocket, _multiSock[j].getServerFd());
				else {
					if (currentEvt.flags & EVFILT_READ) {
						readCase(currentEvt, _multiSock[j]);
						std::cout << "read fd: " << currentEvt.ident << std::endl;
					}
					if (currentEvt.flags & EV_EOF)
						eofCase(currentEvt);
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
