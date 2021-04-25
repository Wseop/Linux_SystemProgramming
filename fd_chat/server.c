#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX(a, b) (a) < (b) ? (b) : (a)

const int BUF_SIZE = 4096;

void AddToFDSet(fd_set* master_set, int fd, int* max_sd) {
	FD_SET(fd, master_set);
	*max_sd = MAX(*max_sd, fd);
}

void RemoveFromFDSet(fd_set* master_set, int fd, int* max_sd) {
	FD_CLR(fd, master_set);
	if (fd == *max_sd) {
		while (FD_ISSET(*max_sd, master_set) == 0) {
			*max_sd = (*max_sd) - 1;
		}
	}
}

void BroadCast(fd_set* master_set, char* buffer, int client_sd, int server_sd, int max_sd) {
	for (int i = 0; i <= max_sd; i++) {
		if (FD_ISSET(i, master_set) && i != server_sd && i != client_sd) {
			send(i, buffer, strlen(buffer), 0);
		}
	}
	printf("BroadCasting %d's message -> %s\n", client_sd, buffer);
}

int main(int argc, char* argv[]) {
	int listen_sd, max_sd = 0;
	struct sockaddr_in addr;
	struct timeval timeout;
	fd_set master_set, select_result;
	int ret;
	int on = 1;
	char buffer[BUF_SIZE];
	int port_number = atoi(argv[1]);

	// Initialize
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0) return -1;
	ret = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	if (ret < 0) return -1;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port_number);
	ret = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) return -1;
	ret = listen(listen_sd, 32);
	if (ret < 0) return -1;

	printf("FD_Chat Server started on port# %d\n", port_number);
	FD_ZERO(&master_set);
	AddToFDSet(&master_set, listen_sd, &max_sd);

	timeout.tv_sec = 60;
	timeout.tv_usec = 0;

	while (1) {
		char buffer2[BUF_SIZE];

		memcpy(&select_result, &master_set, sizeof(master_set));
		//ret = select(max_sd + 1, &select_result, NULL, NULL, &timeout);
		ret = select(max_sd + 1, &select_result, NULL, NULL, NULL);
		if (ret < 0) return -1;
		else if (ret == 0) return -1;
		for (int i = 0; i <= max_sd; i++) {
			if (FD_ISSET(i, &select_result)) {
				if (i == listen_sd) {
					int new_sd = accept(listen_sd, NULL, NULL);
					
					if (new_sd < 0) return -1;
					printf("Client #%d connected\n", new_sd);
					AddToFDSet(&master_set, new_sd, &max_sd);
					sprintf(buffer, "Client #%d entered\n", new_sd);
					BroadCast(&master_set, buffer, new_sd, listen_sd, max_sd);
				} else {
					ret = recv(i, buffer, BUF_SIZE, 0);
					if (ret < 0) return -1;
					else if (ret == 0) {
						printf("Client #%d disconnected\n", i);
						sprintf(buffer2, "Client #%d exit\n", i);
						close(i);
						RemoveFromFDSet(&master_set, i, &max_sd);
					} else {
						buffer[ret] = 0;
						sprintf(buffer2, "Client #%d : %s", i, buffer);
					}
					BroadCast(&master_set, buffer2, i, listen_sd, max_sd);
				}
			}
		}
	}
	for (int i = 0; i < max_sd; i++) {
		if (FD_ISSET(i, &master_set)) close(i);
	}

	return 0;
}
