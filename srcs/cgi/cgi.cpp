/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/26 19:19:37 by adbenoit         ###   ########.fr       */
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

std::string vectorToStr(const std::vector<std::string> &vect)
{
    if (vect.empty())
        return std::string("");
    std::string str = vect[0];
    for (size_t i = 1; i < vect.size(); i++)
    {
        str += " ";
        str += vect[i];
    }
    return str;
}

std::string	Socket::getRequestData(const std::string &name)
{
    if (name == "QUERY_STRING")
    {
        size_t pos = vectorToStr(this->header.data["Referer"]).find("?");
        if (pos == std::string::npos)
            return std::string("");
       return this->header.path.substr(pos);
    }
    else if (name == "REMOTE_HOST")
        return ft_strcut(vectorToStr(this->header.data["Host"]), ':');
    else if (name == "REQUEST_METHOD")
        return this->header.request_method;
    else if (name == "PATH_INFO")
        return this->header.path_constructed;
    else if (name == "SCRIPT_NAME")
        return std::string("/Users/adbenoit/.brew/bin/php-cgi");
    else if (name == "REMOTE_ADDR")
        return vectorToStr(this->header.data["Host"]);
    else if (name == "CONTENT_LENGTH")
        return std::to_string(this->getFileLength(this->header.path_constructed));
    else if (name == "HTTP_ACCEPT")
        return vectorToStr(this->header.data["Accept"]);
    else if (name == "HTTP_ACCEPT_LANGUAGE")
        return vectorToStr(this->header.data["Accept-Language"]);
    else if (name == "HTTP_USER_AGENT")
        return vectorToStr(this->header.data["User-Agent"]);
    else if (name == "HTTP_REFERER")
        return vectorToStr(this->header.data["Referer"]);
    else if (name == "SERVER_SOFTWARE")
        return std::string(HTTP_PROTOCOL_VERSION);
    else if (name == "GATEWAY_INTERFACE")
        return std::string("CGI/1.1");
    else if (name == "CONTENT_TYPE")
        return std::string("");
    else if (name == "REDIRECT_STATUS")
        return std::to_string(getStatus(this->header.path_constructed).first);
    else if (name == "SERVER_PORT")
    {
        std::string str = vectorToStr(this->header.data["Host"]);
        size_t pos = str.find(":");
        if (pos == std::string::npos)
            return std::string("");
       return str.substr(pos + 1);
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
        "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "HTTP_REFERER", "SERVER_SOFTWARE",
        "GATEWAY_INTERFACE", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS"};
    
    // ["SERVER_NAME"] = "";
    // ["SERVER_PROTOCOL"] = "webserv/0.0";
    // ["AUTH_TYPE"] = "";
    // ["REMOTE_USER"] = "";
    // ["REMOTE_IDENT"] = "";

    size_t i = 0;
    size_t size = 16;
    this->_env = (char **)malloc(sizeof(char *) * (size + 1));
    if (!this->_env)
        throw std::bad_alloc();
    while (i < size)
    {
        std::string str = dataNames[i] + "=" + this->_socket.getRequestData(dataNames[i]);
        // std::string str = dataNames[i] + "=";
        this->_env[i] = strdup(str.c_str());
        if (!this->_env[i])
            throw std::bad_alloc();
        ++i;
    }
    this->_env[i] = 0;
    i = -1;
    while(this->_env[++i])
        std::cout << this->_env[i] << std::endl;
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
    if (DEBUG)
    {
        std::cout << "############ CGI OUTPUT ##############" << std::endl;
        std::cout << "content: \n" << content << std::endl;
        std::cout << "######################################" << std::endl;
    }
    return content;
}

_END_NS_WEBSERV
