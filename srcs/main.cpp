/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/10/21 18:55:27 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <stdio.h>

int	main(int ac, char **av)
{
	// webserv::WebServer	serv;
	// std::string			conf = (ac > 1) ? av[1] : DEFAULT_CONFIG_FILE;

	if (ac != 2)
	{
		std::cout << "Enter Port" << std::endl;
		return 1;
	}
	
	const int					port = atoi(av[1]);
	// const std::string			const_path = "./www/index.html";
	webserv::Socket		_sock(port);

	// int			new_socket;
	// ssize_t		valread;

	_sock.startUp();

	webserv::Epoll _epoll(_sock);

	int fd = _sock.getServerFd();
	_sock.setNonBlock(fd);
	_epoll.updateEvents(fd, webserv::Epoll::kReadEvent, false);
	while (true)
	{
		// printf("+++++++ Waiting for new connection ++++++++\n\n");

		// new_socket = _sock.socketAccept();
		
		// char	header[30000] = {0};
		// valread = recv(new_socket, header, 30000, 0);
		// printf("%s\n", header);
		
		// // _sock.parse(new_socket, header);	// in process
		// _sock.parse(new_socket, const_path);		// TO REMOVE
		
		// printf("------------------Hello message sent-------------------\n");
		// close(new_socket);
		_epoll.serverLoop(10000);
	}
	return 0;
}
