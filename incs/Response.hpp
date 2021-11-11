/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:41:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/11 16:17:18 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "defs.hpp"
# include "cgi.hpp"
# include "Request.hpp"

_BEGIN_NS_WEBSERV


class Response
{
	public:
		typedef std::pair<int, std::string> status_type;
		typedef std::map<std::string, std::string> info_type;

		Response(Request *);
		~Response();

	private:
		Response();
		Response(const Response &);
		Response& operator=(const Response &);

	public:
		const std::string&  getHeader(void) const;
		const std::string&  getContent(void) const;
		size_t				getContentLength(void) const;
		const status_type&  getStatus(void) const;
		
		const std::string   generateAutoindexPage(std::string const &) const;
		bool				uploadFile(void);
	
		void		        setContent(const std::string &);
		void		        setHeader(void);
		void		        setStatus(const status_type &);
		void		        setStatus(int);
		void                setErrorContent(void);
		

		void				getMethod(const std::string &);
		void				postMethod(void);
		void				deleteMethod(void);
		bool                isMethodAllowed(const std::string &);
		void				cgi(void);

		int					getCgiStatus(void) { return _cgiStatus; }

	private:
		std::string						_header;
		std::string						_content;
		status_type						_status;
		Request*						_request;
		Cgi*							_cgi;
		const t_location*				_location;

		int	_cgiStatus;
};


_END_NS_WEBSERV

#endif
