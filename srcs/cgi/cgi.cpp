/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/27 16:15:32 by adbenoit         ###   ########.fr       */
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
    _socket(socket), _program(program), _contentLength(0) {
    this->setEnv();
}

cgi::~cgi() {
    this->clear();
}

/*
** Getters
*/

const size_t&	cgi::getContentLength() const {
    return this->_contentLength;
}

char**	cgi::getEnv() const {
    return this->_env;
}

/*
** Modifiers
*/

/* Free _env */
void    cgi::clear() {
    if (this->_env)
    {
        for (int i = 0; this->_env[i]; i++)
            free(this->_env[i]);
        free(this->_env);
        this->_env = 0;
    }
}

/* Set the CGI environment variables.
CGI Environment variables contain data about the transaction
between the client and the server. */
void cgi::setEnv()
{
    std::string envVar[] = {"SERVER_PORT", "CONTENT_LENGTH", "PATH_INFO",
        "SCRIPT_NAME", "REMOTE_HOST", "REMOTE_ADDR", "HTTP_ACCEPT", "HTTP_ACCEPT_LANGUAGE",
        "HTTP_USER_AGENT", "HTTP_REFERER", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS"};
    // "REQUEST_METHOD", "SERVER_SOFTWARE", "GATEWAY_INTERFACE" : security error
    size_t i = 0;
    size_t size = 13;
    
    this->_env = (char **)malloc(sizeof(char *) * (size + 1));
    if (!this->_env)
        throw std::bad_alloc();
    while (i < size)
    {
        std::string str = envVar[i] + "=" + this->_socket.getCgiEnv(envVar[i]);
        this->_env[i] = strdup(str.c_str());
        if (!this->_env[i])
            throw std::bad_alloc();
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
    errno = 0;
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
        // throw CgiError();
    close(fd[1]);
    int n;
    while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
    {
        buffer[n] = 0;
        content += buffer;
    }
    close(fd[0]);
    free(arg[0]);
    free(arg[1]);
    this->_contentLength = content.size();
    size_t pos = content.find("\n\r\n");
    if (pos != std::string::npos)
        this->_contentLength -= pos + 3;

    // ##################################################################
    if (!DEBUG)
    {
        std::cout << "............ CGI ENVIRON ............." <<std::endl;
        int i = -1;
        while(this->_env[++i])
            std::cout << this->_env[i] << std::endl;
        std::cout << "......................................" <<std::endl;
        std::cout << "############ CGI OUTPUT ##############" << std::endl;
        std::cout << content << std::endl;
        std::cout << "######################################" << std::endl;
    }
    
    return content;
}


_END_NS_WEBSERV
