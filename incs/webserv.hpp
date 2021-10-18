/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:22:05 by besellem          #+#    #+#             */
/*   Updated: 2021/10/18 15:45:10 by besellem         ###   ########.fr       */
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

# include "defs.hpp"


class WebServer
{
	
	public:
		WebServer(void)
		{}
		
		WebServer(const WebServer&);
		
		~WebServer()
		{}
		
		WebServer&	operator=(const WebServer&);


		void		parse(const std::string);


	public:
		class ParsingError : public std::exception
		{
			virtual const char*	what() const throw();
		};


	private:
	
	
}; /* class WebServer */


#endif /* !defined(WEBSERV_HPP) */
