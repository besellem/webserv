/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/11/07 17:11:14 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "Socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
	public:
		typedef std::map<const int, Socket>	conn_type;

	public:
		explicit Epoll(Socket *, int const &);
		~Epoll(void);

		/** @brief start server */
		void	startEpoll(void);

	private:
		Epoll(void);

		/** @brief tigger server events */
		void	serverLoop(void);
		
		/** @brief exit prog and show error message */
		void	errorExit(const std::string &) const;
		
		/** 
		 * @brief checker which server is 
		 * 
		 * @param 1: a socket (current) fd to communicate with client
		 * @param 2: list of server socket bind with communication fd
		 * 
		 * @return the server socket */
		Socket	checkServ(int const &, std::map<const int, Socket> &) const;

		/** 
		 * @brief checker which server is 
		 * 
		 * @param 1: a socket (to find) fd to communicate with client
		 * @param 2: list of server socket bind with communication fd
		 * 
		 * @return the server socket fd */
		int		servIndex(int const &, std::map<const int, Socket> &) const;

		/** 
		 * @brief start of connexion, create a new socket fd to communicate with client
		 * coming soon ...
		 */
		bool	clientConnect(int const &, std::map<const int, Socket> &);

		/** 
		 * @brief end of connexion, close and delete the communication fd (event) 
		 * coming soon ...
		 */
		void	clientDisconnect(int const &, std::map<const int, Socket> &);

		/** 
		 * @brief handling events, receive and send request
		 * coming soon ...
		*/
		void	handleRequest(int const &, Socket &);

		static void	*handleThread(void *arg);

	private:
		Socket				*_serverSocks;
		int					_serverSize;
		int					_epollFd;

		static const int	_nEvents = 1024;	// max tigger event list
		struct kevent		_chlist[_nEvents]; 			// listen event list
		struct kevent		_evlist[_nEvents];	// tigger event list

		struct kevent		_currEvt;
		conn_type			_sockConn;
		Socket				_tmp;

		__unused pthread_t	_tid;
};

_END_NS_WEBSERV

#endif
