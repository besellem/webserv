/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/29 19:32:37 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

/** @brief public function */

Epoll::Epoll(void) {}
Epoll::~Epoll(void) {
	delete [] _chlist;
	delete [] _evlist;
}

Epoll::Epoll(std::map<const int, Socket> const & multiSock, int const & serverSize) :
_multiSock(multiSock),
_epollFd(-1) {
	_serverSize = serverSize;
	_chlist = new struct kevent[serverSize];
	_evlist = new struct kevent[serverSize];
}

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	int i = 0;
	for (std::map<const int, Socket>::iterator it = _multiSock.begin(); it != _multiSock.end(); it++, i++) {
		int sockFd = it->second.getServerFd();
		std::cout << "Add sock: [" << sockFd << "]" << std::endl;
		EV_SET(&_chlist[i], sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
	}

	int kevt = kevent(_epollFd, _chlist, _serverSize, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("epoll start failed!");
}

int	Epoll::clientConnect(int const & serverFd) {
	int sockFd = serverFd;

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int newSock = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);

	if (newSock == SYSCALL_ERR)
		return SYSCALL_ERR;

	if (SYSCALL_ERR == fcntl(newSock, F_SETFL, O_NONBLOCK))
		errorExit("Non-blocking failed");

	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "] with socket: [" << sockFd << "]\n" << std::endl;

	return newSock;
}

void	Epoll::readCase(int & fd, Socket & sock) {
	// debug msg
	std::cout << "Reading: [" S_RED << fd << S_NONE "] ..."<< "\n" << std::endl;

	sock.readHttpRequest(fd);			
	try {
		sock.resolveHttpRequest();
		sock.sendHttpResponse(fd);
	}
	catch (std::exception &e) {
		EXCEPT_WARNING;
	}
}

void	*Epoll::handleRequest(void * args) {
	Epoll *epollInfo = static_cast<Epoll *>(args);

	epollInfo->readCase(epollInfo->_currConn, epollInfo->_multiSock[epollInfo->_currSockFd]);

	std::cout << "Closing: [" S_RED << epollInfo->_currConn << S_NONE "] ..." << "\n" << std::endl;

	close(epollInfo->_currConn);
	pthread_exit(NULL);
	return NULL;
}

void	Epoll::serverLoop(void) {
	for(;;) {
		int readyEvts = kevent(_epollFd, _chlist, 0, _evlist, _serverSize, NULL);
		if (readyEvts < 0)
			errorExit("kevent failed in loop");
		else if (readyEvts == 0)
			continue ;

		if (readyEvts > 0)
			// debug msg
			std::cout << "---\nStar: Num of request: [" S_GREEN << readyEvts << S_NONE "]" << "\n---\n" << std::endl;

		for (int i = 0; i < readyEvts; i++) {
			struct kevent currentEvt = _evlist[i];
			_currConn = clientConnect(currentEvt.ident);
			_currSockFd = currentEvt.ident;
			if (_currConn == SYSCALL_ERR) {
				std::cout << "Id: [" << currentEvt.ident << "]: connection failed!" << std::endl;
				continue ;
			}

			std::cout << "sock to communicate with client: [" << _currConn << "]\n" << std::endl;

			// sleep(1); // accept get a delay

			readCase(_currConn, _multiSock[_currSockFd]);

			std::cout << S_RED "Closing ..." S_NONE << "\n" << std::endl;

			close(_currConn);

			// if (pthread_create(&_e_tid, NULL, Epoll::handleRequest, (void *)this) != 0) {
			// 	close(_currConn);
			// 	errorExit("create handle thread failed!");
			// }
		}
	}
}

/** @brief private function */

void	Epoll::errorExit(const std::string &str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

_END_NS_WEBSERV
