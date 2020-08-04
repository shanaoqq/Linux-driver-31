#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int fd;
	int buf[6];
	int x,y,z;
	if((fd = open("/dev/mma8451q",O_RDWR))<0){
		perror("open error");
		return -1;
	}

	while(1){
		sleep(1);
		memset(buf,0,sizeof(buf));
		read(fd,buf,sizeof(buf));
		x = (buf[0]<<6 |buf[1]>>2);
		y = (buf[2]<<6 |buf[3]>>2);
		z = (buf[4]<<6 |buf[5]>>2);
		
		printf("x = %d,y = %d,z = %d\n",x,y,z);
	}

	close(fd);

	return 0;
}
