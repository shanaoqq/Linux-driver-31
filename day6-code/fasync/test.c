#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int fd;
char buf[128] = {0};
//信号处理函数
void handle_fasync_func(int signum)
{
	//在信号处理函数中将数据读取到用户空间
	if(signum == SIGIO){
		memset(buf,0,sizeof(buf));
		read(fd,buf,sizeof(buf));
		printf("buf = %s\n",buf);
	}
}

int main(int argc, const char *argv[])
{
	if((fd = open("/dev/mycdev0",O_RDWR))<0){
		perror("open error");
		return -1;
	}

	//注册信号处理函数
	signal(SIGIO,handle_fasync_func);

	//调用驱动的fasync函数
	fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|FASYNC);
	
	//指定接受信号的进程是当前进程
	fcntl(fd,F_SETOWN,getpid());

	while(1){
		sleep(1);
	}

	return 0;
}
