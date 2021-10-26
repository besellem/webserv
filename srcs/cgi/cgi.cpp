/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/26 16:26:59 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

cgi::CgiError::CgiError() {}

const char*	cgi::CgiError::what() const throw() {
	return "CGI Error";
}

cgi::cgi(const Socket &socket, const std::string &program) :
    _socket(socket), _program(program) {
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

std::string	Socket::getRequestData(const std::string &name)
{
    if (this->header.data.find("Host") == this->header.data.end())
    {
        std::cout << "!!!!! ERROR !!!!!" << std::endl;
        this->header.data["Host"].push_back("");
        this->header.data["Accept"].push_back("");
        this->header.data["User-Agent"].push_back("");
        this->header.data["Accept-Language"].push_back("");
        this->header.data["Referer"].push_back("");
    }
    std::cout << this->header.data["Host"][0] << std::endl; 
    std::string dataNames[] = {"SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "SCRIPT_NAME", "REMOTE_HOST", "REMOTE_ADDR", "CONTENT_LENGTH", "HTTP_ACCEPT",
        "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "HTTP_REFERER"};
    std::string  values[] = {std::to_string(this->_port), this->header.request_method,
        this->header.path_constructed, this->header.path_constructed, this->header.data["Host"][0],
        this->header.data["Host"][0], std::to_string(this->getFileLength(this->header.path_constructed)),
        this->header.data["Accept"][0], this->header.data["Accept-Language"][0],
        this->header.data["User-Agent"][0], this->header.data["Referer"][0]};

    if (name == "QUERY_STRING")
    {
        size_t pos = this->header.data["Referer"][0].find("?");
        if (pos == std::string::npos)
            return std::string("");
       return this->header.path.substr(pos, this->header.path.size());
    }
    else if ("SERVER_SOFTWARE")
        return "HTTP/1.1";
    else if ("GATEWAY_INTERFACE")
        return "CGI/1.1";
    else if (name == "REDIRECT_STATUS")
        return std::to_string(this->getStatus().first);
    else if (name == "CONTENT_TYPE")
        return "php";
    for (size_t i = 0; i < 11; i++)
    {
        if (name == dataNames[i])
        {
            std::cout << values[i] << std::endl;
            return values[i];
        }
    }
    std::cout << "!!!!! ERROR !!!!!" << std::endl;
    return std::string();
}

/* Set the CGI environment variables.
CGI Environment variables contain data about the transaction
between the client and the server. */

void cgi::setEnv()
{
    std::string dataNames[] = {"SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "SCRIPT_NAME", "REMOTE_HOST", "REMOTE_ADDR", "CONTENT_LENGTH", "HTTP_ACCEPT",
        "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "HTTP_REFERER"};
    
    // ["SERVER_NAME"] = "";
    // ["SERVER_PROTOCOL"] = "webserv/0.0";
    // ["AUTH_TYPE"] = "";
    // ["REMOTE_USER"] = "";
    // ["REMOTE_IDENT"] = "";

    size_t i = 0;
    size_t size = 11;
    std::string str;
    this->_env = (char **)malloc(sizeof(char *) * (size + 1));
    if (!this->_env)
        throw std::bad_alloc();
    while (i < size)
    {
        LOG;
        std::cout << "i = " << i << " name = " << dataNames[i] << std::endl;
        str = dataNames[i] + "=" + this->_socket.getRequestData(dataNames[i]);
        LOG;
        this->_env[i] = strdup(str.c_str());
        LOG;
        if (!this->_env[i])
            throw std::bad_alloc();
        LOG;
        std::cout << this->_env[i] << std::endl;
        LOG;
        ++i;
    }
    this->_env[i] = 0;
}

/* Executes the CGI program on a file.
Returns the output in a string */

std::string cgi::execute(const std::string &fileName)
{
    pid_t               pid;
    int                 status;
    int                 fd[2];
    char                buffer[4098];
    std::string		    content;

    if (pipe(fd) == -1)
        throw CgiError();
    char *arg[] = {strdup(this->_program.c_str()),
            strdup(fileName.c_str()), NULL};
    std::cout << "arg: " << arg[0] << ", " << arg[1] << std::endl;
    errno = 0;
    if ((pid = fork()) == -1)
        throw CgiError();
    else if (pid == 0)
    {
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1)
        {
            std::cout << strerror(errno) << std::endl;
            close(fd[1]);
            // throw CgiError();
        }
        close(fd[1]);
        if (execve(arg[0], arg, this->_env)== -1)
            throw CgiError();
        exit(EXIT_FAILURE);
    }
    waitpid(-1, &status, 0);
    // if (status != 0)
        // throw CgiError();
    close(fd[1]);
    while (read(fd[0], buffer, sizeof(buffer)) > 0)
        content += buffer;
    close(fd[0]);
    free(arg[0]);
    free(arg[1]);
    std::cout << "content: \n" << content << std::endl;
    return content;
}

_END_NS_WEBSERV
