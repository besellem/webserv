int main() {
	struct epoll_event polledEv[MAX_PORTS];
	struct epoll_event readyEv[MAX_PORTS];

	int i;
	for (i = 0; i < argc; ++i) {
		polledEv[i].data.fd = -1;
	}

	int epollFd = epoll_create1(0);
	if (epollFd < 0) {
		perror("fail to create epoll for stdin");
		exit(EXIT_FAILURE);
	}

	polledEv[0].data.fd = STDIN_FILENO;
	polledEv[0].events = EPOLLIN;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, STDIN_FILENO, &polledEv[0]) < 0) {
		perror("epoll_ctl: stdin");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i < argc; ++i) {
		int sockFd = createServerSocket(atoi(argv[i]));
		if (sockFd < 0) {
			perror("fail to create socket");
			continue; 
		} 

		polledEv[i].data.fd = sockFd;
		polledEv[i].events = EPOLLIN;
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, sockFd, &polledEv[i]) < 0) {
			perror("epoll_ctl");
			close(sockFd);
			exit(EXIT_FAILURE);
		}
	}

	int running = 1;
	while (running) {
		int ready = epoll_wait(epollFd, readyEv, MAX_PORTS, 5);
		if (ready < 0) {
			perror("epoll_wait:");
			goto errout;
		}
		else if (ready == 0) {
			//printf("timeout\n")
			continue;
		}
		else {
			for (i = 0; i < ready; ++i) {
				int fd = acceptConnecton(readyEv[i].data.fd);
				if (fd < 0) {
					perror("fail to accept request");
					continue;
				}

				pthread_t tid;
				if (pthread_create(&tid, NULL, handleRequest, (void*)&fd) != 0) {
					perror("fail to create handle thread");
					close(fd);
					goto errout;
				}
			}
		}
	}
}

void handleRequest(void* args)
{
	int fd = *((int*)args);
	if (fd < 0) {
		pthread_exit(NULL);
	}

	printf("handleRequest(): from fd=%d\n", fd);

	char buf[MAX_BUF_SIZE];

	while(1) {
		if (recv(fd, buf, MAX_BUF_SIZE, 0) < 0) {
			printf("fail to receive message from client\n");
			break;
		}

		printf("Client(%d): %s\n", fd, buf);

		int len = strlen(buf);
		buf[len] = 0;
		if (send(fd, buf, len, 0) != len) {
			printf("fail to echo");
			break;
		}
	}

	close(fd);
	pthread_exit(NULL);
}