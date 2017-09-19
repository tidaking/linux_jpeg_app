#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("test insmod robin_driver.ko\n");
	system("cd /home/robin");
	system("insmod robin_driver.ko");
	return 0;
}
