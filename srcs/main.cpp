/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/10/22 18:01:51 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <stdio.h>

#define PORT 8080

int	main(int ac, char **av, __unused char **env)
{
	_INLINE_NAMESPACE::WebServer	serv;
	
	std::string	config_file = (ac > 1) ? av[1] : DEFAULT_CONFIG_FILE;
	try
	{
		serv.parse(config_file);
	}
	catch (std::exception &e)
	{
		std::cerr << S_RED "Error: " S_NONE << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	

	std::vector<std::string>	tmp =_INLINE_NAMESPACE::split_string("image/png,image/svg+xml,image/*;q=0.8,video/*;q=0.8,*/*;q=0.5", ",");

	for (std::vector<std::string>::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		std::cout << *it << std::endl;
	}

	
	return 0;
	// serv.createServers();

	const int					port = serv.getServer(0).port();
	const std::string			const_path = "./www/index.html";
	// const std::string			const_path = "./test_cgi";
	_INLINE_NAMESPACE::Socket	_sock(port);

	int			new_socket;
	ssize_t		valread;


	_sock.startUp();
	while (true)
	{
		printf("+++++++ Waiting for new connection ++++++++\n\n");
		int			svrfd = _sock.getServerFd();
		sockaddr_in	addr = _sock.getAddr();
		size_t		len = _sock.getAddrLen();

		new_socket = _INLINE_NAMESPACE::socketAccept(svrfd, (struct sockaddr *)&addr, (socklen_t*)&len);


		/* CGI Test */
		// __unused char *tmp[] = {"php", "./www/index.php", NULL};
		// int fd = fork();

		// if (fd == 0)
		// {
		// 	dup2(new_socket, STDOUT_FILENO);
		// 	execve("/usr/bin/php", tmp, env);
		// 	exit(1);
		// }
		
		std::cout << "Hellooo\n";
		char	header[30000] = {0};
		valread = recv(new_socket, header, 30000, 0);
		printf("%s\n", header);
		
		// _sock.parse(new_socket, header);	// in process
		_sock.parse(new_socket, const_path);		// TO REMOVE
		printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	
	return EXIT_SUCCESS;
}
