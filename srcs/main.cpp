/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 14:17:07 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 17:15:10 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <fcntl.h>

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Enter Port" << std::endl;
		return 1;
	}
	
	const int			port = atoi(av[1]);
	const std::string	path = "./www/index.html";
	webserv::Socket		_sock(port);

	int					new_socket;
	ssize_t				valread;

	_sock.startUp();
	while (true)
	{
		printf("+++++++ Waiting for new connection ++++++++\n\n");
		int			svrfd = _sock.getServerFd();
		sockaddr_in	addr = _sock.getAddr();
		size_t		len = _sock.getAddrLen();

		new_socket = socketAccept(svrfd, (struct sockaddr *)&addr, (socklen_t*)&len);
		
		char	header[30000] = {0};
		valread = recv(new_socket, header, 30000, 0);
		printf("%s\n", header);
		
		_sock.parse(new_socket, header);
		printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	
	return 0;
}
