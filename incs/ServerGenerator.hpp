/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:17 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 23:05:45 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERGENERATOR_HPP
# define SERVERGENERATOR_HPP

# include "webserv.hpp"
# include "Server.hpp"

_BEGIN_NS_WEBSERV

class WebServer;

/* Config file states */

enum e_state
{
	START = 0,
	NEW_SERVER,
	IN_SERVER,
	NEW_LOCATION,
	IN_LOCATION
};

class ServerGenerator
{
	public:
		typedef Server::tokens_type tokens_type;
		
		ServerGenerator();
		~ServerGenerator();
		ServerGenerator(const ServerGenerator &);
		ServerGenerator&	operator=(const ServerGenerator &);
		
		int			state() const;
		size_t  	size() const;

		/*
		**  Element access
		*/
	
		Server*		operator[](int) const;
		Server*		last() const;
		
		/*
		**  Modifiers / Checkers
		*/
	
		void		newDirective(Server *, const tokens_type &);
		void		newLocation(Server *, const tokens_type &);
		void		newServer(const tokens_type &);
		void		openBlock(const tokens_type &);
		void		closeBlock(const tokens_type &);
		
	private:
		std::vector<Server *>	_servers;
		int						_state;

	public:

}; /* class ServerGenerator */

std::ostream& operator<<(std::ostream &, const ServerGenerator &);

_END_NS_WEBSERV

#endif /* !defined(SERVERGENERATOR_HPP) */
