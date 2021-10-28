/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 16:04:15 by besellem          #+#    #+#             */
/*   Updated: 2021/10/27 19:56:27 by besellem         ###   ########.fr       */
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
		std::string		path;
		std::string		path_constructed;
		char			buf[BUFSIZ];

	friend class Socket;

}; /* class HttpHeader */


_END_NS_WEBSERV

#endif /* !defined(HTTP_HEADER_HPP) */
