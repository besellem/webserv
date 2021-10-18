/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:35:48 by kaye              #+#    #+#             */
/*   Updated: 2021/10/18 18:58:51 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

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
	_addr.sin_addr.s_addr = INADDR_ANY;
}

char const	*webserv::epoll::epollCreateException::what() const throw() { return "Epoll create failed!"; }
char const	*webserv::epoll::socketFailException::what() const throw() { return "Socket failed!"; }
char const	*webserv::epoll::bindFailException::what() const throw() { return "bind failed!"; }
char const	*webserv::epoll::listenFailException::what() const throw() { return "listen failed!"; }