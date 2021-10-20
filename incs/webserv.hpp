/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/20 23:10:56 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <fcntl.h>
# include <cctype>

# include <iostream>
# include <cstring>

# include <exception>
# include <fstream>
# include <iomanip>
# include <sstream>

# include <map>
# include <vector>

# include "defs.hpp"
# include "ServerGenerator.hpp"

_BEGIN_NS_WEBSERV

bool	ft_isNumeric(const std::string &str);

class ServerGenerator;

class WebServer
{
	
	public:
		WebServer(void)
		{}
		
		WebServer(const WebServer&);
		
		virtual ~WebServer()
		{}
		
		WebServer&	operator=(const WebServer&) { return *this; }


		void		parse(const std::string);


	public:
		class ParsingError : public std::exception
		{
            public:
                ParsingError() {}
			    virtual const char*	what() const throw() { return "Config File Error"; }
		};


	private:
		ServerGenerator	_servers;
	
}; /* class WebServer */

_END_NS_WEBSERV

#endif /* !defined(WEBSERV_HPP) */
