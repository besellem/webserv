/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 14:17:07 by kaye              #+#    #+#             */
/*   Updated: 2021/10/19 20:19:59 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include <fcntl.h>

int main(int ac, char **av) {
	if (ac == 1) {
		std::cout << "enter Port" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	int port = atoi(av[1]);

	webserv::Socket	socketVar(port);
	socketVar.startUp();

	int new_socket;
	long valread;

	char *hello = (char *)"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	while(1)
	{
	    printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		int svrfd = socketVar.getServerFd();
		sockaddr_in addr = socketVar.getAddr();
		size_t len = socketVar.getAddrLen();

	    new_socket = socketAccept(svrfd, (struct sockaddr *)&addr, (socklen_t*)&len);
		
	    char buffer[30000] = {0};
	    valread = read(new_socket , buffer, 30000);
	    printf("%s\n",buffer );
	    ::write(new_socket , hello , strlen(hello));
	    printf("------------------Hello message sent-------------------");
	    close(new_socket);
	}
	
	return 0;
}