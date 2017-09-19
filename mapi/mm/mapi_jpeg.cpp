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
#include "mapi_jpeg.h"


#define DEV_NAME "/dev/fb0"
#define JPG_NAME "/home/robin/my_one_4k.jpg"

#define RED    0x00FF0000
#define GREEN  0x0000FF00
#define BLUE   0x000000FF

#define MAX_WIDTH  800
#define MAX_HEIGHT 480

unsigned int* gpfb_mmap = NULL;
unsigned int gu32_fb_len = 0;
struct jpeg_decompress_struct cinfo;
struct jpeg_compress_struct cinfo2;

struct jpeg_error_mgr jerr;
struct jpeg_error_mgr jerr2;

S32 gs32_fb_fd = -1;



std::vector<std::string> vector_jpg;

static COM_RET _decompress(struct jpeg_info *pjpeg,char** pdata,unsigned int* pu32size,const char* filename)
{
	FILE * infile = NULL;
	char *rdata;
	int image_width,image_height,num_components;
	
	cinfo.err = jpeg_std_error(&jerr);
   	jpeg_create_decompress(&cinfo);
	infile = fopen(filename, "r");if(infile == NULL)
	{
		printf("open jpg failed\n");
		return COM_FAILED;
	}
	jpeg_stdio_src(&cinfo,infile);
	jpeg_read_header(&cinfo, TRUE);
	
	image_width = cinfo.image_width;
	image_height = cinfo.image_height;
	num_components = cinfo.num_components;
	
	pjpeg->u32height = image_height;
	pjpeg->u32width = image_width;
	pjpeg->u32num_components = num_components;
	pjpeg->u32scale_num = 0;
	pjpeg->u32scale_denom = 0;

	printf("[Decompress]source img width:%d,height:%d num_components:%d\n",image_width,image_height,num_components);
	// calloc的作用是内存中动态地分配 num 个长度为 size (num_components)的连续空间，并将每一个字节都初始化为 0
	rdata=(char*)calloc(image_width*image_height*num_components,1);
	(*pu32size) = image_width*image_height*num_components;
	(*pdata) = rdata;

	jpeg_start_decompress(&cinfo);

	//JSAMPROW实际上一个char/unsigned char*，指向的是one image row of pixel samples，就是一行图像的起点
	JSAMPROW row_pointer[1];

	while (cinfo.output_scanline < cinfo.output_height)
	{
   		row_pointer[0] = (JSAMPROW)(& rdata[(cinfo.output_scanline)*cinfo.image_width*cinfo.num_components]);
		// 这里将解压的数据放到buffer里面，每次存1行，也可以一次存多行
		jpeg_read_scanlines(&cinfo,row_pointer ,1);
	}

	jpeg_finish_decompress(&cinfo);
	fclose(infile);
	jpeg_destroy_decompress(&cinfo);
	return COM_SUCCESS;
}


static COM_RET _compress(struct jpeg_info stjpeg_info,char* pdata,unsigned int pu32size,const char* filename)
{
	cinfo2.err = jpeg_std_error(&jerr2);
   	jpeg_create_compress(&cinfo2);

	FILE * outfile = NULL;
	//unsigned char *rdata;
	outfile = fopen(filename, "wb");

	if(outfile == NULL)
	{
		printf("open jpg failed\n");
		return COM_FAILED;
	}
	jpeg_stdio_dest(&cinfo2, outfile);
	cinfo2.image_width = stjpeg_info.u32width;                /* image width and height, in pixels */
	cinfo2.image_height = stjpeg_info.u32height;
	cinfo2.input_components = stjpeg_info.u32num_components;         /* # of color components per pixel */
	
	cinfo2.in_color_space = JCS_RGB; 
	jpeg_set_defaults(&cinfo2);

	cinfo2.scale_num=stjpeg_info.u32scale_num;
	cinfo2.scale_denom=stjpeg_info.u32scale_denom;
	printf("scale_num=%d,scale_denom=%d\n",stjpeg_info.u32scale_num,stjpeg_info.u32scale_denom);
	
	jpeg_set_quality(&cinfo2, 100, TRUE);
	jpeg_start_compress(&cinfo2, TRUE);

	
	//int row_stride = 800 * 3; /* JSAMPLEs per row in image_buffer */
	int row_stride = stjpeg_info.u32width * stjpeg_info.u32num_components; /* JSAMPLEs per row in image_buffer */

	JSAMPROW row_pointer[1];
	
	while (cinfo2.next_scanline < cinfo2.image_height)
	{
		row_pointer[0] = (unsigned char*)(&pdata[cinfo2.next_scanline * row_stride]);
		(void)jpeg_write_scanlines(&cinfo2, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo2);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo2);
	return COM_SUCCESS;
}




