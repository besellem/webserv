/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/31 18:02:02 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "defs.hpp"
# include "utils.hpp"
# include "Server.hpp"
# include "HttpHeader.hpp"
# include "Response.hpp"
# include "Request.hpp"


_BEGIN_NS_WEBSERV

class Socket
{
	public:
		typedef std::vector<std::string>                       vector_type;
		typedef std::pair<int, std::string>                    pair_type;

	public:
	/** @brief constructor / destructor */

		explicit Socket(void);
		explicit Socket(const Server *);
		Socket(const Socket &);
		~Socket();

		Socket&		operator=(const Socket &);


		short			getPort(void) const;
		int				getServerFd(void) const;
		const Server*	getServer(void) const;
		sockaddr_in		getAddr(void) const;
		size_t			getAddrLen(void) const;


		/** @brief init socket */
		void			startSocket(void);
		
		void		readHttpRequest(Request *, int);
		void		resolveHttpRequest(Request *);
		void		sendHttpResponse(Request *, int);

		void		setNonBlock(int & fd);
		int			socketAccept(void);


	/* Static public functions */
	public:
		static pair_type	getStatus(const std::string &);

	private:
		void		errorExit(const std::string &) const;
		void		bindStep(const int &, const sockaddr_in &);
		void		listenStep(const int &);
		
		void		setHeaderData(const std::string &);
		std::string	constructPath(void);

	private:
		const Server	*_server_block; // which was parsed
		short			_port;
		int				_serverFd;
		sockaddr_in		_addr;
		size_t			_addrLen;
	
}; /* class Socket */


_END_NS_WEBSERV

#endif /* !defined(SOCKET_HPP) */
