#include "common.h"
#define MAX_LEN 10000
#define ip_len 20

char *get_path(char *, int);
void record(char *, int, char *);
void Write(char *, char *);
char *Read(char *);
void record_pihealthd(char *, char *);
void record_warning(int, char *);

int main(int argc, char const *argv[]) {
	int server_listen, socket_fd, port, pid;
	if (argc != 2) exit(0);
	port = atoi(argv[1]);
	if ((server_listen = socket_create(port)) < 0) exit(1);

	while(1) {
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		if((socket_fd = accept(server_listen, (struct sockaddr*) &client_addr, &len)) < 0) break;

		struct sockaddr_in peer;
		socklen_t peer_len = sizeof(struct sockaddr_in);
		bzero(&peer, sizeof(struct sockaddr_in));
		getpeername(socket_fd, (struct sockaddr *)&peer, &peer_len);

		char buff_peer[64] = {'\0'};
		inet_ntop(AF_INET, (void*)&peer.sin_addr, buff_peer, 63);
		if ((pid = fork()) < 0) printf("Error\n");

		if (pid == 0) {
			close(server_listen);
			char *path = get_path(buff_peer, socket_fd);
			record(path, socket_fd, buff_peer);

			close(socket_fd);
		}
		close(socket_fd);
	}

	return 0;
}

char *get_path(char ip[ip_len], int socket_fd) {
	char buff[MAX_LEN];
	int n = recv(socket_fd, buff, MAX_LEN, 0);
	//printf("%s\n", buff);
	char *path = (char *)malloc(MAX_LEN * sizeof(char *));
	strcat(path, "/Users/zou-jianfeng/HZ/8.23/on_line/");
	strcat(path, ip);
	FILE *fp = fopen(path, "r");
	if (!fp) {
		char *temp = (char *)malloc(MAX_LEN);
		strcat(temp, "mkdir ");
		strcat(temp, path);
		system(temp);
	}
	strcat(path, "/");
	strcat(path, buff);
	strcat(path, ".log");
	return path;
}

void record(char path[100], int socket_fd, char buff_peer[100]) {
	int n;
	char buff[MAX_LEN];
	while ((n = recv(socket_fd, buff, MAX_LEN, 0)) > 0) {
		printf("%s", buff);
		if (buff[0] == '&') {
			record_warning(socket_fd, buff_peer);
		}
		Write(buff, path);
	}
	record_pihealthd(path, buff_peer);
}

void record_warning(int socket_fd, char buff_peer[100]) {
	char buff[MAX_LEN] = {0};
	char temp[MAX_LEN];
	int n;
	n = recv(socket_fd, temp, MAX_LEN, 0);
	strcat(buff, buff_peer);
	strcat(buff, " ");
	strcat(buff, temp);
	printf("%s", buff);
	Write(buff, "/Users/zou-jianfeng/HZ/8.23/on_line/warning.log");
	return ;
}

void record_pihealthd(char path[100], char buff_peer[100]) {
	char buff[MAX_LEN] = {0};
	strcpy(buff, Read("bash time.sh"));
	int n = strlen(buff);
	buff[n - 1] = ' ';
	strcat(buff, buff_peer);
	strcat(buff, path);
	strcat(buff, "\n");
	Write(buff, "/Users/zou-jianfeng/HZ/8.23/on_line/pihealthd.log");
}

char *Read(char commond[10]) {
	int i = 0;
	char c;
	char *buff = (char *)malloc(MAX_LEN * sizeof(char));
	FILE *fp = popen(commond, "r");
	while (~fscanf(fp, "%c", &c)) {
		buff[i++] = c;
	}
	pclose(fp);
	return buff;
}

void Write(char buff[MAX_LEN], char path[100] ) {
	FILE *fp = fopen(path, "a+");
	if (!fp) return;
	flock(fp->_file, LOCK_EX); // 对整个文件枷锁
	fputs(buff, fp);
	fclose(fp);
}