/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 23:44:26 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/12 17:57:28 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

_BEGIN_NS_WEBSERV

Request::Request(void) :
	_isChunked(false)
{}

Request::Request(const Request &x)
{ *this = x; }

Request::~Request()
{}

Request&			Request::operator=(const Request &x)
{
	if (this == &x)
		return *this;
	_header = x._header;
	_constructPath = x._constructPath;
	_content = x._content;
	_server = x._server;
	_isChunked = x._isChunked;
	_fileInfo = x._fileInfo;
	_boundary = x._boundary;
	return *this;
}

/*
** Getters
*/
HttpHeader&			Request::getHeader(void)              { return this->_header ; }
const std::string&	Request::getContent(void)       const { return this->_content; }
const std::string&	Request::getConstructPath(void) const { return this->_constructPath; }
size_t				Request::getContentLength(void) const { return this->_content.size(); }
const Server*		Request::getServer(void)        const { return this->_server; }

/* Find the location of the request */
const t_location*	Request::getLocation(void) const
{
	const location_type		loc = this->_server->locations();
	std::string				path = ft_strcut(this->_header.uri, '?');
	size_t					pos;

	/* find a match with a location block */
	while (true)
	{
		/* looking for an exact match with path */
		for (location_type::const_iterator it = loc.begin(); it != loc.end(); ++it)
		{
			if (path == (*it)->path)
				return *it;
		}
		
		/* no match found */
		if (path == "/")
			return NULL;
		
		/* get the longest prefixes */
		if (path.find_last_of('/') == 0)
			path = "/";
		else
		{
			pos = path.substr(0, path.size() - 1).find_last_of('/');
			path = path.substr(0, pos + 1);
		}
	}
	return NULL;
}

/*
** Setters
*/
bool	Request::setRequestFirstLine(const std::string &first_line)
{
	const vector_type	line = split_string(first_line, " ");

	if (line.size() != 3)
		return false;
	
	this->getHeader().request_method = line[0];
	this->getHeader().uri = line[1];
	return true;
}

void	Request::setServer(const Server *server) { this->_server = server; }

void	Request::setContent(void)
{
	std::string					buf(this->_header.content);
	const std::string			delim(DELIMITER);
	const size_t				pos = buf.find(delim);
	
	vector_type					content_;
	vector_type::const_iterator	it;
	std::string					tmp_string = "";

	if (pos == std::string::npos)
		this->_content = "";
	else
	{
		buf = buf.substr(pos + delim.length());
		if (this->_header.chunked)
		{
			content_ = split_string(buf, NEW_LINE);
			it = content_.begin();
			if (it != content_.end() && (it + 1) != content_.end())
			{
				++it;
				for ( ; it != content_.end(); it += 2)
				{
					if ((it + 1) == content_.end())
						break ;
					tmp_string += *it;
				}
			}
			else
				tmp_string = buf;
		}
		else
			tmp_string = buf;
		
		this->_content = tmp_string;
	}
	std::cout << S_GREEN "> CONTENT" S_NONE << std::endl;
	std::cout << "[" << this->_content << "]" << std::endl;
	std::cout << S_GREEN "< END CONTENT" S_NONE << std::endl;
}

