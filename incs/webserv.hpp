/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 18:11:40 by adbenoit         ###   ########.fr       */
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
# include "socket.hpp"



_BEGIN_NS_WEBSERV

bool	ft_isNumeric(const std::string &str);

class ServerGenerator;

class WebServer
{
	
	public:
		WebServer(void)
		{}
		
		WebServer(const WebServer&);
		
		virtual ~WebServer()
		{}
		
		WebServer&	operator=(const WebServer&) { return *this; }
		const Server&	servers(int i) const { return *(this->_servers[i]); }
		size_t			nServ() const { return this->_servers.size(); }

		void		parse(const std::string);
		void		execute_cgi(const t_location &, const std::string, int, char *envp[]);

	public:
		class ParsingError : public std::exception
		{
            public:
                ParsingError() {}
			    virtual const char*	what() const throw() { return "Config File Error"; }
		};


	private:
		ServerGenerator	_servers;
	
}; /* class WebServer */

/** @brief function declare */
int	socketAccept(int fd, sockaddr *addr, socklen_t *addrLen);


_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
