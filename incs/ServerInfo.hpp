/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:27:04 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 12:47:56 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

// # include "webserv.hpp"
# include <iostream>
# include <cstring>

# include <exception>
# include <fstream>
# include <iomanip>

# include <map>
# include <vector>

// class Webserv;

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

class ServerInfo
{
		
	private:
		int					        _port;			// listen port
		std::string					_name;			// server name
		std::map<int, std::string>  _errorPages;	// default error pages
		int							_cliMaxSize;	// max client body size
		std::vector<t_location>		_locations;		// routes with rules

	public:
		ServerInfo();
		~ServerInfo();
		ServerInfo(const ServerInfo &);
		ServerInfo& operator=(const ServerInfo &);
		
		void	setPort(int);
		void	setName(const std::string &);
		void	setErrorPages(int error, const std::string &);
		void	setCliMaxSize(int);

		void	setPort(const std::vector<std::string> &);
		void	setName(const std::vector<std::string> &);
		void	setErrorPages(const std::vector<std::string> &);
		void	setCliMaxSize(const std::vector<std::string> &);
};

typedef void (ServerInfo::*method_function)(const std::vector<std::string> &);

#endif
