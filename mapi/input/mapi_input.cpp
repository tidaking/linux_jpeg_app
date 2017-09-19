#include <stdio.h>
#include <linux/input.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


#include "com_type.h"
#include "com_error.h"
#include "com_event.h"
#include "mapi_input.h"


#define DEV_TS_EVENT "/dev/input/event2"



static time_t  old_time_sec = 0;
static time_t  old_time_usec = 0;

static time_t  new_time_sec = 0;
static time_t  new_time_usec = 0;

//pthread_mutex_t mutex_arry_event;

//struct ST_Event_TS* garry_event[1] = {NULL};

static bool gbFirstPost = false;

pthread_t pthread_TS_Evnet_Manager; 
EventClient input_event_client;

extern EventManager gEventManager;




static void *_Check_TS_Event(void *);


//static void _post_event(enum EN_Event_TS_Type en_event_ts_type,int u32x,int u32y)
static void _post_event(EN_Event en_event_ts_type,struct ST_abs_data* pabs)
{
	//printf("[POST] type:%d x:%d y:%d\n",en_event_ts_type,u32x,u32y);
	gEventManager.sendEvent(en_event_ts_type,(void*)pabs);
}




COM_RET MAPI_Input_RegisterCB(EventCallback fpcb,EN_Event event)
{
	printf("[%s]\n",__func__);
	input_event_client.register_callback(fpcb,event);
	return COM_SUCCESS;
}

COM_RET MAPI_Input_UnRegisterCB(EventCallback fpcb,EN_Event event)
{
	printf("[%s]\n",__func__);
	input_event_client.unregister_callback(fpcb,event);
	return COM_SUCCESS;
}



COM_RET MAPI_Input_Start_Get_TS_Event()
{
	S32 ret = -1;
	printf("[%s]\n",__func__);
	ret = pthread_create(&pthread_TS_Evnet_Manager ,NULL,_Check_TS_Event, NULL);
	if(ret != 0)
	{
		printf("pthread_create failed:%d\n",ret);
		return COM_FAILED;
	}
	return COM_SUCCESS;
}

COM_RET MAPI_Input_Stop_Get_TS_Event()
{
	S32 ret = -1;
	printf("[%s]\n",__func__);
	ret = pthread_cancel(pthread_TS_Evnet_Manager);
	if(ret != 0)
	{
		printf("pthread_cance failed:%d\n",ret);
		return COM_FAILED;
	}
	return COM_FAILED;
}



static void *_Check_TS_Event(void *)
{
	struct input_event st_input_event = {0};
	int fd = -1;
	fd = open(DEV_TS_EVENT,O_RDWR);  
	int ret = -1;
	struct ST_abs_data abs_data = {0};
	//int x = 0,y = 0;
	if(-1 == fd)
	{  
		printf("open TS event fair!\n");  
		return NULL;  
	}
	while(1)
	{
		read(fd,&st_input_event,sizeof(st_input_event));
		//printf("type:%d code=%d value=%d\n",st_input_event.type,st_input_event.code,st_input_event.value);
		if(st_input_event.type == ABS_RX && st_input_event.code == ABS_MT_POSITION_X)
		{
			abs_data.x = st_input_event.value;
			read(fd,&st_input_event,sizeof(st_input_event));
			if(st_input_event.type == ABS_RX && st_input_event.code == ABS_MT_POSITION_Y)
			{
				//read(fd,&st_input_event,sizeof(st_input_event));//type == 0x30
				//read(fd,&st_input_event,sizeof(st_input_event));// type == 0x39
				//等待发送SYN_REPORT，表示数据发送结束
				abs_data.y = st_input_event.value;
				do
				{
					read(fd,&st_input_event,sizeof(st_input_event));
					if(st_input_event.type==SYN_REPORT)
					{
						if(gbFirstPost == false)
						//if(0)
						{
							_post_event(EVENT_TOUCH,&abs_data);
							old_time_sec =  st_input_event.time.tv_sec;
							old_time_usec =  st_input_event.time.tv_usec;
							gbFirstPost = true;
						}
						else
						{
							new_time_sec = st_input_event.time.tv_sec;
							new_time_usec = st_input_event.time.tv_usec;
							//50ms 内发生2次MT_SYNC事件
							if((((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000)) < 200))
							{
								old_time_sec = new_time_sec;
								old_time_usec = new_time_usec;								
							}
							else
							{
								_post_event(EVENT_TOUCH,&abs_data);
								old_time_sec = new_time_sec;
								old_time_usec = new_time_usec;
							}
						}
					}
				}
				while(!st_input_event.type==SYN_REPORT);

			}
		}
	}

	close(fd);
	
	return NULL;
	
}







