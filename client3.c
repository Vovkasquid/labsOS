#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>

#define BUFF_SIZE 512
#define name_size 10

int sockfd;

void *listener()
{
	int nread;

	while (1) {
		char buf1[BUFF_SIZE] = {0};

		nread = read(sockfd, buf1, BUFF_SIZE + name_size - 1);
		if (nread != 0)
			printf("%s", buf1);
		else
			exit(1);
	}
}

int main(void)
{

	pthread_t a_thread;
	char name[name_size];
	int len;
	struct sockaddr_in address;
	int result;
	char buf[BUFF_SIZE];
	pthread_attr_t thread_attr;

	pthread_attr_init(&thread_attr);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("Hello! Input your name\n");
	fgets(name, name_size, stdin);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(9734);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);
	if (result == -1) {
		perror("error : client");
		exit(1);
	}
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&a_thread, &thread_attr, &listener, (void *)0);

	while (1) {
		char buf_sup[BUFF_SIZE + name_size] = {0};

		strcpy(buf_sup, name);
		int n = strlen(buf_sup);

		buf_sup[n-1] = ':';
		fgets(buf, BUFF_SIZE, stdin);
		strcat(buf_sup, buf);
		write(sockfd, buf_sup, strlen(buf_sup));
	}
}
