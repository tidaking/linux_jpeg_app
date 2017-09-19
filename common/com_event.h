#ifndef __COM_EVENT_H__
#define __COM_EVENT_H__
#include <vector>

//EventManager gEventManager;

typedef void (*EventCallback)(void* pdata);

enum EN_Event
{
	EVENT_TOUCH = 0,
	EVENT_UP,
	EVENT_DOWN,
	EVENT_LEFT,
	EVENT_RIGHT,
	EVENT_LONG_TIME,

	EVENT_MAX_NUM,
};



//class EventManager;

class EventClient
{
	public:
		COM_RET register_callback(EventCallback func,EN_Event en_event);
		COM_RET unregister_callback(EventCallback func,EN_Event en_event);
		//void sendEvent(EN_Event en_event);
		EventClient();
		~EventClient();
};



class EventManager
{
	public:
		EventManager();
		~EventManager();
		void sendEvent(EN_Event en_event,void* data);
		
		// 友元，以便其他函数能够访问到private成员变量
		//friend EventClient;
		friend COM_RET EventClient::register_callback(EventCallback func,EN_Event en_event);
		friend COM_RET EventClient::unregister_callback(EventCallback func,EN_Event en_event);
	private:
		std::vector<EventCallback> vector_touch;
		std::vector<EventCallback> vector_up;
		std::vector<EventCallback> vector_down;
		std::vector<EventCallback> vector_left;
		std::vector<EventCallback> vector_right;
		std::vector<EventCallback> vector_long;
};



#endif

