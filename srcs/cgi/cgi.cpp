/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/22 14:01:41 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

void    WebServer::execute_cgi(const t_location& loc, const std::string file, char *envp[])
{
    pid_t               pid;
    int                 status;
    int                 p[2];
    const std::string   tmp_file("www/tmp.html");

    errno = 0;
    std::cout << "trying to executing cgi ...\n";
	std::fstream os;
    int fd1 = open(tmp_file.c_str(), O_WRONLY | O_CREAT, 0666);
    if (fd1 == -1)
            std::cout << "cgi: " << tmp_file << ": " << strerror(errno) << std::endl;
    pid = fork();
    pipe(p);
    char *arg[] = {strdup(loc.cgi[0].c_str()), strdup(file.c_str()), NULL};
    if (pid == 0)
    {
        if (dup2(p[0], STDIN_FILENO) == -1)
            std::cout << "cgi: " << strerror(errno) << std::endl;
        if (dup2(fd1, STDOUT_FILENO) == -1)
            std::cout << "cgi: " << strerror(errno) << std::endl;
        close(fd1);
        close(p[0]);
        close(p[1]);
        if (execve(arg[0], arg, envp) == -1)
            std::cout << "cgi: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
        std::cout << "cgi: Fork failed" << std::endl;
    close(p[0]);
    close(p[1]);
    waitpid(-1, &status, 0);
    if (status == 0)
        std::cout << "====== OK =====" << std::endl;
    else
        std::cout << "====== KO ======" << std::endl;
    std::cout << std::endl;
    close(fd1);
    free(arg[0]);
    free(arg[1]);
}

_END_NS_WEBSERV
