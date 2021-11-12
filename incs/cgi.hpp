/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/12 16:32:35 by kaye             ###   ########.fr       */
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
		
	private:
		Cgi(void);
		Cgi(const Cgi &);
		Cgi& 				operator=(const Cgi &);
	
	public:
		const std::string&	getExtension(void) const;
		const std::string&	getProgram(void) const;
		char**				getEnv(void) const;
		const std::string	getEnv(const std::string &);
		std::string			getHeaderData(const std::string &);
		const int&			getStatus(void) const;
		const int&			getCgiStatus(void) const;

	
		void			execute(void);
		void				clear(void);
		
		std::pair<bool, std::string>	parseCgiContent(void);

	private:
		// std::string			getOuput(int);
		std::pair<bool, std::string>	getOuput(int);
		void				handleProcess(int, time_t);
		void				setEnv(void);
		void				setStatus(void);

	public:
		class CgiError : public std::exception
		{
			public:
				virtual const char*	what() const throw();
		};
		
	private:
		char 				**_env;
		std::string			_program;
		std::string			_extension;
		Request*			_request;
		std::string			_header;
		int					_status;
		int					_cgiStatus;

		int					_cgiFd;
		
}; /* class Cgi */

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
