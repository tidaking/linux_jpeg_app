#ifndef __EVENTMANAGER_H__

#define __EVENTMANAGER_H__


//pthread_mutex_t mutex_arry_event;
//struct ST_Event_TS* garry_event[1] = {NULL};


void *Check_TS_Event(void *);

enum EN_Event_TS_Type
{
	EVENT_TS_UP = 0,
	EVENT_TS_DOWN,
	EVENT_TS_LEFT,
	EVENT_TS_RIGHT,
	EVENT_TS_TOUCH,
	EVENT_TS_LONG,
	EVENT_TS_RELEASE,
};

struct ST_Event_TS
{
	//struct input_event st_linux_input_event;
	enum EN_Event_TS_Type en_event_ts_type;
	int u32x;
	int u32y;
};



#endif
