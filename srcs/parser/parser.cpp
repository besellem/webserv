/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 15:53:23 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/19 00:57:45 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

typedef void (ServerInfo::*t_function)(std::string);

void    parseTokens(ServerInfo& info, std::vector<std::string> tokens)
{
    if (tokens.empty())
        return ;
    static t_function   set[] = {ServerInfo::setPort, ServerInfo::setName,
            ServerInfo::setErrorPages, ServerInfo::setCliMaxSize}; // Trouver comment faire un tableau de methodes
    static std::string  directives[] = {"listen", "server_name",
            "error_pages", "cli_max_size"};
    for (int i = 0; i < 4; i++)
        if (tokens[0] == directives[i])
            info.(set[i])(*(tokens.begin() + 1));
}

std::vector<std::string>    getTokens(std::string line)
{
    std::vector<std::string> tokens;

    size_t pos = line.find(' ');
    pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;

    while (pos != std::string::npos)
    {
        std::string str = line.substr(0, pos);
        if (!str.empty())
            tokens.push_back(str);
        line.erase(0, pos + 1);
        pos = line.find(' ');
        pos = (line.find('\t', pos) != std::string::npos) ? line.find('\t', pos) : pos;
    }
    
    return tokens;
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
    while(i) // trouver le EOF
    {
        std::getline(os, line);
        parseTokens(this->_info, getTokens(line));
        --i;
    }
}
