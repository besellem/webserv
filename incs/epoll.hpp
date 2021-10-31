/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/10/31 16:49:21 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
	public:
		explicit Epoll(std::map<const int, Socket> const &, int const &);
		~Epoll(void);

		void	startEpoll(void);
		void	serverLoop(void);

	private:
		Epoll(void);

		void		errorExit(const std::string &) const;
		
		Socket		checkServ(int const &, std::map<const int, Socket> &) const;
		int			clientConnect(int const &);
		void		readCase(int &, Socket &);
		static void	*handleRequest(void * args);

	private:
		std::map<const int, Socket>	_multiSock;
		int							_epollFd;
		int							_serverSize;

		struct kevent				*_chlist; // listen event
		struct kevent				*_evlist; // tigger event

		__unused pthread_t					_e_tid;

		int							_currSockFd;
		int							_currConn;
};

_END_NS_WEBSERV

#endif
