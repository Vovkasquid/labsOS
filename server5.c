#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int result;
	fd_set readfds, testfds;
	//Создаём сокет для сервера и присваиваем ему имя
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);

	bind(server_sockfd, (struct sockaddr *)&server_address,
						 server_len);
	//Создаём очередь запросов на соединение и инициализиурем
	//множество readfds для обработки ввода с сокета
	// server_sockfd:
	listen(server_sockfd, 5);
	
	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);
	//Ждём запросов. Если select вернёт < 1, то всё плохо
	while(1) {
		char ch;
		int fd;
		int nread;
	
		testfds = readfds;
	
		printf("server waiting\n");
		result = select(FD_SETSIZE, &testfds, (fd_set *)0,
		(fd_set *)0, (struct timeval *) 0);
	
		if (result < 1) {
			perror("server5");
			exit(1);
		}
		//Выясняем какой из дескриптеров активен
		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
		//если активность на server_sockfds, то это может
		//быть запрос на новое соединение, добавляем новый
		//client_sockfd:
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
					FD_SET(client_sockfd, &readfds);
					printf("adding client of fd %d\n", client_sockfd);
				}
				//Иначе активность проявляет клиент
				//Если close, то его можно удалить
				//из множества дескриптеров
				//иначе, обслуживаем клиента
				else {
					ioctl(fd, FIONREAD, &nread);
	
					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
}
					else {
						read(fd, &ch, 1);
						sleep(5);
						printf("serving client on fd %d\n", fd);
						ch++;
						write(fd, &ch, 1);
					}
				}
			}
		}
	}
}
