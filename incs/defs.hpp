/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:18:09 by besellem          #+#    #+#             */
/*   Updated: 2021/11/04 16:13:47 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFS_HPP
# define DEFS_HPP

# define DEBUG     true // print some logs

/** @brief Anscii code */
# define S_NONE    "\e[0m"
# define S_BLACK   "\e[1;30m"
# define S_RED     "\e[1;31m"
# define S_GREEN   "\e[1;32m"
# define S_YELLOW  "\e[1;33m"
# define S_BLUE    "\e[1;34m"
# define S_PURPLE  "\e[1;35m"
# define S_CYAN    "\e[1;36m"
# define S_CLRLINE "\e[K\r"

/** @brief namespace define */
# define _INLINE_NAMESPACE  webserv
# define _BEGIN_NS_WEBSERV  namespace _INLINE_NAMESPACE {
# define _END_NS_WEBSERV    }


# define CONFIG_DEFAULT_PATH  "./config_files"
# define CONFIG_FILETYPE      ".conf"
# define CONFIG_FILETYPE_LEN  std::strlen(CONFIG_FILETYPE)
# define DEFAULT_CONFIG_FILE  CONFIG_DEFAULT_PATH "/default" CONFIG_FILETYPE

# define ROOT_PATH            "./www"

# define BUFFER_SIZE  4096

# define OFF          0
# define ON           1

/** @brief epoll utils define */
# define EP_EVENTS    2 // 2: read / write

/* Logs & messages to print */
# define LOG \
	std::cout << S_RED << __FILE__ << ":" << __LINE__ << S_NONE ": Here\n" << std::endl
# define EXCEPT_WARNING(exception) std::cerr << S_BLUE "Warning: " S_NONE << exception.what() << std::endl
# define EXCEPT_ERROR(exception)   std::cerr << S_RED  "Error: "   S_NONE << exception.what() << std::endl

# define STRINGIFY(x)   #x
# define TOSTRING(x)    STRINGIFY(x)
# define PRINT(x)       std::cout << TOSTRING(x) << ": [" << (x) << "]" << std::endl;

/* Http response new lines (may change based on the sytem -- to check) */
# define NEW_LINE               "\r\n"
# define DELIMITER              "\r\n\r\n"

# define HTTP_PROTOCOL_VERSION  "HTTP/1.1"

/* Most syscalls return -1 */
# define SYSCALL_ERR (-1)

/* Listen Backlog */
#ifndef SOMAXCONN
# define SOMAXCONN 128
#endif

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
# include <sys/types.h>
# include <dirent.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>

# include <string>
# include <cstring>
# include <cstdio>
# include <cstdlib>
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
