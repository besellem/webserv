/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/11/16 15:51:40 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

INLINE_NAMESPACE::WebServer	serv;

void	ft_sig(int sig)
{
	printf("\n\033[1;31m[%s]\033[0m\n", "SIGINT");
	if (sig == SIGINT)
		exit(0);
}

int	main(int ac, char **av, __unused char **env)
{
	
	signal(SIGINT, ft_sig);

	try
	{
		serv.parse((ac > 1) ? av[1] : DEFAULT_CONFIG_FILE);
	}
	catch (std::exception &e)
	{
		EXCEPT_ERROR(e);
		return EXIT_FAILURE;
	}

	serv.createServers();

	return EXIT_SUCCESS;
}
