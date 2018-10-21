#include "common.h"
#define MAX_LEN 100000
#define Path_LEN 100
#define LEN 100		
#define commondsize 20	
#define frequency 1
// #define path "home/Zou-JianFeng/"

int Connect(int, char *);
void Write(char *, char *);
char *Read(char *, int);
char *Name(char *);

void record(char *, char *);
char *ten_record(char *, int, int);
void off_line(char *, int, int);
void Check_connect(char *, int, int);
void send_record(char *, int, int);
void send_off(char *, int);
void send_on(char *, int, int);

// char *get_config(char key_name[10]) {
// 	int n;
// 	char path[Path_LEN] = {0};
// 	char *temp = Read("pwd");
// 	n = strlen(temp) - 1;
// 	temp[n] = '/';
// 	strcat(path, temp);
// 	strcat(path, "config.txt");
// 	printf("%s\n", path);

// 	FILE *fp = fopen(path, "r");
// 	if(!fp) printf("wrong\n");

// 	char *value = (char *)malloc(LEN);
//     char buffer[100];
//     while(fgets(buffer, sizeof(buffer), fp) > 0) {
//         if(!strncmp(buffer,key_name,strlen(key_name))) {
//             buffer[strlen(buffer) - 1] = '\0';
//             strcpy(value , buffer + strlen(key_name) + 1);
//             fclose(fp);
//             return value;

//         }
//        memset(buffer,0,sizeof(buffer));
//     }
//     fclose(fp);
//     return 0;
// }

int main(int argc, char *argv[]) {
	int socket_fd;

	char *ip_addr = "192.168.1.182";
	int port = atoi("2112");
	//char a = get_config("ip");
	//char b = get_config("port");

	// printf("%s %d\n", ip_addr, port);

	int pid = 1, x;
	for (int i = 0; i < 8; ++i) {
		if (pid != 0) {
			x = i;
			pid = fork();
		}
	}

	while(1) {
		if (pid == 0) {
			socket_fd = Connect(port, ip_addr);
			#define temp(a, b) Check_connect(a, b, socket_fd); break;
			switch (x) {
				case 0: temp("bash system.sh", 5)
				case 1: temp("bash user.sh", 5)
				case 2: temp("bash cpu.sh", 5)
				case 3: temp("bash mem.sh", 5)
				case 4: temp("bash disk.sh", 5)
				case 5: temp("bash disknote.sh", 5)
				case 6: temp("bash evilcpu.sh", 5)
				case 7: temp("bash evilmem.sh", 5)
				default :
					printf("fork出错\n");
					break;
			}
			#undef temp
		} else {
			sleep(1);
		}
	}
 	return 0;
}

void Check_connect(char commond[commondsize], int n, int socket_fd) {
	if (socket_fd == -1) off_line(commond, socket_fd, n);
	else send_record(commond, socket_fd, n);
}

void off_line(char commond[commondsize], int socket_fd, int n) {
	char path[LEN] = "/home/Zou-JianFeng/data/";
	char *name = Name(commond);
	strcat(path, name);
	strcat(path, ".log");

	char *buff = ten_record(commond, n, socket_fd);

	Write(buff, path);
	//free(name);
	//free(buff);
	record(": 请求连接失败 ", commond);
}

void send_record(char commond[commondsize], int socket_fd, int n) {
	char *name =  Name(commond);
	send(socket_fd, name, strlen(name), 0);
	send_off(commond, socket_fd);
	send_on(commond, socket_fd, n);
}

void record(char ans[20], char commond[commondsize]) {
	char path[LEN] = "/home/Zou-JianFeng/data/pihealthd.log";
	char buff[MAX_LEN] = {0};
	char *name = Name(commond);

	strcpy(buff, Read("bash time.sh", 0));
	int n = strlen(buff);
	buff[n - 1] = ' ';

	strcat(buff, ans);
	strcat(buff, name);
	strcat(buff, "\n");
	//strcat(buff, "\0");

	Write(buff, path);
	//free(name);
}

char *ten_record(char commond[commondsize], int n, int socket_fd) {
	int f = frequency;
	char *buff = (char *)calloc(MAX_LEN, sizeof(char));
	while (f--) {
		char *temp = Read(commond, socket_fd);
		//strcat(buff, Check_warning(temp, socket_fd));
		strcat(buff, temp);
		//free(temp);
		sleep(n);
	}
	//strcat(buff, "\0");
	printf("%s", buff);
	return buff;
}

void send_off(char commond[commondsize], int socket_fd) {
	char path[LEN] = "/home/Zou-JianFeng/data/";
	char *name = Name(commond);
	strcat(path, name);
	strcat(path, ".log");
	FILE *fp = fopen(path, "r");
	if (!fp) return ;
	char buff[MAX_LEN];
	while (fgets(buff, 1000, (FILE*)fp) != NULL) {
		send(socket_fd, buff, strlen(buff), 0);
	}
	remove(path);
	record(": 离线发送成功 ", commond);
	//free(path);
	//free(name);
	//free(buff);
}

void send_on(char commond[commondsize], int socket_fd, int n) {
	char *buff = ten_record(commond, n, socket_fd);
	send(socket_fd, buff, strlen(buff), 0);
	//free(buff);
	record(": 在线发送成功 ", commond);

}

int Connect(int port, char* ip_addr) {
	int socket_fd;
	socket_fd = socket_connect(port, ip_addr);
	if (socket_fd < 0) return -1;
	return socket_fd;
} 

void Write(char buff[MAX_LEN], char path[Path_LEN]) {
	FILE *fp = fopen(path, "a+");
	if (!fp) return;
	flock(fp->_fileno, LOCK_EX); // 对整个文件枷锁
	fputs(buff, fp);
	//flock(fp->_fileno, LOCK_UN); //解锁
	//free(buff);
	//free(path);
	fclose(fp);
}

char *Read(char commond[commondsize], int socket_fd) {
	int i = 0;
	char c;
	char *buff = (char *)calloc(MAX_LEN, sizeof(char));
	FILE *fp = popen(commond, "r");
	int flag = 0;
	while (~fscanf(fp, "%c", &c)) {
		if (c == '&') flag = 1;
		buff[i++] = c;
	}
	pclose(fp);
	if (flag) {
		buff[i - 2] = '\n';
		buff[i - 1] = '\0';
		if (socket_fd != -1) {
			send(socket_fd, "&", 1, 0);
			send(socket_fd, buff, strlen(buff), 0);
		}
	}
	return buff;
}

char *Name(char commond[commondsize]) {
	char *name = (char *)calloc(LEN, sizeof(char));
	strcpy(name, commond);
	int n = strlen(name) - 3;
	name[n] = '\0';
	return name + 5;
}



