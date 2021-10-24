/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/24 16:42:09 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

CgiError::CgiError() {}

const char*	CgiError::what() const throw() {
	return "CGI Error";
}

std::string addHeader(const std::string& content)
{
    std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    header += std::to_string(content.size());
    header += "\n\n";
    return header + content;
}

std::string execute_cgi(const t_location& loc, const std::string file, char *envp[])
{
    pid_t               pid;
    int                 status;
    int                 fd[2];
    char                buffer[4098];
    std::string		    content;

    if (pipe(fd) == -1)
        throw CgiError();
    char *arg[] = {strdup(loc.cgi[0].c_str()), strdup(file.c_str()), NULL};
    if ((pid = fork()) == -1)
        throw CgiError();
    else if (pid == 0)
    {
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1)
            throw CgiError();
        close(fd[1]);
        if (execve(arg[0], arg, envp) == -1)
            throw CgiError();
        exit(EXIT_FAILURE);
    }
    waitpid(-1, &status, 0);
    if (status != 0)
        throw CgiError();
    close(fd[1]);
    while ( read(fd[0], buffer, sizeof(buffer)) > 0)
        content += buffer;
    close(fd[0]);
    free(arg[0]);
    free(arg[1]);

    return addHeader(content);
}

_END_NS_WEBSERV