COM_RET MAPI_Jpeg2Rpg(const char* path)
{
	char *rdata;
	unsigned int size = 0;
	int image_width,image_height,num_components;
	int scale_denom = 1,scale_num = 1;
	bool scaled = false;
	struct jpeg_info stjpeg_info = {0};

	_decompress(&stjpeg_info,&rdata,&size,path);
	image_width = stjpeg_info.u32width;
	image_height= stjpeg_info.u32height;
	num_components = stjpeg_info.u32num_components;
	
	if(image_width > MAX_WIDTH || image_height > MAX_HEIGHT)
	{
		if((image_width/MAX_WIDTH) > (image_height/MAX_HEIGHT))
		{
			scale_denom = image_width/MAX_WIDTH +1;
		}
		else
		{
			scale_denom = image_height/MAX_HEIGHT +1;
		}
		stjpeg_info.u32scale_num = scale_num;
		stjpeg_info.u32scale_denom = scale_denom;
		
		scaled = true;
	}

	int starty = 0,startx = 0;
	int x = 0,y = 0,z = 0;
	unsigned int color = 0;

	if(scaled)
	{
		do
		{
			_compress(stjpeg_info,rdata,size,"/tmp/temp.jpg");
			free(rdata);
			rdata = NULL;
			_decompress(&stjpeg_info,&rdata,&size,"/tmp/temp.jpg");
			printf("scaled\n");
			printf("width:%d height:%d\n",stjpeg_info.u32width,stjpeg_info.u32height);
			image_height = stjpeg_info.u32height;
			image_width = stjpeg_info.u32width;
			if(image_height > MAX_HEIGHT || image_width > MAX_WIDTH)
			{
				printf("out of range!loop!\n");
				if((image_width/MAX_WIDTH) > (image_height/MAX_HEIGHT))
				{
					scale_denom = image_width/MAX_WIDTH +1;
				}
				else
				{
					scale_denom = image_height/MAX_HEIGHT +1;
				}
				//stjpeg_info.u32scale_num = scale_num;
				stjpeg_info.u32scale_num = 1;
				stjpeg_info.u32scale_denom = scale_denom;
		
				scaled = true;
				//	return -1;
			}
		}
		while(image_height > MAX_HEIGHT || image_width > MAX_WIDTH);
		
	}
	// 居中显示
	{
		//先全屏清屏
		memset(gpfb_mmap,0,MAX_WIDTH*MAX_HEIGHT*4);
		for(starty=(MAX_HEIGHT-image_height)/2 ;starty<((MAX_HEIGHT-image_height)/2+image_height);starty++)
		{
			for(startx=(MAX_WIDTH-image_width)/2;startx<((MAX_WIDTH-image_width)/2+image_width);startx++)
			{
				color = 0;
				color = (rdata[z]<<16 )| (rdata[z+1] << 8)| (rdata[z+2] << 0);
				z+=3;
				*(gpfb_mmap+starty*MAX_WIDTH+startx) = color;
			}
		}

	}

	free(rdata);

	return COM_SUCCESS;
}


COM_RET MAPI_Draw_Back(U32 width,U32 hight,U32 color)
{
	int x = 0,y = 0;
	printf("draw_back\n");
	for(y=0;y<hight;y++)
	{
		for(x=0;x<width;x++)
		{
			*(gpfb_mmap+y*width+x) = color;
		}
	}
	return COM_SUCCESS;
}

