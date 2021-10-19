/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:17 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 17:12:26 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERGENERATOR_HPP
# define SERVERGENERATOR_HPP

# include "Server.hpp"
# include "webserver.hpp"

class WebServ;

enum e_state
{
	START = 0,
	NEW_SERVER,
	SERVER_OPEN,
	NEW_LOCATION,
	LOCATION_OPEN,
	END
};

class ServerGenerator
{
		friend class WebServ;
	public:
		typedef std::vector<std::string> tokens_type;
		
		ServerGenerator();
		~ServerGenerator();
		ServerGenerator(const ServerGenerator &);
		ServerGenerator&	operator=(const ServerGenerator &);

		Server*	operator[](size_t i);
		Server*	lastServer() const;

		void	newDirective(Server *, const tokens_type &)
		void	newLocation(Server *, const tokens_type &);
		void	newServer(const tokens_type &);
		void	openBlock(const tokens_type &);
		void	closeBlock(const tokens_type &);
		
	private:
		std::vector<Server *>	_servers;
		e_state					_state;

}; /* class ServerGenerator */

#endif /* !defined(SERVERGENERATOR_HPP) */
