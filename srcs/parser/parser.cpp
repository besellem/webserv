/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/18 23:13:39 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// typedef void (ServerInfo::*t_function)(std::string);

void    parse_line(const std::string &line) {
    if (line.empty())
        return ;
}

void	WebServer::parse(const std::string config_file)
{
    int pos = config_file.rfind(".conf");
    if (pos != (int)config_file.size() - 5)
        throw ParsingError();
    std::fstream os;
    std::string line;
    os.open(config_file);
    int i = 10;
    while(i)
    {
        std::getline(os, line);
        std::vector<std::string> tokens;
        size_t pos = line.find(' ');
        pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;
        while (pos != std::string::npos)
        {
            std::string str = line.substr(0, pos);
            if (!str.empty())
                tokens.push_back(str);
            // if (tokens.size() != 0)
            //     std::cout << tokens[tokens.size() - 1] << std::endl;
            line.erase(0, pos + 1);
            pos = line.find(' ');
            pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;
        }
        --i;
    }
}
