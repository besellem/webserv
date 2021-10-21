/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:18:09 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 04:53:50 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFS_HPP
# define DEFS_HPP

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

/** @brief epoll */
# define kReadEvent  1
# define kWriteEvent 2

#endif /* !defined(DEFS_HPP) */
