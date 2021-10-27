/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 16:09:26 by besellem          #+#    #+#             */
/*   Updated: 2021/10/27 20:00:04 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpHeader.hpp"


_BEGIN_NS_WEBSERV

HttpHeader::HttpHeader(void) :
	data(),
	request_method(),
	path(),
	path_constructed()
{ this->resetBuffer(); }

HttpHeader::HttpHeader(const HttpHeader &x)
{ *this = x; }

HttpHeader::~HttpHeader()
{ this->resetBuffer(); }

HttpHeader&			HttpHeader::operator=(const HttpHeader &x)
{
	if (this == &x)
		return *this;
	data = x.data;
	request_method = x.request_method;
	path = x.path;
	path_constructed = x.path_constructed;
	memcpy(buf, x.buf, sizeof(buf));
	return *this;
}

HttpHeader::pointer	HttpHeader::resetBuffer(void)
{ return std::memset(buf, 0, sizeof(buf)); }


const char*			HttpHeader::HttpHeaderParsingError::what() const throw()
{ return "incomplete http header received"; }

const char*			HttpHeader::HttpBadRequestError::what() const throw()
{ return "bad http request"; }


_END_NS_WEBSERV
