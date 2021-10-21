/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/21 21:50:33 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

_BEGIN_NS_WEBSERV

void    WebServer::execute_cgi(const t_location& loc, const std::string input, int fd, char *envp[])
{
    pid_t       pid;
    int         status;
    int         p[2];

    std::cout << "trying to executing cgi ...\n";
	std::fstream os;
    int fd1 = open("./www/tmp.html", O_APPEND | O_CREAT);
    pid = fork();
    pipe(p);
    char *arg[] = {strdup("/usr/bin/php"), strdup("test.php"), NULL};
    if (pid == 0)
    {
        dup2(p[0], STDIN_FILENO);
        dup2(fd1, STDOUT_FILENO);
        execve(arg[0], arg, envp);
        exit(0);
    }
    close(p[0]);
    close(p[1]);
    waitpid(-1, &status, 0);
    if (status != 0)
        std::cout << "====== OK =====" << std::endl;
    else
        std::cout << "====== KO ======" << std::endl;
    std::cout << std::endl;
    close(fd1);
    free(arg[0]);
    free(arg[1]);
}

_END_NS_WEBSERV
