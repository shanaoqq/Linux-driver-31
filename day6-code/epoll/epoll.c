#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

#define ARRAY_SIZE(res) (sizeof(res)/sizeof(res[0]))
#define PRINT_ERR(errmsg) do{perror(errmsg);return -1;}while(0)

char buf[128] = {0};

int main(int argc, const char *argv[])
{
	int epfd,fd,i,ret;
	struct epoll_event event;
	struct epoll_event revents[10];
	if((epfd = epoll_create(10))< 0)
		PRINT_ERR("open epoll error");

	for(i=1; i<argc; i++){
		if((fd = open(argv[i],O_RDWR))<0)
			PRINT_ERR("open file error");

		event.events = EPOLLIN;
		event.data.fd = fd;
		if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event)<0)
			PRINT_ERR("epoll ctl add");
	}

	while(1){
		if((ret = epoll_wait(epfd,revents,ARRAY_SIZE(revents),-1))<0)
			PRINT_ERR("epoll wait");

		for(i=0; i<ret; i++){
			if(revents[i].events & EPOLLIN){
				memset(buf,0,sizeof(buf));
				read(revents[i].data.fd,buf,sizeof(buf));
				printf("buf%d = %s\n",revents[i].data.fd,buf);
			}
		}
	}

	close(epfd);

	return 0;
}
