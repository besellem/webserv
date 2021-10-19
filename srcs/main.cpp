/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 14:17:07 by kaye              #+#    #+#             */
/*   Updated: 2021/10/19 15:57:10 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(void) {
	// init step ... have many exception ... maybe modify later with version no exception
	try {
		webserv::epoll epollVar(9999);
		
		epollVar.bindStep(); // bind to 0.0.0.0:port
		epollVar.listenStep();

		std::cout << "fd: [" S_GREEN << epollVar.getListenFd() << "]" S_NONE " listening at: " << epollVar.getPort() << std::endl;

		epollVar.setNonBlock(epollVar.getListenFd());
		epollVar.updateEvents(epollVar.getEpollFd(), epollVar.getListenFd(), kReadEvent, false);

		// loop
		for (;;)
			epollVar.run(10000);

	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}