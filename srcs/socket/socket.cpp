/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/26 23:41:54 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "cgi.hpp"


_BEGIN_NS_WEBSERV

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"
#pragma clang diagnostic pop

Socket::Socket(void) :
	_port(0),
	_serverFd(SYSCALL_ERR),
	_addrLen(sizeof(sockaddr_in)),
	header()
{}

Socket::~Socket(void)
{}

Socket::Socket(const Server *serv) :
	_server_block(serv),
	_port(serv->port()),
	_addrLen(sizeof(sockaddr_in)),
	header()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (SYSCALL_ERR == _serverFd)
		errorExit("socket init");
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

	int	optval = 1;
	if (SYSCALL_ERR == setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) // can rebind
	{
		close(_serverFd);
		errorExit("set opt");
	}
	setNonBlock(_serverFd);
}

Socket::Socket(const Socket &x)
{ *this = x; }

Socket&		Socket::operator=(const Socket &x)
{
	if (this == &x)
		return *this;
	_server_block = x._server_block;
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
	typedef std::map<std::string, std::string>                 map_type;
	

	std::string					key = __line;
	std::string					value = __line;
	const size_t				pos = __line.find(": ");
	
	if (pos == std::string::npos) // (?) HTTP_REQUEST_ERROR;
	{
		// throw HttpHeader::HttpHeaderParsingError();
		return ;
	}

	vector_type					methods;
	vector_type::const_iterator	method_it;
	map_type					opts;
	map_type::const_iterator	opt_it;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	opts.insert(std::make_pair("Host",            " " ));
	opts.insert(std::make_pair("Connection",      " " ));
	opts.insert(std::make_pair("Accept",          "," ));
	opts.insert(std::make_pair("User-Agent",      ""  ));
	opts.insert(std::make_pair("Accept-Language", " " ));
	opts.insert(std::make_pair("Referer",         " " ));
	opts.insert(std::make_pair("Accept-Encoding", ", "));


	key = key.substr(0, pos); // get only the key (eg: "Host" or "User-Agent")
	value = value.substr(pos + 2);  // get only the value (eg: "localhost:8080")
	for (opt_it = opts.begin(); opt_it != opts.end(); ++opt_it)
	{
		if (opt_it->first == key)
		{
			header.data[key] = split_string(value, opt_it->second);
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

	std::cout << "Contructed Path -> [" S_CYAN << header.path_constructed << S_NONE "]\n";

	/* Parse the remaining buffer line by line */
	for ( ; line != buffer.end(); ++line)
	{
		this->checkHttpHeaderLine(*line);
	}

	// std::cout << S_RED "path_constructed: " S_NONE << header.path_constructed << std::endl;
}

ssize_t		Socket::getFileLength(const std::string& path)
{
	// check if the path is a file or directory
	struct stat		statBuf;

	stat(path.c_str(), &statBuf);
	if (S_ISDIR(statBuf.st_mode))
			return 0;
	
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
	std::string		content;
	ssize_t			content_length;

	// Content
	if (getExtension(header.path) == ".php")
	{
		try
		{
			// cgi cgi(*this, "/Users/adbenoit/.brew/bin/php");
			cgi cgi(*this, "/usr/local/bin/php-cgi");
			content = cgi.execute(header.path_constructed);
			content_length = cgi.getContentLength();
		}
		catch(const std::exception& e)
		{
			EXCEPT_WARNING;
		}
	}
	else
	{
		content = getFileContent(header.path_constructed);
		if (content.empty())
			content = generateAutoindexPage();
		content_length = content.size();
		content = "\n" + content;
	}
	
	// Header
	pair_type		status = getStatus(header.path_constructed);
	response =  HTTP_PROTOCOL_VERSION " ";
	response += std::to_string(status.first) + " ";
	response += status.second + NEW_LINE;
	response += "Content-Length: " + std::to_string(content_length);
	response += "\n";
	
	response += content;
	
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

std::string	Socket::generateAutoindexPage(void) const {
	DIR				*dir = opendir(ROOT_PATH);
	struct dirent	*dirInfo;
	struct stat		statBuf;

	std::string		addPrefix;

	std::string		fileName;
	std::string		lastModTime;
	std::string		fileSize;

	std::string		content;
	
	/* begin html */
	content = "<html>\n";
	content += "<head><title>autoindex</title></head>\n";
	content += "<body>\n";
	content += "<h1>Index of /</h1><hr/>\n";

	/* create table */
	content += "<table width=\"100%\" border=\"0\">\n";
	content += "<tr>\n";
	content += "<th align=\"left\">Name</th>\n";
	content += "<th align=\"left\">Last modified</th>\n";
	content += "<th align=\"left\">size</th>\n";
	content += "</tr>\n";

	if (dir == NULL) {
		std::cout << "open dir error!" << std::endl;
		return NULL;
	}

	/* create table content */
	while ((dirInfo = readdir(dir)) != NULL) {
		fileName = dirInfo->d_name;
		if (fileName == ".") {
			fileName.clear();
			continue;
		}

		/* get absolut path */
		addPrefix = ROOT_PATH;
		addPrefix += "/";
		if (fileName != "..")
			addPrefix += fileName;
		else
			addPrefix = fileName;

		/* get file status */
		stat(addPrefix.c_str(), &statBuf);

		/* get file modify time */
		lastModTime = ctime(&statBuf.st_mtime);
		lastModTime.erase(lastModTime.end() - 1);

		/* get file size */
		fileSize = std::to_string(statBuf.st_size);

		/* begin of content */
		content += "<tr>\n";

		/* element 1: path access */
		content += "<td><a href=\"";
		content += fileName;
		if (S_ISDIR(statBuf.st_mode))
			content += "/";
		content += "\">";
		content += fileName;
		if (S_ISDIR(statBuf.st_mode))
			content += "/";		
		content += "</a></td>\n";

		/* element 2: modify time */
		content += "<td>";
		content += lastModTime;
		content += "</td>";

		/* element 3: file size */
		content += "<td>";
		content += fileSize;
		content += "</td>";

		/* end of content */
		content += "</tr>\n";

		addPrefix.clear();
		fileName.clear();
		lastModTime.clear();
		fileSize.clear();
	}

	closedir(dir);

	/* end of html */
	content += "</table>\n";
	content += "</body>\n";
	content += "</html>\n";

	return content;
}

_END_NS_WEBSERV
