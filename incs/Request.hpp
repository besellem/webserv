/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:41:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/01 16:38:21 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "defs.hpp"
# include "HttpHeader.hpp"
# include "Server.hpp"
# include "cgi.hpp"

_BEGIN_NS_WEBSERV

class Cgi;

class Request
{
	public:
		Request(const Server *);
		~Request();

	private:
		Request();
		Request(const Request &);
		Request&			operator=(const Request &);

	public:
		HttpHeader&			getHeader(void);
		const std::string&	getContent(void) const;
		const std::string&	getConstructPath(void) const;
		size_t				getContentLength(void) const;
		const Server*		getServer(void) const;
		const t_location*	getLocation(void) const;
		
		const std::string	getEnv(const std::string &);
		
		void				setConstructPath(void);
		void				setContent(const std::string &);
		void				setHeaderData(const std::string &);

	private:
		HttpHeader			_header;
		std::string			_constructPath;
		std::string			_content;
		const Server		*_server;
};


_END_NS_WEBSERV

#endif
