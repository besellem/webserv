/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/24 18:37:04 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

CgiError::CgiError() {}

const char*	CgiError::what() const throw() {
	return "CGI Error";
}

char **cgiEnv(const Server& server, const std::string &method)
{
    std::map<std::string, std::string>  env;
    
    env["SERVER_SOFTWARE"] = "HTTP/1.1";
    env["SERVER_NAME"] = server.name()[0];
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    
    env["SERVER_PROTOCOL"] = "webserv/0.0";
    env["SERVER_PORT"] = std::to_string(server.port());
    env["REQUEST_METHOD"] = method;
    env["PATH_INFO"] = server.locations(0).cgi[0];
    env["SCRIPT_NAME"] = "";
    env["QUERY_STRING"] = "";
    env["REMOTE_HOST"] = "";
    env["REMOTE_ADDR"] = "";
    env["AUTH_TYPE"] = "";
    env["REMOTE_USER"] = "";
    env["REMOTE_IDENT"] = "";
    env["CONTENT_TYPE"] = "";
    env["CONTENT_LENGTH"] = "";
    
    env["HTTP_ACCEPT"] = "*/*";
    env["HTTP_ACCEPT_LANGUAGE"] = "en-US,en;q=0.5";
    env["HTTP_USER_AGENT"] = "Safari/12.1.2";
    env["HTTP_COOKIE"] = "";
    env["HTTP_REFERER"] = "";

    char **envp;
    size_t i = 0;
    std::string str;
    envp = (char **)malloc(sizeof(char *) * env.size() + 1);
    for (std::map<std::string, std::string>::iterator  it = env.begin(); it != env.end(); it++, i++)
    {
        str = it->first + "=" + it->second;
        envp[i] = strdup(str.c_str());
        std::cout << envp[i] << std::endl;
    }
    envp[i] = 0;
    return envp;
}

std::string addHeader(const std::string& content)
{
    std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    header += std::to_string(content.size());
    header += "\n\n";
    return header + content;
}

std::string execute_cgi(const t_location& loc, const std::string file, char **envp)
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
        if (execve(arg[0], arg, envp)== -1)
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
    std::cout << "content: \n" << content << std::endl;
    for (int i = 0; envp[i]; i++)
        free(envp);
    free(envp);
    return addHeader(content);
}

_END_NS_WEBSERV
