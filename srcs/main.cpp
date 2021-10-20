/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/10/20 06:47:31 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <stdio.h>

int	main(int ac, char **av)
{
	WebServer		serv;
	std::string		conf = (ac > 1) ? av[1] : DEFAULT_CONFIG_FILE;

	try
	{
		serv.parse(conf);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return EXIT_SUCCESS;
}
