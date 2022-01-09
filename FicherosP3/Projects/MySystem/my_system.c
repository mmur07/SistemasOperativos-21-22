#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	char* command;
	printf("num args: %d\n", argc);
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}
	command = argv[1];
	int child = fork();
	if(child == -1){ //Failed
		printf("Failed fork @ my_system.c\n");
		return -1;
	}
	else if (child == 0){ //Hijo
		//printf("%s\n", command);
		execl("/bin/bash", "bash", "-c", command, (char *) NULL);
		return 1;
	}
	else{ //Padre
		while(wait(NULL) == -1){
			if(errno != ECHILD) {
				return -1;
			}
		}
		return 0;
	}
}