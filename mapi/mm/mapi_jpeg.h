#ifndef __MAPI_JPEG_H__
#define __MAPI_JPEG_H__

struct jpeg_info
{
	unsigned int u32width;
	unsigned int u32height;
	unsigned int u32num_components;
	unsigned int u32scale_num;
	unsigned int u32scale_denom;
};




COM_RET MAPI_Jpeg2Rpg(const char* path);
COM_RET MAPI_Draw_Back(U32 width,U32 hight,U32 color);
COM_RET MAPI_Draw_Picture(U32 x,U32 y,U32 width,U32 hight,U32 color);
COM_RET MAPI_FB_Init();
COM_RET MAPI_FB_DeInit();



#endif
