#include <errno.h>
#include "../include/utils.h"

int log(int mode, char* string, int errno){

switch(mode)
 {
    int fd;

    case 0:
    fd = openInfoLog();
    void writeInfoLog(fd, string);
    void closeLog(fd);
    break;

    case 1: // for error
    fd = openErrorLog();
    void writeErrorLog(fd, string, errno);
    void closeLog(fd);   
    break;
}

}