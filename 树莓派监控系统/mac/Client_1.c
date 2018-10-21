#include "common.h"
#define MAX_LEN 100000				

int Connect(int port, char* ip_addr) {
	int socket_fd;
	socket_fd = socket_connect(port, ip_addr);
	if (socket_fd < 0) return -1;
	return socket_fd;
}

void record_log(char ans[20], char commond[20]) {
	char path[100] = "/Users/zou-jianfeng/HZ/8.23/off_line/Pi_1/pihealthd.log";
	char time[100];
	FILE *p = popen("date", "r");
	fgets(time, 100, p);
	pclose(p);

	FILE *fp = NULL;
	fp = fopen(path, "a+");
	fputs(commond + 5, fp);
	fputs(ans, fp);
	fputs(time, fp);
	fclose(fp);
	printf("离线存储成功 : %s\n", path);
	return ;
}

char *ten_record(char commond[10]) {
	int n = 10, i = 0;
	char *buff = (char *)malloc(MAX_LEN);
	while (n--) {
		char c;
		FILE *fp = popen(commond, "r");
		while (~fscanf(fp, "%c", &c)) {
			buff[i++] = c;
		}
		pclose(fp);
		sleep(3);
	}
	buff[i] = '\0';
	return buff;
}

void off_line_record(char commond[10]) {
	char path[100] = "/Users/zou-jianfeng/HZ/8.23/off_line/Pi_1/";
	strcat(path, commond + 5);
	strcat(path, ".log");
	char *buff = ten_record(commond);
	//printf("%s\n", buff);
	FILE *fp = fopen(path, "a+");
	fputs(buff, fp);
	fclose(fp);
	free(buff);
	record_log(": 请求连接失败 ", commond);
	return ;
}

void send_on_line_record(char commond[10], int socket_fd) {
	FILE *fp = NULL;
	char buff[1000];
	fp = popen(commond, "r");
	if (!fp) {
		//perror("popen");
		return ;
	}
	while (fgets(buff, 1000, fp) != NULL) {
		send(socket_fd, buff, strlen(buff), 0);
		//printf("%s\n", buff);
		//memset(buff, 0, sizeof(buff));
	}
	fp = popen("date", "r");
	fgets(buff, 1000, fp);
	pclose(fp);
	close(socket_fd);
	//record_log(2, buff, commond);
	sleep(5);
}

void send_off_line_record(char commond[10], int socket_fd) {
	char path[100] = "/Users/zou-jianfeng/HZ/8.23/off_line/Pi_1/";
	strcat(path, commond);
	strcat(path, ".log");
	FILE *fp = NULL;
	char buff[1000];
	fp = fopen(path, "r");
	if (!fp) {
		return ;
	}
	while (fgets(buff, 1000, (FILE*)fp) != NULL) {
		send(socket_fd, buff, strlen(buff), 0);
		//memset(buff, 0, sizeof(buff));
	}
	fp = popen("date", "r");
	fgets(buff, 1000, fp);
	fclose(fp);
	remove(path); 
	//record_log(3, buff, commond);
	return ;
}

void request_connect(char commond[10], int socket_fd) {
	send(socket_fd, commond, strlen(commond), 0);
	send_off_line_record(commond, socket_fd);
	send_on_line_record(commond, socket_fd);

	return ;
}

void choose_commond(char commond[20], int socket_fd) {
	if (socket_fd == -1) off_line_record(commond);
	else request_connect(commond, socket_fd);
}

int main(int argc, char *argv[]) {
	int socket_fd;
	char* ip_addr = argv[1];
	int port = atoi(argv[2]);

	int pid = 1, x;
	for (int i = 0; i < 6; ++i) {
		if (pid != 0) {
			x = i;
			pid = fork();
		}
	}

	while(1) {
		if (pid == 0) {
			socket_fd = Connect(port, ip_addr);
			#define temp(a) choose_commond(a, socket_fd); break;
			switch (x) {
				case 0: temp("Bash disk.sh")
				case 1: temp("Bash system.sh") 
				case 2: temp("Bash test1.sh") 
				case 3: temp("Bash test2.sh") 
				case 4: 
					break;
				case 5: 
					break;
				default :
					printf("fork出错\n");
					break;
			}
			#undef temp
		}
	}
 	return 0;
}