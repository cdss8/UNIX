#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define PORT 3001

int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char str[50];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);

	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);	
	if (result < 0)	
	{
		perror("error");
		exit(1);
	}

	while(1){
		scanf("%s", str);
		send(sockfd, str, 50, 0);
		if (!strcmp(str, "#close")) 
			break;
	}
	
	close(sockfd);
	puts("Connection closed");
	exit(0);
}