/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/03 23:01:12 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/06 16:45:46 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

_BEGIN_NS_WEBSERV

Response::Response(Request *req) : _contentLength(0), _request(req), _location(req->getLocation()) {
    if (_location && !_location->cgi.first.empty())
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
	int 		codeTab[] = {200, 202, 204 , 300, 301, 302, 303, 304, 308, 403, 404, 405, 408, 413, 500};
	std::string actionTab[] = {"OK", "Accepted", "No Content", "Multiple Choice", "Moved Permanently",
			"Found", "See Other", "Not Modified", "Temporary Redirect", "Forbidden", "Not Found",
			"Method Not Allowed", "Request Timeout", "Request Entity Too Large", "Internal Server Error"};
	int			i = 0;

	if (this->_location && !this->_location->redirection.second.empty())
		code = is_valid_path(ROOT_PATH + this->_location->redirection.second) ?
			this->_location->redirection.first : 404;
	while (i < 15 && codeTab[i] != code)
		++i;
	this->_status = std::make_pair(codeTab[i], actionTab[i]);
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
		if (this->_cgi && !this->_cgi->getHeaderData("Location").empty())
			this->_header += this->_cgi->getHeaderData("Location");
		else
			this->_header += this->_location->redirection.second;
	}
}

bool	Response::isMethodAllowed(const std::string &method)
{
	// GET is always allowed
	if (method == "GET")
		return true;

	if (this->_location)
	{
		for (size_t i = 0; i < this->_location->methods.size(); i++)
		{
			if (method == this->_location->methods[i])
				return true;
		}
	}
	this->setStatus(405);
	return false;
}

void	Response::getMethod(const std::string &file_content) {
	 if (this->_status.first != 200)
	 	return ;
		 
	// Autoindex
	if (ft_isDirectory(this->_request->getConstructPath()))
	{
		if (this->_location && this->_location->autoindex == ON)
			this->_content  = generateAutoindexPage(this->_request->getConstructPath());
		else
			this->setStatus(403);
	}
	// Default case
	else
		this->_content = file_content;
	this->_contentLength = this->_content.size();
	this->_content = NEW_LINE + this->_content;
}

void	Response::postMethod(void) {
	 if (this->_status.first != 200)
	 	return ;
	// upload case
	if (this->_location && !this->_location->uploadStore.empty())
		if (uploadFile() == true)
			return ;
	
	this->_content = NEW_LINE + this->_request->getContent();
}

void	Response::deleteMethod(void) {
	 if (this->_status.first != 200)
	 	return ;
	
	if (std::remove(this->_request->getConstructPath().c_str()) != 0)
		this->setStatus(403);
}

void	Response::cgi(void) {
	
	 if (this->_status.first != 200)
	 	return ;
	
	try
	{
		this->_content = this->_cgi->execute();
		std::string status = this->_cgi->getHeaderData("Status").c_str();
		if (!status.empty())
			this->setStatus(atoi(status.c_str()));
		this->_contentLength = this->_cgi->getContentLength();
	}
	catch(const std::exception& e)
	{
		if (this->_cgi->timeout())
			this->setStatus(408);
		else
			this->setStatus(500);
		EXCEPT_WARNING(e);
	}
}

void    Response::setContent(const std::string &file_content)
{
	this->isMethodAllowed(this->_request->getHeader().request_method);
	
	// CGI case
	if (this->_cgi && getExtension(this->_request->getConstructPath()) == this->_cgi->getExtension())
		this->cgi();
	// GET case
	else if (this->_request->getHeader().request_method == "GET")
		this->getMethod(file_content);
	// POST case
	else if (this->_request->getHeader().request_method == "POST")
		this->postMethod();
	// DELETE case
	else if (this->_request->getHeader().request_method == "DELETE")
		this->deleteMethod();
		
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
	std::string content = "<!DOCTYPE html>\n";
	content += "<html lang=\"en\">\n";
	content += "<head>\n";
	content += "<meta charset=\"utf-8\" /><meta http-equiv=\"X-UA-Compatible\" ";
	content += "content=\"IE=edge\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n";
	content += "<title>";
	content += this->_status.second;
	content += " | ";
	content += std::to_string(this->_status.first);
	content += "</title>\n";
	content += "<style type=\"text/css\">";
    content += "body{margin:0}sub{bottom:-.25em}sup{top:-.5em}body,";
	content += "html{width:100%;height:100%;background-color:#21232a}";
	content += "body{color:#fff;text-align:center;text-shadow:0 2px 4px rgba(0,0,0,.5);padding:0;min-height:100%;";
	content += "-webkit-box-shadowinset 0 0 100px rgba(0,0,0,.8);box-shadow:inset 0 0 100px rgba(0,0,0,.8);display:";
	content += "table;font-family:\"Open Sans\",Arial,sans-serif}";
	content += "h1{font-family:inherit;font-weight:500;line-height:1.1;color:inherit;font-size:36px}";
	content += "h1 small{font-size:68%;font-weight:400;line-height:1;color:#777}";
	content += "a{text-decoration:none;color:#fff;font-size:inherit;border-bottom:dotted 1px #707070}";
	content += ".lead{color:silver;font-size:21px;line-height:1.4}";
	content += ".cover{display:table-cell;vertical-align:middle;padding:0 20px}";
	content += "</style>\n";
	content += "</head>\n";
	content += "<body>\n";
	content += "<body>\n";
	content += "<div class=\"cover\"><h1>";
	content += this->_status.second;
	content += " <small>";
	content += std::to_string(this->_status.first);
	content += "</small></h1></div>\n";
	content += "</body>\n";
	
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

bool	Response::uploadFile(void) {
	if (_request->parseFile() == true) {
		std::map<std::string, std::string> fileInfo = _request->getFileInfo();
		std::string const absolutePath = ROOT_PATH + this->_location->uploadStore;
		for (info_type::iterator it = fileInfo.begin(); it != fileInfo.end(); it++) {
			std::string toUploadPath = absolutePath + it->first;
			if (this->_request->getServer()->clientMaxBodySize() && getFileLength(toUploadPath) > this->_request->getServer()->clientMaxBodySize()) {
				this->setStatus(413);
				return false;
			}
			std::ofstream ofs(toUploadPath, std::ofstream::out);
			if (!ofs.is_open()) {
				this->setStatus(403);
				return false;
			}
			ofs << it->second;
			ofs.close();
		}
		return true;
	}
	return false;
}

_END_NS_WEBSERV
