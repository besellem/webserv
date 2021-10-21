/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/21 16:26:10 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "defs.hpp"

_BEGIN_NS_WEBSERV

// class HttpHeader
// {
	
// };

class Socket {
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

		void		parse(int skt, const char *);
		void		parse(int skt, const std::string &);

		void		setNonBlock(int & fd);
		int			socketAccept(void);

	private:
		Socket(void);

		void	errorExit(const std::string &) const;
		void	bindStep(const int &, const sockaddr_in &);
		void	listenStep(const int &);

		void	getParsing(int skt, const char *); // -- in process
		void	getParsing(int skt, const std::string &); // TO REMOVE
		void	_parse_wrapper(const char *);


	private:
		short		_port;
		int			_serverFd;
		sockaddr_in	_addr;
		size_t		_addrLen;
};

_END_NS_WEBSERV

#endif