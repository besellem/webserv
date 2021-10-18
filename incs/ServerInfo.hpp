/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:27:04 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/18 18:04:40 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include <map>
# include <vector>

class ServerInfo
{
    private:
        std::string                 _name;
        int                         _port;
        std::string                 _root;
        std::string                 _index;
        int                         _autoindex;
        std::map<int, std::string>  _error_pages;
        int                         _cli_max_size;
        std::vector<std::string>    _allow;
        std::string                 _alias;
        std::string                 _cgi;
        Server                      _location;
        
};

#endif
