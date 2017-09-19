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


#include "event_manager.h"
#include "applet.h"


#define DEV_TS_EVENT "/dev/input/event2"



/*

struct ST_Evnet_TS_Up
{};

struct ST_Evnet_TS_Down
{};

struct ST_Evnet_TS_Touch
{};

struct ST_Evnet_TS_Touch_Long
{};

struct ST_Evnet_TS_Release
{};
*/

static time_t  old_time_sec = 0;
static time_t  old_time_usec = 0;

static time_t  new_time_sec = 0;
static time_t  new_time_usec = 0;

pthread_mutex_t mutex_arry_event;
struct ST_Event_TS* garry_event[1] = {NULL};
static bool gbFirstPost = false;
static int garry_x[6]={0};
static int garry_y[6]={0};
static int garry_msec[6]={0};




static void _post_event(enum EN_Event_TS_Type en_event_ts_type,int u32x,int u32y)
{
	int ret = -1;
	ret = pthread_mutex_lock(&mutex_arry_event);
	if(ret != 0)
	{
		printf("[POST]lock failed...\n");
	}
	if(garry_event[0] == NULL)
	{
		struct ST_Event_TS* pst_Event_Ts = NULL;
		pst_Event_Ts = (struct ST_Event_TS*)malloc(sizeof(struct ST_Event_TS));
		pst_Event_Ts->en_event_ts_type = en_event_ts_type;
		pst_Event_Ts->u32x = u32x;
		pst_Event_Ts->u32y = u32y;
		printf("[POST]...\n");
		garry_event[0] = pst_Event_Ts;
	}
	else
	{
		printf("Event queue is FULL\n");
	}
	ret = pthread_mutex_unlock(&mutex_arry_event);
	if(ret != 0)
	{
		printf("[POST]unlock failed...\n");
	}
}


void *Check_TS_Event(void *)
{
	struct input_event st_input_event = {0};
	int fd = -1;
	fd = open(DEV_TS_EVENT,O_RDWR);  
	int ret = -1;
	int x = 0,y = 0,dx = 0,dy = 0;
	int times = 0;
	int dms = 0;
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
			x = st_input_event.value;
			read(fd,&st_input_event,sizeof(st_input_event));
			if(st_input_event.type == ABS_RX && st_input_event.code == ABS_MT_POSITION_Y)
			{
				//read(fd,&st_input_event,sizeof(st_input_event));//type == 0x30
				//read(fd,&st_input_event,sizeof(st_input_event));// type == 0x39
				//等待发送SYN_REPORT，表示数据发送结束
				y = st_input_event.value;
				do
				{
					read(fd,&st_input_event,sizeof(st_input_event));
					if(st_input_event.type==SYN_REPORT)
					{
						if(gbFirstPost == false)
						//if(0)
						{
							_post_event(EVENT_TS_TOUCH,x,y);
							old_time_sec =  st_input_event.time.tv_sec;
							old_time_usec =  st_input_event.time.tv_usec;
							gbFirstPost = true;
						}
						else
						{
							new_time_sec = st_input_event.time.tv_sec;
							new_time_usec = st_input_event.time.tv_usec;
							
							if((((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000)) < 50))
							{
								//printf("111:times=%d,ms=%d\n",times,((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000)));
								//printf("x:%d,y:%d,time=%d,size=%d\n",x,y,times,sizeof(garry_x));
								//printf("Event not Out...:%fs\n",((float)((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000))/1000));
								//if(times < sizeof(garry_x))
								if(times < 6)
								{
									garry_x[times]=x;
									garry_y[times]=y;
									times++;
									old_time_sec = new_time_sec;
									old_time_usec = new_time_usec;
								}
								else
								{
									dx = garry_x[5]-garry_x[0];
									dy = garry_y[5]-garry_y[0];
									//printf("dx=%d,dy=%d\n",dx,dy);
									// right
									if(dx >= 70 && abs(dy) < 20)
									{
										_post_event(EVENT_TS_RIGHT,dx,dy);
									}
									// left
									else if(dx <= -70 && abs(dy) < 20)
									{
										_post_event(EVENT_TS_LEFT,dx,dy);
									}
									// up
									else if(dy <= -70 && abs(dx) < 20)
									{
										_post_event(EVENT_TS_UP,dx,dy);
									}
									//down
									else if(dy >= 70 && abs(dx) < 20)
									{
										_post_event(EVENT_TS_DOWN,dx,dy);
									}
									//else if(abs(dx) < 15 && abs(dy) < 15)
									//{
									//	_post_event(EVENT_TS_TOUCH,dx,dy);
									//}
									else
									{
										//printf("Fuck...\n");
									}
									times=0;
									memset(garry_x,0,sizeof(garry_x));
									memset(garry_y,0,sizeof(garry_y));
									old_time_sec = new_time_sec;
									old_time_usec = new_time_usec;
								}
							}
							else
							{
								//printf("222:times=%d,ms=%d\n",times,((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000)));
								//printf("[POST]:%ds%dus\n",new_time_sec - old_time_sec,new_time_usec - old_time_usec);
								//printf("[POST]:%fs\n",((float)((new_time_sec*1000+new_time_usec/1000)-(old_time_sec*1000+old_time_usec/1000))/1000));
								_post_event(EVENT_TS_TOUCH,x,y);
								times=0;
								memset(garry_x,0,sizeof(garry_x));
								memset(garry_y,0,sizeof(garry_y));
								old_time_sec = new_time_sec;
								old_time_usec = new_time_usec;
							}
						}
					}
				}
				while(!st_input_event.type==SYN_REPORT);

			}
		}
		else
		{
			x=0;
			y=0;
			//printf("Do Nothing...\n");
		}



	}

	close(fd);
	
	return NULL;
	
}






