/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 12:20:57 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/25 13:48:08 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	main(int ac, char **av)
{
	webserv::WebServer	webServ;
 	std::string			conf = (ac > 1) ? av[1] : DEFAULT_CONFIG_FILE;

	try
 	{
 		webServ.parse(conf);
	}
	 catch(const std::exception& e)
 	{
 		std::cerr << e.what() << '\n';
 		return (EXCEPT_ERROR), EXIT_FAILURE;
 	}
	 
	const int			port = webServ.getServer(0).port();
 	const std::string	const_path = "./www/tmp.html";
 	webserv::Socket		_sock(port);

 	int			new_socket;
 	ssize_t		valread;

 	_sock.startSocket();
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
 		try
 		{
 			webserv::cgi newCgi(_sock, webServ.getServer(0).locations(0).cgi[0]);
 			std::string content = newCgi.execute("test.php");
 			send(new_socket, content.c_str(), content.length(), 0);

 		}
 		catch(const std::exception& e)
 		{
			EXCEPT_WARNING;
 		}

 		// _sock.parse(new_socket, header);	// in process
 		// _sock.parse(new_socket, const_path);		// TO REMOVE
 		// printf("------------------Hello message sent-------------------\n");
 		close(new_socket);
 	}
	return EXIT_SUCCESS;
}