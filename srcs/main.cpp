/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 14:20:57 by besellem          #+#    #+#             */
/*   Updated: 2021/10/24 18:03:49 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <stdio.h>

int	main(int ac, char **av)
{
	webserv::WebServer	webserv;
	std::string			conf = (ac > 1) ? av[1] : DEFAULT_CONFIG_FILE;
	
	try
	{
		webserv.parse(conf);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	const int					port = webserv.servers(0).port();
	const std::string			const_path = "./www/tmp.html";
	webserv::Socket		_sock(port);

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
		
		char	header[30000] = {0};
		valread = recv(new_socket, header, 30000, 0);
		printf("%s\n", header);
		std::string content = execute_cgi(webserv.servers(0).locations(0), "test.php", cgiEnv(webserv.servers(0), "GET"));
		send(new_socket, content.c_str(), content.length(), 0);

		// _sock.parse(new_socket, header);	// in process
		// _sock.parse(new_socket, const_path);		// TO REMOVE
		// printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	
	return 0;
}
