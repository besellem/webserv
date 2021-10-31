/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/31 12:12:02 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

Cgi::CgiError::CgiError() {}

const char*	Cgi::CgiError::what() const throw() {
	return "CGI Error";
}

Cgi::Cgi(Request *request) : _request(request), _contentLength(0) {
    const t_location *loc = request->getLocation();
    if (loc && !loc->cgi.empty())
    {
        _extension = loc->cgi[0];
        _program = loc->cgi[1];
        this->setEnv();
    }
}

Cgi::~Cgi() {
    this->clear();
}

/*
** Getters
*/

const size_t&	    Cgi::getContentLength() const { return this->_contentLength; }
const std::string&	Cgi::getExtension() const { return this->_extension; }
const std::string&	Cgi::getProgram() const { return this->_program; }
char**	            Cgi::getEnv() const { return this->_env; }

/*
** Modifiers
*/

/* Free _env */
void    Cgi::clear() {
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
void Cgi::setEnv()
{
    std::string envVar[] = {"SERVER_PORT", "PATH_INFO", "REQUEST_METHOD", "PATH_TRANSLATED",
        "SCRIPT_NAME", "REMOTE_ADDR", "HTTP_ACCEPT", "HTTP_ACCEPT_LANGUAGE",
        "HTTP_USER_AGENT", "HTTP_REFERER", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS",
        "HTTP_ACCEPT_ENCODING", "HTTP_CONNECTION", "SERVER_PROTOCOL", "GATEWAY_INTERFACE", ""};
    size_t i = 0;
    size_t size = 0;
    
    while (!envVar[size].empty())
        ++size;
    this->_env = (char **)malloc(sizeof(char *) * (size + 1));
    if (!this->_env)
        throw std::bad_alloc();
    for (; i < size; i++)
    {
        std::string str = envVar[i] + "=" + this->_request->getEnv(envVar[i]);
        this->_env[i] = strdup(str.c_str());
        if (!this->_env[i])
            throw std::bad_alloc();
    }
    this->_env[i] = 0;
}


/* Executes the CGI program on a file.
Returns the output in a string */
std::string Cgi::execute(const std::string &fileName)
{
    pid_t               pid;
    int                 status;
    int                 fd[2];
    char                buffer[4098];
    std::string		    content;
    std::string         method = this->_request->getEnv("REQUEST_METHOD");

    if (pipe(fd) == -1)
        throw CgiError();

     char               *arg[3] = {strdup(this->_program.c_str()),
                            strdup(fileName.c_str()), NULL};
                            
    if ((pid = fork()) == -1)
        throw CgiError();
    else if (pid == 0)
    {
        if (dup2(fd[0], STDIN_FILENO) == -1)
            throw CgiError();
        if (dup2(fd[1], STDOUT_FILENO) == -1)
            throw CgiError();
        close(fd[1]);
        if (execve(arg[0], arg, this->_env)== -1)
            throw CgiError();
        exit(EXIT_FAILURE);
    }
    if (method == "POST")
        write(fd[1], this->_request->getContent().c_str(), this->_contentLength);
    waitpid(-1, &status, 0);
    if (status != 0)
        throw CgiError();
    close(fd[1]);
    int n;
    while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
    {
        buffer[n] = 0;
        content += buffer;
    }
    close(fd[0]);
    this->_contentLength = content.size();
    size_t pos = content.find("\n\r\n");
    if (pos != std::string::npos)
        this->_contentLength -= pos + 3;

    // ##################################################################
    if (DEBUG)
    {
        std::cout << "Command : " << arg[0] << " " << arg[1] << std::endl;
        std::cout << "............ CGI ENVIRON ............." <<std::endl;
        int i = -1;
        while(this->_env[++i])
            std::cout << this->_env[i] << std::endl;
        std::cout << "......................................" <<std::endl;
    }
    // ##################################################################
    
    free(arg[0]); free(arg[1]);
    
    return content;
}


_END_NS_WEBSERV
