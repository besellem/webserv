/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/11/11 18:29:56 by kaye             ###   ########.fr       */
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
		_errorExit("epoll create");
	
	for (int i = 0; i < _serverSize; ++i) {
		int sockFd = _serverSocks[i].getServerFd();

		std::cout << "Add sock: [" << sockFd << "]" << std::endl;
		_updateEvt(sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL, "failed in init add!");
	}

	_serverLoop();
}

/** @brief private function */

void	Epoll::_errorExit(const std::string &str) const {
	std::cerr << "Exiting: " S_RED << str << S_NONE << std::endl;
	exit(EXIT_FAILURE);
}

void	Epoll::_warnMsg(const std::string &str) const {
	std::cerr << "Warning: " S_YELLOW << str << S_NONE <<std::endl;
}

void	Epoll::_updateMsg(const std::string &str) const {
	std::cout << "Updating: " S_PURPLE << str << S_NONE << std::endl;
}

void	Epoll::_serverLoop(void) {
	struct timespec tmout = {5, 0};

	for(;;) {
		// get ready event
		int readyEvts = kevent(_epollFd, NULL, 0, _evlist, _nEvents, &tmout);
		if (readyEvts <= 0) {
			if (readyEvts == 0)
				_warnMsg("time out, no event actives during 5 sec ...");
			else if (readyEvts < 0)
				_warnMsg("get event failed, kevent failed!");
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
					_warnMsg("Error: could not find client fd!");

					_updateEvt(currEvt.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL, "failed in delete write!");
					// _updateEvt(currEvt.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL, "failed in delete write!");

					close(currEvt.ident);
					continue ;
				}

				// if (currEvt.flags & EV_EOF) {
				// 	_updateMsg("Client quit (EOF case)");
				// 	_clientDisconnect(currEvt.ident, _connMap);
				// 	continue ;
				// }

				Socket tmp = _checkServ(currEvt.ident, _connMap);
				if (tmp.getServerFd() == SYSCALL_ERR) {
					_warnMsg("connexion not found!");
					close(currEvt.ident);
					continue ;
				}
				
				_handleRequest(currEvt, tmp);
				// if (true == _handleRequest(currEvt, tmp))
					// _clientDisconnect(currEvt.ident, _connMap);
			}
		}
	}
}

void	Epoll::_updateEvt(int ident, short filter, u_short flags, u_int fflags, int data, void *udata, std::string msg) {
	struct kevent chlist;
	
	EV_SET(&chlist, ident, filter, flags, fflags, data, udata);
	int ret = kevent(_epollFd, &chlist, 1, NULL, 0, NULL);
	if (ret < 0)
		_warnMsg(msg);
}

Socket	Epoll::_checkServ(int const & currConn, std::map<const int, Socket> & _connMap) const {
	Socket sock;

	if (_connMap.empty() == true)
		return sock;

	for (std::map<const int, Socket>::iterator it = _connMap.begin(); it != _connMap.end(); it++) {
		if (currConn == it->first)
			return it->second;
	}
	return sock;
}

bool	Epoll::_checkClient(int const & clientFd) const {
	conn_type::const_iterator it = _connMap.find(clientFd);

	if (it == _connMap.end())
		return false;
	return true;
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
		_warnMsg("non-blocking failed at client connect!");

	std::cout << "Client Connected form: [" S_GREEN << inet_ntoa(clientAddr.sin_addr)
		<< S_NONE "]:[" S_GREEN << ntohs(clientAddr.sin_port) << S_NONE "] with socket: [" << toConnect << "]\n" << std::endl;

	_connMap[newSock] = _serverSocks[i];

	_updateEvt(newSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL, "failed in add read!");
	// _updateEvt(newSock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL, "failed in add write!");

	return true;
}

void	Epoll::_clientDisconnect(int const & toClose, std::map<const int, Socket> & _connMap) {
	std::cout << "closing: [" S_RED << toClose << S_NONE "] ..."<< "\n" << std::endl;
	_updateEvt(toClose, EVFILT_READ, EV_DELETE, 0, 0, NULL, "failed in delete!");
	// _updateEvt(toClose, EVFILT_WRITE, EV_DELETE, 0, 0, NULL, "failed in delete!");

	_connMap.erase(toClose);
	close(toClose);
}

bool	Epoll::_handleRequest(struct kevent const & currEvt, Socket & sock) {
	std::cout << "Reading: [" S_RED << currEvt.ident << S_NONE "] ..."<< "\n" << std::endl;
	Request	request(sock.getServer());

	if (READ_OK == sock.readHttpRequest(&request, currEvt.ident)) {
		if (RESOLVE_OK == sock.resolveHttpRequest(&request)) {
			if (SEND_OK == sock.sendHttpResponse(&request, currEvt.ident)) {
				return true;
			}
			else
				_warnMsg("SEND FAILED");
		}
		else
			_warnMsg("RESOLVE FAILED");
	}
	else
		_warnMsg("READ FAILED");
	return false;

	// if (currEvt.filter == EVFILT_READ) {
	// 	_updateMsg("receive request (READ case)");

	// 	Request	request(sock.getServer());
	// 	_reqMap[currEvt.ident] = &request;

	// 	req_type::iterator it = _reqMap.find(currEvt.ident);
	// 	if (it != _reqMap.end()) {
	// 		std::cout << "found!" << std::endl;
	// 	}

	// 	sock.readHttpRequest(&(*(it->second)), currEvt.ident);

	// 	_updateEvt(currEvt.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL, "failed in read disable");
		// _updateEvt(currEvt.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL, "failed in write enable");
	// }
	// else if (currEvt.filter == EVFILT_WRITE) {
	// 	_updateMsg("send reponse (WRITE case)");

	// 	req_type::iterator it = _reqMap.find(currEvt.ident);
	// 	if (it != _reqMap.end()) {
	// 		std::cout << "found!" << std::endl;
	// 	}


	// 	LOG;

	// 	sock.resolveHttpRequest(&(*(it->second)));

	// 	LOG;
	// 	std::cout << it->second-> << std::endl;

	// 	sock.sendHttpResponse(&(*(it->second)), currEvt.ident);

	// 	LOG;

	// 	_updateEvt(currEvt.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL, "failed in read enable");
		// _updateEvt(currEvt.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL, "failed in write disable");
	// }
	// return false;
}

_END_NS_WEBSERV
