/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:46:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/11 16:12:14 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <errno.h>

_BEGIN_NS_WEBSERV

Cgi::CgiError::CgiError() {}

const char*	Cgi::CgiError::what() const throw() {
	return "cgi failed";
}

Cgi::Cgi(Request *request) : _request(request), _header(""), _status(200)
{
	const t_location	*loc = request->getLocation();

	if (loc && !loc->cgi.first.empty())
	{
		_extension = loc->cgi.first;
		_program = loc->cgi.second;
		this->setEnv();
	}
	else
		this->_env = NULL;
}

Cgi::~Cgi() {
	this->clear();
}

/*
** Getters
*/

const std::string&	Cgi::getExtension(void)		const { return this->_extension; }
const std::string&	Cgi::getProgram(void)		const { return this->_program; }
char**				Cgi::getEnv(void)			const { return this->_env; }
const int&			Cgi::getStatus(void)		const { return this->_status; }

std::string	Cgi::getHeaderData(const std::string &data) {
	std::vector<std::string> lines = split_string(this->_header, NEW_LINE);
	
	for (size_t i = 0 ; i < lines.size(); i++)
	{
		if (ft_strcut(lines[i], ':') == data)
			return lines[i].substr(data.size() + 2);
	}
	return std::string();
}

/* Returns the value of a cgi environment variables */
const std::string	Cgi::getEnv(const std::string &varName)
{
	std::string	envVar[] = {
		"CONTENT_LENGTH",
		"CONTENT_TYPE",
		"GATEWAY_INTERFACE",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"QUERY_STRING",
		"REDIRECT_STATUS",
		"REQUEST_METHOD",
		"SCRIPT_FILENAME",
		"SERVER_PROTOCOL",
		"SERVER_PORT",
		""
	};
	std::string	str;
	int			i = 0;
	
	for ( ; !envVar[i].empty(); ++i)
	{
		if (varName == envVar[i])
			break ;
	}
	switch (i)
	{
	case 0:
		if (_request->getHeader().request_method == "GET")
			return std::to_string(_request->getHeader().queryString.size());
		else
			return std::to_string(_request->getContent().size());
	case 1: return vectorJoin(_request->getHeader().data["Content-Type"], '\0');
	case 2: return "CGI/1.1";
	case 3: return _request->getConstructPath().substr(sizeof(ROOT_PATH) - 1);
	case 4: return _request->getConstructPath().substr(sizeof(ROOT_PATH) - 1);
	case 5: return _request->getHeader().queryString;
	case 6: return "200";
	case 7: return _request->getHeader().request_method;
	case 8: return _request->getConstructPath();
	case 9: return HTTP_PROTOCOL_VERSION;
	case 10: return std::to_string(_request->getServer()->port());
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
		for (int i = 0; this->_env[i]; ++i)
			free(this->_env[i]);
		free(this->_env);
		this->_env = 0;
	}
}

/* Modify the status according to the header */
void	Cgi::setStatus(void) {
	std::string status = this->getHeaderData("Status");
	if (!status.empty())
		std::stringstream(status) >> this->_status;
}

/* Set the CGI environment variables.
CGI Environment variables contain data about the transaction
between the client and the server. */
void Cgi::setEnv()
{
	std::string	envVar[] = {
		"CONTENT_LENGTH",
		"CONTENT_TYPE",
		"GATEWAY_INTERFACE",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"QUERY_STRING",
		"REDIRECT_STATUS",
		"REQUEST_METHOD",
		"SCRIPT_FILENAME",
		"SERVER_PROTOCOL",
		"SERVER_PORT",
		""
	};
	size_t	size = 0;
	size_t	i = 0;
	
	while (!envVar[size].empty())
		++size;
	this->_env = (char **)malloc(sizeof(char *) * (size + 1));
	if (!this->_env)
		throw std::bad_alloc();
	for ( ; i < size; ++i)
	{
		std::string str = envVar[i] + "=" + this->getEnv(envVar[i]);
		this->_env[i] = strdup(str.c_str());
		if (!this->_env[i])
			throw std::bad_alloc();
	}  
	this->_env[i] = NULL;
}

/* Read the standard output of the program */
std::string	Cgi::getOuput(int fd)
{
	int			ret;
	char		buffer[BUFFER_SIZE];
	std::string	output;
	
	if (fcntl(fd, F_SETFL, O_NONBLOCK))
		std::cout << "set get output non blocking failed" << std::endl;
	while ((ret = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[ret] = 0;
		output += buffer;
	}
	if (ret < 0) {
		std::cout << "CGI FAILED" << std::endl;
		throw CgiError();
	}
	return output;
}

/* Check if the cgi failed or timeout */
// void	Cgi::handleProcess(int pid, time_t beginTime) {

// 	int		status = 0;
// 	double	seconds;

// 	while ((seconds = difftime(time(NULL), beginTime)) < TIMEOUT)
// 	{
// 		if (waitpid(-1, &status, WNOHANG) == pid)
// 			break ;
// 		usleep(100);
// 	}

// 	if (seconds == TIMEOUT) {
// 		this->_status = 408;
// 		kill(pid, SIGKILL);
// 	}
// 	else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
// 		this->_status = 502;
// 	else
// 		return ;
	
// 	throw CgiError();
// }

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

	std::cout << "Executing cgi ..." << std::endl;
	if (pipe(fdIn) == SYSCALL_ERR || pipe(fdOut) == SYSCALL_ERR)
		throw CgiError();

	// Send variables to the standard input of the program
	if (write(fdIn[1], this->_request->getContent().c_str(), this->_request->getContent().size()) < 0)
		throw CgiError();

	// time_t beginTime = time(NULL);
	if ((pid = fork()) == SYSCALL_ERR)
		throw CgiError();
	else if (pid == 0)
	{
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
		execve(this->_program.c_str(), NULL, this->_env);
		exit(EXIT_FAILURE);
	}

	close(fdOut[1]);
	close(fdIn[0]);
	close(fdIn[1]);

	waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
		throw CgiError();

	content = this->getOuput(fdOut[0]);
	close(fdOut[0]);

	this->_header = content.substr(0, content.find(DELIMITER));
	
	// remove cgi header
	size_t pos = content.find(DELIMITER);
	if (std::string::npos != pos)
		content = content.substr(pos + 4);
	
	this->setStatus();
		
	// ##################################################################
	if (DEBUG)
	{
		std::cout << "request content :\n" << this->_request->getContent() << std::endl;
		std::cout << "............ CGI HEADER ............." <<std::endl;
		std::cout << this->_header << std::endl;
		std::cout << "............ CGI ENVIRON ............." <<std::endl;
		int i = -1;
		while(this->_env && this->_env[++i])
			std::cout << this->_env[i] << std::endl;
		std::cout << "......................................" <<std::endl;
	}
	// ##################################################################
	
	return content;
}

_END_NS_WEBSERV