void	Request::setConstructPath(void)
{
	std::string			ret;
	const t_location	*loc = this->getLocation();
	std::string			uriPath = ft_strcut(this->_header.uri, '?');
	
	// tmp variables
	std::string							index_tmp;
	Server::tokens_type::const_iterator	idx; // iterator on indexes
	
	ret = ROOT_PATH;
	if (loc != NULL)
	{
		std::cout << "location        : [" S_GREEN << loc->path << S_NONE "]" << std::endl;
		// add the root to the path
		if (!loc->root.empty())
		{
			std::cout << "location root   : [" S_GREEN << loc->root << S_NONE "]" << std::endl;
			ret += loc->root;
			if (ret[ret.size() - 1] != '/')
				ret += uriPath.substr(loc->path.size() - 1);
			else
				ret += uriPath.substr(loc->path.size());
			// if (ret[ret.size() - 1] == '/')
			// 	ret += uriPath.substr(uriPath.find_last_of('/') + 1);
			// else
			// 	ret += uriPath.substr(uriPath.find_last_of('/'));
		}
		else
			ret += uriPath;

		// use default index
		if (ft_isDirectory(ret))
		{
			if (ret[ret.size() - 1] != '/')
				ret += "/";
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
	{
		std::cout << "location root   : [" S_RED << "unknow" << S_NONE "]" << std::endl;
		ret += uriPath;
	}
	
	this->_constructPath = ret;
}

void	Request::setConstructPath(const std::string &path)
{
	this->_constructPath = path;
}

void	Request::setHeaderData(const std::string& line_)
{
	static pair_type	option[] = {
		std::make_pair("Host",						" " ),
		std::make_pair("Origin",					""  ),
		std::make_pair("Content-Type",				""  ),
		std::make_pair("Accept-Encoding",			", "),
		std::make_pair("Connection",				" " ),
		std::make_pair("Upgrade-Insecure-Requests",	""  ),
		std::make_pair("Accept",					"," ),
		std::make_pair("Transfer-Encoding",         ", "),
		std::make_pair("User-Agent",				""  ),
		std::make_pair("Referer",					" " ),
		std::make_pair("Accept-Language",			" " )
	};
	static info_type	options(option, option + sizeof(option) / sizeof(pair_type));
	static std::string	separator = ": ";
	const size_t		pos = line_.find(separator);
	pair_type			mapped;
	
	/* if we don't find a ": " in a line of the header */
	if (pos == std::string::npos)
		return ;
	
	/* set the key (eg: "Host") and its value (eg: "localhost:8080") */
	mapped = std::make_pair(line_.substr(0, pos), line_.substr(pos + separator.length()));

	/* Find the right key to add to the map */
	for (info_type::const_iterator opt_it = options.begin(); opt_it != options.end(); ++opt_it)
	{
		if (opt_it->first == mapped.first)
		{
			this->_header.data[mapped.first] = split_string(mapped.second, opt_it->second);
		}
	}
}

void	Request::setChunked(void)
{
	const vector_type			transfert_encoding = _header.data["Transfer-Encoding"];
	vector_type::const_iterator	it = transfert_encoding.begin();

	_header.chunked = false;
	for ( ; it != transfert_encoding.end(); ++it)
	{
		if (*it == "chunked")
		{
			if (DEBUG)
			{
				std::cout << S_GREEN "Chunked request found" S_NONE << std::endl;
			}
			_header.chunked = true;
			break ;
		}
	}
}

const Request::info_type&	Request::getFileInfo(void) const { return this->_fileInfo; }
const std::string&			Request::getBoundary(void) const { return this->_boundary; }

bool	Request::checkIsUploadCase(void) {
	std::string	toFind[] = {"POST", "multipart/form-data", "boundary", "filename", ""};
	std::string	headerBuf(_header.content);
	size_t		pos;

	for (int i = 0; toFind[i].empty() == false; i++) {
		if ((pos = headerBuf.find(toFind[i])) == std::string::npos)
			return false;
	}

	std::string boundary = "boundary=";
	_boundary = headerBuf;
	if ((pos = _boundary.find(boundary)) != std::string::npos) {
		_boundary.erase(0, pos + boundary.length());
		if ((pos = _boundary.find(NEW_LINE)) != std::string::npos) {
			_boundary.erase(pos);
			_boundary.insert(0, "--");
		}
		else
			return false;
	}
	else
		return false;
	return true;
}

bool	Request::parseFile(void) {
	if (this->checkIsUploadCase() == false) {
		return false;
	}

	std::string toParse = this->getContent();
	std::string key[] = {"filename=\"", "\"", "Content-Type", NEW_LINE, DELIMITER, this->getBoundary() + "--" NEW_LINE};

	std::string fileName;
	std::string fileContent;

	size_t begin;
	size_t end;

	while (true) {
		// get file name
		if ((begin = toParse.find(key[FN])) != std::string::npos)
			toParse.erase(0, begin + key[FN].length());
		if ((end = toParse.find(key[DQ])) != std::string::npos)
			fileName = toParse.substr(0, end);

		// get file content;
		if ((begin = toParse.find(key[CT])) != std::string::npos)
			toParse.erase(0, begin + key[CT].length());
		if ((begin = toParse.find(key[DRN])) != std::string::npos)
			toParse.erase(0, begin + key[DRN].length());
		if ((end = toParse.find(key[RN])) != std::string::npos)
			fileContent = toParse.substr(0, end);

		// add to list of file info
		if(!fileName.empty())
			_fileInfo.insert(std::make_pair(fileName, fileContent));
		toParse.erase(0, fileContent.length() + key[RN].length());
		if (toParse == key[LAST_BOUNDARY])
			break ;
		else
			toParse.erase(0, this->getBoundary().length() + key[RN].length());
	}

	// for (std::map<std::string, std::string>::iterator it = _fileInfo.begin(); it != _fileInfo.end(); it++)
	// 	std::cout << "[" << it->first << "]: [" << it->second << "]" << std::endl;
	return true;
}

_END_NS_WEBSERV
