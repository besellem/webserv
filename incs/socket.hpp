/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/22 17:09:24 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "defs.hpp"


_BEGIN_NS_WEBSERV

class HttpHeader
{

	public:
		typedef	void*                                                pointer;
		typedef std::map<std::string, std::vector<std::string> >     value_type;
	
	public:
		HttpHeader(void) : data()
		{ resetBuffer(); }

		HttpHeader(const HttpHeader &x)
		{ *this = x; }

		~HttpHeader()
		{}

		pointer		resetBuffer(void)
		{ return memset(buf, 0, sizeof(buf)); }
		
		HttpHeader&	operator=(const HttpHeader &x)
		{
			if (this == &x)
				return *this;
			data = x.data;
			memcpy(buf, x.buf, sizeof(buf));
			return *this;
		}
	
	
	public:
		class HttpHeaderParsingError : public std::exception
		{
			public:
				virtual const char*	what() const throw()
				{ return "incomplete http header received"; }
		};


	public: // TO REMOVE
		value_type		data;
		char			buf[BUFSIZ];

};

class Socket
{
	
	public:
	/** @brief constructor / destructor */

		explicit Socket(const short &);
		Socket(void);
		~Socket(void);

		short		getPort(void) const;
		int			getServerFd(void) const;
		sockaddr_in	getAddr(void) const;
		size_t		getAddrLen(void) const;


		/** @brief init socket */
		void		startUp(void);
		
		void		readHttpRequest(int);
		void		resolveHttpRequest(void);

		void		parse(int skt, const char *);
		void		parse(int skt, const std::string &);


	private:
		void	errorExit(const std::string &) const;
		void	bindStep(const int &, const sockaddr_in &);
		void	listenStep(const int &);

		void	checkHttpHeaderLine(const std::string &);

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
