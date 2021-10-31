/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 22:41:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/31 18:25:36 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "defs.hpp"
# include "cgi.hpp"
# include "Request.hpp"

_BEGIN_NS_WEBSERV


class Response
{
    public:
		typedef std::pair<int, std::string> status_type;

        Response(Request *);
        ~Response();

    private:
        Response();
        Response(const Response &);
        Response& operator=(const Response &);

    public:
        const std::string&  getHeader(void) const;
        const std::string&  getContent(void) const;
        const size_t&       getContentLenght(void) const;
        const status_type&  getStatus(void) const;
        
        const std::string   generateAutoindexPage(std::string const &) const;
    
        void		        setContent(const std::string &);
        void		        setHeader(void);
        void		        setStatus(const status_type &);
        void		        setStatus(int);
        void                setErrorContent(void);
        
    private:
        std::string         _header;
        std::string         _content;
        size_t              _contentLenght;
        status_type         _status;
        Request*            _request;
        Cgi*                _cgi;
};


_END_NS_WEBSERV

#endif
