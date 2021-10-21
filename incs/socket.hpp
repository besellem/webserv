/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/21 09:16:07 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "defs.hpp"


_BEGIN_NS_WEBSERV

class HttpHeader
{

	public:
		typedef std::map<std::string, std::string>                   value_type;
	
	public:
		HttpHeader(void) : data()
		{
			memset(buf, 0, sizeof(buf));
		}

		HttpHeader(const HttpHeader &);
		~HttpHeader() {}
		
		HttpHeader&	operator=(const HttpHeader &);


	public: // TO REMOVE
		value_type		data;
		char			buf[BUFSIZ];

};

class Socket
{
	
	public:
	/** @brief constructor / destructor */

		explicit Socket(const short& port);
		Socket(void);
		~Socket(void);

		short		getPort(void) const;
		int			getServerFd(void) const;
		sockaddr_in	getAddr(void) const;
		size_t		getAddrLen(void) const;


		/** @brief init socket */
		void		startUp(void);
		
		void		resolveHttpRequest(int);

		void		parse(int skt, const char *);
		void		parse(int skt, const std::string &);


	private:
		void	errorExit(const std::string &) const;
		void	bindStep(const int &, const sockaddr_in &);
		void	listenStep(const int &);

		void	getParsing(int skt, const char *); // -- in process
		void	getParsing(int skt, const std::string &); // TO REMOVE
		void	_parse_wrapper(const char *);


	private:
		short		_port;
		int			_serverFd;
		sockaddr_in	_addr;
		size_t		_addrLen;
		HttpHeader	header;
	
	
	friend class HttpHeader;
	
};


_END_NS_WEBSERV

#endif
