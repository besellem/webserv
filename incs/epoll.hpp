/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/10/21 18:10:01 by kaye             ###   ########.fr       */
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

		void	updateEvents(int &, int const &, bool const &);
		void	handleAccept(void);
		void	handleRead(int & sockFd);
		void	handleWrite(int & sockFd);

		void	serverLoop(int const & waitMs);

	private:
		Epoll(void);

		void	errorExit(const std::string &) const;

	public:
		static short const kReadEvent;
		static short const kWriteEvent;

	private:
		Socket			_sock;
		int				_epollFd;
};

_END_NS_WEBSERV

#endif
