/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 23:44:26 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/01 14:28:03 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

_BEGIN_NS_WEBSERV

Request::Request(const Server *server) : _server(server) {}

Request::~Request() {}


/*
** Getters
*/

HttpHeader&         Request::getHeader(void) { return this->_header ; }
const std::string&  Request::getContent(void) const { return this->_content; }
const std::string&  Request::getConstructPath(void) const { return this->_constructPath; }
size_t       		Request::getContentLenght(void) const { return this->_content.size(); }
const Server*       Request::getServer(void) const { return this->_server ; }

/* Find the location of the request */
const t_location*   Request::getLocation(void) const
{
	typedef std::vector<t_location *>	location_type;
	std::string							parent_dir;
	location_type::const_iterator		it;
	const location_type					loc = this->_server->locations();
	
	parent_dir = ft_strcut(this->_header.path, '?');
	if (!ft_isDirectory(ROOT_PATH + parent_dir))
	{
		size_t	pos = parent_dir.find_last_of('/');
		if (pos == std::string::npos)
			parent_dir = "/";
		else
			parent_dir = parent_dir.substr(0, pos);
	}
	for (it = loc.begin(); it != loc.end(); ++it)
	{
		if (parent_dir == (*it)->path)
			return *it;
	}
	return NULL;
}

/*
** Setters
*/

void	Request::setContent(void) {
	std::string buf(this->_header.buf);
	
	size_t pos = buf.find("\r\n\r\n");
	if (pos == std::string::npos)
		this->_content = "";
	else
		this->_content = buf.substr(pos + 4, std::string::npos);
}


void	Request::setConstructPath(void)
{
	// main paths
	std::string			ret;
	const t_location	*loc = this->getLocation();
	
	// tmp variables
	std::string							index_tmp;
	Server::tokens_type::const_iterator	idx; // iterator on indexes
	
	ret = ROOT_PATH;
	if (loc)
	{
		std::cout << "location root   : [" S_GREEN << loc->root << S_NONE "]" << std::endl;
		ret += loc->root;
		ret += this->_header.path.substr(loc->path.size(), this->_header.path.size());

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
		ret += this->_header.path;
	}
	
	this->_constructPath = ft_strcut(ret, '?');
}

void	Request::setHeaderData(const std::string& line_)
{
	typedef std::map<std::string, std::string>  map_type;

	std::vector<std::string>			methods;
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
			this->_header.data[mapped.first] = split_string(mapped.second, opt_it->second);
		}
	}
}

_END_NS_WEBSERV
