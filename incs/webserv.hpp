/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/20 12:43:49 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h>

#include <unistd.h>
#include <cstdio>

# include <iostream>
# include <fstream>
# include <string>
# include <cstring>

# include "defs.hpp"
# include "socket.hpp"

/** @brief function declare */
int	socketAccept(int fd, sockaddr *addr, socklen_t *addrLen);

#endif /* !defined(WEBSERV_HPP) */