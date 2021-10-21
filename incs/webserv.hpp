/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 16:27:18 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "defs.hpp"
# include "ServerGenerator.hpp"
# include "socket.hpp"
# include "epoll.hpp"

_BEGIN_NS_WEBSERV

bool	ft_isNumeric(const std::string &str);

class ServerGenerator;
class Socket;
class Epoll;

class WebServer {
	public:
		WebServer(void) {}
		~WebServer(void) {}

		WebServer(const WebServer&);
		
		WebServer&	operator=(const WebServer&) { return *this; }

		void		parse(const std::string);

	public:
		class ParsingError : public std::exception {
			public:
				ParsingError() {}
				virtual const char*	what() const throw() { return "Config File Error"; }
		};

	private:
		ServerGenerator	_servers;
}; /* class WebServer */

_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
