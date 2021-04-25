#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio_ext.h>

#define MAX(a, b) (a) < (b) ? (b) : (a)

const int BUF_SIZE = 4096;

int main(int argc, char* argv[]) {
	int sock_fd, max_sd = 0;
	struct sockaddr_in addr;
	struct timeval timeout;
	fd_set master_set, select_result;
	int ret;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	int port_number = atoi(argv[1]);

	// Initialize
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) return -1;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);
	ret = inet_pton(AF_INET, argv[2], &addr.sin_addr);
	if (ret <= 0) return -1;
	ret = connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) return -1;

	FD_ZERO(&master_set);
	FD_SET(fileno(stdin), &master_set);
	FD_SET(sock_fd, &master_set);
	max_sd = MAX(fileno(stdin), sock_fd);

	timeout.tv_sec = 60;
	timeout.tv_usec = 0;

	printf("Connected to server\n");

	while (1) {
		memcpy(&select_result, &master_set, sizeof(master_set));
		//ret = select(max_sd + 1, &select_result, NULL, NULL, &timeout);
		ret = select(max_sd + 1, &select_result, NULL, NULL, NULL);
		if (FD_ISSET(fileno(stdin), &select_result)) {
			fgets(send_buf, BUF_SIZE, stdin);
			send(sock_fd, send_buf, strlen(send_buf), 0);
			//fflush(stdin);
			__fpurge(stdin);
		} else if (FD_ISSET(sock_fd, &select_result)) {
			ret = recv(sock_fd, recv_buf, BUF_SIZE, 0);
			if (ret < 0) return -1;
			else if (ret == 0) {
				printf("Server disconnected\n");
				close(sock_fd);
				break;
			} else {
				recv_buf[ret] = 0;
				printf("%s", recv_buf);
				//fflush(stdin);
				__fpurge(stdin);
			}
		}
	}
	close(sock_fd);

	return 0;
}
