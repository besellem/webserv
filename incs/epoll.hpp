/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/10/28 18:05:27 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
	public:
		explicit Epoll(Socket const &);
		explicit Epoll(std::vector<Socket> const &);
		~Epoll(void);

		void	startEpoll(void);
		int   	checkSocketFd(void);

		void	addEvents(int const &);
		void	deleteEvents(int const &);

		void	clientConnect(int &, int const &);
		void	clientDisconnect(int const &);

		void	readCase(struct kevent &, Socket &);
		void	readCase(int &);
		void	eofCase(struct kevent &);

		void	serverLoop(void);

	private:
		Epoll(void);

		void	errorExit(const std::string &) const;

	private:
		Socket					_sock;
		std::vector<Socket>		_multiSock;
		int						_epollFd;

		static const int	maxEvent = 32; // why 32? IDK

		struct kevent		*_chlist; // listen event
		struct kevent		_evlist[maxEvent]; // tigger event

		int	_nListenEvent;
};

_END_NS_WEBSERV

#endif
