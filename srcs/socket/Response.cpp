/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:54:55 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/31 18:18:39 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

_BEGIN_NS_WEBSERV

Response::Response(Request *req) : _request(req) {
    if (req->getLocation() && !req->getLocation()->cgi.empty())
        _cgi = new Cgi(req);
    else
        _cgi = NULL;
}

Response::~Response() {
    delete _cgi;
}

/*
** Getters
*/

const std::string&  			Response::getHeader(void) const { return this->_header; }
const std::string&  			Response::getContent(void) const { return this->_content; }
const size_t&       			Response::getContentLenght(void) const { return this->_contentLenght; }
const Response::status_type&	Response::getStatus(void) const {return this->_status; }

/*
** Setters
*/

void    Response::setStatus(const status_type& status) {
	this->_status = status;
}

void    Response::setStatus(int code) {
	int 		codeTab[] = {200, 404, 500};
	std::string actionTab[] = {"OK", "Not Found", "Internal Server Error"};
	int			i = 0;

	while (i < 3 && codeTab[i] != code)
		++i;
	this->_status = std::make_pair<int, std::string>(codeTab[i], actionTab[i]);
}

void    Response::setHeader(void)
{
    this->_header = HTTP_PROTOCOL_VERSION " ";
	this->_header += std::to_string(this->_status.first) + " ";
	this->_header += this->_status.second + NEW_LINE;
	this->_header += "Content-Length: " + std::to_string(this->_contentLenght);
}

void    Response::setContent(const std::string &file_content)
{
	
	const t_location	*loc = this->_request->getLocation();

	// cgi case
	if (this->_status.first == 200 && this->_cgi
	&& getExtension(this->_request->getConstructPath()) == this->_cgi->getExtension())
	{
		try
		{
			this->_content = this->_cgi->execute(this->_request->getConstructPath());
			this->_contentLenght = this->_cgi->getContentLength();
			return ;
		}
		catch(const std::exception& e)
		{
			this->setStatus(500);
			EXCEPT_WARNING;
		}
	}
	
	// Error case
	if (this->_status.first != 200)
	{
		this->setErrorContent();
		return ;
	}

	// Default case
	this->_content = file_content;
	if (this->_content.empty() && loc && loc->autoindex == ON)
		this->_content = generateAutoindexPage(this->_request->getConstructPath());
	this->_contentLenght = this->_content.size();
	this->_content = "\n" + this->_content;
}

void Response::setErrorContent(void)
{
	std::map<int, std::string>::const_iterator	it;
	it = this->_request->getServer()->errorPages().find(this->_status.first);

	// Default error page setup case
	if (it != this->_request->getServer()->errorPages().end() && is_valid_path(it->second))
	{
		// cgi
		if (this->_cgi && getExtension(it->second) == this->_cgi->getExtension())
		{
			try
			{
				this->_content = this->_cgi->execute(ROOT_PATH + std::string("/") + it->second);
				this->_contentLenght = this->_cgi->getContentLength();
				return ;
			}
			catch(const std::exception& e)
			{
				EXCEPT_WARNING;
			}
		}

		this->_content = "\n" + getFileContent(it->second);
		this->_contentLenght = this->_content.size() - 1;
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
	
	this->_content = "\n" + content;
	this->_contentLenght = content.size();
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
