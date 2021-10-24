/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:18:09 by besellem          #+#    #+#             */
/*   Updated: 2021/10/24 17:35:10 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFS_HPP
# define DEFS_HPP

# define DEBUG     true // print some logs

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


# define CONFIG_DEFAULT_PATH  "./config_files"
# define CONFIG_FILETYPE      ".conf"
# define CONFIG_FILETYPE_LEN  std::strlen(CONFIG_FILETYPE)
# define DEFAULT_CONFIG_FILE  CONFIG_DEFAULT_PATH "/default" CONFIG_FILETYPE

# define ROOT_PATH            "./www"


# define GET     (1L << 0)
# define POST    (1L << 1)
# define DELETE  (1L << 2)

# define OFF     0
# define ON      1

/** @brief epoll utils define */
# define EP_EVENTS 2 // 2: read / write

/* Compare two strings. Returns a bool */
# define CMP_STRINGS(s1, s2) (0 == std::strcmp((s1), (s2)))

/* Logs & messages to print */
# define LOG \
	std::cout << S_RED << __FILE__ << ":" << __LINE__ << S_NONE ": Here\n" << std::endl
# define EXCEPT_WARNING std::cerr << S_BLUE "Warning: " S_NONE << e.what() << std::endl
# define EXCEPT_ERROR   std::cerr << S_RED  "Error: "   S_NONE << e.what() << std::endl

/* Http response new lines (may change based on the sytem -- to check) */
# define NEW_LINE "\r\n"


/*
** -- Includes --
*/
# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/cdefs.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>

# include <string>
# include <cstring>
# include <cstdio>
# include <cctype>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <sstream>
# include <exception>

# include <map>
# include <vector>
# include <list>
# include <algorithm>

# include "utils.hpp"


#endif /* !defined(DEFS_HPP) */
