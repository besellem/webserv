/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 20:25:06 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/18 22:31:52 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerInfo.hpp"

ServerInfo::ServerInfo() {}

ServerInfo::~ServerInfo() {}

ServerInfo::ServerInfo(const ServerInfo &x) { *this = x; }

ServerInfo& ServerInfo::operator=(const ServerInfo &x) {
    if (this == &x)
        return *this;
	this->_port = x._port;
    this->_name = x._name;
    this->_errorPages = x._errorPages;
	this->_cliMaxSize = x._cliMaxSize;
    // reflechir a location
    return *this;
}

/*
**  Setters
*/

void	ServerInfo::setName(const std::string &name) { this->_name = name; }
void	ServerInfo::setPort(int port) { this->_port = port; }
void	ServerInfo::setErrorPages(int code_err, const std::string &page_name) {
    this->_errorPages.insert(std::make_pair(code_err, page_name));
}
void	ServerInfo::setCliMaxSize(int size) { this->_cliMaxSize = size; }
