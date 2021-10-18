/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:27:04 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/18 19:17:30 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

# include "webserv.hpp"
# include <map>
# include <vector>

class ServerInfo
{
		friend class WebServer;
		
	private:
		std::string					_name;
		int					        _port;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::map<int, std::string>  _errorPages;
		int							_cliMaxSize;
		std::vector<std::string>    _allow;
		std::string					_alias;
		std::string					_cgi;
		// ServerInfo					_location;

	public:
		ServerInfo();
		~ServerInfo();
		ServerInfo(const ServerInfo &);
		ServerInfo& operator=(const ServerInfo &);
		
		void	setName(std::string);
		void	setPort(int);
		void	setPort(std::string);
		void	setRoot(std::sjtring);
		void	setIndex(std::string);
		// void	setAutoindex(std::string);
		void	setAutoindex(int);
		void	setErroPages(std::string);
		void	setErrorPages(int error, std::stringpage);
		void	setCliMaxSize(int);
		void	setAllow(std::string);
		void	setAlias(std::string);
		void	setCgi(std::string);

		void	setDirective(const std::string& name, std::string value) {
			std::string directives[] = {"server_name", "listen", "root", "index",
			"autoindex", "error_pages", "cli_max_size", "allow", "alias", "cgi"};
			void	(*set[])(std::string) = {&setName, &setPort, &setRoot, &setIndex,
			&setAutoindex, &setErrorPages, &setCliMaxSize, &setAllow, &setAlias, &setCgi};
			
			for (int i = 0; i < 10; i++)
				if (name == directives[i])
					return set[i](value);
	}
};

#endif
