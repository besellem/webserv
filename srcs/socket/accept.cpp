/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accept.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:40:01 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 17:59:18 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

int	socketAccept(int fd, sockaddr *addr, socklen_t *addrLen)
{
	int	socketFd = accept(fd, addr, addrLen);
	
	if (socketFd < 0)
	{
		std::cout << "Error: accept" << std::endl;
		exit(EXIT_FAILURE);
	}
	return socketFd;
}

int	socketAccept(const Socket &x)
{
	__unused sockaddr_in	addr = x.getAddr();
	__unused size_t		len = x.getAddrLen();

	return socketAccept(x.getServerFd(), NULL, NULL);
}

_END_NS_WEBSERV
