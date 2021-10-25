/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/25 11:49:36 by adbenoit         ###   ########.fr       */
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

# include "defs.hpp"
# include "ServerGenerator.hpp"
# include "cgi.hpp"
# include "socket.hpp"
# include "epoll.hpp"

_BEGIN_NS_WEBSERV

class ServerGenerator;
class cgi;
class Socket;
class Epoll;

class WebServer
{
	
	public:
		WebServer(void);
		WebServer(const WebServer&);
		virtual ~WebServer();
		WebServer&	operator=(const WebServer&);
		
		const Server&	getServer(int i) const;
		size_t			serverSize() const;
		void			createServers(void);

		void		parse(const std::string);

	public:
		class ParsingError : public std::exception
		{
            public:
                ParsingError();
			    virtual const char*	what() const throw();
		};

	private:
		ServerGenerator	_servers;
		Socket			*_socks;
	
}; /* class WebServer */

/** @brief function declare */
int		socketAccept(int fd, sockaddr *addr, socklen_t *addrLen);
int		socketAccept(const Socket &);
bool	ft_isNumeric(const std::string &str);


_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
