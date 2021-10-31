/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/31 11:17:44 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"

_BEGIN_NS_WEBSERV

class Request;

class Cgi
{
	public:
		Cgi(Request *);
		~Cgi();
		
		char**				getEnv() const;
		const size_t&		getContentLength() const;
		const std::string&	getExtension() const;
		const std::string&	getProgram() const;
		
		std::string			execute(const std::string &);
		void				clear();
		
	private:
		Cgi();
		Cgi(const Cgi &);
		Cgi& 			operator=(const Cgi &);
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
		std::string			_program;
		std::string			_extension;
		Request*			_request;
		size_t				_contentLength;
		
};

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
