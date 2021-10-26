/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:49:10 by besellem          #+#    #+#             */
/*   Updated: 2021/10/26 23:59:30 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "defs.hpp"

_BEGIN_NS_WEBSERV

std::vector<std::string>	split_string(const std::string& s, const std::string& delim);
std::string	                getExtension(const std::string& fileName);
std::string                 ft_strcut(const std::string& str, char delimiter);
std::vector<std::string>	ft_vectorcut(const std::vector<std::string>& vect, char delimiter);
std::string                 vectorJoin(const std::vector<std::string> &vect);

_END_NS_WEBSERV

#endif /* !defined(UTILS_HPP) */
