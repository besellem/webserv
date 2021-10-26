/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/26 23:12:15 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "defs.hpp"
# include "Server.hpp"
# include "HttpHeader.hpp"


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
		// explicit Socket(short const &);
		Socket(const Socket &);
		~Socket();

		Socket&		operator=(const Socket &);


		short		getPort(void) const;
		int			getServerFd(void) const;
		sockaddr_in	getAddr(void) const;
		size_t		getAddrLen(void) const;


		/** @brief init socket */
		void		startSocket(void);

		
		void		readHttpRequest(int);
		void		resolveHttpRequest(void);
		pair_type	getStatus(void) const;                // old - to remove
		pair_type	getStatus(const std::string &) const; // (?) may be static
		std::string	getRequestData(const std::string &);
		void		sendHttpResponse(int);

		void		setNonBlock(int & fd);
		int			socketAccept(void);


	/* Static public functions */
	public:
		static ssize_t		getFileLength(const std::string &);
		static std::string	getFileContent(const std::string &);


	private:
		void		errorExit(const std::string &) const;
		void		bindStep(const int &, const sockaddr_in &);
		void		listenStep(const int &);
		
		void		checkHttpHeaderLine(const std::string &);
		std::string	constructPath(void) const;
		std::string	generateAutoindexPage(void) const;

	private:
		const Server*	_server_block; // which was parsed
		short			_port;
		int				_serverFd;
		sockaddr_in		_addr;
		size_t			_addrLen;
		HttpHeader		header;
	
}; /* class Socket */


_END_NS_WEBSERV

#endif /* !defined(SOCKET_HPP) */
