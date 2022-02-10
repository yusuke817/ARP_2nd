#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "../include/utils.h"

int log_dr2(int mode, char* string, int errno){

    int fd;
    
    //char exp_dr2[] = string;
    char con_dr2[80] = "drone2 ";
    strcat(con_dr2, string);

switch(mode)
 {


    case 0:
    fd = openInfoLog();
    writeInfoLog(fd, con_dr2);
    closeLog(fd);
    break;

    case 1: // for error
    fd = openErrorLog();
    writeErrorLog(fd, con_dr2, errno);
    closeLog(fd);   
    break;
}

}

int main(){
	int i = 0;	
	while(1){
	usleep(1);
	char text[80];	
	sprintf(text, "%d", i);
	log_dr2(0 ,text, 0);
	i++;
	}
}
