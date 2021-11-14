/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/11/14 17:16:17 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

_BEGIN_NS_WEBSERV

#define WRITING true

/** @brief public function */

Epoll::Epoll(void) {}
Epoll::~Epoll(void) {}

Epoll::Epoll(Socket *multiSock, int const & serverSize) :
	_serverSocks(multiSock),
	_serverSize(serverSize),
	_epollFd(-1)
{}

void	Epoll::startEpoll(void) {
	_epollFd = kqueue();
	if (_epollFd < 0)
		errorExit("epoll create");
	
	for (int i = 0; i < _serverSize; ++i) {
		int sockFd = _serverSocks[i].getServerFd();

		std::cout << "Add sock: [" << sockFd << "]" << std::endl;
		_updateEvt(sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL, "failed in init add!");
	}

	_serverLoop();
}

/** @brief private function */

void	Epoll::_serverLoop(void) {
	struct timespec tmout = {5, 0};

	for(;;) {
		// get ready event
		int readyEvts = kevent(_epollFd, NULL, 0, _evlist, _nEvents, &tmout);
		if (readyEvts <= 0) {
			if (readyEvts == 0)
				warnMsg("time out, no event actives during 5 sec ...");
			else if (readyEvts < 0)
				warnMsg("get event failed, kevent failed!");
			continue ;
		}

		if (readyEvts > 0)
			std::cout << "---\nStar: Num of request: [" S_GREEN << readyEvts << S_NONE "]" << "\n---\n" << std::endl;

		// handle ready event
		for (int i = 0; i < readyEvts; i++) {
			struct kevent currEvt = _evlist[i];

			if (true == _clientConnect(currEvt.ident, _connMap))
				continue ;
			else {
				if (false == _checkClient(currEvt.ident)) {
					warnMsg("Error: could not find client fd!");

					_updateEvt(currEvt.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL, "failed in delete write!");
					_updateEvt(currEvt.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL, "failed in delete write!");

					close(currEvt.ident);
					continue ;
				}

				if (currEvt.flags & EV_EOF) {
					updateMsg("Client quit (EOF case)");
					_clientDisconnect(currEvt.ident, _connMap);
					continue ;
				}

				Socket *tmp = _checkServ(currEvt.ident, _connMap);
				// if (tmp.getServerFd() == SYSCALL_ERR) {
				// 	warnMsg("connexion not found!");
				// 	close(currEvt.ident);
				// 	continue ;
				// }
				if (tmp == NULL) {
					warnMsg("connexion not found!");
					close(currEvt.ident);
					continue ;
				}
				
				_handleRequest(currEvt, *tmp);
			}
		}
	}
}

void	Epoll::_updateEvt(int ident, short filter, u_short flags, u_int fflags, int data, void *udata, std::string msg) {
	struct kevent chlist;
	
	EV_SET(&chlist, ident, filter, flags, fflags, data, udata);
	int ret = kevent(_epollFd, &chlist, 1, NULL, 0, NULL);
	if (ret < 0)
		warnMsg(msg);
}

Socket *	Epoll::_checkServ(int const & currConn, std::map<const int, Socket> & _connMap) const {
	if (_connMap.empty() == true)
		return NULL;

	for (std::map<const int, Socket>::iterator it = _connMap.begin(); it != _connMap.end(); it++) {
		if (currConn == it->first)
			return &it->second;
	}
	return NULL;
}

bool	Epoll::_checkClient(int const & clientFd) const {
	conn_type::const_iterator it = _connMap.find(clientFd);

	return (it != _connMap.end());
}

bool	Epoll::_clientConnect(int const & toConnect, std::map<const int, Socket> & _connMap) {
	bool isClient = false;

	int i = 0;
	for (; i < _serverSize; ++i) {
		if (toConnect == _serverSocks[i].getServerFd()) {
			isClient = true;
			break ;
		}
	}

	if (false == isClient)
		return false;

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int newSock = accept(toConnect, (sockaddr *)&clientAddr, &clientAddrLen);

	if (newSock == SYSCALL_ERR)
		return false;

	if (SYSCALL_ERR == fcntl(newSock, F_SETFL, O_NONBLOCK))
		warnMsg("non-blocking failed at client connect!");

	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "] with socket: [" << toConnect << "]\n" << std::endl;

	_connMap[newSock] = _serverSocks[i];

	_updateEvt(newSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL, "failed in add read!");
	_updateEvt(newSock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL, "failed in add write!");

	return true;
}

void	Epoll::_clientDisconnect(int const & toClose, std::map<const int, Socket> & _connMap) {
	std::cout << "closing: [" S_RED << toClose << S_NONE "] ..."<< "\n" << std::endl;
	_updateEvt(toClose, EVFILT_READ, EV_DELETE, 0, 0, NULL, "failed in delete!");
	_updateEvt(toClose, EVFILT_WRITE, EV_DELETE, 0, 0, NULL, "failed in delete!");

	_connMap.erase(toClose);
	close(toClose);
}

void	Epoll::_handleRequest(struct kevent const & currEvt, Socket & sock) {
	if (currEvt.filter == EVFILT_READ) {
		updateMsg("receive request (READ case)");

		Request *request = new Request();

		int readStatus = sock.readHttpRequest(request, currEvt);

		if (readStatus == READ_FAIL || readStatus == READ_DISCONNECT) {
			_clientDisconnect(currEvt.ident, _connMap);
		}

		_reqMap[currEvt.ident] = request;

		_updateEvt(currEvt.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL, "failed in read disable");
		_updateEvt(currEvt.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL, "failed in write enable");
	}
	else if (currEvt.filter == EVFILT_WRITE) {
		updateMsg("send reponse (WRITE case)");
		int sendStatus = SEND_OK;

		req_type::iterator it = _reqMap.find(currEvt.ident);
		if (it != _reqMap.end())
			sendStatus = sock.sendHttpResponse(it->second, currEvt.ident);

		if (it == _reqMap.end() || sendStatus == SEND_OK) {
			delete it->second;
			_reqMap.erase(currEvt.ident);
			_updateEvt(currEvt.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL, "failed in read enable");
			_updateEvt(currEvt.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL, "failed in write disable");
		}
	}
}

_END_NS_WEBSERV
