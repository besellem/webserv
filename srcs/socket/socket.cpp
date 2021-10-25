/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/25 17:56:42 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"


_BEGIN_NS_WEBSERV

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

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

#pragma clang diagnostic pop


Socket::Socket(void) :
	// _server_block(nullptr),
	_port(0),
	_serverFd(SYSCALL_ERR),
	_addrLen(sizeof(sockaddr_in)),
	header()
{}

Socket::~Socket(void)
{}

// Socket::Socket(Server *serv) :
Socket::Socket(short const & port) :
	// _server_block(serv),
	// _port(serv->port()),
	_port(port),
	_addrLen(sizeof(sockaddr_in)),
	header()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (SYSCALL_ERR == _serverFd)
		errorExit("socket init");
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = _port;
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

	int	optval = 1;
	if (SYSCALL_ERR == setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) // can rebind
	{
		close(_serverFd);
		errorExit("set opt");
	}
	// setNonBlock(_serverFd);
}

Socket::Socket(const Socket &x)
{ *this = x; }

Socket&		Socket::operator=(const Socket &x)
{
	if (this == &x)
		return *this;
	// _server_block = x._server_block;
	_port = x._port;
	_serverFd = x._serverFd;
	_addrLen = x._addrLen;
	header = x.header;
	memcpy(&_addr, &x._addr, sizeof(sockaddr_in));
	return *this;
}

/** @brief public function */

short		Socket::getPort(void)     const { return _port; }
int			Socket::getServerFd(void) const { return _serverFd; }
sockaddr_in	Socket::getAddr(void)     const { return _addr; }
size_t		Socket::getAddrLen(void)  const { return _addrLen; }

void	Socket::startSocket(void)
{
	bindStep(_serverFd, _addr);
	listenStep(_serverFd);
}

void	Socket::setNonBlock(int & fd)
{
	if (SYSCALL_ERR == fcntl(fd, F_SETFL, O_NONBLOCK))
	{
		std::cout << "Error: set non block" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/* Read the socket's http request */
void	Socket::readHttpRequest(int socket_fd)
{
	int	ret;

	header.resetBuffer();
	ret = recv(socket_fd, header.buf, sizeof(header.buf), 0);
	
	if (DEBUG)
	{
		std::cout << "++++++++++++++ REQUEST +++++++++++++++" << std::endl;
		write(STDOUT_FILENO, header.buf, ret); // (?) may be chunked
		std::cout << "++++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
	}
}

void	Socket::checkHttpHeaderLine(const std::string& __line)
{	
	vector_type					methods;
	vector_type					opts;
	vector_type::const_iterator	opt_it;
	vector_type::const_iterator	method_it;
	
	std::string					key = __line;
	std::string					value = __line;
	const size_t				pos = __line.find_first_of(": ");
	
	
	if (pos == std::string::npos) // (?) HTTP_REQUEST_ERROR;
	{
		// throw HttpHeader::HttpHeaderParsingError();
		return ;
	}
	
	key.substr(0, pos); // get only the key (eg: "Host" or "User-Agent")
	value.substr(pos);  // get only the value (eg: "localhost:8080")
	

	for (size_t i = 0; g_options[i].token; ++i)
	{
		// for (size_t j = 0; g_methods[j].token; ++j)
		// {
			
		// }
		if (CMP_STRINGS(key.c_str(), g_options[i].token))
		{
			header.data[key] = split_string(value, g_options[i].delim);
		}
	}
}

std::string	Socket::constructPath(void) const
{
	// const std::string	path = this->header.path;
	// std::string			parent_dir = path.substr(0, path.find_last_of("/"));
	// std::string			real;

	// if (parent_dir == )

	// ROOT_PATH + 
	
	// return real;
	return ROOT_PATH + this->header.path;
}

void	Socket::resolveHttpRequest(void)
{
	vector_type				buffer = split_string(this->header.buf, "\n");
	vector_type::iterator	line = buffer.begin();

	/*
	** Parse first line of request. Must be formatted like so:
	**   GET /index.html HTTP/1.1
	*/
	vector_type				first_line = split_string(*line, " ");
	if (first_line.size() != 3)
		throw HttpHeader::HttpBadRequestError();	
	header.request_method = first_line[0];
	header.path = first_line[1];
	header.path_constructed = constructPath();
	++line;

	/* Parse the remaining buffer line by line */
	for ( ; line != buffer.end(); ++line)
	{
		this->checkHttpHeaderLine(*line);
	}

	// std::cout << S_RED "path_constructed: " S_NONE << header.path_constructed << std::endl;
}

ssize_t		Socket::getFileLength(const std::string& path)
{
	std::ifstream	ifs(path, std::ios::binary | std::ios::ate);

	if (ifs.is_open())
	{
		ssize_t	size = ifs.tellg();
		ifs.close();
		return size;
	}
	return SYSCALL_ERR; // may want to throw an error or something
}

std::string	Socket::getFileContent(const std::string& path)
{
	std::string		content;
	std::ifstream	ifs(path, std::ios::in);

	std::string		gline;
	if (ifs.is_open())
	{
		do
		{
			std::getline(ifs, gline);
			content += gline;
			if (ifs.eof())
				break ;
			content += "\n";
		} while (true);
	}
	else
		std::cout << "open file error: for get content" << std::endl;
	ifs.close();
	return content;
}

Socket::pair_type	Socket::getStatus(void) const
{
	return std::make_pair<int, std::string>(200, "OK");
}

// new one - in process
Socket::pair_type	Socket::getStatus(__unused const std::string& path) const
{
	return std::make_pair<int, std::string>(200, "OK");
}

void		Socket::sendHttpResponse(int socket_fd)
{
	std::string		response;
	pair_type		status = getStatus(header.path_constructed);
	const ssize_t	content_length = getFileLength(header.path_constructed);

	// Header
	response =  HTTP_PROTOCOL_VERSION " ";
	response += std::to_string(status.first) + " ";
	response += status.second + NEW_LINE;

	response += "Content-Length: " + std::to_string(content_length);
	response += "\n\n";

	// Content
	response += getFileContent(header.path_constructed);

	// -- Send to client --
	send(socket_fd, response.c_str(), response.length(), 0);

	if (DEBUG)
	{
		std::cout << "------------- RESPONSE ---------------" << std::endl;
		std::cout << response.c_str() << std::endl;
		std::cout << "--------------------------------------" << std::endl << std::endl;
	}
}

/** @brief private function */

void	Socket::errorExit(const std::string& str) const
{
	std::cout << "Exit: " << str << std::endl;
	exit(EXIT_FAILURE);
}

void	Socket::bindStep(const int& serverFd, const sockaddr_in& addr)
{
	if (SYSCALL_ERR == bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)))
		errorExit("bind step");
}

void	Socket::listenStep(const int& serverFd)
{
	if (SYSCALL_ERR == listen(serverFd, SOMAXCONN))
		errorExit("listen step");
}

_END_NS_WEBSERV
