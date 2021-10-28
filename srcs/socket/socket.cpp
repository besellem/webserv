/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:04:47 by kaye              #+#    #+#             */
/*   Updated: 2021/10/28 19:19:26 by adbenoit         ###   ########.fr       */
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
	if (ret == sizeof(header.buf) || ret == -1)
		LOG;
	if (DEBUG)
	{
		std::cout << "++++++++++++++ REQUEST +++++++++++++++" << std::endl;
		write(STDOUT_FILENO, header.buf, ret); // (?) may be chunked
		std::cout << "++++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
	}
}

void	Socket::checkHttpHeaderLine(const std::string& line_)
{
	typedef std::map<std::string, std::string>                 map_type;
	

	vector_type							methods;
	map_type							opts;
	map_type::const_iterator			opt_it;
	std::pair<std::string, std::string>	mapped;
	const size_t						pos = line_.find(": ");
	
	// if we don't find a ": " in a line of the header
	if (pos == std::string::npos) // (?) HTTP_REQUEST_ERROR;
	{
		// throw HttpHeader::HttpHeaderParsingError();
		return ;
	}
	
	// set the key (eg: "Host") and its value (eg: "localhost:8080")
	mapped = std::make_pair(line_.substr(0, pos), line_.substr(pos + 2));

	// fill methods and options to parse
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	opts.insert(std::make_pair("Host",						" " ));
	opts.insert(std::make_pair("Origin",					""));
	opts.insert(std::make_pair("Content-Type",				""));
	opts.insert(std::make_pair("Accept-Encoding",			", "));
	opts.insert(std::make_pair("Connection",				" " ));
	opts.insert(std::make_pair("Upgrade-Insecure-Requests",	""));
	opts.insert(std::make_pair("Accept",					"," ));
	opts.insert(std::make_pair("User-Agent",				""  ));
	opts.insert(std::make_pair("Referer",					" " ));
	opts.insert(std::make_pair("Accept-Language",			" " ));

	for (opt_it = opts.begin(); opt_it != opts.end(); ++opt_it)
	{
		if (opt_it->first == mapped.first)
		{
			header.data[mapped.first] = split_string(mapped.second, opt_it->second);
		}
	}
}

t_location*	Socket::getLocation(const std::string &path)
{
	typedef std::vector<t_location *>	location_type;
	std::string							parent_dir;
	size_t								pos = path.find('/', 1);
	location_type::const_iterator		it;
	const location_type					loc = this->_server_block->locations();
	
	if (ft_isDirectory(ROOT_PATH + path))
		parent_dir = path;
	else if (pos == std::string::npos)
		parent_dir = "/";
	else
		parent_dir = path.substr(0, pos);
	parent_dir = ft_strcut(parent_dir, '?');
	for (it = loc.begin(); it != loc.end(); ++it)
	{
		if (parent_dir == (*it)->path)
			return *it;
	}
	return NULL;
}

