/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/25 13:40:21 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

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
