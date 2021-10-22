/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/22 18:04:15 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"


_BEGIN_NS_WEBSERV

struct s_options
{
	char	*token;
	char	*delim;
};

const char	*g_methods[] = {
	"GET",
	"POST",
	"DELETE",
	NULL
};

const struct s_options	g_options[] = {
	{"Host",            " " },
	{"Connection",      " " },
	{"Accept",          "," },
	{"User-Agent",      ""  },
	{"Accept-Language", " " },
	{"Referer",         " " },
	{"Accept-Encoding", ", "},
	{NULL, NULL}
};


Socket::Socket(void)  {}
Socket::~Socket(void) {}

Socket::Socket(const short& port) :
	_port(port),
	_addrLen(sizeof(sockaddr_in))
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		errorExit("socket init");

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(port);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));
}

/** @brief public function */

short		Socket::getPort(void)       const { return _port; }
int			Socket::getServerFd(void)   const { return _serverFd; }
sockaddr_in	Socket::getAddr(void)       const { return _addr; }
size_t		Socket::getAddrLen(void)    const { return _addrLen; }
// HttpHeader&	Socket::getHttpHeader(void) const { return _header; };

void	Socket::startUp(void)
{
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

void	Socket::_parse_wrapper(const char* http_header)
{
	// std::map<std::string, std::string>
	std::string		head(http_header);
	std::string		content;

	do
	{
		size_t pos = head.find('\n');
		if (pos == std::string::npos)
			break ;
		// head.substr(0, );
	} while (true);
}

// -- in process --
void	Socket::parse(int skt, const char* http_header)
{
	getParsing(skt, http_header);
}


#include <stdio.h> // printf
void	Socket::readHttpRequest(int socket_fd)
{
	int		ret;

	header.resetBuffer();
	ret = recv(socket_fd, header.buf, sizeof(header.buf), 0);
	
	if (!DEBUG)
	{
		write(STDOUT_FILENO, header.buf, ret);
		// printf("%s\n", header.buf);
	}
}

void	Socket::checkHttpHeaderLine(const std::string& __line)
{
	typedef std::vector<std::string>                                vector_type;
	
	vector_type					methods;
	vector_type					opts;
	vector_type::const_iterator	opt_it;
	vector_type::const_iterator	method_it;
	
	std::string					key = __line;
	std::string					value = __line;
	const size_t				pos = __line.find(": ");
	
	
	if (pos == std::string::npos) // (?) HTTP_REQUEST_ERROR;
		throw HttpHeader::HttpHeaderParsingError();
	
	key.substr(0, pos); // get only the key (eg: "Host" or "User-Agent")
	value.substr(pos);  // get only the value (eg: "localhost:8080")
	

	for (size_t i = 0; g_options[i].token; ++i)
	{
		if (CMP_STRINGS(key.data(), g_options[i].token))
		{
			header.data[key] = split_string(value, g_options[i].delim);
		}
	}
}

void	Socket::resolveHttpRequest(void)
{
	typedef std::vector<std::string>     vector_type;
	
	vector_type				buffer = split_string(this->header.buf, "\n");
	vector_type::iterator	line = buffer.begin();

	// parse the buffer line by line
	for ( ; line != buffer.end(); ++line)
	{
		this->checkHttpHeaderLine(*line);
	}
}

// TO REMOVE
void	Socket::parse(int skt, const std::string& path)
{
	getParsing(skt, path);
}

/** @brief private function */

void	Socket::errorExit(const std::string& str) const
{
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	Socket::bindStep(const int& serverFd, const sockaddr_in& addr)
{
	if (bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		errorExit("bind step");
}

void	Socket::listenStep(const int& serverFd)
{
	if (listen(serverFd, 20) < 0)
		errorExit("listen step");
}

// -- in process --
void	Socket::getParsing(int skt, const char* http_header)
{
	std::string		content = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";

	std::ifstream	ifs(http_header, std::ios::binary | std::ios::ate);
	if (ifs.is_open())
	{
		int	fileSize = ifs.tellg();
		ifs.close();
		content += std::to_string(fileSize);
		content += "\n\n";
	}
	else
		std::cout << "open file error: for found size of file" << std::endl;

	ifs.open(http_header, std::ios::in);
	std::string	gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof() == true)
				break ;
			content += "\n";
		} while (true);
		send(skt, content.c_str(), content.length(), 0);
	}
	else
		std::cout << "open file error: for get content";
	ifs.close();
}

// TO REMOVE
void	Socket::getParsing(int skt, const std::string& path)
{
	std::string		content = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";

	std::ifstream	ifs(path, std::ios::binary | std::ios::ate);
	if (ifs.is_open())
	{
		int	fileSize = ifs.tellg();
		ifs.close();
		content += std::to_string(fileSize);
		content += "\n\n";
	}
	else
		std::cout << "open file error: for found size of file" << std::endl;

	ifs.open(path, std::ios::in);
	std::string	gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof() == true)
				break ;
			content += "\n";
		} while (true);
		send(skt, content.c_str(), content.length(), 0);
	}
	else
		std::cout << "open file error: for get content";
	ifs.close();
}


_END_NS_WEBSERV
