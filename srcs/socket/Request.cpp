/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 23:44:26 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/03 17:59:38 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

_BEGIN_NS_WEBSERV

Request::Request(const Server *server) : _server(server) {}

Request::~Request() {}


/*
** Getters
*/

HttpHeader&			Request::getHeader(void)              { return this->_header ; }
const std::string&	Request::getContent(void)       const { return this->_content; }
const std::string&	Request::getConstructPath(void) const { return this->_constructPath; }
size_t				Request::getContentLength(void) const { return this->_content.size(); }
const Server*		Request::getServer(void)        const { return this->_server ; }

/* Find the location of the request */
const t_location*	Request::getLocation(void) const
{
	typedef std::vector<t_location *>	location_type;
	location_type::const_iterator		it;
	const location_type					loc = this->_server->locations();
	std::string							path;
	size_t								pos;
	
	path = ft_strcut(this->_header.uri, '?');
	
	// found a match with a location block
	while (1)
	{
		// looking for an exact match with path
		for (it = loc.begin(); it != loc.end(); ++it)
		{
			if (path == (*it)->path)
				return *it;
		}
		
		// no match found
		if (path == "/")
			return NULL;
			
		// get the longest prefixes
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

void	Request::setContent(void) {
	std::string buf(this->_header.buf);
	
	size_t pos = buf.find(DELIMITER);
	if (pos == std::string::npos)
		this->_content = "";
	else
		this->_content = buf.substr(pos + 4);
}

void	Request::setConstructPath(const std::string &path) {
	this->_constructPath = path;
}

void	Request::setConstructPath(void)
{
	// main paths
	std::string			ret;
	const t_location	*loc = this->getLocation();
	
	// tmp variables
	std::string							index_tmp;
	Server::tokens_type::const_iterator	idx; // iterator on indexes
	std::string							uriPath = ft_strcut(this->_header.uri, '?');
	
	ret = ROOT_PATH;
	if (loc)
	{
		std::cout << "location        : [" S_GREEN << loc->path << S_NONE "]" << std::endl;
		// add the root to the path
		if (!loc->root.empty() )
		{
			std::cout << "location root   : [" S_GREEN << loc->root << S_NONE "]" << std::endl;
			ret += loc->root;
			if (ret[ret.size() - 1] == '/')
				ret += uriPath.substr(uriPath.find_last_of('/') + 1);
			else
				ret += uriPath.substr(uriPath.find_last_of('/'));
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

void	Request::setHeaderData(const std::string& line_)
{
	typedef std::vector<std::string>                       vector_type;
	typedef std::map<std::string, std::string>             map_type;
	typedef std::pair<std::string, std::string>            pair_type;
	
	static std::string	method[] = {
		"GET",
		"POST",
		"DELETE"
	};
	static pair_type	option[] = {
		std::make_pair("Host",						" " ),
		std::make_pair("Origin",					""  ),
		std::make_pair("Content-Type",				""  ),
		std::make_pair("Accept-Encoding",			", "),
		std::make_pair("Connection",				" " ),
		std::make_pair("Upgrade-Insecure-Requests",	""  ),
		std::make_pair("Accept",					"," ),
		std::make_pair("Transfer-Encoding",         ""  ),
		std::make_pair("User-Agent",				""  ),
		std::make_pair("Referer",					" " ),
		std::make_pair("Accept-Language",			" " )
	};
	static vector_type	methods(method, method + sizeof(method) / sizeof(std::string));
	static map_type		options(option, option + sizeof(option) / sizeof(pair_type));
	static std::string	separator = ": ";
	const size_t		pos = line_.find(separator);
	pair_type			mapped;

	
	/* if we don't find a ": " in a line of the header */
	if (pos == std::string::npos)
	{
		// throw HttpHeader::HttpHeaderParsingError();
		return ;
	}
	
	/* set the key (eg: "Host") and its value (eg: "localhost:8080") */
	mapped = std::make_pair(line_.substr(0, pos), line_.substr(pos + separator.length()));

	/* Find the right key to add to the map */
	for (map_type::const_iterator opt_it = options.begin(); opt_it != options.end(); ++opt_it)
	{
		if (opt_it->first == mapped.first)
		{
			this->_header.data[mapped.first] = split_string(mapped.second, opt_it->second);
		}
	}
}

_END_NS_WEBSERV
