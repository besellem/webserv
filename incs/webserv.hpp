/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 08:26:22 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "defs.hpp"
# include "ServerGenerator.hpp"
# include "socket.hpp"


_BEGIN_NS_WEBSERV

bool	ft_isNumeric(const std::string &str);

class ServerGenerator;

class WebServer
{
	
	public:
		WebServer(void);
		WebServer(const WebServer&);
		~WebServer();
		
		WebServer&	operator=(const WebServer&);

		void				parse(const std::string&);
		size_t				serverSize(void) const;
		const Server&		getServer(int);

		void				createServers(void);


	public:
		class ParsingError : public std::exception
		{
			public:
				virtual const char*	what() const throw();
		};


	private:
		ServerGenerator		_servers;
		Socket				*_socks;
	
}; /* class WebServer */

/** @brief function declare */
int		socketAccept(int fd, sockaddr *addr, socklen_t *addrLen);


_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
