/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/11/07 17:12:00 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

/** @brief public function */

Epoll::Epoll(void) {}
Epoll::~Epoll(void) {}

Epoll::Epoll(Socket *multiSock, int const & serverSize) :
_serverSocks(multiSock),
_serverSize(serverSize),
_epollFd(-1) {}

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	for (int i = 0; i < _serverSize; ++i) {
		int sockFd = _serverSocks[i].getServerFd();

		// debug msg
		std::cout << "Add sock: [" << sockFd << "]" << std::endl;

		EV_SET(&_chlist[i], sockFd, EVFILT_READ, EV_ADD, 0, 0, 0);
	}
	int kevt = kevent(_epollFd, _chlist, _serverSize, NULL, 0, NULL);
	if (kevt < 0)
		errorExit("epoll start failed!");

	serverLoop();
}

/** @brief private function */

void	Epoll::errorExit(const std::string &str) const {
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	Epoll::serverLoop(void) {

	// std::map<const int, Socket> _sockConn;
	// struct timespec tmout = {5, 0};

	for(;;) {
		int readyEvts = kevent(_epollFd, NULL, 0, _evlist, _nEvents, NULL);
		if (readyEvts < 0)
			errorExit("kevent failed in loop");
		// else if (readyEvts == 0) {
		// 	if (_sockConn.empty() == true)
		// 		continue ;
		// 	else {
		// 		int i = 0;
		// 		for (std::map<const int, Socket>::iterator it = _sockConn.begin(); it != _sockConn.end(); it++, i++) {
		// 			close(it->first);
		// 			EV_SET(&_chlist[i], it->first, EVFILT_READ, EV_DELETE, 0, 0, 0);
		// 		}
		// 		int delEvts = kevent(_epollFd, _chlist, i, NULL, 0, NULL);
		// 		if (delEvts < 0)
		// 			errorExit("kevent failed in loop");
		// 		_sockConn.clear();
		// 		std::cout << "time out!" << std::endl;
		// 		// set time out here ... ?
		// 		continue ;
		// 	}
		// }

		if (readyEvts > 0)
			// debug msg
			std::cout << "---\nStar: Num of request: [" S_GREEN << readyEvts << S_NONE "]" << "\n---\n" << std::endl;

		for (int i = 0; i < readyEvts; i++) {
			 _currEvt = _evlist[i];

			if (clientConnect(_currEvt.ident, _sockConn))
				continue ;
			else {
				_tmp = checkServ(_currEvt.ident, _sockConn);
				if (_tmp.getServerFd() == SYSCALL_ERR) {
					std::cout << "connexion not found!" << std::endl;
					close(_currEvt.ident);
					continue ;
				}

				// if (pthread_create(&_tid, NULL, handleThread, (void*)this) != 0) {
				// 	close(_currEvt.ident);
				// 	errorExit("thread failed!");
				// }
				handleRequest(_currEvt.ident, _tmp);
				clientDisconnect(_currEvt.ident, _sockConn);
			}
		}
	}
}

void	*Epoll::handleThread(void *arg) {
	Epoll *obj = static_cast<Epoll*>(arg);

	Socket	tmp = obj->_tmp;
	int		currentFd = obj->_currEvt.ident;

	// if (obj->handleRequest(currentFd, tmp) == true)
	obj->handleRequest(currentFd, tmp);
	obj->clientDisconnect(currentFd, obj->_sockConn);
	pthread_exit(NULL);
}

Socket	Epoll::checkServ(int const & currConn, std::map<const int, Socket> & _sockConn) const {
	Socket sock;

	if (_sockConn.empty() == true)
		return sock;

	for (std::map<const int, Socket>::iterator it = _sockConn.begin(); it != _sockConn.end(); it++) {
		if (currConn == it->first)
			return it->second;
	}
	return sock;
}

int		Epoll::servIndex(int const & toFind, std::map<const int, Socket> & _sockConn) const {
	int i = 0;

	for (; i < _serverSize; i++) {
		if (_sockConn[toFind].getServerFd() == _serverSocks[i].getServerFd())
			return i;
	}
	return i;
}

bool	Epoll::clientConnect(int const & toConnect, std::map<const int, Socket> & _sockConn) {
	bool isClient = false;

	int i = 0;
	for (; i < _serverSize; ++i) {
		if (toConnect == _serverSocks[i].getServerFd()) {
			isClient = true;
			break ;
		}
	}

	if (isClient == false)
		return false;

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int newSock = accept(toConnect, (sockaddr *)&clientAddr, &clientAddrLen);

	if (newSock == SYSCALL_ERR)
		return false;

	if (SYSCALL_ERR == fcntl(newSock, F_SETFL, O_NONBLOCK))
		errorExit("Non-blocking failed");

	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "] with socket: [" << toConnect << "]\n" << std::endl;

	_sockConn[newSock] = _serverSocks[i];

	EV_SET(&_chlist[i], newSock, EVFILT_READ, EV_ADD, 0, 0, 0);
	int addEvts = kevent(_epollFd, _chlist + i, 1, NULL, 0, NULL);
	if (addEvts < 0)
		errorExit("kevent failed in add");

	return true;
}

void	Epoll::clientDisconnect(int const & toClose, std::map<const int, Socket> & _sockConn) {
	// debug msg
	std::cout << "closing: [" S_RED << toClose << S_NONE "] ..."<< "\n" << std::endl;

	int servI = servIndex(toClose, _sockConn);

	EV_SET(&_chlist[servI], toClose, EVFILT_READ, EV_DELETE, 0, 0, 0);
	int delEvts = kevent(_epollFd, _chlist + servI, 1, NULL, 0, NULL);
	if (delEvts < 0)
		errorExit("kevent failed in delete");

	_sockConn.erase(toClose);
	close(toClose);
}

void	Epoll::handleRequest(int const & fd, Socket & sock) {
	// debug msg
	std::cout << "Reading: [" S_RED << fd << S_NONE "] ..."<< "\n" << std::endl;

	Request	request(sock.getServer());

	// if (sock.readHttpRequest(&request, fd) == false) {
	// 	clientDisconnect(fd, _sockConn);
	// 	return false;
	// }
	sock.readHttpRequest(&request, fd);
	try {
		sock.resolveHttpRequest(&request);
		sock.sendHttpResponse(&request, fd);
	}
	catch (std::exception &e) {
		EXCEPT_WARNING(e);
	}
	// return true;
}

_END_NS_WEBSERV
