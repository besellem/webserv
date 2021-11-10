/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/11/10 16:25:35 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
	public:
		/**
		 * @brief connexion map
		 *
		 * @param 1st: communication socket fd
		 * @param 2nd: server socket
		 */
		typedef std::map<const int, Socket>	conn_type;

	public:
		explicit Epoll(Socket *, int const &);
		~Epoll(void);

		/** @brief start server */
		void	startEpoll(void);

	private:
		Epoll(void);

		/** @brief tigger server events */
		void	_serverLoop(void);
		
		/** @brief exit prog and show error message */
		void	_errorExit(const std::string &) const;

		/** @brief show debug message */
		void	_warnMsg(const std::string &) const;

		/** @brief show update message */
		void	_updateMsg(const std::string &) const;
		
		/** 
		 * @brief checker which server is 
		 * 
		 * @param 1st: a socket (current) fd to communicate with client
		 * @param 2nd: list of server socket bind with communication fd
		 * 
		 * @return the server socket */
		Socket	_checkServ(int const &, std::map<const int, Socket> &) const;

		bool	_checkClient(int const &) const;

		/** 
		 * @brief coming soon ...
		 * coming soon ...
		 */
		void	_updateEvt(int ident, short filter, u_short flags, u_int fflags, int data, void *udata, std::string debugMsg);

		/** 
		 * @brief start of connexion, create a new socket fd to communicate with client
		 * coming soon ...
		 */
		bool	_clientConnect(int const &, std::map<const int, Socket> &);

		/** 
		 * @brief end of connexion, close and delete the communication fd (event) 
		 * coming soon ...
		 */
		void	_clientDisconnect(int const &, std::map<const int, Socket> &);

		/** 
		 * @brief handling events, receive and send request
		 * coming soon ...
		*/
		void	_handleRequest(int const &, Socket &);

	private:
		Socket				*_serverSocks;
		int					_serverSize;
		int					_epollFd;

		static const int	_nEvents = 1024;	// max tigger event list
		struct kevent		_evlist[_nEvents];	// tigger event list

		conn_type			_connList;
};

_END_NS_WEBSERV

#endif
