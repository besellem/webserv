/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/18 19:03:07 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void    parse_line(const std::string &line)
{
    
}

void	WebServer::parse(const std::string config_file)
{
    int pos = config_file.rfind(".conf");
    if (pos != (int)config_file.size() - 5)
        throw ParsingError();
    std::fstream os;
    std::string line;
    os.open(config_file);
    while(line[line.size() - 1] != EOF)
    {
        std::getline(os, line);
        parse_line(line);
    }
}
