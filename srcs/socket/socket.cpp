/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/24 17:45:54 by besellem         ###   ########.fr       */
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
	_port(0),
	_serverFd(-1),
	_addrLen(sizeof(sockaddr_in))
{
	this->header = new HttpHeader;
}

Socket::~Socket(void)
{
	if (this->header != nullptr)
		delete this->header;
}

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
	
	this->header = new HttpHeader;
}

Socket::Socket(const Socket &x)
{
	*this = x;
}

Socket&		Socket::operator=(const Socket &x)
{
	if (this == &x)
		return *this;
	
	if (this->header != nullptr)
		delete this->header;
	_port = x._port;
	_serverFd = x._serverFd;
	_addrLen = x._addrLen;
	header = new HttpHeader(*(x.header));
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
	if (fcntl(fd, F_SETFL, O_NONBLOCK))
	{
		std::cout << "Error: set non block" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/* Read the socket's http request */
void	Socket::readHttpRequest(int socket_fd)
{
	int	ret;

	header->resetBuffer();
	ret = recv(socket_fd, header->buf, sizeof(header->buf), 0);
	
	if (!DEBUG)
	{
		std::cout << "++++++++++++++ REQUEST +++++++++++++++" << std::endl;
		write(STDOUT_FILENO, header->buf, ret); // (?) may be chunked
		std::cout << "++++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
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
		if (CMP_STRINGS(key.data(), g_options[i].token))
		{
			header->data[key] = split_string(value, g_options[i].delim);
		}
	}
}

void	Socket::resolveHttpRequest(void)
{
	typedef std::vector<std::string>     vector_type;
	
	vector_type				buffer = split_string(this->header->buf, "\n");
	vector_type::iterator	line = buffer.begin();


	/* parse first line of request */
	vector_type				first_line = split_string(*line, " ");
	if (first_line.size() != 3)
		throw HttpHeader::HttpBadRequestError();
	
	header->request_method = first_line[0];
	header->path_info = ROOT_PATH + first_line[1];
	++line;

	/* parse the buffer line by line */
	for ( ; line != buffer.end(); ++line)
	{
		this->checkHttpHeaderLine(*line);
	}


	// print data parsed
	HttpHeader::value_type::const_iterator	it = header->data.begin();
	HttpHeader::value_type::const_iterator	ite = header->data.end();

	for ( ; it != ite; ++it)
	{
		vector_type					tmp = it->second;
		vector_type::const_iterator	vec_it = tmp.begin();
		vector_type::const_iterator	vec_ite = tmp.end();
	
		std::cout << it->first << std::endl;
		for ( ; vec_it != vec_ite; ++vec_it)
		{
			std::cout << "    " << *vec_it << std::endl;
		}
	}
}

int		Socket::getStatusCode(void) const
{
	return 200;
}

const char *	Socket::getStatusMessage(int status_code) const
{
	if (200 == status_code)
		return "OK";
	return "";
}

size_t		Socket::getContentLength(void) const
{
	std::ifstream	ifs(header->path_info, std::ios::binary | std::ios::ate);

	if (ifs.is_open())
	{
		size_t	size = ifs.tellg();
		ifs.close();
		return size;
	}
	return 0; // may want to throw an error or something
}

std::string	Socket::getFileContent(void)
{
	std::string		content;
	std::ifstream	ifs(header->path_info, std::ios::in);

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

void		Socket::sendHttpResponse(int socket_fd)
{
	std::string			response;
	const std::string	path = ROOT_PATH + header->path_info;
	const int			status_code = getStatusCode();
	const std::string	status_message = getStatusMessage(status_code);
	const size_t		content_length = getContentLength();

	// header
	response =  "HTTP/1.1 ";
	response += std::to_string(status_code) + " ";
	response += status_message + NEW_LINE;

	response += "Content-Length: " + std::to_string(content_length);
	response += "\n\n";

	// content
	response += getFileContent();

	// -- Send --
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
	if (bind(serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		errorExit("bind step");
}

void	Socket::listenStep(const int& serverFd)
{
	if (listen(serverFd, 20) < 0)
		errorExit("listen step");
}

_END_NS_WEBSERV