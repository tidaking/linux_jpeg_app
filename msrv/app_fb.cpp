#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/input.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>


#include <string>
#include <vector>

//extern "C"
//{
#include <jpeglib.h>
#include <jerror.h>
#include <string.h>
#include <stdlib.h>
//}
#include "com_type.h"
#include "com_error.h"
#include "com_event.h"


#include "mapi_jpeg.h"
#include "mapi_input.h"


#define RED    0x00FF0000
#define GREEN  0x0000FF00
#define BLUE   0x000000FF

extern std::vector<std::string> vector_jpg;
extern EventManager gEventManager;



int readinput(char* path);


COM_RET _show_dir(char* path)
{  
	int filesize = 0;	  
	DIR *dirptr=NULL;	
	int i=1;  
	int len = 0;
	char pathname[200]={0};
	memcpy(pathname,path,strlen(path));
	struct dirent *entry;    
	if((dirptr = opendir(path))==NULL)  
	{  
		printf("opendir:%s failed!\n",path);  
		return COM_FAILED;  
	}
	else  
	{   
		while(entry=readdir(dirptr))	
	  	{  
	   		//printf("filename%d = %s  ",i,entry->d_name);  //输出文件或者目录的名称，一般文件含格式名，目录没有格式名  
	   		//printf("filetype = %d\n",entry->d_type);  //输出文件类型  
	   		//i++;  
	   		len= strlen(entry->d_name);
			if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0)
			{
				continue;
			}
			if(entry->d_type == DT_DIR)
			{
				strcpy(pathname+strlen(pathname),"/");
				strcpy(pathname+strlen(pathname),entry->d_name);
				printf("*************enter DIR:%s ***********\n",pathname);
				//printf("dir:%s\n",entry->d_name);
				_show_dir(pathname);
				printf("*************leave DIR:%s ***********\n",pathname);
				memset(pathname,0,sizeof(pathname));
				memcpy(pathname,path,strlen(path));
				
			}
	   		if((strcmp(".jpg",entry->d_name+len-4) == 0) ||\
				(strcmp(".JPG",entry->d_name+len-4) == 0))
			{
				//printf("filename:%s\n",entry->d_name);
				std::string jpgfilename(path);
				jpgfilename.append("/");
				jpgfilename.append(entry->d_name);
				const char* jpg = NULL;
				jpg = jpgfilename.c_str();
				//printf("filename:%s\n",jpgfilename);
				printf("filename:%s\n",jpg);
				vector_jpg.push_back(jpgfilename);
	   		}
			
	  	}  
		closedir(dirptr);  
	  
	}	
	return COM_SUCCESS;	
} 


static void test_cb_robin(void* pdata)
{
	printf("this is test_cb robin\n");
}
static void test_cb_touch(void* pdata)
{
	struct ST_abs_data* pabs = NULL;
	static U32 i=0;
	U32 jpg_size = 0;
	jpg_size = vector_jpg.size();
	if(pdata == NULL)
	{
		printf("PTR is NULL\n");
	}
	else
	{
		pabs = (struct ST_abs_data*)pdata;
		U32 x = 0;
		U32 y = 0;
		x = pabs->x;
		y = pabs->y;
		printf("this is test_cb_touch,x=%d,y=%d\n",x,y);

		if(i >= jpg_size)
		{
			i = 0;
		}
		MAPI_Jpeg2Rpg(vector_jpg[i++].c_str());
	}
	
}




int main()
{

	MAPI_FB_Init();
	_show_dir("/home/robin");

	//EventClient test_client;
	//test_client.register_callback(test_cb_robin,EVENT_TOUCH);
	//while(1)
	//{
	//	gEventManager.sendEvent(EVENT_TOUCH);
	//	sleep(1);
	//}
	MAPI_Input_RegisterCB(test_cb_touch,EVENT_TOUCH);
	MAPI_Input_Start_Get_TS_Event();
	sleep(5);
	MAPI_Input_Stop_Get_TS_Event();
	MAPI_Input_UnRegisterCB(test_cb_touch,EVENT_TOUCH);
	MAPI_FB_DeInit();
	sleep(5);
	return COM_SUCCESS;
}


int readinput()
{
	struct input_event st_input_event = {0};
	int fd = open("/dev/input/event2",O_RDWR);  
	int value ;  
    int type ;  
    int i ;  
    int buffer[10]={0};  
    if(-1 == fd){  
        printf("open mouse event fair!\n");  
        return -1 ;  
    }  

	bool bflag = true;
	int u32time = 0;
	while(1)
	{
		read(fd,&st_input_event,sizeof(st_input_event));
		printf("type:%d code=%d value=%d\n",st_input_event.type,st_input_event.code,st_input_event.value);
		if(st_input_event.type == ABS_RX && \
			st_input_event.code == ABS_MT_POSITION_X &&\
			st_input_event.value >=0 &&st_input_event.value <=100)
			{
				read(fd,&st_input_event,sizeof(st_input_event));
				if(st_input_event.type == ABS_RX && \
					st_input_event.code == ABS_MT_POSITION_Y &&\
				st_input_event.value >=0 &&st_input_event.value <=100)
				{
					u32time++;
					if(u32time >=8)
					{
						//jpeg2rgp(bflag);
						bflag = !bflag;
						u32time = 0;
						//sleep(1);
					}
				}
			}

	}

	close(fd);
	
	return 0;
	
}



