/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/01 20:43:24 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

Cgi::CgiError::CgiError() {}

const char*	Cgi::CgiError::what() const throw() {
	return "cgi failed";
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

/* Returns the value of a environment variables of the request */
const std::string	Cgi::getEnv(const std::string &varName)
{
    std::string envVar[] = {"SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "SCRIPT_NAME", "REMOTE_ADDR", "REMOTE_IDENT", "HTTP_ACCEPT",
        "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "HTTP_REFERER", "SERVER_PROTOCOL",
        "GATEWAY_INTERFACE", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS",
		"HTTP_ACCEPT_ENCODING", "HTTP_CONNECTION", "PATH_TRANSLATED", "REMOTE_USER",
		"CONTENT_LENGTH", "SCRIPT_FILENAME", "SERVER_SOFTWARE", "SERVER_NAME", "REQUEST_URI", ""};
    std::string str;
    size_t      pos;
    int         i = 0;
    
    for (; !envVar[i].empty(); i++)
        if (varName == envVar[i])
            break ;
    switch (i)
    {
    case 0:
        str = vectorJoin(_request->getHeader().data["Host"], ' ');
        pos = str.find(":");
        if (pos == std::string::npos)
            return std::string("");
       return str.substr(pos + 1);
    case 1: return _request->getHeader().request_method;
    case 2: return _request->getConstructPath().substr(sizeof(ROOT_PATH) - 1);
    case 3: return _request->getConstructPath();
    case 4: return std::string("127.0.0.1");
    case 5: return std::string("");
    case 6: return vectorJoin(_request->getHeader().data["Accept"], ',');
    case 7: return vectorJoin(_request->getHeader().data["Accept-Language"], ' ');
    case 8: return vectorJoin(_request->getHeader().data["User-Agent"], ' ');
    case 9: return vectorJoin(_request->getHeader().data["Referer"], ' ');
    case 10: return std::string(HTTP_PROTOCOL_VERSION);
    case 11: return std::string("CGI/1.1");
    case 12: return vectorJoin(_request->getHeader().data["Content-Type"], ' ');
    case 13: return _request->getHeader().queryString;
    case 14: return std::string("200");
    case 15: return vectorJoin(_request->getHeader().data["Accept-Encoding"], ' ');
    case 16: return vectorJoin(_request->getHeader().data["Connection"], ' ');
    case 17: return _request->getConstructPath();
    case 18: return std::string("");
    case 19: return std::to_string(_request->getContent().size());
    case 20: return _request->getConstructPath();
    case 21: return "Webserv/1.0";
    case 22: return vectorJoin(_request->getServer()->name(), ' '); //tous ou juste 1 ?
    case 23: return _request->getHeader().path;
    default: return std::string("");
    }
}

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
			"REDIRECT_STATUS", "QUERY_STRING", "SERVER_PROTOCOL", "SCRIPT_FILENAME",
			"CONTENT_TYPE", "CONTENT_LENGTH", "GATEWAY_INTERFACE", "SERVER_SOFTWARE", ""};
	size_t i = 0;
	size_t size = 0;
	
	while (!envVar[size].empty())
		++size;
	this->_env = (char **)malloc(sizeof(char *) * (size + 1));
	if (!this->_env)
		throw std::bad_alloc();
	for (; i < size; i++)
	{
		std::string str = envVar[i] + "=" + this->getEnv(envVar[i]);
		this->_env[i] = strdup(str.c_str());
		if (!this->_env[i])
			throw std::bad_alloc();
	}  
	this->_env[i] = 0;
}

/* Read the standard output of the program */
std::string	Cgi::getOuput(int fd)
{
	int			ret;
	char		buffer[4098];
	std::string	output;
	
	ret = 1;
	while (ret > 0)
	{
		ret = read(fd, buffer, sizeof(buffer) - 1);
		buffer[ret] = 0;
		output += buffer;
	}
	return output;
}

void	Cgi::setContentLength(const std::string &output) {
	this->_contentLength = output.size();
	
	// Subtract the size of the Cgi header
	size_t pos = output.find("\r\n\r\n"); // delimiter
	if (pos != std::string::npos)
		this->_contentLength -= pos + 4;
}

/* Executes the CGI program on a file.
Returns the output in a string */
std::string Cgi::execute(void)
{
	pid_t		pid;
	int			status = 0;
	int			fdIn[2];
	int			fdOut[2];
	std::string	content;
	std::string	method = this->getEnv("REQUEST_METHOD");

	if (pipe(fdIn) == SYSCALL_ERR || pipe(fdOut) == SYSCALL_ERR)
		throw CgiError();
		
	// Send variables to the standard input of the program
	if (write(fdIn[1], this->_request->getContent().c_str(), this->_request->getContent().size()) < 0)
		throw CgiError();

	if ((pid = fork()) == SYSCALL_ERR)
		throw CgiError();
	else if (pid == 0)
	{
		char * const * nll = NULL;

		// Modify standard input and output
		if (dup2(fdIn[0], STDIN_FILENO) == SYSCALL_ERR)
			exit(EXIT_FAILURE);
		close(fdIn[0]); 
		close(fdIn[1]);
		if (dup2(fdOut[1], STDOUT_FILENO) == SYSCALL_ERR)
			exit(EXIT_FAILURE);
		close(fdOut[0]);
		close(fdOut[1]);
		
		// Execute the cgi program on the file
		execve(this->_program.c_str(), nll, this->_env);
		exit(EXIT_FAILURE);
	}
	
	close(fdOut[1]);
	close(fdIn[0]);
	close(fdIn[1]);

	waitpid(-1, &status, 0);
	std::cout << "###################################\n";
	if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
		throw CgiError();

	content = this->getOuput(fdOut[0]);
	this->setContentLength(content);
	close(fdOut[0]);

	// ##################################################################
	if (DEBUG)
	{
		std::cout << "request content : " << this->_request->getContent() << std::endl;
		std::cout << "............ CGI ENVIRON ............." <<std::endl;
		int i = -1;
		while(this->_env[++i])
			std::cout << this->_env[i] << std::endl;
		std::cout << "......................................" <<std::endl;
	}
	// ##################################################################
	
	return content;
}


_END_NS_WEBSERV
