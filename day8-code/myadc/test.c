#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int fd;
	int data;
	float rd;
	if((fd = open("/dev/myadc",O_RDWR)) < 0){
		perror("open adc");
		return -1;
	}

	while(1){
		read(fd,&data,sizeof(data));
		rd = data/4096.0 *1.8;
		printf("VOL = %.2f\n",rd);
		sleep(1);
	}


	close(fd);
	return 0;
}
