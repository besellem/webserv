/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:36:44 by kaye              #+#    #+#             */
/*   Updated: 2021/10/18 18:53:32 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

class epoll {
	public:
		explicit epoll(short const & port);
		
		~epoll(void);

	private:
		epoll(void);

	private:
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
};

_END_NS_WEBSERV

#endif