/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/25 01:21:16 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"


_BEGIN_NS_WEBSERV

class WebServer;

class cgi
{
	public:
		cgi(const Server *, const t_location *, const std::string &, const std::string &);
		~cgi();
		
		char**			getEnv() const;
		std::string		execute();
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
		const Server		*_server;
		const t_location	*_location;
		std::string			_fileName; // ou fd ?
		std::string			_method;
		
};

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
