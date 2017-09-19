#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "applet.h"
#include "event_manager.h"

extern pthread_mutex_t mutex_arry_event;
extern struct ST_Event_TS* garry_event[1];

void *Get_TS_Event(void *pData)
{
	int ret = -1;
	char str[5] = {0};
	while(1)
	{
		usleep(1*1*1000);

		ret = pthread_mutex_lock(&mutex_arry_event);
		if(ret != 0)
		{
			printf("[GET]lock failed...\n");
		}
		if(garry_event[0] == NULL)
		{
		}
		else
		{
			switch(garry_event[0]->en_event_ts_type)
			{
				case EVENT_TS_UP:
					strncpy(str,"UP",strlen("UP"));
					break;
				case EVENT_TS_DOWN:
					strncpy(str,"DOWN",strlen("DOWN"));
					break;
				case EVENT_TS_LEFT:
					strncpy(str,"LEFT",strlen("LEFT"));
					break;
				case EVENT_TS_RIGHT:
					strncpy(str,"RIGHT",strlen("RIGHT"));
					break;
				case EVENT_TS_TOUCH:
					strncpy(str,"TOUCH",strlen("TOUCH"));
					break;
				default:
					strncpy(str,"UN",strlen("UN"));
					
			}
			printf("[GET]event=%s,x=%d,y=%d\n",str,garry_event[0]->u32x,garry_event[0]->u32y);
			memset(str,0,sizeof(str));
			free(garry_event[0]);
			garry_event[0] = NULL;
		}
		ret = pthread_mutex_unlock(&mutex_arry_event);
		if(ret != 0)
		{
			printf("[GET]unlock failed...\n");
		}
	}
	return NULL;
	
	
}


