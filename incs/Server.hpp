/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:27 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 22:41:05 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstring>
# include <exception>
# include <fstream>
# include <iomanip>
# include <map>
# include <vector>
# include "webserv.hpp"

class WebServer;

typedef struct s_location
{
	std::string					path;			// location
	std::vector<std::string>	methods;		// list of accepted HTTP Methods for the root
	std::string					redirection;	// HTTP redirection
	std::string					root;			// directory or a file from where the file should be search
	std::string					index;			// default file to answer if the request is a directory
	bool						autoindex;		// turn on or off directory listing
	std::string					cgi;			// a comprendre
}				t_location;

class Server
{
	public:
		typedef std::vector<std::string> tokens_type;

		Server();
		~Server();
		Server(const Server &);
		Server& operator=(const Server &);
		
		/*
		**  Getters
		*/
	
		const int&							port() const;
		const std::string&					name() const;
		const std::map<int, std::string>&	errorPages() const;
		const int&							cliMaxSize() const;
		const std::vector<t_location *>&	locations() const;

		/*
		**  Setters
		*/
	
		void	setPort(const tokens_type &);
		void	setName(const tokens_type &);
		void	setErrorPages(const tokens_type &);
		void	setCliMaxSize(const tokens_type &);
		void	setLocationMethods(const tokens_type &);
		void	newLocationDirective(const tokens_type &);
		
		void	newLocation(const std::vector<std::string> &);
		void	newDirective(const std::vector<std::string> &);
		
	private:
		int					        _port;			// listen port
		std::string					_name;			// server name
		std::map<int, std::string>  _errorPages;	// default error pages
		int							_cliMaxSize;	// max client body size
		std::vector<t_location *>	_locations;		// routes with rules

}; /* class Server */

std::ostream& operator<<(std::ostream &, const Server &);

typedef void (Server::*method_function)(const std::vector<std::string> &);

#endif /* !defined(SERVER_HPP) */
