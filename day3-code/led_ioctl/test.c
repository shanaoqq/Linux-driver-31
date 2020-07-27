#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cmd.h"

int main(int argc, const char *argv[])
{
	int fd;
	fd = open("/dev/myled",O_RDWR);
	if(fd == -1){
		perror("open error");
		return -1; 
	}

	while(1){
		ioctl(fd,RED_ON);	
		sleep(1);
		ioctl(fd,RED_OFF);	
		sleep(1);
	}

	close(fd);
	return 0;
}
