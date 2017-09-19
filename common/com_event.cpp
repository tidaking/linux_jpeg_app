#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <vector>

#include "com_type.h"
#include "com_error.h"
#include "com_event.h"

//#define EVENT_LIST_MAX_NUM 10

EventManager gEventManager;



EventManager::EventManager()
{
	printf("%s\n",__func__);
}

EventManager::~EventManager()
{
	printf("%s\n",__func__);
}


void EventManager::sendEvent(EN_Event en_event,void* data)
{
	unsigned long size = vector_touch.size();
	printf("%s size=%ld\n",__func__,size);
	int i = 0;
	if(en_event == EVENT_TOUCH)
	{
		for(i=0;i<size;i++)
		{
			if(vector_touch[i] != NULL)
			{
				vector_touch[i](data);
			}
		}
	}
}




EventClient::EventClient()
{
		printf("%s\n",__func__);
}

EventClient::~EventClient()
{
		printf("%s\n",__func__);
}

COM_RET EventClient::register_callback(EventCallback func,EN_Event en_event)
{
	//if(pobjclient != this->pobjclient)
	//{
	//	printf("ERR!wrong obj\n");
	//	return -1;
	//}
	if(func == NULL)
	{
		printf("ERR!cb pointer is NULL!\n");
		return COM_PTR_NULL_ERR;
	}
	switch(en_event)
	{
		case EVENT_TOUCH:
			gEventManager.vector_touch.push_back(func);
			break;
		case EVENT_UP:
			gEventManager.vector_up.push_back(func);
			break;
		case EVENT_DOWN:
			gEventManager.vector_down.push_back(func);
			break;
		case EVENT_LEFT:
			gEventManager.vector_left.push_back(func);
			break;
		case EVENT_RIGHT:
			gEventManager.vector_right.push_back(func);
			break;
		case EVENT_LONG_TIME:
			gEventManager.vector_long.push_back(func);
			break;
		default:
			printf("[%s][%d] faild\n",__func__,__LINE__);
			return COM_FAILED;
	}
	


	return 0;
}

COM_RET EventClient::unregister_callback(EventCallback func,EN_Event en_event)
{
	if(func == NULL)
	{
		printf("ERR!cb pointer is NULL!\n");
		return COM_PTR_NULL_ERR;
	}
	int size = 0;
	// 迭代器iter，它指向一个vector，这个vector里面装的是EvenCallback
	std::vector<EventCallback>::iterator iter ;
	switch(en_event)
	{
		case EVENT_TOUCH:
			size = gEventManager.vector_touch.size();
			iter = gEventManager.vector_touch.begin();
			break;
		case EVENT_UP:
			size = gEventManager.vector_up.size();
			iter = gEventManager.vector_up.begin();
			break;
		case EVENT_DOWN:
			size = gEventManager.vector_down.size();
			iter = gEventManager.vector_down.begin();
			break;
		case EVENT_LEFT:
			size = gEventManager.vector_left.size();
			iter = gEventManager.vector_left.begin();
			break;
		case EVENT_RIGHT:
			size = gEventManager.vector_right.size();
			iter = gEventManager.vector_right.begin();
			break;
		case EVENT_LONG_TIME:
			size = gEventManager.vector_long.size();
			iter = gEventManager.vector_long.begin();
			break;
		default:
			printf("[%s][%d] faild\n",__func__,__LINE__);
			return COM_FAILED;
	}
	
	int i = 0;
	for(i=0;i<size;i++)
	{
		if(iter[i] == func)
		{
			iter[i] = NULL;
			printf("found it:%d!\n",i);
			return 0;
		}
	}
	printf("Not found\n");
	return COM_FAILED;
}




static void test_cb(void* pdata)
{
	printf("this is test_cb\n");
}
static void test_cb1(void* pdata)
{
	printf("this is test_cb1\n");
}
static void test_cb2(void* pdata)
{
	printf("this is test_cb2\n");
}
static void test_cb3(void* pdata)
{
	printf("this is test_cb3\n");
}

static void test_cb4(void* pdata)
{
	printf("this is test_cb4\n");
}


static void *Get_TS_Event_1(void *pData)
{
	EventClient test_client;
	test_client.register_callback(test_cb,EVENT_TOUCH);
	test_client.register_callback(test_cb1,EVENT_UP);
	test_client.register_callback(test_cb2,EVENT_DOWN);
	

	while(1);
	return NULL;
}

static void *Get_TS_Event_2(void *pData)
{
	EventClient test_client2;
	test_client2.register_callback(test_cb3,EVENT_TOUCH);
	test_client2.register_callback(test_cb4,EVENT_RIGHT);

	while(1);
	return NULL;
}


static int test_main()
{
	int ret = -1;
	pthread_t pthread_EvnetClient_Manager; 

	ret =  pthread_create(&pthread_EvnetClient_Manager ,NULL,Get_TS_Event_1, NULL);
	if(ret != 0)
	{
		printf("pthread_create 1 failed:%d\n",ret);
		return -1;
	}
	ret =  pthread_create(&pthread_EvnetClient_Manager ,NULL,Get_TS_Event_2, NULL);
	if(ret != 0)
	{
		printf("pthread_create 2 failed:%d\n",ret);
		return -1;
	}

	sleep(1);
	//test_client->sendEvent(EVENT_TOUCH);
	while(1)
	{
		gEventManager.sendEvent(EVENT_TOUCH,NULL);
		sleep(1);
	}
	
	return 0;
}


