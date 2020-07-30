#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char buf[128] = {0};
int main(int argc, const char *argv[])
{
	int fd,fd1,ret;
	fd_set rfds;
	if((fd = open("/dev/mycdev0",O_RDWR))< 0){
		perror("open error");
		return -1;
	}
	if((fd1 = open("/dev/input/mice",O_RDWR))< 0){
		perror("open error");
		return -1;
	}

	while(1){
		FD_ZERO(&rfds); //清空表
		FD_SET(fd,&rfds);//将文件描述符放入读表
		FD_SET(fd1,&rfds);//将文件描述符放入读表

		ret = select(fd1+1,&rfds,NULL,NULL,NULL);
		if(ret < 0){
			perror("select error");
			return -1;
		}

		if(FD_ISSET(fd,&rfds)){ //判断文件描述符是否准备好了
			memset(buf,0,sizeof(buf));
			read(fd,buf,sizeof(buf));
			printf("mycdev0 buf = %s\n",buf);
		}
		if(FD_ISSET(fd1,&rfds)){ //判断文件描述符是否准备好了
			memset(buf,0,sizeof(buf));
			read(fd1,buf,sizeof(buf));
			printf("mouse0 buf = %s\n",buf);
		}

	}

	close(fd);
	close(fd1);
	return 0;
}
