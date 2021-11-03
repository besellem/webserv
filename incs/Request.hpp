/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:41:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/03 19:18:23 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "defs.hpp"
# include "HttpHeader.hpp"
# include "Server.hpp"
# include "cgi.hpp"

_BEGIN_NS_WEBSERV

enum e_uploadKey
{
	FN = 0,
	DQ,
	CT,
	RN,
	DRN,
	LAST_BOUNDARY
};

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
		HttpHeader&									getHeader(void);
		const std::string&							getContent(void) const;
		const std::string&							getConstructPath(void) const;
		size_t										getContentLength(void) const;
		const Server*								getServer(void) const;
		const t_location*							getLocation(void) const;
		
		
		void				setConstructPath(void);
		void				setConstructPath(const std::string &);
		void				setContent(void);
		void				setHeaderData(const std::string &);

		const std::pair<std::string, std::string>&	getFileInfo(void) const;
		bool				checkIsUploadCase(void);
		const std::string&	getBoundary(void) const;
		bool				parseFile(void);

	private:
		HttpHeader							_header;
		std::string							_constructPath;
		std::string							_content;
		const Server						*_server;

		std::pair<std::string, std::string>	_fileInfo;
		std::string							_boundary;
};


_END_NS_WEBSERV

#endif
