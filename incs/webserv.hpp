/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/11/10 15:22:29 by adbenoit         ###   ########.fr       */
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

class WebServer {
	public:
		WebServer(void);
		WebServer(const WebServer &);
		~WebServer();
		
		WebServer&	operator=(const WebServer &);

		void				parse(const std::string &);
		size_t				serverSize(void) const;
		const Server&		getServer(int) const ;

		void				createServers(void);

	private:
		size_t					countSocket() const;
		std::vector<Server *>	getSocketConfigs(size_t& index);

	public:
		class ParsingError : public std::exception
		{
			public:
                ParsingError();
				virtual const char*	what() const throw();
		};
		
	private:
		ServerGenerator		_servers;
		Socket				*_socks;
	
}; /* class WebServer */

/** @brief function declare */
int		socketAccept(int fd, sockaddr *addr, socklen_t *addrLen);
int		socketAccept(const Socket &);


_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
