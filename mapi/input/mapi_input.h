#ifndef __MAPI_INPUT_H__

#define __MAPI_INPUT_H__

#include "com_type.h"
#include "com_error.h"
#include "com_event.h"

/*
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
*/

struct ST_abs_data
{
	U32 x;
	U32 y;
};



COM_RET MAPI_Input_RegisterCB(EventCallback fpcb,EN_Event event);
COM_RET MAPI_Input_UnRegisterCB(EventCallback fpcb,EN_Event event);


COM_RET MAPI_Input_Start_Get_TS_Event();
COM_RET MAPI_Input_Stop_Get_TS_Event();
//static void *Check_TS_Event(void *);


#endif

