/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 14:17:07 by kaye              #+#    #+#             */
/*   Updated: 2021/10/20 14:55:09 by besellem         ###   ########.fr       */
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

	std::string path = "/Users/kaye/42/42_Project/webserv/www/www.apple.com.html";

	webserv::Socket	socketVar(port);
	socketVar.startUp();

	int new_socket;
	long valread;

	// char *hello = (char *)"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\n\n<h1>Hello world!<\\h1>";

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
	    // ::write(new_socket , hello , strlen(hello));
		socketVar.parsing(new_socket, path);
	    printf("------------------Hello message sent-------------------");
	    close(new_socket);
	}
	
	return 0;
}