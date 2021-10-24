/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/25 01:22:31 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

cgi::CgiError::CgiError() {}

const char*	cgi::CgiError::what() const throw() {
	return "CGI Error";
}

cgi::cgi(const Server *server, const t_location *loc, const std::string &file, const std::string &method) :
_server(server), _location(loc), _fileName(file), _method(method) {
    this->setEnv();
}

cgi::~cgi() {
    this->clear();
}

void    cgi::clear() {
    if (this->_env)
    {
        for (int i = 0; this->_env[i]; i++)
            free(this->_env[i]);
        free(this->_env);
        this->_env = 0;
    }
}

char**	cgi::getEnv() const {
    return this->_env;
}

/* il faut les infos du header au moment du read */

void cgi::setEnv()
{
    std::map<std::string, std::string>  env;
    
    env["SERVER_SOFTWARE"] = "HTTP/1.1";
    env["SERVER_NAME"] = this->_server->name()[0];
    env["GATEWAY_INTERFACE"] = "CGI/1.1";

    env["SERVER_PROTOCOL"] = "webserv/0.0";
    env["SERVER_PORT"] = std::to_string(this->_server->port());
    env["REQUEST_METHOD"] = this->_method;
    env["PATH_INFO"] = this->_location->cgi[0];
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
    
    env["REDIRECT_STATUS"] = "200";

    size_t i = 0;
    std::string str;
    this->_env = (char **)malloc(sizeof(char *) * (env.size() + 1));
    if (!this->_env)
        throw std::bad_alloc();
    for (std::map<std::string, std::string>::iterator  it = env.begin(); it != env.end(); it++, i++)
    {
        str = it->first + "=" + it->second;
        this->_env[i] = strdup(str.c_str());
        if (!this->_env[i])
            throw std::bad_alloc();
    }
    this->_env[i] = 0;
}

std::string addHeader(const std::string& content)
{
    std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    header += std::to_string(content.size());
    header += "\n\n";
    return header + content;
}

std::string cgi::execute()
{
    pid_t               pid;
    int                 status;
    int                 fd[2];
    char                buffer[4098];
    std::string		    content;

    if (pipe(fd) == -1)
        throw CgiError();
    char *arg[] = {strdup(this->_location->cgi[0].c_str()),
            strdup(this->_fileName.c_str()), NULL};
    if ((pid = fork()) == -1)
        throw CgiError();
    else if (pid == 0)
    {
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1)
            throw CgiError();
        close(fd[1]);
        if (execve(arg[0], arg, this->_env)== -1)
            throw CgiError();
        exit(EXIT_FAILURE);
    }
    waitpid(-1, &status, 0);
    // if (status != 0)
    //     throw CgiError();
    close(fd[1]);
    while (read(fd[0], buffer, sizeof(buffer)) > 0)
        content += buffer;
    close(fd[0]);
    free(arg[0]);
    free(arg[1]);
    std::cout << "content: \n" << content << std::endl;

    return addHeader(content);
}

_END_NS_WEBSERV
