/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 16:04:15 by besellem          #+#    #+#             */
/*   Updated: 2021/11/02 17:49:28 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HEADER_HPP
# define HTTP_HEADER_HPP

# include "defs.hpp"

_BEGIN_NS_WEBSERV

class HttpHeader
{
	public:
		typedef	void*                                                pointer;
		typedef std::map<std::string, std::vector<std::string> >     value_type;
	
	public:
		HttpHeader(void);
		HttpHeader(const HttpHeader &x);
		~HttpHeader();
		HttpHeader&		operator=(const HttpHeader &x);

		pointer			resetBuffer(void);
		
	
	public:
		class HttpHeaderParsingError : public std::exception
		{
			public:
				virtual const char*	what() const throw();
		};

		class HttpBadRequestError : public std::exception
		{
			public:
				virtual const char*	what() const throw();
		};


	private:
		value_type		data;
		std::string		request_method;
		std::string		uri;
		std::string		queryString;
		char			buf[BUFFER_SIZE];

	friend class Socket;
	friend class Request;
	friend class Response;
	friend class Cgi;

}; /* class HttpHeader */


_END_NS_WEBSERV

#endif /* !defined(HTTP_HEADER_HPP) */
