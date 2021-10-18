/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/18 16:38:26 by adbenoit         ###   ########.fr       */
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

# include <iostream>
# include <cstring>

# include <exception>
# include <fstream>
# include <iomanip>

# include "defs.hpp"


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
			    virtual const char*	what() const throw() { return "File error"; }
		};


	private:
	
	
}; /* class WebServer */


#endif /* !defined(WEBSERV_HPP) */
