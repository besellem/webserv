/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/19 16:11:00 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

#define __DEBUG std::cout << __FILE__ << ":" << __LINE__ << ": Here" << std::endl;

webserv::epoll::epoll(void) {}
webserv::epoll::~epoll(void) {}

webserv::epoll::epoll(short const & port) : _port(port) {
	// create epoll
	_epollFd = kqueue();
	if (_epollFd < 0)
		throw webserv::epoll::epollCreateException();

	// socket init
	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0)
		throw webserv::epoll::socketFailException();

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	// memset(&_addr.sin_zero, 0, sizeof(_addr.sin_zero));
}

void	webserv::epoll::bindStep(void) const {
	if (bind(_listenFd, (struct sockaddr *)&_addr, sizeof(struct sockaddr)) < 0)
		throw webserv::epoll::bindFailException();
}

void	webserv::epoll::listenStep(void) const {
	if (listen(_listenFd, 20) < 0)
		throw webserv::epoll::listenFailException();
}

void	webserv::epoll::setNonBlock(int fd) const {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw webserv::epoll::nonBlockException();
}

void	webserv::epoll::updateEvents(int epollFd, int fd, int events, bool modify) {
	struct kevent	ev[2]; // event read & write
	int				n = 0; // event index

	if (events & kReadEvent)
		EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0 ,0, (void *)(intptr_t)fd);
	else if (modify)
		EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0 ,0, (void *)(intptr_t)fd);

	if (events & kWriteEvent)
		EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0 ,0, (void *)(intptr_t)fd);
	else if (modify)
		EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0 ,0, (void *)(intptr_t)fd);

	std::cout << (modify ? "modify fd: [" S_GREEN : "add fd: [" S_GREEN) << fd
		<<  S_NONE "] events read: [" S_GREEN << std::boolalpha
		<< static_cast<bool>(events & kReadEvent) << S_NONE "] write: [" S_GREEN
		<< static_cast<bool>(events & kWriteEvent) << S_NONE "]" << std::endl;

	if (kevent(epollFd, ev, n, NULL, 0, NULL) < 0)
		throw webserv::epoll::updateEventsException();
}

void	webserv::epoll::run(int waitms) {
	struct timespec timeout;
	timeout.tv_sec = waitms / 1000;
	timeout.tv_nsec = (waitms & 1000) * 1000 * 1000;
	
	const int		kMaxEvents = 20;
	struct kevent	activeEvts[20];

	/** @brief n: event numbers */
	int 			n = kevent(getEpollFd(), NULL, 0, activeEvts, kMaxEvents, &timeout);
	
	std::cout << "epoll wait return: [" S_GREEN << n << S_NONE "]" << std::endl;

	for (int i = 0; i < n; i++) {
		int 	fd = (int)(intptr_t)activeEvts[i].udata;
		int 	events = activeEvts[i].filter;
		if (events == EVFILT_READ) {
			if (fd == getListenFd())
				handleAccept(fd);
			else
				handleRead(fd);
		}
		else if (events == EVFILT_WRITE)
			handleWrite(fd);
		else {
			std::cout << "Unknow event!" << std::endl;
			exit(1);
		}
	}
}

void	webserv::epoll::handleAccept(int fd) {
	// accept step, i dont undertand ... but i think, it's get the local addr and connect it.
	struct sockaddr_in	laddr;
	socklen_t			lsize = sizeof(laddr);
	int					lfd = accept(fd, (struct sockaddr *)&laddr, & lsize);

	if (lfd < 0) {
		std::cout << "accept failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (getsockname(lfd, (sockaddr *)&laddr, &lsize) < 0) {
		std::cout << "getsockname failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "accept a connection from [" S_GREEN << "127.0.0.1" << S_NONE "]" << std::endl;

	// setNonBlock(lfd);
	updateEvents(_epollFd, lfd, kReadEvent | kWriteEvent, false);
}

void	webserv::epoll::handleRead(int fd) {
	char	buffer[4096] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	int		n = 0;

	std::cout << "Here" << std::endl;
	while ((n = ::read(fd, buffer, sizeof(buffer))) > 0) {
		std::cout << "read: [" S_GREEN << n << S_NONE "]"  << std::endl;
		int r = :: write(fd, buffer, n);
		if (r <= 0) {
			std::cout << "write error!" << std::endl;
			exit(1);
		}
		if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) // here is forbidden by subject
			return ;
		if (n < 0) {
			std::cout << "read error!" << std::endl;
			exit(1);
		}
		std::cout << "fd: [" S_GREEN << fd << S_NONE "] closed" << std::endl;
		close(fd);
	}
}

void	webserv::epoll::handleWrite(int fd) {
	updateEvents(_epollFd, fd, kReadEvent, true);
}

short				webserv::epoll::getPort(void)     const { return _port; }
int					webserv::epoll::getEpollFd(void)  const { return _epollFd; }
int					webserv::epoll::getListenFd(void) const { return _listenFd; }
struct sockaddr_in	webserv::epoll::getSockAddr(void) const { return _addr; }

char const	*webserv::epoll::epollCreateException::what()  const throw() { return "Epoll create failed!"; }
char const	*webserv::epoll::socketFailException::what()   const throw() { return "Socket failed!"; }
char const	*webserv::epoll::bindFailException::what()     const throw() { return "bind failed!"; }
char const	*webserv::epoll::listenFailException::what()   const throw() { return "listen failed!"; }
char const	*webserv::epoll::nonBlockException::what()     const throw() { return "fcntl failed!"; }
char const	*webserv::epoll::updateEventsException::what() const throw() { return "kevent failed!"; }
