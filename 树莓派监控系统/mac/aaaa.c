/*************************************************************************
	> File Name: get_conf_value.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月13日 星期三 18时26分46秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char get_conf_value( char* key_name, char *value) {
    FILE *fp = fopen("/home/ygy/text", "r");
    if(fp == NULL) return NULL;
    char buffer[100];
    while(fgets(buffer, sizeof(buffer), fp) > 0) {
        if(!strncmp(buffer,key_name,strlen(key_name))) {
            buffer[strlen(buffer) - 1] = '\0';
            strcpy(value , buffer + strlen(key_name) + 1);
            fclose(fp);
            return value;

        }
       // memset(buffer,0,sizeof(buffer));
    }
    fclose(fp);
    return 0;
}

int main() {
    char value[100];
    char key_name[100];
    char pathname;
   // char path[100] = "/home/ygy/linux/linux项目/piheadlthd.conf";
    char name[100] = "port";
    strcpy(key_name,name);
   // strcpy(pathname,path);
    char a = get_conf_value(key_name,value);
    printf("%s",a);

    return 0;
}