/* Find the location based on the path requested */
std::string	Socket::constructPath(void)
{
	// main paths
	std::string			ret;
	const t_location	*loc = getLocation(this->header.path);
	
	// tmp variables
	std::string							root_tmp;
	std::string							index_tmp;
	Server::tokens_type::const_iterator	idx; // iterator on indexes
	
	ret = ROOT_PATH;
	if (loc)
	{
		std::cout << "location root: [" S_GREEN << loc->root << S_NONE "]" << std::endl;
		ret += loc->root;
		if (ret[ret.size() - 1] != '/')
			ret += "/";
		ret += this->header.path.substr(loc->path.size(), this->header.path.size());
		if (ft_isDirectory(ret))
		{
			/* loop through indexes */
			for (idx = loc->index.begin(); idx != loc->index.end(); ++idx)
			{
				index_tmp = ret + *idx;
				if (is_valid_path(index_tmp))
				{
					ret = index_tmp;
					break ;
				}
			}
		}
	}
	else
		ret += this->header.path;
	
	return ft_strcut(ret, '?');
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
	if (it != this->_server_block->errorPages().end())
	{
		status = getStatus(ROOT_PATH + std::string("/") + it->second);
		if (status.first == 200)
		{
			if (getExtension(ROOT_PATH + std::string("/") + it->second) == ".php") // replace with location.cgi[0] + check if cgi exist
			{
				try
				{
					cgi cgi(*this, CGI_PROGRAM); // replace with location.cgi[1]
					return cgi.execute(ROOT_PATH + std::string("/") + it->second);
				}
				catch(const std::exception& e)
				{
					EXCEPT_WARNING;
				}
			}
			return getFileContent(ROOT_PATH + std::string("/") + it->second);
		}
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
	std::string			response;
	std::string			content;
	ssize_t				content_length;
	pair_type			status = getStatus(header.path_constructed);
	std::string			file_content;
	const t_location	*loc = getLocation(this->header.path);

	
	if (DEBUG)
	{
		std::cout << "_______ LOCATION ______" << std::endl;
		if (!loc)
			std::cout << "Location " << this->header.path << " not found" << std::endl;
		else if (loc->cgi.empty())
			std::cout << "CGI not found" << std::endl;
		else
			std::cout << "cgi command: " << loc->cgi[0] << " " << loc->cgi[1] << std::endl;
		std::cout << "__________________________" << std::endl;
	}
	// Content
	if (status.first == 200 && loc && !loc->cgi.empty()
	&& getExtension(header.path_constructed) == loc->cgi[0])
	{
		try
		{
			cgi cgi(*this, loc->cgi[1]);
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
		if (status.first != 200)
			file_content = getErrorPage(status);
		else
			file_content = getFileContent(header.path_constructed);
		if (file_content.empty() && loc && loc->autoindex == ON)
			file_content = generateAutoindexPage(header.path_constructed);
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

std::string	Socket::generateAutoindexPage(std::string const & path) const {
	DIR				*dir = opendir(path.c_str());
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
		// return ""; // if return NULL, get a seg ...
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
		addPrefix = path;
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
        "GATEWAY_INTERFACE", "CONTENT_TYPE", "QUERY_STRING", "REDIRECT_STATUS",
		"HTTP_ACCEPT_ENCODING", "HTTP_CONNECTION"};
    std::string str;
    size_t      pos;
    int         i = 0;
    
    for (;i < 18; i++)
        if (varName == envVar[i])
            break ;
    switch (i)
    {
    case 0:
        str = vectorJoin(this->header.data["Host"], ' ');
        pos = str.find(":");
        if (pos == std::string::npos)
            return std::string("");
       return str.substr(pos + 1);
    case 1: return this->header.request_method;
    case 2: return this->header.path_constructed;
    case 3: return getLocation(this->header.path)->cgi[1];
    case 4: return ft_strcut(vectorJoin(this->header.data["Host"], ' '), ':');
    case 5: return vectorJoin(this->header.data["Host"], ' ');
    case 6: return std::to_string(this->getFileLength(this->header.path_constructed));
    case 7: return vectorJoin(this->header.data["Accept"], ',');
    case 8: return vectorJoin(this->header.data["Accept-Language"], ' ');
    case 9: return vectorJoin(this->header.data["User-Agent"], ' ');
    case 10: return vectorJoin(this->header.data["Referer"], ' ');
    case 11: return std::string(HTTP_PROTOCOL_VERSION);
    case 12: return std::string("CGI/1.1");
    case 13: return vectorJoin(this->header.data["Content-Type"], ' ');
    case 14:
        pos = this->header.path.find("?");
        if (pos == std::string::npos)
            return std::string("");
       return this->header.path.substr(pos + 1);
    case 15: return std::string("CGI");
    case 16: return vectorJoin(this->header.data["Accept-Encoding"], ' ');
    case 17: return vectorJoin(this->header.data["Connection"], ' ');
    default: return std::string();
    }
}


_END_NS_WEBSERV
