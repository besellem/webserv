/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accept.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:40:01 by kaye              #+#    #+#             */
/*   Updated: 2021/10/19 17:43:07 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	socketAccept(int fd, sockaddr *addr, socklen_t *addrLen) {
	int socketFd = accept(fd, addr, addrLen);
	
	if (socketFd < 0) {
		std::cout << "Error: accept" << std::endl;
		exit(EXIT_FAILURE);
	}
	return socketFd;
}