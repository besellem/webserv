/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 00:36:05 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/25 12:13:25 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "socket.hpp"

_BEGIN_NS_WEBSERV

class cgi
{
	public:
		cgi(const Socket &, const std::string &);
		~cgi();
		
		char**			getEnv() const;
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
		
};

_END_NS_WEBSERV

#endif /* !defined(CGI_HPP) */
