/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:27 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/04 14:09:47 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <defs.hpp>
# include "Server.hpp"


_BEGIN_NS_WEBSERV

typedef struct s_location
{
	typedef	std::pair<int, std::string> 		redirection_type;
	typedef	std::pair<std::string, std::string>	cgi_type;
	
	std::string					path;			// location
	std::vector<std::string>	methods;		// list of accepted HTTP Methods for the root
	redirection_type			redirection;	// HTTP redirection
	std::string					root;			// directory or a file from where the file should be search
	std::vector<std::string>	index;			// default file to answer if the request is a directory
	bool						autoindex;		// turn on or off directory listing
	cgi_type					cgi;			// execute the cgi program
	std::string					uploadStore;	// execute the cgi program
}				t_location;

class Server
{
	public:
		typedef std::vector<std::string> tokens_type;

		Server();
		~Server();
		
		/*
		**  Getters
		*/
	
		const int&							port() const;
		const std::vector<std::string>&		name() const;
		const std::map<int, std::string>&	errorPages() const;
		const int&							cliBodyMaxSize() const;
		const t_location&					locations(int) const;
		const std::vector<t_location *>&	locations(void) const;
		size_t								nLoc() const;

		/*
		**  Setters
		*/
	
		void	setPort(const tokens_type &);
		void	setName(const tokens_type &);
		void	setErrorPages(const tokens_type &);
		void	setcliBodyMaxSize(const tokens_type &);
		void	setMethods(t_location  *, const tokens_type &);
		void	setRedirection(t_location  *, const tokens_type &);
		void	setRoot(t_location  *, const tokens_type &);
		void	setIndex(t_location  *, const tokens_type &);
		void	setAutoIndex(t_location  *, const tokens_type &);
		void	setCgi(t_location  *, const tokens_type &);
		void	setUploadStore(t_location  *, const tokens_type &);
		void	newLocationDirective(const tokens_type &);
		
		void	newLocation(const std::vector<std::string> &);
		void	newDirective(const std::vector<std::string> &);
		
	private:
		int					        _port;				// listen port
		std::vector<std::string>	_name;				// server names
		std::map<int, std::string>  _errorPages;		// default error pages
		int							_cliBodyMaxSize;	// client max body size (byte)
		std::vector<t_location *>	_locations;			// routes with rules

		Server& operator=(const Server &);
		Server(const Server &);
		
}; /* class Server */

std::ostream& operator<<(std::ostream &, const Server &);

typedef void (Server::*method_function)(const Server::tokens_type &);
typedef void (Server::*method_function1)(t_location  *, const Server::tokens_type &);

_END_NS_WEBSERV

#endif /* !defined(SERVER_HPP) */
