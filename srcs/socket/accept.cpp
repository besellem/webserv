/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accept.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:40:01 by kaye              #+#    #+#             */
/*   Updated: 2021/10/21 05:01:25 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


_BEGIN_NS_WEBSERV

int	socketAccept(int fd, sockaddr *addr, socklen_t *addrLen) {
	int socketFd = accept(fd, addr, addrLen);
	
	if (socketFd < 0) {
		std::cout << "Error: accept" << std::endl;
		exit(EXIT_FAILURE);
	}
	return socketFd;
}


_END_NS_WEBSERV
