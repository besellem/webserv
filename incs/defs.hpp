/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:18:09 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 16:51:48 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFS_HPP
# define DEFS_HPP

# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <cctype>

#include <unistd.h>
#include <cstdio>

# include <iostream>
# include <fstream>
# include <string>
# include <cstring>

# include <exception>
# include <fstream>
# include <iomanip>
# include <sstream>

# include <map>
# include <vector>


/** @brief Anscii code */
# define S_NONE    "\033[0m"
# define S_BLACK   "\033[1;30m"
# define S_RED     "\033[1;31m"
# define S_GREEN   "\033[1;32m"
# define S_YELLOW  "\033[1;33m"
# define S_BLUE    "\033[1;34m"
# define S_PURPLE  "\033[1;35m"
# define S_CYAN    "\033[1;36m"
# define S_CLRLINE "\033[K\r"

/** @brief namespace define */
# define _INLINE_NAMESPACE  webserv
# define _BEGIN_NS_WEBSERV  namespace _INLINE_NAMESPACE {
# define _END_NS_WEBSERV    }



# define DEFAULT_CONFIG_FILE  "./config_files/default.conf"

# define GET     (1L << 0)
# define POST    (1L << 1)
# define DELETE  (1L << 2)

# define OFF     0
# define ON      1

/** @brief epoll utils define */
# define EP_EVENTS 2 // 2: read / write

#endif /* !defined(DEFS_HPP) */
