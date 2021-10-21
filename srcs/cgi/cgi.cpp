/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/21 18:28:48 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

void    WebServer::execute_cgi(const t_location& loc, const std::string input, int fd, char *envp[])
{
    pid_t       pid;
    // std::string cmd;
    int         status;

    for (size_t i = 0; i < loc.cgi.size(); i++)
    {
        // cmd = loc.cgi[i] + " " + input + " > " + output + "2> /dev/null";
        // pid = fork();
        // if (pid == 0)
        // {
        //     if (std::system(cmd.c_str()))
        //         std::cout << "cgi OK\n";
        //     else
        //         std::cout << "cgi KO\n";
        //     sleep(1);
        //     exit(0);
        // }
        pid = fork();
        if (pid == 0)
        {
            // char *arg[] = {loc.cgi[0].c_str(), input.c_str()};
            char *arg[] = {"/usr/bin/php", "text.php"};
            // fcntl(0, F_DUPFD);
            execve((loc.cgi[i]).c_str(), arg, envp); // utilise surement write donc doit trouver une solution pour ecrire dans un buffer
            exit(0);
        }
    }
    for (size_t i = 0; i < loc.cgi.size(); i++)
        waitpid(-1, &status, 0);
}

_END_NS_WEBSERV
