/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/10/25 16:49:09 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	main(int ac, char **av, __unused char **env)
{
	_INLINE_NAMESPACE::WebServer	serv;
	
	try
	{
		serv.parse((ac > 1) ? av[1] : DEFAULT_CONFIG_FILE);
	}
	catch (std::exception &e)
	{
		return (EXCEPT_ERROR), EXIT_FAILURE;
	}

	serv.createServers();
	
	return EXIT_SUCCESS;
}
