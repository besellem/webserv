/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 16:49:04 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 14:17:24 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "defs.hpp"
# include "Server.hpp"

_BEGIN_NS_WEBSERV

class HttpHeader
{

	public:
		typedef	void*                                                pointer;
		typedef std::map<std::string, std::vector<std::string> >     value_type;
	
	public:
		HttpHeader(void) :
			data(),
			request_method(),
			path_info() { resetBuffer(); }

		HttpHeader(const HttpHeader &x) { *this = x; }

		~HttpHeader() {
			std::cout << "destructor ???\n";
		}

		pointer		resetBuffer(void) { return memset(buf, 0, sizeof(buf)); }
		
		HttpHeader&	operator=(const HttpHeader &x)
		{
			if (this == &x)
				return *this;
			data = x.data;
			request_method = x.request_method;
			path_info = x.path_info;
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

		class HttpBadRequestError : public std::exception
		{
			public:
				virtual const char*	what() const throw()
				{ return "bad http request"; }
		};


	public: // TO REMOVE
		value_type		data;
		std::string		request_method;
		std::string		path_info;
		char			buf[BUFSIZ];

};

class Socket {
	public:
	/** @brief constructor / destructor */

		explicit Socket(void);
		explicit Socket(const short &);
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
		int			getStatusCode(void) const;
		const char*	getStatusMessage(int) const;
		size_t		getContentLength(void) const;
		std::string	getFileContent(void);
		void		sendHttpResponse(int);
		HttpHeader&	getHeader() const { return *this->header; }

		void		setNonBlock(int & fd);
		int			socketAccept(void);

	private:
		void	errorExit(const std::string &) const;
		void	bindStep(const int &, const sockaddr_in &);
		void	listenStep(const int &);
		
		void	checkHttpHeaderLine(const std::string &);


	private:
		short		_port;
		int			_serverFd;
		sockaddr_in	_addr;
		size_t		_addrLen;
		HttpHeader	*header;
	
	
	friend class HttpHeader;
	
};

_END_NS_WEBSERV

#endif
