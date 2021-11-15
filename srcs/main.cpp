/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/11/02 15:33:17 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

INLINE_NAMESPACE::WebServer	serv;

void	ft_sig(int sig)
{
	printf("\n\033[1;31m[%s]\033[0m\n", "SIGINT");
	if (sig == SIGINT)
	{
		system("leaks webserv");
		kill(getpid(), SIGKILL);
	}
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

	// sigaction()
	// system("leaks webserv");
	return EXIT_SUCCESS;
}
