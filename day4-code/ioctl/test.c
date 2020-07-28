#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int fd;
	if((fd=open("/dev/myled2",O_RDWR))==-1){
		perror("open error");
		return 0;
	}

	ioctl(fd,1,100);
	ioctl(fd,3,300);

	close(fd);

	return 0;
}
