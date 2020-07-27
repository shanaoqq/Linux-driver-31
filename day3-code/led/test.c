#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char ubuf[2] = {0,0};
int main(int argc, const char *argv[])
{
	int fd;
	fd = open("/dev/myled",O_RDWR);
	if(fd == -1){
		perror("open error");
		return -1; 
	}

	while(1){
		sleep(1);
		//ubuf[1] = !ubuf[1];  
		
		ubuf[1] = ubuf[1]?0:1;

		write(fd,ubuf,sizeof(ubuf));
	}

	close(fd);
	return 0;
}
