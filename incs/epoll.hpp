/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:36:44 by kaye              #+#    #+#             */
/*   Updated: 2021/10/19 15:13:33 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

class epoll {
	public:
	/** @brief constructor / destructor */

		explicit epoll(short const & port);
		~epoll(void);

	public:
	/** @brief members function */

		/** @brief getters */
		short				getPort(void) const;
		int					getEpollFd(void) const;
		int					getListenFd(void) const;
		struct sockaddr_in	getSockAddr(void) const;

		/** @brief server step */
		void	bindStep(void) const;
		void	listenStep(void) const;
		void	setNonBlock(int fd) const;

		void	handleAccept(int fd);
		void	handleRead(int fd);
		void	handleWrite(int fd);

		/** @brief server start */
		void	updateEvents(int epollFd, int fd, int events, bool modify);
		void	run(int waitms);

	private:
		epoll(void);

	private:
	/** @brief attributes */

		short				_port;
		int					_epollFd;
		int					_listenFd;
		struct 	sockaddr_in	_addr;

	/** @brief exception */
	public:
		class epollCreateException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};

		class socketFailException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};

		class bindFailException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};

		class listenFailException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};

		class nonBlockException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};

		class updateEventsException : public std::exception {
			public:
				virtual char const	*what() const throw();
		};
};

_END_NS_WEBSERV

#endif