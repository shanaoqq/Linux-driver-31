#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
//R G B
//0 1 2
//ON  OFF
//0   1
char buf[2] = {1,1};
int main(int argc, const char *argv[])
{
	int fd;

	fd = open("./hello",O_RDWR);
	if(fd == -1){
		perror("open ./hello error");
		return -1;
	}

	write(fd,buf,sizeof(buf));

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
	printf("buf = %s\n",buf);

	close(fd);

	return 0;
}
