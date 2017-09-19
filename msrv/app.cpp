#include <stdio.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
	int fd = -1;
	printf("Hello World...\n");
	int ret = -1;



	fd = open("/dev/robin_dev0",O_RDWR);
	if(fd < 0)
	{
		printf("open failed:%d\n",fd);
		return -1;
	}



	char userbuf[50]={0};

	write(fd,"What's up!?",11);
	read(fd,(void*)userbuf,11);
	printf("read from dri:%s\n",userbuf);

	int i = 1;
	while(i<10)
	{
		write(fd,"on",2);
		sleep(1);
		write(fd,"off",3);
		sleep(1);
		i++;
	}


	ret = close(fd);
	if(ret != 0)
	{
		printf("fclose failed:%d\n",ret);
		return -1;
	}

	printf("Hello World...\n");
	return 0;
}
