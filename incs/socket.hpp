/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 12:36:07 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

class Socket {
	public:
	/** @brief constructor / destructor */

		explicit Socket(short const & port);
		~Socket(void);

		short		getPort(void);
		int			getServerFd(void);
		sockaddr_in	getAddr(void);
		size_t		getAddrLen(void);

		/** @brief init socket */
		void		startUp(void);

		void		parsing(int skt, std::string);

	private:
		Socket(void);

		void errorExit(std::string const & str) const;
		void bindStep(int const & serverFd, sockaddr_in const & addr);
		void listenStep(int const & serverFd);

		void		getParsing(int skt, std::string path);

	private:
		short		_port;
		int			_serverFd;
		sockaddr_in	_addr;
		size_t		_addrLen;
};

_END_NS_WEBSERV

#endif