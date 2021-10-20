/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 17:25:31 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

class HttpHeader
{
	
};

class Socket
{
	
	public:
	/** @brief constructor / destructor */

		explicit Socket(const short& port);
		~Socket(void);

		short		getPort(void) const;
		int			getServerFd(void) const;
		sockaddr_in	getAddr(void) const;
		size_t		getAddrLen(void) const;

		/** @brief init socket */
		void		startUp(void);

		void		parse(int skt, const char* http_header);


	private:
		Socket(void);

		void	errorExit(const std::string& str) const;
		void	bindStep(const int& serverFd, const sockaddr_in& addr);
		void	listenStep(const int& serverFd);

		void	getParsing(int skt, const char* http_header);
		void	_parse_wrapper(const char*);


	private:
		short		_port;
		int			_serverFd;
		sockaddr_in	_addr;
		size_t		_addrLen;
	
};

_END_NS_WEBSERV

#endif
