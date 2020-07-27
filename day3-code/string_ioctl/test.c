#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cmd.h"
char ubuf[50] = "hello everyone.............";
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

		ioctl(fd,ACCESS_STRING_W,ubuf);
		memset(ubuf,0,sizeof(ubuf));
		ioctl(fd,ACCESS_STRING_R,ubuf);
		printf("ubuf = %s\n",ubuf);

	}

	close(fd);
	return 0;
}
