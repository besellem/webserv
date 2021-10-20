/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 15:53:17 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/20 06:34:44 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERGENERATOR_HPP
# define SERVERGENERATOR_HPP

# include "Server.hpp"

enum e_state
{
	START = 0,
	NEW_SERVER,
	IN_SERVER,
	NEW_LOCATION,
	IN_LOCATION,
	END
};

class ServerGenerator
{
		friend class WebServer;
	public:
		typedef std::vector<std::string> tokens_type;
		
		ServerGenerator();
		~ServerGenerator();
		ServerGenerator(const ServerGenerator &);
		ServerGenerator&	operator=(const ServerGenerator &);

		Server*	operator[](int i) const;
		Server*	lastServer() const;
		
		void	newDirective(Server *, const tokens_type &);
		void	newLocation(Server *, const tokens_type &);
		void	newServer(const tokens_type &);
		void	openBlock(const tokens_type &);
		void	closeBlock(const tokens_type &);
		
	private:
		std::vector<Server *>	_servers;
		int						_state;

	public:
		friend std::ostream& operator<<(std::ostream& os, const ServerGenerator& config);

}; /* class ServerGenerator */

#endif /* !defined(SERVERGENERATOR_HPP) */
