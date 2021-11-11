/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/07 22:52:33 by adbenoit         ###   ########.fr       */
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
		Cgi& 			operator=(const Cgi &);
	
	public:
		const std::string&	getExtension(void) const;
		const std::string&	getProgram(void) const;
		char**				getEnv(void) const;
		const std::string	getEnv(const std::string &);
		std::string			getHeaderData(const std::string &);
		const int&			getStatus(void) const;

	
		std::string			execute(void);
		void				clear(void);

	private:
		std::string		getOuput(int);
		void			handleProcess(int, time_t);
		void			setEnv(void);
		void			setStatus(void);

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
		
};

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
