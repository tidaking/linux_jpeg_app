#include <sys/types.h>      
#include <sys/socket.h>

#include <netinet/in.h>


	  

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "com_type.h"
#include "com_error.h"
#include "mapi_network.h"

void* MAPI_Net_Server(void*)
{
	S32 socket_fd = -1;
	S32 ret = -1;
	//struct sockaddr st_addr = {0};
	struct sockaddr_in st_addr_in = {0};
	st_addr_in.sin_family = PF_INET;
	st_addr_in.sin_port = htons(6666);
	in_addr_t temp;
	temp = inet_addr("1.2.3.5");
	memcpy(&st_addr_in.sin_addr,&temp,sizeof(in_addr_t));
	//st_addr_in.sin_addr = temp;


	//st_addr.sa_family = AF_INET;
	//st_addr.


	// 1.创建socket
	socket_fd = socket(AF_INET,SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		printf("socket failed\n");
		return NULL;
	}
	
	// 2.将网络协议/ip地址/端口绑定到对应socket上
	ret = bind(socket_fd,(struct sockaddr *)(&st_addr_in),sizeof(st_addr_in));
	if(ret < 0)
	{
		printf("bind failed\n");
		return NULL;
	}

	// 3.设置监听socket，最大等待连接数10
	// 2/3步是针对服务器的
	ret = listen(socket_fd,10);
	if(ret < 0)
	{
		printf("listen failed\n");
		return NULL;
	}

	
	struct sockaddr_in st_client_addr_in = {0};
	// 获取到连接到这个监听socket的client socket的信息
	// 一旦连接上，服务器端通过这个返回来的新的socket去通信
	// accept是阻塞的
	S32 new_accepted_fd = -1;
	socklen_t len = 0;
	printf("server:waiting...\n");
	new_accepted_fd = accept(socket_fd,(struct sockaddr *)(&st_client_addr_in),&len);
	if(new_accepted_fd < 0)
	{
		printf("accept failed\n");
		return NULL;
	}
	printf("server:waiting done...\n");

	int read_size = 0;
	char* read_buffer = NULL;
	read_buffer = (char*)malloc(100);

	char* write_buffer = NULL;
	write_buffer = (char*)malloc(100);

	memset(read_buffer,0,100);
	memset(write_buffer,0,100);
	
	

	while(1)
	{
		read_size =  read(new_accepted_fd, (void *)read_buffer, 100);
		if(read_size < 0)
		{
			printf("read failed\n");
			return NULL;
		}
		else if(read_size > 0)
		{
			printf("[Server]recv:%s\n",read_buffer);
			if(0 == strncmp(read_buffer,"quit",strlen("quit")))
			{
				break;
			}
			memset(read_buffer,0,100);
			//sleep(1);
		}

	}
	
	ret = close(socket_fd);
	if(ret < 0)
	{
		printf("close socket_fd failed\n");
		return NULL;
	}

	ret = close(new_accepted_fd);
	if(ret < 0)
	{
		printf("close new_accepted_fd failed\n");
		return NULL;
	}
	free(read_buffer);
	free(write_buffer);
	printf("server died\n");

	return NULL;
	

}


void* MAPI_Net_Client(void*)
{
	S32 socket_client_fd = -1;
	S32 ret = -1;

	// 服务器端的信息
	struct sockaddr_in st_addr_in = {0};
	st_addr_in.sin_family = PF_INET;
	st_addr_in.sin_port = htons(6666);
	in_addr_t temp ;
	temp = inet_addr("1.2.3.5");
	//st_addr_in.sin_addr = temp;
	memcpy(&st_addr_in.sin_addr,&temp,sizeof(in_addr_t));
	

	// 1.创建clinet socket
	socket_client_fd = socket(AF_INET,SOCK_STREAM, 0);
	if(socket_client_fd < 0)
	{
		printf("socket client failed\n");
		return NULL;
	}

	
	// 2.客户端通过connect来建立与服务器端的连接
	printf("client:waiting...\n");
	ret = connect(socket_client_fd,(struct sockaddr *)(&st_addr_in),sizeof(st_addr_in));
	if(ret < 0)
	{
		printf("close new_accepted_fd failed\n");
		return NULL;
	}
	printf("client:waiting done...\n");


	int write_size = 0;
	char* read_buffer = NULL;
	read_buffer = (char*)malloc(100);

	char* write_buffer = NULL;
	write_buffer = (char*)malloc(100);
	memset(read_buffer,0,100);
	memset(write_buffer,0,100);
	
	strncpy(write_buffer,"hello_world:this is client", strlen("hello_world:this is client"));

	int i = 10;
	while(i--)
	{
		//printf("[CLIENT:writing...]\n");
		write_size =  write(socket_client_fd, write_buffer, strlen("hello_world:this is client"));
		if(write_size < 0 )
		{
			printf("write failed\n");
			close(socket_client_fd);
			return NULL;
		}
		//printf("[CLIENT:writing done...]\n");
		sleep(1);
	}

	
	memset(write_buffer,0,100);
	strncpy(write_buffer,"quit", strlen("quit"));
	write_size =  write(socket_client_fd, write_buffer, strlen("quit"));
	if(write_size < 0 )
	{
		printf("write failed\n");
		close(socket_client_fd);
		return NULL;
	}
	


	ret = close(socket_client_fd);
	if(ret < 0)
	{
		printf("close socket_client_fd failed\n");
		return NULL;
	}


	free(read_buffer);
	free(write_buffer);
	printf("client died\n");

	return NULL;
}
pthread_t pthread_Net_Server; 

pthread_t pthread_Net_Client; 

COM_RET MAPI_Net_Test()
{
	int ret = -1;
	ret = pthread_create(&pthread_Net_Server ,NULL,MAPI_Net_Server, NULL);
	sleep(1);
	ret = pthread_create(&pthread_Net_Client ,NULL,MAPI_Net_Client, NULL);
	if(ret != 0)
		printf("pthread_create failed\n");
	while(1);
	return COM_SUCCESS;
}



