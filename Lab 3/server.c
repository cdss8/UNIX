#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define PORT 3001

volatile sig_atomic_t wasSigHup = 0; 
void sigHupHandler(int r) {
	wasSigHup = 1;
}

int main() {
	int fd = 0;
	fd = socket(AF_INET, SOCK_STREAM, 0);	
	if(fd == -1){	
		perror("error in socket");
		return -1;
	}
	
	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
		perror ("error in setsockopt");
		return -1;
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));		
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	addr.sin_port = htons(PORT);	

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {	
		perror("error in bind");
		close(fd);
		return -1;
	}
	
	if (listen(fd, SOMAXCONN) < 0) {	
		perror("error in listen");
		return -1;
	}

	struct sigaction sa;
	sigaction(SIGHUP, NULL, &sa);
	sa.sa_handler = sigHupHandler;
	sa.sa_flags |= SA_RESTART;
	sigaction(SIGHUP, &sa, NULL);	

	sigset_t blockedMask, origMask;	
	sigemptyset(&blockedMask);
	sigaddset(&blockedMask, SIGHUP);
	sigprocmask(SIG_BLOCK, &blockedMask, &origMask);

	char str[50];
	int client_fd;
	int clientsCounter = 0;

	while (!wasSigHup) {

		memset(str, 0, sizeof(str));	
		int maxFd = -1;	

		fd_set fds;	
		FD_ZERO(&fds);	
		FD_SET(fd, &fds);	

		if (fd > maxFd) {
			maxFd = fd;
		}
		if (clientsCounter) {
			FD_SET(client_fd, &fds);
			if(client_fd > maxFd) maxFd = client_fd;
		}
			
		
		if (pselect (maxFd + 1, &fds, NULL, NULL, NULL, &origMask) == -1) {
			if (errno == EINTR){
				if (wasSigHup) {	
					puts("Connection interrupted by signal SIGHUP");
					return -1;
				}
			}
		}

		if (FD_ISSET(fd, &fds)) {
			int client_sockfd = accept(fd, NULL, NULL);		
			if (clientsCounter >= 1) {
				close(client_sockfd);
				printf("New connection refused\n");
			}
			else if (client_fd >= 0) {
				client_fd = client_sockfd;
				clientsCounter++;
				printf("New connection\n\n");
			}
		}

		if (FD_ISSET(client_fd, &fds)) {
			read(client_fd, &str, 50);
			int k = strlen(str);

			if (k > 0) {
				printf("Size of the message: %d\n", k);
				k = 0;
			}
			else {
				close(client_fd);
				printf("Connection closed\n");
				client_fd = -1;
				clientsCounter--;
			}
		}
	}
}