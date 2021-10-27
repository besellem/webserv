/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 17:49:10 by besellem          #+#    #+#             */
/*   Updated: 2021/10/27 13:25:16 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "defs.hpp"

_BEGIN_NS_WEBSERV


std::vector<std::string>	split_string(const std::string& s, const std::string& delim);
bool						is_valid_path(const std::string& path);


_END_NS_WEBSERV

#endif /* !defined(UTILS_HPP) */
