/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/10/26 23:07:51 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
	public:
		explicit Epoll(Socket const & sock);
		~Epoll(void);

		void	startEpoll(void);

		void	addEvents(int const &);
		void	deleteEvents(int const &);

		void	clientConnect(int &);
		void	clientDisconnect(int const &);

		void	readCase(struct kevent &);
		void	eofCase(struct kevent &);

		void	serverLoop(void);

	private:
		Epoll(void);

		void	errorExit(const std::string &) const;

	private:
		Socket				_sock;
		int					_epollFd;

		static const int	maxEvent = 32; // why 32? IDK

		struct kevent		_chlist[1]; // listen event
		struct kevent		_evlist[maxEvent]; // tigger event
};

_END_NS_WEBSERV

#endif
