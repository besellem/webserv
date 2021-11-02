/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:54:55 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/02 23:48:07 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

_BEGIN_NS_WEBSERV

Response::Response(Request *req) : _request(req), _location(req->getLocation()) {
    if (_location->cgi.first.empty())
        _cgi = new Cgi(req);
		
    else
        _cgi = NULL;
	setStatus(200);
}

Response::~Response() {
    delete _cgi;
}

/*
** Getters
*/

const std::string&  			Response::getHeader(void) const { return this->_header; }
const std::string&  			Response::getContent(void) const { return this->_content; }
const size_t&       			Response::getContentLength(void) const { return this->_contentLength; }
const Response::status_type&	Response::getStatus(void) const {return this->_status; }

/*
** Setters
*/

void    Response::setStatus(const status_type& status) {
	this->_status = status;
}

void    Response::setStatus(int code) {
	int 		codeTab[] = {200, 202, 300, 301, 302, 303, 304, 307, 308, 403, 404, 405, 500};
	std::string actionTab[] = {"OK", "Accepted", "Multiple Choice", "Moved Permanently",
			"Found", "See Other", "Not Modified", "Temporary Redirect", "Temporary Redirect",
			"Forbidden", "Not Found", "Method Not Allowed", "Internal Server Error"};
	int			i = 0;

	while (i < 13 && codeTab[i] != code)
		++i;
	std::cout << "code : " << code << " redirection : " << _location->redirection.first << std::endl;
	if (code == 404 && this->_location->redirection.first == 301)
	{
		this->_request->setConstructPath(ROOT_PATH + this->_location->redirection.second);
		i = 3;
	}
	this->_status = std::make_pair<int, std::string>(codeTab[i], actionTab[i]);
}

void    Response::setHeader(void)
{
    this->_header = HTTP_PROTOCOL_VERSION " ";
	this->_header += std::to_string(this->_status.first) + " ";
	this->_header += this->_status.second + NEW_LINE;
	this->_header += "Content-Length: " + std::to_string(this->_contentLength);
	if (this->_status.first >= 300 && this->_status.first <= 400)
	{
		this->_header += NEW_LINE;
		this->_header += "Location: ";
		this->_header += this->_request->getConstructPath().substr(sizeof(ROOT_PATH) - 1);
	}
}

bool	Response::isMethodAllowed(const std::string &method)
{
	if (method == "GET")
		return 1;

	if (this->_location)
	{
		for (size_t i = 0; i < this->_location->methods.size(); i++)
			if (method == this->_location->methods[i])
				return 1;
	}
	this->setStatus(405);
	return 0;
}

void    Response::setContent(const std::string &file_content)
{
	this->isMethodAllowed(this->_request->getHeader().request_method);
	
	const t_location	*loc = this->_location;
	
	// cgi case
	if (this->_status.first == 200 && this->_cgi
	&& getExtension(this->_request->getConstructPath()) == this->_cgi->getExtension())
	{
		try
		{
			this->_content = this->_cgi->execute();
			this->_contentLength = this->_cgi->getContentLength();
			return ;
		}
		catch(const std::exception& e)
		{
			this->setStatus(500);
			EXCEPT_WARNING(e);
		}
	}
	
	// Valid case
	if (this->_status.first < 400)
	{
		// Autoindex
		if (ft_isDirectory(this->_request->getConstructPath()))
		{
			if (loc && loc->autoindex == ON)
				this->_content  = generateAutoindexPage(this->_request->getConstructPath());
			else
				this->setStatus(403);
		}
		else
			this->_content = file_content;
		this->_contentLength = this->_content.size();
		this->_content = NEW_LINE + this->_content;
	}

	// Error case
	if (this->_status.first >= 400)
		this->setErrorContent();
}

void Response::setErrorContent(void)
{
	std::map<int, std::string>::const_iterator	it;
	it = this->_request->getServer()->errorPages().find(this->_status.first);
	
	// Default error page setup case
	if (it != this->_request->getServer()->errorPages().end() && is_valid_path(it->second))
	{
		this->_content = NEW_LINE + getFileContent(it->second);
		this->_contentLength = this->_content.size() - 2;
		return ;
	}
	
	// Default case
	std::string content = "<html>\n";
	content += "  <head>\n";
	content += "    <meta charset=\"utf-8\">\n";
	content += "    <title>";
	content += std::to_string(this->_status.first);
	content += " ";
	content += this->_status.second;
	content += "</title>\n";
	content += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
	content += "  </head>\n";
	content += "<body>\n";
	content += "    <h1>";
	content += std::to_string(this->_status.first);
	content += " ";
	content += this->_status.second;
	content += "</h1>\n";
	content += "</body>\n";
	content += "</html>\n";
	
	this->_content = NEW_LINE + content;
	this->_contentLength = content.size();
}

const std::string	Response::generateAutoindexPage(std::string const &path) const
{
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
		if (S_ISDIR(statBuf.st_mode))
			content += "_";
		else
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
