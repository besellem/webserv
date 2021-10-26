/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/26 22:19:33 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "socket.hpp"

_BEGIN_NS_WEBSERV

class Socket;

class cgi
{
	public:
		cgi(const Socket &, const std::string &);
		~cgi();
		
		char**			getEnv() const;
		const size_t&	getContentLength() const;
		std::string		execute(const std::string &);
		void			clear();
		
	private:
		cgi();
		cgi(const cgi &);
		cgi& 			operator=(const cgi &);
		void			setEnv();
		
	public:
		class CgiError : public std::exception
		{
			public:
				CgiError();
				virtual const char*	what() const throw();
		};
		
	private:
		char 				**_env;
		Socket				_socket;
		const std::string	_program;
		size_t				_contentLength;
		
};

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
