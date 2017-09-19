#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "event_manager.h"
#include "applet.h"


pthread_t pthread_TS_Evnet_Manager; 
pthread_t pthread_FB_Applet; 

extern pthread_mutex_t mutex_arry_event;
extern struct ST_Event_TS* garry_event[1];

int main()
{
	// 1.创建采集TS(并以Event形式发送出去)线程

	int ret = -1;
	ret =  pthread_mutex_init(&mutex_arry_event,NULL);
	if(ret != 0)
	{
		printf("pthread_create failed:%d\n",ret);
		return -1;
	}

	ret =  pthread_create(&pthread_FB_Applet ,NULL,Get_TS_Event, NULL);
	if(ret != 0)
	{
		printf("pthread_create FB failed:%d\n",ret);
		return -1;
	}
	
	ret =  pthread_create(&pthread_TS_Evnet_Manager ,NULL,Check_TS_Event, NULL);
	if(ret != 0)
	{
		printf("pthread_create TS failed:%d\n",ret);
		return -1;
	}



	while(1);
	return 0;
}



