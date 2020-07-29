#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
char buf[128] = "i am user data......";
int main(int argc, const char *argv[])
{
	int fd;

	fd = open("/dev/mycdev0",O_RDWR);
	if(fd == -1){
		perror("open /dev/mycdev0 error");
		return -1;
	}

	write(fd,buf,sizeof(buf));
	sleep(5);
	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
	printf("buf = %s\n",buf);

	close(fd);

	return 0;
}
