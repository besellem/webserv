/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/27 15:45:16 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "cgi.hpp"


_BEGIN_NS_WEBSERV

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
	typedef std::vector<t_location *>                             location_type;

	// const
	const std::string		path = this->header.path;
	const location_type		loc = this->_server_block->locations();
	
	// main paths
	std::string		parent_dir = path.substr(0, path.find_last_of("/"));
	std::string		ret;
	
	// tmp variables
	std::string								root_tmp;
	std::string								index_tmp;
	location_type::const_iterator			it;  // iterator on locations
	Server::tokens_type::const_iterator		idx; // iterator on indexes


	std::cout << "parent_dir: [" S_GREEN << parent_dir << S_NONE "]" << std::endl;
	
	/* find the location based on the path requested */
	for (it = loc.begin(); it != loc.end(); ++it)
	{
		ret = ROOT_PATH "/";
		std::cout << "location path: [" S_GREEN << (*it)->path << S_NONE "]" << std::endl;
		if (parent_dir == (*it)->path)
		{
			root_tmp = (*it)->root;

			/* add root path if it exists */
			if (root_tmp.size() != 0)
			{
				// remove '/' from the root path if it exists
				if (root_tmp[root_tmp.size() - 1] == '/')
					ret += root_tmp.substr(0, root_tmp.size() - 1);
				else
					ret += root_tmp;
			}
			
			ret += path.substr(path.find_last_of("/"));
			
			/* if the requested page is a folder */
			if (ret[ret.size() - 1] == '/')
			{
				/* loop through indexes */
				for (idx = (*it)->index.begin(); idx != (*it)->index.end(); ++idx)
				{
					index_tmp = ret + *idx;
					if (is_valid_path(index_tmp))
					{
						ret = index_tmp;
						break ;
					}
				}
			}
			std::cout << "location root: [" S_GREEN << (*it)->root << S_NONE "]" << std::endl;
			return ret;
		}
	}

	/* default case */
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

	std::cout << "Path ->            [" S_CYAN << header.path << S_NONE "]\n";
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

std::string Socket::getErrorPage(pair_type status)
{
	std::map<int, std::string>::const_iterator it = this->_server_block->errorPages().find(status.first);
	std::cout << ">>>>>>> " << ROOT_PATH + std::string("/") + it->second << std::endl;
	if (it != this->_server_block->errorPages().end())
	{
		status = getStatus(ROOT_PATH + std::string("/") + it->second);
		if (status.first == 200)
			return getFileContent(ROOT_PATH + std::string("/") + it->second);
	}
	std::string content = "<html>\n";
	content += "  <head>\n";
	content += "    <meta charset=\"utf-8\">\n";
	content += "    <title>";
	content += std::to_string(status.first);
	content += " ";
	content += status.second;
	content += "</title>\n";
	content += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
	content += "  </head>\n";
	content += "<body>\n";
	content += "    <h1>";
	content += std::to_string(status.first);
	content += " ";
	content += status.second;
	content += "</h1>\n";
	// content += "    <p>Sorry, the page you're looking for doesn't exist.</p>\n";
	content += "</body>\n";
	content += "</html>\n";
	
	return content;
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
	LOG;
	ifs.close();
	return content;
}

// new one - in process
Socket::pair_type	Socket::getStatus(const std::string& path)
{
	if (is_valid_path(path))
		return std::make_pair<int, std::string>(200, "OK");
	else
		return std::make_pair<int, std::string>(404, "Not Found");
}

void		Socket::sendHttpResponse(int socket_fd)
{
	std::string		response;
	std::string		content;
	ssize_t			content_length;
	pair_type		status = getStatus(header.path_constructed);
	std::string		file_content;

	
	// Content
	if (status.first == 200 && getExtension(header.path) == ".php") // replace with location.cgi[0] + check if cgi exist
	{
		try
		{
			cgi cgi(*this, CGI_PROGRAM); // replace with location.cgi[1]
			file_content = cgi.execute(header.path_constructed);
			content_length = cgi.getContentLength();
		}
		catch(const std::exception& e)
		{
			EXCEPT_WARNING;
		}
	}
	else
	{
		if (status.first == 200)
			file_content = getFileContent(header.path_constructed);
		else
			file_content = getErrorPage(status);
		if (file_content.empty())
			file_content = generateAutoindexPage();
		content_length = file_content.size();
		file_content = "\n" + file_content;
	}
	
	// Header
	response =  HTTP_PROTOCOL_VERSION " ";
	response += std::to_string(status.first) + " ";
	response += status.second + NEW_LINE;
	response += "Content-Length: " + std::to_string(content_length);
	response += "\n";

	// Content
	response += file_content;

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
	DIR				*dir = opendir(header.path_constructed.c_str());
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


/* Returns the value of a cgi environment variables */
std::string	Socket::getCgiEnv(const std::string &varName)
{
    std::string envVar[] = {"SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "SCRIPT_NAME", "REMOTE_HOST", "REMOTE_ADDR", "CONTENT_LENGTH", "HTTP_ACCEPT",
        "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "HTTP_REFERER", "SERVER_SOFTWARE",
        "GATEWAY_INTERFACE", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS"};
    std::string str;
    size_t      pos;
    int         i = 0;
    
    for (;i < 16; i++)
        if (varName == envVar[i])
            break ;
    switch (i)
    {
    case 0:
        str = vectorJoin(this->header.data["Host"]);
        pos = str.find(":");
        if (pos == std::string::npos)
            return std::string("");
       return str.substr(pos + 1);
    case 1: return this->header.request_method;
    case 2: return this->header.path_constructed;
    case 3: return std::string(CGI_PROGRAM);
    case 4: return ft_strcut(vectorJoin(this->header.data["Host"]), ':');
    case 5: return vectorJoin(this->header.data["Host"]);
    case 6: return std::to_string(this->getFileLength(this->header.path_constructed));
    case 7: return vectorJoin(this->header.data["Accept"]);
    case 8: return vectorJoin(this->header.data["Accept-Language"]);
    case 9: return vectorJoin(this->header.data["User-Agent"]);
    case 10: return vectorJoin(this->header.data["Referer"]);
    case 11: return std::string(HTTP_PROTOCOL_VERSION);
    case 12: return std::string("CGI/1.1");
    case 13: return std::string("");
    case 14:
        pos = vectorJoin(this->header.data["Referer"]).find("?");
        if (pos == std::string::npos)
            return std::string("");
       return this->header.path.substr(pos);
    case 15: return std::string("CGI");
    default: return std::string();
    }
}


_END_NS_WEBSERV
