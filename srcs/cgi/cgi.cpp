/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/31 16:41:18 by adbenoit         ###   ########.fr       */
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
    std::string envVar[] = {"PATH_INFO", "REQUEST_METHOD", "PATH_TRANSLATED",
            "REDIRECT_STATUS", "QUERY_STRING", ""};
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
    int                 fdIn[2];
    int                 fdOut[2];
    char                buffer[4098];
    std::string		    content;
    std::string         method = this->_request->getEnv("REQUEST_METHOD");

    if (pipe(fdIn) == -1 || pipe(fdOut) == -1)
        throw CgiError();

     char   *arg[3] = {strdup(this->_program.c_str()), strdup(fileName.c_str()), NULL};
                            
    if ((pid = fork()) == -1)
        throw CgiError();
    else if (pid == 0)
    {
        // Modify standard input and output
        if (dup2(fdIn[0], STDIN_FILENO) == -1)
            throw CgiError();
        close(fdIn[0]); close(fdIn[1]);
        if (dup2(fdOut[1], STDOUT_FILENO) == -1)
            throw CgiError();
        close(fdOut[0]); close(fdOut[1]);
        
        // Execute the cgi program on the file
        if (execve(arg[0], arg, this->_env)== -1)
            throw CgiError();
        exit(EXIT_FAILURE);
    }
    close(fdOut[1]);
    
    // Send variables to the standard input of the program
    if (method == "POST")
        write(fdIn[1], this->_request->getContent().c_str(), this->_request->getContent().size());
        
    close(fdIn[1]); close(fdIn[0]);
    waitpid(-1, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
        throw CgiError();

    // Read the standard output of the program
    int n;
    while ((n = read(fdOut[0], buffer, sizeof(buffer))) > 0)
    {
        buffer[n] = 0;
        content += buffer;
    }
    
    close(fdOut[0]);
    
    this->_contentLength = content.size();
    size_t pos = content.find("\r\n\r\n");
    if (pos != std::string::npos)
        this->_contentLength -= pos + 4;

    // ##################################################################
    if (DEBUG)
    {
        std::cout << "Command : " << arg[0] << " " << arg[1] << std::endl;
        std::cout << "request content : " << this->_request->getContent() << std::endl;
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
