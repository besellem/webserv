/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 14:27:23 by kaye              #+#    #+#             */
/*   Updated: 2021/10/31 20:03:24 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "defs.hpp"
# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Epoll {
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

	private:
		Socket				*_serverSocks;
		int					_serverSize;
		int					_epollFd;

		static const int	_nEvents = 1024;	// max tigger event list
		struct kevent		*_chlist; 			// listen event list
		struct kevent		_evlist[_nEvents];	// tigger event list
};

_END_NS_WEBSERV

#endif
