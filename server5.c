#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define buf_size 512
#define name_size 10
int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	int i;
	int currentFd = -1;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int result;
	int sockStorage[20] ={0};
	int storageSave = 0;
	char buf[buf_size + name_size];
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
			//write(fd, "test", 1);
			if (FD_ISSET(fd, &testfds)) {
		//если активность на server_sockfds, то это может
		//быть запрос на новое соединение, добавляем новый
		//client_sockfd:
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
					FD_SET(client_sockfd, &readfds);
					printf("adding client of fd %d\n", client_sockfd);
					sockStorage[storageSave] = client_sockfd;
					printf("fd in mas = %d\n",sockStorage[storageSave]);
					printf("sS = %d\n", storageSave);
					storageSave++;
				}
				//Иначе активность проявляет клиент
				//Если close, то его можно удалить
				//из множества дескриптеров
				//иначе, обслуживаем клиента
				else {
					//ioctl(fd, FIONREAD, &nread);
					char buf1[buf_size + name_size] = {0};
					nread = read(fd, buf1, buf_size + name_size - 1);
					if (nread == 0) {
						for(i = 0; i < 20; ++i) 
							if(sockStorage[i] == fd)
								sockStorage[i] = -1;
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
					}

					else {// Пишем во все "живые сокеты"
						for(i = 0; i < storageSave; ++i)
							if((sockStorage[i] > 0) && (fd != sockStorage[i])) {
								printf("fd = %d\n", fd);								
								printf("i = %d\n", i);
								printf("sSF = %d\n", storageSave);
								printf("nread = %d \n", nread);
								printf("string = %s \n", buf1);
								printf("sock to write = %d \n", sockStorage[i]);
								
								write(sockStorage[i], &buf1, nread);
							}
					}
					
				}
			}
		}
	}
}
