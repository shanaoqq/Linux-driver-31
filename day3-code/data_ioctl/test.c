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
	int data = 50;
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
		ioctl(fd,ACCESS_DATA_W,&data);
		sleep(1);
		data = 0;
		ioctl(fd,ACCESS_DATA_R,&data);
		printf("data = %d\n",data);
	}

	close(fd);
	return 0;
}