COM_RET MAPI_Draw_Picture(U32 x,U32 y,U32 width,U32 hight,U32 color)
{
	int startx = 0,starty = 0;
	if(x+width > 800 || y+hight>480)
	{
		printf("Out of ranger...\n");
		return COM_FAILED;
	}
	
	printf("draw_picture\n");
	for(starty=y;starty<(y+hight);starty++)
	{
		for(startx=x;startx<(x+width);startx++)
		{
			*(gpfb_mmap+starty*800+startx) = color;
		}
	}
	return COM_SUCCESS;
}

COM_RET MAPI_FB_Init()
{
	S32 ret = -1;
	
	gs32_fb_fd = open(DEV_NAME,O_RDWR);
	if(gs32_fb_fd < 0)
	{
		printf("open fb failed!return fd is:%d\n",gs32_fb_fd);
		return COM_FAILED;	
	}
	
	struct fb_var_screeninfo st_var_screeninfo = {0};
	struct fb_fix_screeninfo st_fix_screeninfo = {0};
		
	
	ret = ioctl(gs32_fb_fd,FBIOGET_FSCREENINFO,&st_fix_screeninfo);
	if(ret < 0)
	{
		printf("ioctl fix failed!return value:%d\n",ret);
		return COM_FAILED;
	}
	
	printf("smem_start:0x%x,smem_len:%ld\n",st_fix_screeninfo.smem_start,st_fix_screeninfo.smem_len);
	
	printf("mmio_start:0x%x,mmio_len:%ld\n",st_fix_screeninfo.mmio_start,st_fix_screeninfo.mmio_len);
	
	ret = ioctl(gs32_fb_fd,FBIOGET_VSCREENINFO,&st_var_screeninfo);
	if(ret < 0)
	{
		printf("ioctl var failed!return value:%d\n",ret);
		return COM_FAILED; 
	}
	
	printf("xres:%d,yres:%d\n",st_var_screeninfo.xres,st_var_screeninfo.yres);
	printf("x_offset:%d,y_offset:%d\n",st_var_screeninfo.xoffset,st_var_screeninfo.yoffset);
	printf("xres_virtual:%d,yres_virtual:%d\n",st_var_screeninfo.xres_virtual,st_var_screeninfo.yres_virtual);
	printf("bbp:%d\n",st_var_screeninfo.bits_per_pixel);
	
	gu32_fb_len = st_fix_screeninfo.smem_len;
	
	//gpfb_mmap = mmap((void*)st_fix_screeninfo.smem_start,gu32_fb_len,PROT_EXEC,MAP_ANON,-1,0);
	//	gu32_fb_len = st_var_screeninfo.xres_virtual*st_var_screeninfo.yres_virtual*st_var_screeninfo.bits_per_pixel/8;
	
	
	
	printf("gu32_fb_len = %ld\n",gu32_fb_len);
	//printf("gu32_fb_len = %ld\n",st_var_screeninfo.xres_virtual*st_var_screeninfo.yres_virtual*st_var_screeninfo.bits_per_pixel/8);
	void* ptemp = NULL;
	
	ptemp = mmap(NULL, gu32_fb_len, PROT_READ | PROT_WRITE, MAP_SHARED, gs32_fb_fd, 0);
	gpfb_mmap = (unsigned int*)ptemp;
	
	
	if(gpfb_mmap ==(void*) -1)
	{
		printf("mmap failed:ret = %d\n",gpfb_mmap);
		return COM_FAILED;
	}
	
	//ptemp = NULL;
	//ptemp = malloc(gu32_fb_len);
	//if(ptemp == NULL)
	//{
	//	printf("malloc failed\n");
	//	return COM_FAILED;
	//}
	
	//memcpy(ptemp,gpfb_mmap,gu32_fb_len);

	return COM_SUCCESS;
}


COM_RET MAPI_FB_DeInit()
{
	S32 ret = -1;
	printf("******************\n");
	ret = munmap(gpfb_mmap,gu32_fb_len);
	if(ret < 0)
	{
		printf("munmap fix failed!return valure:%d\n",ret);
		return COM_FAILED;
	}
		
	ret = close(gs32_fb_fd);
		
	if(ret < 0)
	{
		printf("close fb failed!return valure:%d\n",ret);
		return COM_FAILED; 
	}
	return COM_SUCCESS;

}

