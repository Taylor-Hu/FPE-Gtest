/**
@file OSD_osd.cpp
@def group osd测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/

/*测试集名称定义*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif

#define CASE_NAME TestOsd


#include "test_menu.h"
#include "test_modules.h"

static CHDRV_OSD_RegionHandle gh_MainRegion = NULL, gh_CacheRegion = NULL;	

#define OSD_USEEABLE					(1) //可以不用，只为方便测试使用
#define OSD_SMOOTH 						(1) //流畅测试

#define OSD_ENV_WIDTH					((S32)fw_getenv_i("OSD_REGION_WIDTH"))
#define OSD_ENV_HEIGHT 					((S32)fw_getenv_i("OSD_REGION_HEIGHT"))

#define OSD_PIC_ENV_WIDTH				((S32)fw_getenv_i("OSD_PIC_REGION_WIDTH"))
#define OSD_PIC_ENV_HEIGHT 				((S32)fw_getenv_i("OSD_PIC_REGION_HEIGHT"))





#define OSD_HANDLE  					((void *)0x87650000) //bmphandle(the first region handle) 一般用作有效句柄
#define OSD_HANDLE_INVALID   			((void *)0x87650129)  //handle count max=128; 超出句柄最大值，一般作无效参数


#define OSD_PARAM_RIGHT					(200)     // x<1280 y<720
#define OSD_PARAM_MINUS    				(-1)		//参数为负数，一般作为无效参数
#define OSD_PARAM_OVER_WRANGE 	 		(OSD_PIC_ENV_WIDTH + 1) 	//  超出窗口宽度的赋值，作为错误参数测试之一
#define OSD_PARAM_OVER_HRANGE  			(OSD_PIC_ENV_HEIGHT + 1)  //  超出窗口高度的赋值，作为错误参数测试之一

#define OSD_GIFDEC_COUNT_INVALID   		(17)  //GIFDEC_COUNT=16
#define OSD_GIFDEC_RIGHT   				(1)   //GIFDEC_COUNT=16

#define OSD_BOOL_FLASE     				(1)   // bool flase
#define OSD_B00L_TRUE					(0)   // bool true 



#define OSD_PIC_PATH					(fw_getenv("OSD_BMP_FILEPATH"))  //I帧图片路径

#define OSD_GIF_PATH					(fw_getenv("OSD_GIF_FILEPATH"))  	 	//原图的相对路径
#define OSD_GIF_WIDTH 					((S32)fw_getenv_i("OSD_GIF_WIDTH"))		//原图的宽
#define OSD_GIF_HEIGHT					((S32)fw_getenv_i("OSD_GIF_HEIGHT"))	//原图的高


#define OSD_JPEG_PATH					(fw_getenv("OSD_JPEG_FILEPATH"))
#define OSD_JPEG_WIDTH 					((S32)fw_getenv_i("OSD_JPG_WIDTH"))
#define OSD_JPEG_HEIGHT					((S32)fw_getenv_i("OSD_JPG_HEIGHT"))



#define OSD_PNG_PATH					(fw_getenv("OSD_PNG_FILEPATH"))
#define OSD_PNG_WIDTH 					((S32)fw_getenv_i("OSD_PNG_WIDTH"))
#define OSD_PNG_HEIGHT					((S32)fw_getenv_i("OSD_PNG_HEIGHT"))


#define OSD_BMP_PATH					(fw_getenv("OSD_BMP_FILEPATH"))
#define OSD_BMP_WIDTH 					((S32)fw_getenv_i("OSD_BMP_WIDTH"))
#define OSD_BMP_HEIGHT					((S32)fw_getenv_i("OSD_BMP_HEIGHT"))



#define OSD_MEM_WIDTH					(480)     	//用作 bitblit时创建的内存块或region的宽
#define OSD_MEM_HEIGHT					(320)		//用作 bitblit时创建的内存块或region的高





#define OSD_REGION_COLOR4444_WHITE		(0xFFFF)   //argb  4444，白色
#define OSD_REGION_COLOR4444_RED		(0xFF00)   //argb  4444，红色
#define OSD_REGION_COLOR4444_GREEN		(0xF0F0)   //argb  4444，绿色
#define OSD_REGION_COLOR4444_BLUE		(0xF00F)   //argb  4444，蓝色
#define OSD_REGION_COLOR4444_BPP		(2)		   //argb  4444 region 的 byte per pixel
#define OSD_REGION_COLOR_CLEAR			(0x0)      // 无色


#define OSD_REGION_COLOR8888_WHITE		(0xFFFFFFFF)   //argb  8888，白色
#define OSD_REGION_COLOR8888_RED		(0xFFFF0000)   //argb  8888，红色
#define OSD_REGION_COLOR8888_GREEN		(0xFF00FF00)   //argb  8888，绿色
#define OSD_REGION_COLOR8888_BLUE		(0xFF0000FF)   //argb  8888，蓝色


#define OSD_REGION_COLOR1555_WHITE		(0xFFFF)    //argb 1555 白色
#define OSD_REGION_COLOR1555_RED		(0xFC00)    //argb 1555 红色



static CHDRV_OSD_RegionHandle	     grh_BmpDrawHandle = NULL;	//用来显示I帧图片的draw region handle
static CHDRV_OSD_RegionHandle 	  grh_BmpDisplayHandle = NULL;	//用来显示I帧图片的display region handle

static CHDRV_OSD_RegionHandle	 	grh_DrawHandleOsd1 = NULL;   //osd1的draw region handle
static CHDRV_OSD_RegionHandle 	 grh_DisplayHandleOsd1 = NULL;	//osd1的display region handle

static CHDRV_OSD_RegionHandle	 	 grh_DrawHandleMem = NULL;	//以MEM的长宽为参数的region handle

static CHDRV_OSD_RegionHandle	    grh_DrawHandleOsd2 = NULL;    //osd2的draw region handle
static CHDRV_OSD_RegionHandle	 grh_DisplayHandleOsd2 = NULL;    //osd2的display region handle

static U8 *	 gpuc_Data = NULL;	//get area时用来存数据以供put area时使用的内存块指针
static void * 	 gpuc_Mem = NULL;	//用作bufbitblit时创建的内存块的指针

extern int osd_info(int argc, CMD_PARAM argv[]);



static void OSD_Test_End(void)
{
	grh_BmpDrawHandle 	    = NULL;
	grh_BmpDisplayHandle	= NULL;
	grh_DrawHandleOsd1		= NULL;
	grh_DisplayHandleOsd1	= NULL;
	grh_DrawHandleOsd2		= NULL;
	grh_DisplayHandleOsd2	= NULL;

}

//检查region是否还存在，如果不存在则重新创建,避免在测试过程中因为重启而导致空句柄,影响测试结果
static int InitRegion(CHDRV_OSD_COLOR_TYPE_e color_type, CHDRV_OSD_RegionHandle*rph_DispRegion, CHDRV_OSD_RegionHandle *rph_DrawRegion, int color)
{
	CHDRV_OSD_RegionParam_t rstru_Params;
	int ret;

	if(*rph_DispRegion || *rph_DrawRegion)
	{
		return 0;
	}
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= color_type;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;

	ret = CHDRV_OSD_CreateRegion(&rstru_Params,rph_DispRegion) ;
	rstru_Params.enm_RegionType 	 = CHDRV_OSD_REGION_DRAW;
	ret |= CHDRV_OSD_CreateRegion(&rstru_Params,rph_DrawRegion) ;
	ret |= CHDRV_OSD_LinkRegion(*rph_DrawRegion, *rph_DispRegion);
	ret |= CHDRV_OSD_ShowRegion(*rph_DrawRegion);
	ret |= CHDRV_OSD_ClearFullRegion(*rph_DrawRegion, color);
	ret |= CHDRV_OSD_UpdateRegion(*rph_DrawRegion);

	return ret;
}




#if ENABLE_TESTCASE
//对比某区域的数据是否符合预期
/*
void *					rpAreaHandle,//对比区域的所属的region句柄
S32 					ri_PosX, 	//对比区域的x,y,w,h
S32 					ri_PosY,
S32 					ri_Width,
S32 					ri_Height,
void  					rp_Data		 //预期数据
CHDRV_OSD_COLOR_TYPE_e  enm_ColorType //预期数据

*/
//对比某区域的数据是否符合预期，预期数据只有一种值
static CHDRV_OSD_RESULT_e Osd_Compare_SingleData(
	void * 	rpAreaHandle,    
	S32         ri_PosX,
	S32         ri_PosY,
	S32         ri_Width,
	S32         ri_Height,
	void *  	rpv_Data,
	CHDRV_OSD_COLOR_TYPE_e  enm_ColorType)
{
	U8*	 rpuc_Data = NULL;//
	S32   i_Loop = 0;//
	if(enm_ColorType == CHDRV_OSD_COLOR_TYPE_ARGB1555 || enm_ColorType ==CHDRV_OSD_COLOR_TYPE_ARGB4444)
	{
		rpuc_Data = (U8 *)CHDRV_OSD_AllocateMemory(ri_Width*ri_Height*2);
		if(rpuc_Data != NULL)
		{
			EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(rpAreaHandle,ri_PosX,ri_PosY,ri_Width,ri_Height,(U8 *)rpuc_Data));
			for(i_Loop = 0;i_Loop < ri_Width*ri_Height;i_Loop ++)
			{
				if( *((U16 *)rpuc_Data + i_Loop ) != *((U16 *)rpv_Data) )
				{
					LOG("\n %x VS %x \n", *((U16 *)rpv_Data),*((U16 *)rpuc_Data + i_Loop ));
					break;
				}
			}
		}
	}
	else if(enm_ColorType == CHDRV_OSD_COLOR_TYPE_ARGB8888)
	{
		rpuc_Data = (U8 *)CHDRV_OSD_AllocateMemory(ri_Width*ri_Height*4);
		if(rpuc_Data != NULL)
		{
			EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(rpAreaHandle,ri_PosX,ri_PosY,ri_Width,ri_Height,(U8 *)rpuc_Data));
			for(i_Loop = 0;i_Loop < ri_Width*ri_Height;i_Loop ++)
			{
				if( *((U32 *)rpuc_Data+i_Loop ) != *((U32 *)rpv_Data) )
				{
					LOG("\n %x VS %x \n", *((U32 *)rpv_Data),*(rpuc_Data + i_Loop ));
					break;
				}
			}
		}
	}
	else
	{
		LOG("add this Colortype = %d \n",enm_ColorType);//
	}
	if(rpuc_Data != NULL)
	{
		CHDRV_OSD_FreeMemory(rpuc_Data);
	}
	if(ri_Width*ri_Height != i_Loop)
	{
		LOG("\n data is not correct \n");
		return CHDRV_OSD_FAIL;
	}

	return CHDRV_OSD_OK;
}



/*
void *					rpAreaHandle,//对比区域的所属的region句柄
S32 					ri_PosX, 	//对比区域的x,y,w,h
S32 					ri_PosY,
S32 					ri_Width,
S32 					ri_Height,
void * 					rp_Data		//预期数据
CHDRV_OSD_COLOR_TYPE_e  enm_ColorType //图形的数据类型

*/
//对比某区域的数据是否符合预期
static int Osd_Compare_Data(
	void * 	rpAreaHandle,    
	S32         ri_PosX,
	S32         ri_PosY,
	S32         ri_Width,
	S32         ri_Height,
	void * 	rpv_Data,
	CHDRV_OSD_COLOR_TYPE_e  enm_ColorType,
	char *rpuc_PicName)
{
	U8*   puc_RegionData= NULL , *puc_ImgData = NULL;
    	S32   i_Loop;
	S32	 i_ColorType = 2;
	int ret = 0;
	
	if(enm_ColorType == CHDRV_OSD_COLOR_TYPE_ARGB1555 || enm_ColorType ==CHDRV_OSD_COLOR_TYPE_ARGB4444)
	{
		i_ColorType = 2;
	}
	else if(enm_ColorType == CHDRV_OSD_COLOR_TYPE_ARGB8888)
	{
		i_ColorType = 4;
	}
	else
	{
		LOG("add this Colortype = %d \n",enm_ColorType);//
	}
	puc_RegionData = (U8 *)CHDRV_OSD_AllocateMemory(ri_Width*ri_Height*i_ColorType);
	if(puc_RegionData != NULL)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_GetAreaData(rpAreaHandle,ri_PosX,ri_PosY,ri_Width,ri_Height,puc_RegionData));
		if(ret != CHDRV_OSD_OK)
		{
			CHDRV_OSD_FreeMemory(puc_RegionData);
			return ret;
		}
		ret = fsys_read_file((const char *) rpv_Data,(char*&) puc_ImgData);
		if(ret == -1)
		{
			EXPECT_LT(ret, 0) << "file not exsit,file=" << (const char *) rpv_Data;
		}
		else if(ret < -1)
		{
			EXPECT_LT(ret, 0) << "can not malloc memory,size=" << -ret;
		}
		if(ret != ri_Width*ri_Height*i_ColorType || puc_ImgData == NULL)
		{
			PROMPT("%s显示是否正常 pos=(%d,%d,%d,%d),画面上不应看到白色框,是请输入Y，否则输入N", (const char *) rpuc_PicName, ri_PosX,ri_PosY,ri_Width,ri_Height);
			EXPECT_EQ(CH_TRUE, ret = test_input_yes()) << "测试员认为显示不正确,filename=" <<  (const char *) rpuc_PicName << ",size=" << ret;
			if(ret)
			{
				ret = fsys_write_file((const char*)rpv_Data, (char*)puc_RegionData, ri_Width*ri_Height*i_ColorType);
				if(ret == ri_Width*ri_Height*i_ColorType)
				{
					LOG("新增模板%s\n", (const char*)rpv_Data);
				}
				ret = 0;
			}
			else
			{
				ret = -2;
			}
			CHDRV_OSD_FreeMemory(puc_RegionData);
			if(puc_ImgData)
			{
				CH_RTOS_FreeMemory(gh_APP_HEAP, puc_ImgData);
			}
			return ret;
		}
		ret = 0;
		for(i_Loop = 0;i_Loop < ri_Width*ri_Height*i_ColorType;i_Loop ++)
		{
			//EXPECT_EQ(*(puc_RegionData+i_Loop ) , *((U8 *)puc_ImgData+i_Loop)) << "解码后数据不一致, i_Loop=" << i_Loop << "pic name=" << rpuc_PicName << ",size=" << ri_Width*ri_Height*i_ColorType;
			if( *(puc_RegionData+i_Loop ) != *((U8 *)puc_ImgData+i_Loop))
			{
				PROMPT("%s解码数据对比不符合，人工观察显示是否正常? pos=(%d,%d,%d,%d), 是请输入Y，否则输入N", (const char *) rpuc_PicName, ri_PosX,ri_PosY,ri_Width,ri_Height);
				EXPECT_EQ(CH_TRUE, ret = test_input_yes()) << "测试员认为显示不正确. pic name=" << rpuc_PicName << ",size=" << ri_Width*ri_Height*i_ColorType;
				if(ret)
				{
					ret = fsys_write_file((const char*)rpv_Data, (char*)puc_RegionData, ri_Width * ri_Height * i_ColorType);
					if(ret == ri_Width * ri_Height * i_ColorType)
					{
						LOG("更新模板%s\n", rpv_Data);
					}
					ret = 0;
				}
				else
				{
					ret = -2;
				}
			
				break;
			}
		}
		if(ret == 0)
		{
			CH_RTOS_TaskDelay(2000);
		}
	}
	else
	{
		LOG("alloc mem fail , size=%d\n", ri_Width*ri_Height*i_ColorType);
		return CHDRV_OSD_FAIL;
	}
	if(puc_ImgData)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, puc_ImgData);
	}
	if(puc_RegionData != NULL)
	{
		CHDRV_OSD_FreeMemory(puc_RegionData);
	}	
	if(ret != 0)
	{
		LOG("\n data is not correct \n");
		return CHDRV_OSD_FAIL;
	}

	return CHDRV_OSD_OK;
}

static int osd_decode_pic(const char *file_name, const char * data_name, int w, int h, int *pi_use_time = NULL)
{
	CH_Rect_t stru_Rect;
	int ret;
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	CHDRV_OSD_RegionAttrib_t  stru_RegionAttrib;
	U32 time1;
	
	memset(&stru_Rect, 0, sizeof(stru_Rect));
	if(w > fw_getenv_i("OSD_PIC_REGION_WIDTH"))
	{
		w = fw_getenv_i("OSD_PIC_REGION_WIDTH");
	}
	if(h > fw_getenv_i("OSD_PIC_REGION_HEIGHT"))
	{
		h = fw_getenv_i("OSD_PIC_REGION_HEIGHT");
	}
	if(w > 0)
	{
		stru_Rect.s_X = (fw_getenv_i("OSD_PIC_REGION_WIDTH") - w) /2;
	}
	if(h > 0)
	{
		stru_Rect.s_Y = (fw_getenv_i("OSD_PIC_REGION_HEIGHT") - h) /2;;
	}
	stru_Rect.s_Width = w;
	stru_Rect.s_Height = h;

	
	memset(&stru_ImgData,0, sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)file_name;

	if(w == 0 && h == 0)
	{
		CHDRV_OSD_ImageData_t stru_ImgData;
		CHDRV_OSD_ImageInfo_t stru_ImgInfo;

		memset(&stru_ImgData, 0, sizeof(stru_ImgData));
		memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
		stru_ImgData.puc_FileName = (const U8*)file_name;
		EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		w = stru_ImgInfo.ui_Width;
		h = stru_ImgInfo.ui_Height;
		stru_Rect.s_X = (fw_getenv_i("OSD_PIC_REGION_WIDTH") - w) /2;
		stru_Rect.s_Y = (fw_getenv_i("OSD_PIC_REGION_HEIGHT") - h) /2;;
	}
	EXPECT_EQ(CHDRV_OSD_OK,ret = CHDRV_OSD_ClearArea(grh_DrawHandleOsd1, 
		0, 
		0,
		fw_getenv_i("OSD_PIC_REGION_WIDTH") ,
		fw_getenv_i("OSD_PIC_REGION_HEIGHT") , 
		0));
	EXPECT_EQ(CHDRV_OSD_OK,ret = CHDRV_OSD_ClearArea(grh_DrawHandleOsd1, 
		stru_Rect.s_X,
		stru_Rect.s_Y, 
		stru_Rect.s_Width,
		stru_Rect.s_Height, 
		0xffffffff));

	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DecodeImage(grh_DrawHandleOsd1,stru_Rect, &stru_ImgData));
	if(pi_use_time)
	{
		*pi_use_time = CH_RTOS_GetSysTimenow() - time1;
	}

		
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	if(ret != 0)
	{
		return ret;
	}
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1, &stru_RegionAttrib));
	if(data_name)
	{
		EXPECT_EQ(0, ret = Osd_Compare_Data(grh_DrawHandleOsd1,stru_Rect.s_X,stru_Rect.s_Y,w, h,(void*)data_name, stru_RegionAttrib.enm_ColorType, (char*)file_name)) << "解码错误，filename=" << file_name;
		LOG("Osd_Compare_Data return %d\n", ret);
	}
	return ret;
}

static int osd_decode_data(const char *file_name, const char * data_name, int w, int h, int *pi_use_time = NULL)
{
	CH_Rect_t stru_Rect;
	int ret;
	int len;
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	CHDRV_OSD_RegionAttrib_t    stru_RegionAttrib;
	U32 time1;
	
	stru_Rect.s_X = 0;
	stru_Rect.s_Y = 0;
	if(w > fw_getenv_i("OSD_PIC_REGION_WIDTH"))
	{
		w = fw_getenv_i("OSD_PIC_REGION_WIDTH");
	}
	if(h > fw_getenv_i("OSD_PIC_REGION_HEIGHT"))
	{
		h = fw_getenv_i("OSD_PIC_REGION_HEIGHT");
	}
	if(w > 0)
	{
		stru_Rect.s_X = (fw_getenv_i("OSD_PIC_REGION_WIDTH") - w) /2;
	}
	if(h > 0)
	{
		stru_Rect.s_Y = (fw_getenv_i("OSD_PIC_REGION_HEIGHT") - h) /2;;
	}
	stru_Rect.s_Width = w;
	stru_Rect.s_Height = h;

	
	memset(&stru_ImgData,0, sizeof(CHDRV_OSD_ImageData_t));
	EXPECT_LT(0,  len = fsys_read_file(file_name, (char*&)stru_ImgData.puc_ImgAddr)) 
	<<(len == -1 ? "文件不存在" : "内存分配失败" )<< "filename=" << file_name;
	if(len < 0)
	{
		return len;
	}
	stru_ImgData.ui_Length = len;
	if(w == 0 && h == 0)
	{
		CHDRV_OSD_ImageInfo_t stru_ImgInfo;

		memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
		EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		w = stru_ImgInfo.ui_Width;
		h = stru_ImgInfo.ui_Height;
		stru_Rect.s_X = (fw_getenv_i("OSD_PIC_REGION_WIDTH") - w) /2;
		stru_Rect.s_Y = (fw_getenv_i("OSD_PIC_REGION_HEIGHT") - h) /2;;
	}
	EXPECT_EQ(CHDRV_OSD_OK,ret = CHDRV_OSD_ClearArea(grh_DrawHandleOsd1, 0, 
		0, 
		fw_getenv_i("OSD_PIC_REGION_WIDTH") ,
		fw_getenv_i("OSD_PIC_REGION_HEIGHT"), 
		0));
	EXPECT_EQ(CHDRV_OSD_OK,ret = CHDRV_OSD_ClearArea(grh_DrawHandleOsd1, 
			stru_Rect.s_X,
			stru_Rect.s_Y,
			stru_Rect.s_Width, 
			stru_Rect.s_Height, 
			0xffffffff));
	if(stru_ImgData.ui_Length > 0)
	{
		time1 = CH_RTOS_GetSysTimenow();
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DecodeImage(grh_DrawHandleOsd1,stru_Rect, &stru_ImgData));
		if(pi_use_time)
		{
			*pi_use_time = CH_RTOS_GetSysTimenow() - time1;
		}
	}
	else
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
		if(stru_ImgData.puc_ImgAddr)
		{
			CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
		}
		return -1;
	}
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	if(ret != 0)
	{
		if(stru_ImgData.puc_ImgAddr)
		{
			CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
		}
		return ret;
	}
	if(data_name)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1, &stru_RegionAttrib ));
		EXPECT_EQ(0, ret = Osd_Compare_Data(grh_DrawHandleOsd1,stru_Rect.s_X ,stru_Rect.s_Y,w,h,(void*)data_name,stru_RegionAttrib.enm_ColorType, (char*)file_name)) << "解码错误，filename=" << file_name;
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}
	
	return ret;
}
#endif /* ENABLE_TESTCASE */

void CASE_NAME::SetUpTestCase() {
//excute before each OSD suit
//std::cout << "init SetUp rtos OSDCase " << std::endl;
	RUN_CMD("route.create 0 0");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n"
		"vout.open 1 1 1 1 0 0 0 0\n"
		"vout.set_port 1 0 0 0 0 0 0 1 0\n"
		"vid.ram_start 0 %s\n",
		fw_getenv("VIDEO_BASIC_HD"));
#else
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n"
		"vout.set_port 1 0 0 0 0 0 0 1 0\n"
		"vid.ram_start 0 %s\n",
		fw_getenv("VIDEO_BASIC_SD"));
#endif
	dbg_checkmem(1);
}
void CASE_NAME::TearDownTestCase() {
//excute after each OSD suit
//std::cout << "deinit tuner rtos OSDCase " << std::endl;
	RUN_CMD("vout.close 1\n"
	"vout.close 0\n"
	"vid.ram_stop 0\n");
	if(gpuc_Mem != NULL)
	{
		CHDRV_OSD_FreeMemory(gpuc_Mem);
		gpuc_Mem = NULL;
	}
	if(gpuc_Data !=NULL)
	{
		CHDRV_OSD_FreeMemory(gpuc_Data);
		gpuc_Data = NULL;
	}
	//EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_HideRegion(grh_BmpDrawHandle));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_BmpDrawHandle, 0));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_BmpDrawHandle));
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDrawHandle));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDisplayHandle));

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1, 0));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd1));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd1));

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd2, 0));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd2));
	OSD_Test_End();
	delete_prompt_region();
	dbg_checkmem(0);
}

void CASE_NAME::SetUp()
{
//execute before Testcase
//std::cout << "Chdrv SetUp" << std::endl;
}
void CASE_NAME::TearDown()
{
//execute before Testcase
//std::cout << "Chdrv TearDown" << std::endl;
dbg_checkmem(0);
}
#if ENABLE_TESTCASE

CASE_F(TEST_OSD_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init());
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_OSD_INIT2,
	"测试模块是否可被销毁") {
	//EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Term());
	//EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init());
	//test_av_ready();
}


CASE_F(TEST_OSD_INIT3,
	"测试CHDRV_OSD_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_OSD_DELAY,
	"观看视频播放情况") {
	//CH_RTOS_TaskDelay(2000);
}
 //OSD wrong paramters of functions




CASE_F(TEST_OSD_PARAMS1,
	"CHDRV_OSD_AddToMemBitMap错误参数测试") {

	//CHDRV_OSD_AddToMemBitMap
	//CH_RTOS_TaskDelay(10000);
}

CASE_F(TEST_OSD_PARAMS2,
	"CHDRV_OSD_AllocateMemory错误参数测试") {
	//U32 rui_Size=0; 
	//void * rpv_Memory;
	//EXPECT_EQ(NULL,CHDRV_OSD_AllocateMemory(rui_Size ));
	//返回值为 void * 型，非CHDRV_OSD_RESULT_e 型
	//CHDRV_OSD_FreeMemory(rpv_Memory);
}
 

CASE_F(TEST_OSD_PARAMS3,
	"CHDRV_OSD_BitBlit错误参数测试") {

	CHDRV_OSD_BitBlitParam_t 		rstru_BitBlitParam;

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( NULL, OSD_HANDLE, &rstru_BitBlitParam));//测参数1空指针
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE_INVALID, OSD_HANDLE, &rstru_BitBlitParam));//测参数1无效句柄
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, NULL, &rstru_BitBlitParam));//测参数2空指针，
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, OSD_HANDLE_INVALID, &rstru_BitBlitParam));//测参数2无效句柄 
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, OSD_HANDLE, NULL));//测参数3空指针,(返回值不为CHDRV_OSD_INVALID_PARAM)
}
 




CASE_F(TEST_OSD_PARAMS6,
	"CHDRV_OSD_ClearArea错误参数测试") {


	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, 0));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, 0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT, OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, 0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS, OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, 0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_MINUS,OSD_PARAM_RIGHT, 0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT, 0));	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_RIGHT,OSD_PARAM_MINUS, 0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT, OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE, 0));
}



CASE_F(TEST_OSD_PARAMS7,
	"CHDRV_OSD_ClearFullRegion错误参数测试") {
	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearFullRegion(NULL,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearFullRegion(OSD_HANDLE_INVALID,0));
}



CASE_F(TEST_OSD_PARAMS8,
	"CHDRV_OSD_CopyRect错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(NULL,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE_INVALID,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE_INVALID,OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CopyRect(OSD_HANDLE,OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_WRANGE));
}




CASE_F(TEST_OSD_PARAMS10,
	"CHDRV_OSD_CreateRegion错误参数测试") {

	CHDRV_OSD_RegionHandle   	 rh_Region;
	CHDRV_OSD_RegionParam_t   rstru_Params;
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(NULL, &rph_Region));// 
	rstru_Params.i_PosX = 0;
	rstru_Params.i_PosY = 0;
	rstru_Params.i_Width = OSD_PARAM_RIGHT;
	rstru_Params.i_Height = OSD_PARAM_RIGHT;
	rstru_Params.enm_RegionType = CHDRV_OSD_REGION_DRAW;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, NULL));
	rstru_Params.i_PosX = OSD_PARAM_MINUS;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));
	rstru_Params.i_PosX = 0;
	rstru_Params.i_PosY = OSD_PARAM_MINUS;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));
	rstru_Params.i_PosY = 0;
	rstru_Params.i_Width = OSD_PARAM_MINUS;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));
	rstru_Params.i_Width = OSD_PARAM_RIGHT;
	rstru_Params.i_Height = OSD_PARAM_MINUS;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));
	rstru_Params.i_Height=OSD_PARAM_RIGHT;
	rstru_Params.enm_RegionType = CHDRV_OSD_REGION_TYPE_MAX;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));	//内部在参数未完全
}



CASE_F(TEST_OSD_PARAMS12,
	"CHDRV_OSD_DecodeImage错误参数测试") {
	
	CH_Rect_t 				      stru_Rect = {0,0,0,0};
	CHDRV_OSD_ImageData_t     stru_ImgData ;
	stru_ImgData.puc_FileName = (const U8 *)OSD_PIC_PATH;
	stru_ImgData.puc_ImgAddr = NULL;
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DecodeImage(NULL, stru_Rect,&stru_ImgData));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DecodeImage(OSD_HANDLE_INVALID, stru_Rect,&stru_ImgData));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DecodeImage(OSD_HANDLE, stru_Rect,NULL));
	stru_ImgData.puc_FileName = NULL;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DecodeImage(OSD_HANDLE, stru_Rect,&stru_ImgData));
	stru_ImgData.puc_ImgAddr = (U8 *)CHDRV_OSD_AllocateMemory(sizeof(U8));
	stru_ImgData.ui_Length = 0;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DecodeImage(OSD_HANDLE, stru_Rect,&stru_ImgData));	
	if(stru_ImgData.puc_ImgAddr != NULL)
	{
		CHDRV_OSD_FreeMemory(stru_ImgData.puc_ImgAddr);
	}
}

CASE_F(TEST_OSD_PARAMS13,
	"CHDRV_OSD_DeleteRegion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DeleteRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DeleteRegion(OSD_HANDLE_INVALID));
}


#if OSD_USEEABLE
CASE_F(TEST_OSD_PARAMS21,
	"CHDRV_OSD_FreeMemory错误参数测试") {
	
	//void * rpv_Memory=NULL;
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_FreeMemory(rpv_Memory));
	//CHDRV_OSD_FreeMemory 返回值类型为void
}
#endif 

CASE_F(TEST_OSD_PARAMS22,
	"CHDRV_OSD_GetAreaData错误参数测试") {
		
	U8  					ruc_Data;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,NULL));
}


CASE_F(TEST_OSD_PARAMS24,
	"CHDRV_OSD_GetDisplayRegionAttrib错误参数测试") {

	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttrib ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(NULL,&rstru_RegionAttrib,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(OSD_HANDLE_INVALID,&rstru_RegionAttrib,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(OSD_HANDLE,NULL,0));
	
}



CASE_F(TEST_OSD_PARAMS27,
	"CHDRV_OSD_GetImageInfo错误参数测试") {

	CHDRV_OSD_ImageInfo_t   stru_ImgInfo;
	CHDRV_OSD_ImageData_t   stru_ImgData;
	stru_ImgData.puc_FileName = (const U8 *)OSD_PIC_PATH;
	stru_ImgData.puc_ImgAddr  = NULL;
	EXPECT_EQ( CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetImageInfo(&stru_ImgData,NULL) );
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetImageInfo(NULL,&stru_ImgInfo) );
	stru_ImgData.puc_FileName = NULL;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetImageInfo(&stru_ImgData,&stru_ImgInfo) );
	stru_ImgData.puc_ImgAddr  = (U8 *)CHDRV_OSD_AllocateMemory(sizeof(U8));
	stru_ImgData.ui_Length    = 0;	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetImageInfo(&stru_ImgData,&stru_ImgInfo) );
	if(stru_ImgData.puc_ImgAddr != NULL )
	{
		CHDRV_OSD_FreeMemory(stru_ImgData.puc_ImgAddr);
	}
}

CASE_F(TEST_OSD_PARAMS28,
	"CHDRV_OSD_GetOsdInfo错误参数测试") {

		
	void * *  					  r_TAddressSet =  NULL;
	void * *				      r_BAddressSet =  NULL;
	S32  								ri_PosX ;	
	S32  								ri_PosY ;	
	S32  							   ri_Width ;	
	S32 							  ri_Height ;	
	CH_BOOL  					   rb_Interlace ;	
	CHDRV_OSD_COLOR_TYPE_e  rpenm_ClutColorType = CHDRV_OSD_COLOR_TYPE_MAX;  
	CHDRV_OSD_COLOR_TYPE_e  	rpenm_ColorType = CHDRV_OSD_COLOR_TYPE_MAX;  
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(NULL,r_TAddressSet,r_BAddressSet,&ri_PosX,&ri_PosY,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE_INVALID,r_TAddressSet,r_BAddressSet,&ri_PosX,&ri_PosY,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,NULL,r_BAddressSet,&ri_PosX,&ri_PosY,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,NULL,&ri_PosX,&ri_PosY,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,r_BAddressSet,NULL,&ri_PosY,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,r_BAddressSet,&ri_PosX,NULL,&ri_Width,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,r_BAddressSet,&ri_PosX,&ri_PosY,NULL,&ri_Height,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,r_BAddressSet,&ri_PosX,&ri_PosY,&ri_Width,NULL,&rb_Interlace,&rpenm_ClutColorType,&rpenm_ColorType));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetOsdInfo(OSD_HANDLE,r_TAddressSet,r_BAddressSet,&ri_PosX,&ri_PosY,&ri_Width,&ri_Height,NULL,&rpenm_ClutColorType,&rpenm_ColorType));
}


CASE_F(TEST_OSD_PARAMS29,
	"CHDRV_OSD_GetPixel错误参数测试") {

	CHDRV_OSD_ColorValue  ui_Color ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,&ui_Color));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,&ui_Color));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,&ui_Color));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,NULL));
}

CASE_F(TEST_OSD_PARAMS30,
	"CHDRV_OSD_GetRegionAlpha错误参数测试") {
	
	S32   			  ri_AlphaValue ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(NULL,&ri_AlphaValue));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(OSD_HANDLE_INVALID,&ri_AlphaValue));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS31,
	"CHDRV_OSD_GetRegionAttrib错误参数测试") {

	CHDRV_OSD_RegionAttrib_t   rstru_RegionAttrib ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(NULL,&rstru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(OSD_HANDLE_INVALID,&rstru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS32,
	"CHDRV_OSD_GetRegionPalette错误参数测试") {
	
	CHDRV_OSD_ColorValue   ui_ColorArray ;

	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(NULL,&ui_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(OSD_HANDLE_INVALID,&ui_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS33,
	"CHDRV_OSD_GetRegionPaletteEntry错误参数测试") {

	CHDRV_OSD_ColorValue   ui_Color ;

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(NULL,0,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(OSD_HANDLE_INVALID,0,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(OSD_HANDLE,0,NULL));
}

CASE_F(TEST_OSD_PARAMS34,
	"CHDRV_OSD_GetVersion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetVersion( NULL));
}

CASE_F(TEST_OSD_PARAMS35,
	"CHDRV_OSD_GetWindow错误参数测试") {

	CH_Rect_t 			  stru_Rect ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(NULL,&stru_Rect));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(OSD_HANDLE_INVALID,&stru_Rect));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS36,
	"CHDRV_OSD_HideRegion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_HideRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_HideRegion(OSD_HANDLE_INVALID));
}
CASE_F(TEST_OSD_PARAMS40,
	"CHDRV_OSD_LinkRegion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(NULL,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE_INVALID,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE,NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE,OSD_HANDLE_INVALID));

}


CASE_F(TEST_OSD_PARAMS41,
	"CHDRV_OSD_PutAreaData错误参数测试") {
	
	U8  					ruc_Data ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,&ruc_Data));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,&ruc_Data));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_PutAreaData(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,NULL));
}


CASE_F(TEST_OSD_PARAMS47,
	"CHDRV_OSD_SetPixel错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,0,CHDRV_OSD_ALU_MAX));
}

CASE_F(TEST_OSD_PARAMS48,
	"CHDRV_OSD_SetRegionAlpha错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionAlpha(NULL,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionAlpha(OSD_HANDLE_INVALID,0));
}
 
CASE_F(TEST_OSD_PARAMS49,
	"CHDRV_OSD_SetRegionPalette错误参数测试") {

	CHDRV_OSD_ColorValue   rp_ColorArray ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(NULL,&rp_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(OSD_HANDLE_INVALID,&rp_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS50,
	"CHDRV_OSD_SetRegionPaletteEntry错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPaletteEntry(NULL,0,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPaletteEntry(OSD_HANDLE_INVALID,0,0));
}

CASE_F(TEST_OSD_PARAMS51,
	"CHDRV_OSD_SetWindow错误参数测试") {

	
	CH_Rect_t 			   stru_Rect = {0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT};	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(NULL,stru_Rect));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(OSD_HANDLE_INVALID,stru_Rect));
#if 0	
	stru_Rect = {OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT};  // x < 0
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(OSD_HANDLE,stru_Rect));
	stru_Rect = {0,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT};  // y < 0
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(OSD_HANDLE,stru_Rect));
	stru_Rect = {0,0,OSD_PARAM_MINUS,OSD_PARAM_RIGHT};    // w < 0
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(OSD_HANDLE,stru_Rect));
	stru_Rect = {0,0,OSD_PARAM_RIGHT,OSD_PARAM_MINUS};    // h < 0
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetWindow(OSD_HANDLE,stru_Rect));
#endif

}

CASE_F(TEST_OSD_PARAMS52,
	"CHDRV_OSD_ShowRegion错误参数测试") {

	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ShowRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ShowRegion(OSD_HANDLE_INVALID));
}
CASE_F(TEST_OSD_PARAMS56,
	"CHDRV_OSD_UnlinkRegion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(NULL,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE_INVALID,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE,NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE,OSD_HANDLE_INVALID));
}

CASE_F(TEST_OSD_PARAMS57,
	"CHDRV_OSD_UpdateRegion错误参数测试") {
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegion(OSD_HANDLE_INVALID));
}

CASE_F(TEST_OSD_PARAMS58,
	"CHDRV_OSD_UpdateRegionArea错误参数测试") {
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegionArea(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE));
}

#if 0 	/*sqzou20170807*/
CASE_F(TEST_OSD_PARAMS59,
	"CHDRV_OSD_UpdateScrollRegion错误参数测试") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateScrollRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateScrollRegion(OSD_HANDLE_INVALID));
}
#endif



CASE_F(TEST_OSD_CREATE_1,
	"以合法参数创建一个osd1 draw region，其中宽高从env获取，颜色格式为4444，应返回成功") {
	CHDRV_OSD_RegionParam_t rstru_RegionParams;	

	delete_prompt_region();
	
	rstru_RegionParams.i_Width  		 = OSD_ENV_WIDTH;
	rstru_RegionParams.i_Height 		 = OSD_ENV_HEIGHT;
	rstru_RegionParams.i_PosX 			 = 0;
	rstru_RegionParams.i_PosY			 = 0;
	rstru_RegionParams.enm_Layer		 = CH_LAYER_OSD1;
	rstru_RegionParams.enm_ColorType 	 = CHDRV_OSD_COLOR_TYPE_ARGB4444;
	rstru_RegionParams.enm_ClutColorType = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_RegionParams.enm_RegionType 	 = CHDRV_OSD_REGION_DRAW;
	rstru_RegionParams.h_MemPartion 	 = NULL;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_RegionParams,&grh_DrawHandleOsd1)) ;


}

CASE_F(TEST_OSD_CREATE_2,
	"以合法参数创建一个osd1 display region，其中宽高从env获取，颜色格式为4444，应返回成功") {
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB4444;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
}


CASE_F(TEST_OSD_GET_ATTRIB_1,
	"针对刚刚创建的2个region，分别调用get attrib，应返回成功；属性值应正确") {

	S32 i_Loop = 0;
	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttribGet[2];
	memset(rstru_RegionAttribGet,0,sizeof(rstru_RegionAttribGet));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1,&rstru_RegionAttribGet[0]));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DisplayHandleOsd1,&rstru_RegionAttribGet[1]));
	for( i_Loop = 0;i_Loop < 2;i_Loop++ )
	{	
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosX);
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosY);
		EXPECT_EQ(OSD_ENV_WIDTH,rstru_RegionAttribGet[i_Loop].i_Width);
		EXPECT_EQ(OSD_ENV_HEIGHT,rstru_RegionAttribGet[i_Loop].i_Height);
		EXPECT_EQ(CHDRV_OSD_COLOR_TYPE_ARGB4444,rstru_RegionAttribGet[i_Loop].enm_ColorType);
		EXPECT_EQ(CH_LAYER_OSD1,rstru_RegionAttribGet[i_Loop].enm_Layer); //函数内没有获取该值
		if(i_Loop == 1)
		{
			EXPECT_EQ(CHDRV_OSD_REGION_DISPLAY,rstru_RegionAttribGet[i_Loop].enm_RegionType);	
		}
		else
		{
			EXPECT_EQ(CHDRV_OSD_REGION_DRAW,rstru_RegionAttribGet[i_Loop].enm_RegionType);
		}
	}

}

CASE_F(TEST_OSD_LINK_1,
	"将两个region link在一起，应返回ok") {

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}

CASE_F(TEST_OSD_SHOWREGION_1,
	"调用show region，应返回ok") {

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}

CASE_F(TEST_OSD_CLEAREREGION_1,
	"调用clear full draw region为白色，应返回ok") {
	CHDRV_OSD_ColorValue 	 ui_Color = OSD_REGION_COLOR4444_WHITE;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,ui_Color));
}


CASE_F(TEST_OSD_CLEAREREGION_2,
	"调用update region，句柄传disp region，应返回ok，询问测试员，画面应显示为正确的颜色") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_RED));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否显示为红色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "画面应显示为红色";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_GREEN));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否显示为绿色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "画面应显示为绿色";


	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_BLUE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否显示为蓝色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "画面应显示为蓝色";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否显示为白色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "画面应显示为白色";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否显示为白色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "画面应显示为白色";
}


CASE_F(TEST_OSD_CLEAREREGION_3,
	"调用clear full draw region为红色，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_RED));
	
}

CASE_F(TEST_OSD_UPDATE_1,
	"调用update region area左上角，句柄传disp region，应返回ok，询问测试员，画面应仅仅左上角显示为红色") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DisplayHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2));
	PROMPT("画面是否仅仅左上角显示为红色，其他为白色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "局部更新功能未实现";
}

CASE_F(TEST_OSD_UPDATE_2,
	"调用update region应返回ok，句柄传draw region，询问测试员，画面应显示为全部红色") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否整屏均为红色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "全部更新功能未实现";

}

CASE_F(TEST_OSD_CLEAREREGION_4,
	"调用clear area，将左上角四分之一的区域画为透明色，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,0));
}

CASE_F(TEST_OSD_UPDATE_3,
	"调用update region应返回ok，询问测试员，画面应显示左上角透明色，其他红色") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否仅仅左上角显示为透明色，其他为红色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_ClearArea透明色功能未实现";

}


CASE_F(TEST_OSD_CLEAREREGION_5,
	"调用clear area，将左上角四分之一的区域画为白色，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,OSD_REGION_COLOR4444_WHITE));
}

CASE_F(TEST_OSD_UPDATE_4,
	"调用update region应返回ok，询问测试员，画面应显示左上角白色，其他红色") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否仅仅左上角显示为白色，其他为红色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_ClearArea功能未实现";

}

CASE_F(TEST_OSD_GETDATA_1,
	"调用get area data左上角数据，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	gpuc_Data = (U8 *)CHDRV_OSD_AllocateMemory((OSD_ENV_WIDTH/2)*(OSD_ENV_HEIGHT/2)*2);
	if(gpuc_Data != NULL )
	{
		//memset
		memset(gpuc_Data,0,sizeof(U8)*(OSD_ENV_WIDTH/2)*(OSD_ENV_HEIGHT/2)*2);
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,gpuc_Data));
	}
}

CASE_F(TEST_OSD_PUTDATA_1,
	"调用put area data，将数据put到屏幕的右上角，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	if(gpuc_Data != NULL)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_ENV_WIDTH/2,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,gpuc_Data));
	}
}

CASE_F(TEST_OSD_UPDATE_5,
	"调用update region应返回ok，询问测试员，") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否仅仅上半部分显示为白色，其他为红色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_PutAreaData功能未实现";

}

CASE_F(TEST_OSD_COPYRECT,
	"调用copy rect，将draw regioin的上半部分拷贝到下半部分，应返回ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	//EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CopyRect(grh_DrawHandleOsd1,grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT/2,0,OSD_ENV_HEIGHT/2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CopyRect(grh_DrawHandleOsd1,grh_DisplayHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT/2,0,OSD_ENV_HEIGHT/2));
}

CASE_F(TEST_OSD_UPDATE_6,
	"调用update region应返回ok，询问测试员，画面应全部显示为白色") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
	PROMPT("画面是否整屏显示为白色?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_CopyRect功能未实现";


}

CASE_F(TEST_OSD_CLEARAREA_1,
	"调用clear area，其中坐标分别在上下左右越界，可以返回不成功但不应死机，或屏幕乱") {
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,OSD_REGION_COLOR4444_WHITE));
}


CASE_F(TEST_OSD_GETDATA_2,
	"调用get area data，其中坐标分别在上下左右越界，可以返回不成功但不应死机，或屏幕乱") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,gpuc_Data));
}

CASE_F(TEST_OSD_PUTDATA_2,
	"调用put area data，其中坐标分别在上下左右越界，可以返回不成功但不应死机，或屏幕乱") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,gpuc_Data));
}

CASE_F(TEST_OSD_UPDATA_AREA_1,
	"调用update region area，其中坐标分别在上下左右越界，可以返回不成功但不应死机，或屏幕乱") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1));
}



CASE_F(TEST_OSD_SETPIXEL,
	"调用set pixel函数绘制一个方块，然后调用get area data获取数据进行比较，应成功") {
	S32 					 i_LoopX = 0;
	S32 					 i_LoopY = 0;
	CHDRV_OSD_ColorValue i_Color_Set = OSD_REGION_COLOR4444_GREEN; 
	CHDRV_OSD_ALU_MODE_e enm_AluMode = CHDRV_OSD_ALU_MAX;
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	for(i_LoopX = OSD_PARAM_RIGHT;i_LoopX < OSD_PARAM_RIGHT*2 ;i_LoopX++)
	{
		for(i_LoopY = OSD_PARAM_RIGHT;i_LoopY < OSD_PARAM_RIGHT*2 ;i_LoopY++)
		{
			EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetPixel(grh_DrawHandleOsd1,i_LoopX,i_LoopY,OSD_REGION_COLOR4444_GREEN,enm_AluMode));
		}
	}	
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,
		OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,
		&i_Color_Set,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_SetPixel未正确实现";
}

CASE_F(TEST_OSD_GETPIXEL,
	"调用get pixel函数获取内容，应正确") {
	
	S32 					i_LoopX = 0;
	S32 					i_LoopY = 0;
	CHDRV_OSD_ColorValue     i_Color = 0;
	CH_BOOL b_fail = CH_FALSE;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_GREEN);
	for(i_LoopX = OSD_PARAM_RIGHT; !b_fail && i_LoopX < OSD_PARAM_RIGHT*2 ;i_LoopX++)
	{
		for(i_LoopY = OSD_PARAM_RIGHT;i_LoopY < OSD_PARAM_RIGHT * 2 ;i_LoopY++)
		{
			EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetPixel(grh_DrawHandleOsd1,i_LoopX,i_LoopY,&i_Color));
			//CHDRV_OSD_GetPixel函数内有个颜色值获取写错
			EXPECT_EQ(i_Color, OSD_REGION_COLOR4444_GREEN) << "CHDRV_OSD_GetPixel未正确实现";
			if(i_Color != OSD_REGION_COLOR4444_GREEN)
			{
				b_fail = CH_TRUE;
				break;
			}
		}
	}
}

CASE_F(TEST_OSD_POSITION_0,
	"OSD位置测试，应与I帧的标尺一致") {
	
	CH_Rect_t rect[14];
	int i;
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);

	memset(&rect, 0, sizeof(rect));
	INIT_RECT(rect[0], 0,0,100,100);
	INIT_RECT(rect[1], 100,0,100,100);
	INIT_RECT(rect[2], 0,100,100,100);
	INIT_RECT(rect[3], 100,100,100,100);
	INIT_RECT(rect[4], 100,200,100,100);
	INIT_RECT(rect[5], 200,100,100,100);
	INIT_RECT(rect[6], 200,200,100,100);
	
	if(fw_getenv_i("SUPPORT_HD") == 1)
	{
		INIT_RECT(rect[7], 200,200,200,200);
		INIT_RECT(rect[8], 300,200,200,200);
		INIT_RECT(rect[9], 200,300,200,200);
		INIT_RECT(rect[10], 300,300,200,200);
		INIT_RECT(rect[11], 600,300,200,200);
		INIT_RECT(rect[12], 600,400,200,200);
		INIT_RECT(rect[13], 800,400,200,200);
	}
	
	for(i = 0; i< sizeof(rect) / sizeof(rect[0]) && rect[i].s_Width > 0; i++)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,0));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,rect[i].s_X, rect[i].s_Y, rect[i].s_Width,rect[i].s_Height, OSD_REGION_COLOR4444_WHITE));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
		PROMPT("白色框位置为x=%d,y=%d,w=%d,h=%d,是否和标尺一致?是请输入Y，否则输入N", rect[i].s_X, rect[i].s_Y, rect[i].s_Width,rect[i].s_Height);
		EXPECT_EQ(CH_TRUE, test_input_yes()) << "OSD rect位置(x="<<rect[i].s_X << ",y=" << rect[i].s_Y << ",w=" <<  rect[i].s_Width << ",h=" <<  rect[i].s_Height << ")与标尺不符合";
	}
	
}



#if 0 	/*sqzou20170807*/
CASE_F(TEST_OSD_DRAWLINE,
	"多次调用draw line函数以不同的宽度绘制成一个方块，然后调用get area data获取数据进行比较，应成功") {
//斜线设置宽度才有作用，横或竖线画的时候加宽度没有效果，需要更新mi，有修过这个问题
	CHDRV_OSD_RegionHandle	h_DrawRegion = grh_DrawHandleOsd1; 
	CHDRV_OSD_ColorValue	 i_LineColor = OSD_REGION_COLOR4444_BLUE;
	CHDRV_OSD_ALU_MODE_e	 enm_AluMode = CHDRV_OSD_ALU_MAX;
	S32 		i_StartX = OSD_PARAM_RIGHT;
	S32 		i_StartY = OSD_PARAM_RIGHT;
	S32 		i_LineWidth ;
	S32 		 i_Loop ;
	U16          *puc_Data = (U16 *)CHDRV_OSD_AllocateMemory(OSD_PARAM_RIGHT*150*2);
	CH_BOOL b_fail = CH_FALSE;
	int ret = CHDRV_OSD_OK;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));

	//每次线宽加10 ，起始坐标(200,200),终点坐标(400,350),保持x值不变，向下绘制方块，最终形成一个完整高度150的大方块
	for(i_LineWidth = 10;i_LineWidth <= 50 && ret == CHDRV_OSD_OK;i_LineWidth += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawLine(h_DrawRegion,i_StartX,i_StartY,i_StartX,i_StartY + i_LineWidth,i_LineWidth,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,150,(U8 *)puc_Data));
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT*150;i_Loop ++)
	{
		EXPECT_EQ(OSD_REGION_COLOR4444_BLUE,*(puc_Data+i_Loop )) << "CHDRV_OSD_DrawLine未正确实现";
		if(OSD_REGION_COLOR4444_BLUE != *(puc_Data+i_Loop ))
		{
			break;
		}
	}

	ret = CHDRV_OSD_OK;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	//每次线宽加10 ，起始坐标(200,200),终点坐标(350,400),保持y值不变，向右绘制方块，最终形成一个完整宽度为150的大方块
	for(i_LineWidth = 10;i_LineWidth <= 50 && ret == CHDRV_OSD_OK;i_LineWidth += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawLine(h_DrawRegion,i_StartX,i_StartY,i_StartX + i_LineWidth,i_StartY,i_LineWidth,i_LineColor,enm_AluMode));
		i_StartX += i_LineWidth;
	}

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,150,OSD_PARAM_RIGHT,(U8 *)puc_Data));
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT*150 * 2;i_Loop ++)
	{
		EXPECT_EQ(OSD_REGION_COLOR4444_BLUE,*(puc_Data+i_Loop )) << "CHDRV_OSD_DrawLine未正确实现";
		if(OSD_REGION_COLOR4444_BLUE != *(puc_Data+i_Loop ))
		{
			break;
		}
	}
	
	CHDRV_OSD_FreeMemory(puc_Data);
}

CASE_F(TEST_OSD_DRAWHLINE,
	"多次调用draw H line函数绘制成一个方块，然后调用get area data获取数据进行比较，应成功") {
	//斜线设置宽度才有作用，横或竖线画的时候加宽度没有效果，需要更新mi，有修过这个问题
	CHDRV_OSD_RegionHandle	h_DrawRegion = grh_DrawHandleOsd1; 
	CHDRV_OSD_ColorValue	 i_LineColor = OSD_REGION_COLOR4444_BLUE;
	CHDRV_OSD_ALU_MODE_e	 enm_AluMode = CHDRV_OSD_ALU_MAX;
	S32 		i_StartX = OSD_PARAM_RIGHT;
	S32 		i_StartY = OSD_PARAM_RIGHT;
	S32 		i_LineWidth ;
	S32 		 i_Loop ;
	U16          *puc_Data = (U16 *)CHDRV_OSD_AllocateMemory(OSD_PARAM_RIGHT*OSD_PARAM_RIGHT*2);
	CH_BOOL b_fail = CH_FALSE;
	int ret = CHDRV_OSD_OK;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));

	//每次线宽加10 ，起始坐标(200,200),终点坐标(400,350),保持x值不变，向下绘制方块，最终形成一个完整高度150的大方块
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT && ret == CHDRV_OSD_OK;i_Loop++)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawHLine(h_DrawRegion,0, 0, i_StartX,i_StartY, OSD_PARAM_RIGHT,CHDRV_OSD_COLOR_TYPE_ARGB4444,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}
	

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,(U8 *)puc_Data));

	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,
		OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&i_LineColor,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_DrawHLine未正确实现";
	
	CHDRV_OSD_FreeMemory(puc_Data);
}

CASE_F(TEST_OSD_DRAWVLINE,
	"多次调用draw V line函数绘制成一个方块，然后调用get area data获取数据进行比较，应成功") {
	//斜线设置宽度才有作用，横或竖线画的时候加宽度没有效果，需要更新mi，有修过这个问题
	CHDRV_OSD_RegionHandle	h_DrawRegion = grh_DrawHandleOsd1; 
	CHDRV_OSD_ColorValue	 i_LineColor = OSD_REGION_COLOR4444_BLUE;
	CHDRV_OSD_ALU_MODE_e	 enm_AluMode = CHDRV_OSD_ALU_MAX;
	S32 		i_StartX = OSD_PARAM_RIGHT;
	S32 		i_StartY = OSD_PARAM_RIGHT;
	S32 		i_LineWidth ;
	S32 		 i_Loop ;
	U16          *puc_Data = (U16 *)CHDRV_OSD_AllocateMemory(OSD_PARAM_RIGHT*OSD_PARAM_RIGHT*2);
	CH_BOOL b_fail = CH_FALSE;
	int ret = CHDRV_OSD_OK;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));

	//每次线宽加10 ，起始坐标(200,200),终点坐标(400,350),保持x值不变，向下绘制方块，最终形成一个完整高度150的大方块
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT && ret == CHDRV_OSD_OK;i_Loop++)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawVLine(h_DrawRegion,0, 0, i_StartX,i_StartY, OSD_PARAM_RIGHT,CHDRV_OSD_COLOR_TYPE_ARGB4444,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}
	

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,(U8 *)puc_Data));

	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,
		OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&i_LineColor,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_DrawVLine未正确实现";
	
	CHDRV_OSD_FreeMemory(puc_Data);
}
#endif

CASE_F(TEST_OSD_ALLOCMEM_1,
	"调用osd memory创建一个480X320Xcolor bytes大小的内存") {	
	EXPECT_NE((void*)NULL, gpuc_Mem = CHDRV_OSD_AllocateMemory(OSD_MEM_WIDTH*OSD_MEM_HEIGHT*2))<<"error";
	if(gpuc_Mem != NULL)
	{
		memset(gpuc_Mem, 0, sizeof(U8)*OSD_MEM_WIDTH*OSD_MEM_HEIGHT*2);
	}
	
}
#if 0 	/*sqzou20170925*/

CASE_F(TEST_OSD_DRAWRECT_1,
	"调用draw rect函数以draw memory方式绘制方块，然后进行数据比较，应符合预期") {
	CH_BOOL 				b_IsUseAlpha = OSD_BOOL_FLASE;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect = {0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue   ui_Color = OSD_REGION_COLOR4444_GREEN, ui_ColorTemp;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB4444;	///< 一个像素占16bit，ARGB每分量占4bit，按地址由高至低排列	
	int i;
	U16 *pus_Temp = NULL;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	
	b_IsUseAlpha = CH_FALSE;
	memset(gpuc_Mem, 0, OSD_MEM_WIDTH*OSD_MEM_HEIGHT*2)	;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(gpuc_Mem,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
	pus_Temp = (U16*)gpuc_Mem;
	ui_ColorTemp = OSD_REGION_COLOR4444_GREEN;
	for(i = 0; i < OSD_MEM_WIDTH * OSD_MEM_HEIGHT; i++)
	{
		EXPECT_EQ(pus_Temp[i], ui_ColorTemp) << "CHDRV_OSD_DrawRect未正确实现";
		if(pus_Temp[i] != ui_ColorTemp)
		{
			break;
		}
	}

}


CASE_F(TEST_OSD_BUFBITBLIT_1,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块COPY到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t stru_BitBlitParam;
	
	U32 ui_SrcBufWidth = OSD_MEM_WIDTH;
	U32 ui_SrcBufHeight 	= OSD_MEM_HEIGHT;	

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	memset(&stru_BitBlitParam,0,sizeof(stru_BitBlitParam));
	stru_BitBlitParam.b_IsUseColorKey       = 0;
	stru_BitBlitParam.enm_AluMode           = CHDRV_OSD_ALU_COPY;
	stru_BitBlitParam.stru_SrcRect.s_X      = 0;
	stru_BitBlitParam.stru_SrcRect.s_Y      = 0;
	stru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	stru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;

	stru_BitBlitParam.stru_DstRect.s_X      = 0;
	stru_BitBlitParam.stru_DstRect.s_Y      = 0;
	stru_BitBlitParam.stru_DstRect.s_Width  = OSD_MEM_WIDTH;
	stru_BitBlitParam.stru_DstRect.s_Height = OSD_MEM_HEIGHT;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BufBitBlit(gpuc_Mem,ui_SrcBufWidth,ui_SrcBufHeight,grh_DrawHandleOsd1,0,0,&stru_BitBlitParam));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}

CASE_F(TEST_OSD_GETDATA_3,
	"调用get area data，数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK, Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_2,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块放大到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t stru_BitBlitParam;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	memset(&stru_BitBlitParam,0,sizeof(stru_BitBlitParam));
	U32 rui_SrcBufWidth 					  = OSD_MEM_WIDTH;
	U32 rui_SrcBufHeight 					  = OSD_MEM_HEIGHT;	

	stru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_COPY;
	stru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	stru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	stru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	stru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	stru_BitBlitParam.stru_DstRect.s_X	  = 0;
	stru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	stru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	stru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BufBitBlit(gpuc_Mem,rui_SrcBufWidth,rui_SrcBufHeight,grh_DrawHandleOsd1,0,0,&stru_BitBlitParam));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
}

CASE_F(TEST_OSD_BUFBITBLIT_3,
	"调用get area data，放大后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK, Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_4,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块缩小到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t stru_BitBlitParam;	
	
	U32 rui_SrcBufWidth 	  = OSD_MEM_WIDTH;
	U32 rui_SrcBufHeight   = OSD_MEM_HEIGHT;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	memset(&stru_BitBlitParam,0,sizeof(stru_BitBlitParam));
	stru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_COPY;
	stru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	stru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	stru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	stru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	stru_BitBlitParam.stru_DstRect.s_X	  = 0;
	stru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	stru_BitBlitParam.stru_DstRect.s_Width  = OSD_PARAM_RIGHT;
	stru_BitBlitParam.stru_DstRect.s_Height = OSD_PARAM_RIGHT;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BufBitBlit(gpuc_Mem,rui_SrcBufWidth,rui_SrcBufHeight,grh_DrawHandleOsd1,0,0,&stru_BitBlitParam));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
}

CASE_F(TEST_OSD_BUFBITBLIT_5,
	"调用get area data，缩小后数据应符合预期") {
	
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

	ui_Color = 0;
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_6,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块用alpha blend到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	CHDRV_OSD_RegionHandle rh_DstRegion 	  = grh_DrawHandleOsd1;
	void *	rpv_SrcBuf 						  = gpuc_Mem;
	U32 rui_SrcBufWidth 					  = OSD_MEM_WIDTH;
	U32 rui_SrcBufHeight 					  = OSD_MEM_HEIGHT;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_ALPHA_BLEND;
	rstru_BitBlitParam.enm_AlphaBlendMode	  = CHDRV_OSD_BITBLT_ABLEND_SRC;
	rstru_BitBlitParam.enm_SrcAlphaSel       = CHDRV_OSD_BITBLIT_ALPHA_REPLEACE;
	rstru_BitBlitParam.c_SrcReplaceAlpha     = 255;
	
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BufBitBlit(rpv_SrcBuf,rui_SrcBufWidth,rui_SrcBufHeight,rh_DstRegion,0,0,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}
//<<<<<<<<<<<<
CASE_F(TEST_OSD_BUFBITBLIT_7,
	"调用get area data，alpha blend后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_8,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块用alpha blend到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	U32  rui_SrcBufWidth 					  = OSD_MEM_WIDTH;
	U32  rui_SrcBufHeight					  = OSD_MEM_HEIGHT;		
	rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_ALPHA_BLEND;
	rstru_BitBlitParam.enm_AlphaBlendMode	  = CHDRV_OSD_BITBLT_ABLEND_SRC;
	rstru_BitBlitParam.enm_DstAlphaSel       = CHDRV_OSD_BITBLIT_ALPHA_REPLEACE;
	rstru_BitBlitParam.c_DstReplaceAlpha     = 255;
	
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BufBitBlit(gpuc_Mem,rui_SrcBufWidth,rui_SrcBufHeight,grh_DrawHandleOsd1,0,0,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}
CASE_F(TEST_OSD_BUFBITBLIT_9,
	"调用get area data，alpha blend后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}
#endif

CASE_F(TEST_OSD_BUFBITBLIT_10,
	"再创建一个480X320、相同颜色模式的draw region2，应成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_MEM_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_MEM_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB4444;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;	
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleMem)) ;
	
}

CASE_F(TEST_OSD_BUFBITBLIT_11,
	"调用draw rect函数以draw region handle方式绘制方块到region2上，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB4444;	///< 一个像素占16bit，ARGB每分量占4bit，按地址由高至低排列

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleMem,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleMem,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,enm_ColorType));
	
}

CASE_F(TEST_OSD_BUFBITBLIT_12,
	"先clear draw region，然后调用bitblit将刚刚绘制的方块从region2 COPY到draw region上，应成功") {
	
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	rstru_BitBlitParam.enm_AluMode           = CHDRV_OSD_ALU_COPY;
	rstru_BitBlitParam.stru_SrcRect.s_X      = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y      = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;

	rstru_BitBlitParam.stru_DstRect.s_X      = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y      = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_MEM_HEIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BitBlit(grh_DrawHandleMem,grh_DrawHandleOsd1,&rstru_BitBlitParam)); 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}

CASE_F(TEST_OSD_BUFBITBLIT_13,
	"调用get area data，数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_14,
	"先clear draw region，然后调用bitblit将刚刚绘制的方块放大到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_COPY;
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BitBlit(grh_DrawHandleMem,grh_DrawHandleOsd1,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
}

CASE_F(TEST_OSD_BUFBITBLIT_15,
	"调用get area data，放大后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_16,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块缩小到draw region上，应成功") {

	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	//rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_COPY;
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_PARAM_RIGHT;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_PARAM_RIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BitBlit(grh_DrawHandleMem,grh_DrawHandleOsd1,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}

CASE_F(TEST_OSD_BUFBITBLIT_17,
	"调用get area data，缩小后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

	ui_Color = 0;
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

}

CASE_F(TEST_OSD_BUFBITBLIT_18,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块用alpha blend到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_ALPHA_BLEND;
	rstru_BitBlitParam.enm_AlphaBlendMode	  = CHDRV_OSD_BITBLT_ABLEND_SRC;
	rstru_BitBlitParam.enm_SrcAlphaSel       = CHDRV_OSD_BITBLIT_ALPHA_REPLEACE,
	rstru_BitBlitParam.c_SrcReplaceAlpha     = 255;
	
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BitBlit(grh_DrawHandleMem,grh_DrawHandleOsd1,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}
CASE_F(TEST_OSD_BUFBITBLIT_19,
	"调用get area data，alpha blend后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_20,
	"先clear draw region，然后调用buf bitblit将刚刚绘制的方块用alpha blend到draw region上，应成功") {
	CHDRV_OSD_BitBlitParam_t rstru_BitBlitParam;
	memset(&rstru_BitBlitParam,0,sizeof(rstru_BitBlitParam));
	rstru_BitBlitParam.enm_AluMode 		  = CHDRV_OSD_ALU_ALPHA_BLEND;
	rstru_BitBlitParam.enm_AlphaBlendMode	  = CHDRV_OSD_BITBLT_ABLEND_SRC;
	rstru_BitBlitParam.enm_DstAlphaSel       = CHDRV_OSD_BITBLIT_ALPHA_REPLEACE,
	rstru_BitBlitParam.c_DstReplaceAlpha     = 255;
	
	rstru_BitBlitParam.stru_SrcRect.s_X	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_SrcRect.s_Width  = OSD_MEM_WIDTH;
	rstru_BitBlitParam.stru_SrcRect.s_Height = OSD_MEM_HEIGHT;
	
	rstru_BitBlitParam.stru_DstRect.s_X	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Y	  = 0;
	rstru_BitBlitParam.stru_DstRect.s_Width  = OSD_ENV_WIDTH;
	rstru_BitBlitParam.stru_DstRect.s_Height = OSD_ENV_HEIGHT;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_BitBlit(grh_DrawHandleMem,grh_DrawHandleOsd1,&rstru_BitBlitParam));
 
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
}
CASE_F(TEST_OSD_BUFBITBLIT_21,
	"调用get area data，alpha blend后数据应符合预期") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_DELETEREGIOIN_1,
	"删除创建的所有region，应成功") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd1));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleMem));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd1));
	grh_DrawHandleOsd1     = NULL;
	grh_DrawHandleMem 	   = NULL;
	grh_DisplayHandleOsd1  = NULL;
}

CASE_F(TEST_OSD_CREATEREGION_1,
	"以合法参数创建一个osd1 draw region，其中宽高为720X576，颜色格式为1555，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB1555;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd1)) ;

}


CASE_F(TEST_OSD_CREATEREGION_2,
	"以合法参数创建一个osd1 display region，其中宽高为720X576，颜色格式为1555，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX 			= 0;
	rstru_Params.i_PosY 			= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType		= CHDRV_OSD_COLOR_TYPE_ARGB1555;
	rstru_Params.enm_RegionType 	= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType	= CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer			= CH_LAYER_OSD1;
		
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
		
}
CASE_F(TEST_OSD_GETATTRIB_1,
	"针对刚刚创建的2个region，分别调用get attrib，应返回成功；属性值应正确") {
	
	S32 i_Loop = 0;
	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttribGet[2];
	memset(rstru_RegionAttribGet,0,sizeof(rstru_RegionAttribGet));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1,&rstru_RegionAttribGet[0]));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DisplayHandleOsd1,&rstru_RegionAttribGet[1]));
	for( i_Loop = 0;i_Loop < 2;i_Loop++ )
	{	
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosX);
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosY);
		EXPECT_EQ(OSD_ENV_WIDTH,rstru_RegionAttribGet[i_Loop].i_Width);
		EXPECT_EQ(OSD_ENV_HEIGHT,rstru_RegionAttribGet[i_Loop].i_Height);
		EXPECT_EQ(CHDRV_OSD_COLOR_TYPE_ARGB1555,rstru_RegionAttribGet[i_Loop].enm_ColorType);
		EXPECT_EQ(CH_LAYER_OSD1,rstru_RegionAttribGet[i_Loop].enm_Layer);
		if(i_Loop == 1)
			EXPECT_EQ(CHDRV_OSD_REGION_DISPLAY,rstru_RegionAttribGet[i_Loop].enm_RegionType);		
		else
			EXPECT_EQ(CHDRV_OSD_REGION_DRAW,rstru_RegionAttribGet[i_Loop].enm_RegionType);
	}

}
CASE_F(TEST_OSD_LINK_2,
	"将两个region link在一起，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}
CASE_F(TEST_OSD_SHOWREGION_2,
	"调用show region，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}
CASE_F(TEST_OSD_CLEARREGION_1,
	"调用clear full draw region为白色，应返回ok") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR1555_WHITE;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR1555_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555));

}

CASE_F(TEST_OSD_DRAWRECT_2,
	"调用draw rect函数以draw region handle方式绘制方块，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR1555_RED;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB1555;	///
	CHDRV_OSD_ColorValue 		  ui_32Color = OSD_REGION_COLOR1555_RED;
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_32Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555)) << "CHDRV_OSD_DrawRect未正确实现";
}

CASE_F(TEST_OSD_DRAWRECT_3,
	"调用update region应返回ok，询问测试员，画面显示应符合预期") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	PROMPT("画面是否为白色背景上有个一个480X320的红色框?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect未实现";
	
}

CASE_F(TEST_OSD_DRAWRECT_2_1,
	"调用draw rect函数以draw region handle方式绘制透明方块，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = 0;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB1555;	///
	CHDRV_OSD_ColorValue 		  ui_32Color = 0;
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_32Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555)) << "CHDRV_OSD_DrawRect未正确实现";
}

CASE_F(TEST_OSD_DRAWRECT_3_1,
	"调用update region应返回ok，询问测试员，画面显示应符合预期") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	PROMPT("画面是否为白色背景上有个一个480X320的透明框?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect透明色未实现";
	
}

CASE_F(TEST_OSD_DELETEREGIOIN_2,
	"删除创建的所有region，应成功") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd1));
	grh_DrawHandleOsd1 = NULL;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd1));
	grh_DisplayHandleOsd1 = NULL;
	
}
CASE_F(TEST_OSD_CREATEREGION_3,
	"以合法参数创建一个osd1 draw region，其中宽高为720X576，颜色格式为8888，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;	
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd1)) ;
}
CASE_F(TEST_OSD_CREATEREGION_4,
	"以合法参数创建一个osd1 display region，其中宽高为720X576，颜色格式为8888，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
}
CASE_F(TEST_OSD_GETATTRIB_2,
	"针对刚刚创建的2个region，分别调用get attrib，应返回成功；属性值应正确") {
	
	S32 i_Loop = 0;
	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttribGet[2];
	memset(rstru_RegionAttribGet,0,sizeof(rstru_RegionAttribGet));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1,&rstru_RegionAttribGet[0]));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DisplayHandleOsd1,&rstru_RegionAttribGet[1]));
	for( i_Loop = 0;i_Loop < 2;i_Loop++ )
	{	
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosX);
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosY);
		EXPECT_EQ(OSD_ENV_WIDTH,rstru_RegionAttribGet[i_Loop].i_Width);
		EXPECT_EQ(OSD_ENV_HEIGHT,rstru_RegionAttribGet[i_Loop].i_Height);
		EXPECT_EQ(CHDRV_OSD_COLOR_TYPE_ARGB8888,rstru_RegionAttribGet[i_Loop].enm_ColorType);
		EXPECT_EQ(CH_LAYER_OSD1,rstru_RegionAttribGet[i_Loop].enm_Layer);
		if(i_Loop == 1)
			EXPECT_EQ(CHDRV_OSD_REGION_DISPLAY,rstru_RegionAttribGet[i_Loop].enm_RegionType);		
		else
			EXPECT_EQ(CHDRV_OSD_REGION_DRAW,rstru_RegionAttribGet[i_Loop].enm_RegionType);
	}

}

CASE_F(TEST_OSD_LINK_3,
	"将两个region link在一起，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}
CASE_F(TEST_OSD_SHOWREGION_3,
	"调用show region，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}
CASE_F(TEST_OSD_CLEARREGION_2,
	"调用clear full draw region为白色，应返回ok") {
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
		
}

CASE_F(TEST_OSD_DRAWRECT_4,
	"调用draw rect函数以draw region handle方式绘制方块，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR8888_BLUE;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB8888;	///
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB8888));
}

CASE_F(TEST_OSD_DRAWRECT_5,
	"调用update region应返回ok，询问测试员，画面显示应符合预期") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否为白色背景上有个一个480X320的蓝色框?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect未实现";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	

}
CASE_F(TEST_OSD_DRAWRECT_4_1,
	"调用draw rect函数以draw region handle方式绘制方块，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = 0;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB8888;	///
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB8888));
}

CASE_F(TEST_OSD_DRAWRECT_5_1,
	"调用update region应返回ok，询问测试员，画面显示应符合预期") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("画面是否为白色背景上有个一个480X320的透明框?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect透明色未实现";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	

}

CASE_F(TEST_OSD_CREATEREGION_5,
	"以合法参数创建一个osd2 draw region，其中宽高为720X576，颜色格式为8888，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd2)) ;
}
CASE_F(TEST_OSD_CREATEREGION_6,
	"以合法参数创建一个osd2 display region，其中宽高为720X576，颜色格式为8888，应返回成功") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	CH_Rect_t stru_rect;
	
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd2)) ;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetWindow(grh_DisplayHandleOsd2, &stru_rect)) ;
	LOG("CHDRV_OSD_GetWindow=(%d, %d, %d, %d)\n", stru_rect.s_X, stru_rect.s_Y, stru_rect.s_Width, stru_rect.s_Height);
}


CASE_F(TEST_OSD_GETATTRIB_3,
	"针对刚刚创建的2个region，分别调用get attrib，应返回成功；属性值应正确") {
	
	S32 i_Loop = 0;
	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttribGet[2];
	memset(rstru_RegionAttribGet,0,sizeof(rstru_RegionAttribGet));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd2,&rstru_RegionAttribGet[0]));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetRegionAttrib(grh_DisplayHandleOsd2,&rstru_RegionAttribGet[1]));
	for( i_Loop = 0;i_Loop < 2;i_Loop++ )
	{	
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosX);
		EXPECT_EQ(0,rstru_RegionAttribGet[i_Loop].i_PosY);
		EXPECT_EQ(OSD_ENV_WIDTH,rstru_RegionAttribGet[i_Loop].i_Width);
		EXPECT_EQ(OSD_ENV_HEIGHT,rstru_RegionAttribGet[i_Loop].i_Height);
		EXPECT_EQ(CHDRV_OSD_COLOR_TYPE_ARGB8888,rstru_RegionAttribGet[i_Loop].enm_ColorType);
		EXPECT_EQ(CH_LAYER_OSD2,rstru_RegionAttribGet[i_Loop].enm_Layer);
		if(i_Loop == 1)
			EXPECT_EQ(CHDRV_OSD_REGION_DISPLAY,rstru_RegionAttribGet[i_Loop].enm_RegionType);		
		else
			EXPECT_EQ(CHDRV_OSD_REGION_DRAW,rstru_RegionAttribGet[i_Loop].enm_RegionType);
	}

}

CASE_F(TEST_OSD_LINK_4,
	"将两个region link在一起，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd2,grh_DisplayHandleOsd2));
	
}
CASE_F(TEST_OSD_SHOWREGION_4,
	"调用show region，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd2));

}
CASE_F(TEST_OSD_CLEARAREA_2,
	"调用clear full draw region为白色，应返回ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd2,0));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd2, 0, 0, OSD_ENV_WIDTH, OSD_ENV_HEIGHT / 2, OSD_REGION_COLOR8888_BLUE));
	
}

CASE_F(TEST_OSD_DRAWRECT_6,
	"调用draw rect函数以draw region handle方式绘制方块，然后调用get area data获取数据进行比较，应成功") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR8888_GREEN;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB8888;	///

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd2, 0, 0, &stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
//	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd2,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB8888));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd2,0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB8888));
}


CASE_F(TEST_OSD_DRAWRECT_7,
	"调用update region应返回ok，询问测试员，画面显示应符合预期，与osd1的叠加遮挡效果正常") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd2));
		
	PROMPT("画面是否为下半部白色，上半部为蓝色，上左有一个200X200的绿色方块?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "osd2未正确实现";
}

CASE_F(TEST_OSD_SETALPHA_1,
	"调用set osd alpha设置osd1的透明度分别为100,80,40,0，osd2的透明度应不受影响，询问测试员效果是否OK") {
	int a;
	PROMPT("测试OSD1的透明度，全屏的白色底色部分为OSD1，请注意观察，按任意键继续..");
	test_input_none();
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	for(a = 0; a <= 100; a += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1,a));	
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
		PROMPT("OSD1透明度为%d\n", a);
		CH_RTOS_TaskDelay(200);
		clear_promt();
	}
	PROMPT("画面变化是否正常?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "osd1透明度设置未正确实现";
}


CASE_F(TEST_OSD_HIDE_1,
	"调用hide region 2，应只可见osd1，询问测试员效果是否一致") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_HideRegion(grh_DisplayHandleOsd2));
	PROMPT("屏幕上是否只见白色全屏背景?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_HideRegion未正确实现";
}

CASE_F(TEST_OSD_HIDE_2,
	"调用hide region 1，region 1和2应均不可见，询问测试员效果是否一致") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_HideRegion(grh_DisplayHandleOsd1));
	PROMPT("屏幕上是否无OSD显示，只见方格的I帧画面?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_HideRegion未正确实现";
}

CASE_F(TEST_OSD_SHOWREGION_5,
	"调用show region 2，应只可见osd2，询问操作员效果是否一致") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DisplayHandleOsd2));
	PROMPT("屏幕上是否下半部分为I帧的方格画面，上半部分为蓝色背景和绿色小方块?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_ShowRegion未正确实现";
}

CASE_F(TEST_OSD_SHOWREGION_6,
	"调用show region 1，osd 1和2均可见，询问操作员效果是否一致") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DisplayHandleOsd1));
	PROMPT("屏幕上是否下半部分为白色画面，上半部分为蓝色背景和绿色小方块?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_ShowRegion未正确实现";

}

CASE_F(TEST_OSD_POSITION_1,
	"osd1和osd2上的方块位置、大小，与背景上的I帧标尺是否一致，询问操作员反馈结果") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1, 60));	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd2, 60));	
	PROMPT("屏幕上是否下半部分为白色画面，上半部分为蓝色背景和200X200绿色小方块，位置是否与标尺符合?是请输入Y，否则输入N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "显示位置与I帧不符合";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1, 100));	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd2, 100));

	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UnlinkRegion(grh_DrawHandleOsd2, grh_DisplayHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd2));
	grh_DrawHandleOsd2 = NULL;
	grh_DisplayHandleOsd2 = NULL;


	
}

CASE_F(TEST_OSD_CREATE_3,
	"一次性在OSD1上创建2个OSD_REGION_WH大小，颜色格式为8888的draw region，应返回成功。然后释放") {

	CHDRV_OSD_RegionHandle rh_DrawRegionOSD1 = NULL;
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD2 = NULL;
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&rh_DrawRegionOSD1)) ;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&rh_DrawRegionOSD2)) ;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(rh_DrawRegionOSD1)) ;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(rh_DrawRegionOSD2)) ;
}

CASE_F(TEST_OSD_CREATE_4,
	"一次性在OSD2上创建2个OSD_REGION_WH大小，颜色格式为8888的draw region，应返回成功。然后释放") {
	
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD1 = NULL;
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD2 = NULL;
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env获取） 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//（env获取）
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&rh_DrawRegionOSD1)) ;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&rh_DrawRegionOSD2)) ;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(rh_DrawRegionOSD1)) ;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(rh_DrawRegionOSD2)) ;
}

CASE_F(TEST_OSD_ALLOCMEM_2,
	"osd malloc 4 Mbytes内存，应返回成功") {
	void *  rpv_Memory = NULL;

	EXPECT_NE((void*)NULL, rpv_Memory = CHDRV_OSD_AllocateMemory(0x400000)) << "分配4M OSD MEM不成功";
	if(rpv_Memory != NULL)
	{
		CHDRV_OSD_FreeMemory(rpv_Memory);
	}

}

CASE_F(TEST_OSD_PICTURE1,
	"调用get image info，参数为路径方式，获取GIF图片的信息，应成功") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_GIF_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_GIF_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
	EXPECT_EQ(fw_getenv_i("OSD_GIF_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";

}

CASE_F(TEST_OSD_PICTURE2,
	"调用get image info，参数为路径方式，获取JPEG图片的信息，应成功") {

	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_JPEG_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_JPG_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
	EXPECT_EQ(fw_getenv_i("OSD_JPG_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";

	
}

CASE_F(TEST_OSD_PICTURE3,
	"调用get image info，参数为路径方式，获取PNG图片的信息，应成功") {
	

	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_PNG_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_PNG_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
	EXPECT_EQ(fw_getenv_i("OSD_PNG_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";

}

CASE_F(TEST_OSD_PICTURE4,
	"调用get image info，参数为路径方式，获取BMP图片的信息，应成功") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_BMP_PATH;
	
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_BMP_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
	EXPECT_EQ(fw_getenv_i("OSD_BMP_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";
}

CASE_F(TEST_OSD_PICTURE5,
	"调用get image info，参数为内存指针方式，获取GIF图片的信息，应成功") {

	int len;
	CHDRV_OSD_ImageData_t  stru_ImgData ;

	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_GIF_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "内存不足或文件不存在,filename=" << OSD_GIF_PATH;
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << (const char *) OSD_GIF_PATH;
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	else
	{
		stru_ImgData.ui_Length = len;
	}
	if(stru_ImgData.ui_Length > 0)
	{
		CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
		memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		EXPECT_EQ(fw_getenv_i("OSD_GIF_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
		EXPECT_EQ(fw_getenv_i("OSD_GIF_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}
	
}

CASE_F(TEST_OSD_PICTURE6,
	"调用get image info，参数为内存指针方式，获取JPEG图片的信息，应成功") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_JPEG_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "内存不足或文件不存在,filename=" << OSD_JPEG_PATH;
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << (const char *) OSD_JPEG_PATH;
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	else
	{
		stru_ImgData.ui_Length = len;
	}
	if(stru_ImgData.ui_Length > 0)
	{
		CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
		memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		EXPECT_EQ(fw_getenv_i("OSD_JPG_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
		EXPECT_EQ(fw_getenv_i("OSD_JPG_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}

CASE_F(TEST_OSD_PICTURE7,
	"调用get image info，参数为内存指针方式，获取PNG图片的信息，应成功") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_PNG_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "内存不足或文件不存在,filename=" << OSD_PNG_PATH;
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << (const char *) OSD_PNG_PATH;
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	else
	{
		stru_ImgData.ui_Length = len;
	}
	if(stru_ImgData.ui_Length > 0)
	{
		CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
		memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		EXPECT_EQ(fw_getenv_i("OSD_PNG_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
		EXPECT_EQ(fw_getenv_i("OSD_PNG_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}

CASE_F(TEST_OSD_PICTURE8,
	"调用get image info，参数为内存指针方式，获取BMP图片的信息，应成功") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_BMP_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "内存不足或文件不存在,filename=" << OSD_BMP_PATH;
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << (const char *) OSD_BMP_PATH;
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	else
	{
		stru_ImgData.ui_Length = len;
	}
	if(stru_ImgData.ui_Length > 0)
	{
		CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
		memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
		EXPECT_EQ(fw_getenv_i("OSD_BMP_WIDTH"),stru_ImgInfo.ui_Width) << "获取图片信息宽度错误";
		EXPECT_EQ(fw_getenv_i("OSD_BMP_HEIGHT"),stru_ImgInfo.ui_Height) << "获取图片信息高度错误";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}









CASE_F(TEST_OSD_PICTURE9,
	"调用decoder image GIF，参数为路径方式，输出宽高与原图一致，应成功;比较内存数据与预期一致") {


	char data_path[512];

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_GIF_PATH, OSD_GIF_WIDTH, OSD_GIF_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_GIF_PATH, data_path, OSD_GIF_WIDTH, OSD_GIF_HEIGHT)) << "图片解析不正确, filename=" << OSD_GIF_PATH;

}

CASE_F(TEST_OSD_PICTURE10,
	"调用decoder image JPEG，参数为路径方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {

	char data_path[512];

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_JPEG_PATH, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_JPEG_PATH, data_path, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT)) << "图片解析不正确, filename=" << OSD_JPEG_PATH;
	
}

CASE_F(TEST_OSD_PICTURE11,
	"调用decoder image PNG，参数为路径方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {


	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_PNG_PATH, OSD_PNG_WIDTH, OSD_PNG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_PNG_PATH, data_path, OSD_PNG_WIDTH, OSD_PNG_HEIGHT)) << "图片解析不正确, filename=" << OSD_PNG_PATH;
}

CASE_F(TEST_OSD_PICTURE12,
	"调用decoder image BMP，参数为路径方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {


	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_BMP_PATH, OSD_BMP_WIDTH, OSD_BMP_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_BMP_PATH, data_path, OSD_BMP_WIDTH, OSD_BMP_HEIGHT)) << "图片解析不正确, filename=" << OSD_BMP_PATH;
}

CASE_F(TEST_OSD_PICTURE13,
	"调用decoder image GIF，参数为内存指针方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_GIF_PATH, OSD_GIF_WIDTH, OSD_GIF_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_GIF_PATH, data_path, OSD_GIF_WIDTH, OSD_GIF_HEIGHT)) << "图片解析不正确, filename=" << OSD_GIF_PATH;

}

CASE_F(TEST_OSD_PICTURE14,
	"调用decoder image JPEG，参数为内存指针方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_JPEG_PATH, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_JPEG_PATH, data_path, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT)) << "图片解析不正确, filename=" << OSD_JPEG_PATH;

}

CASE_F(TEST_OSD_PICTURE15,
	"调用decoder image PNG，参数为内存指针方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {
	

	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_PNG_PATH, OSD_PNG_WIDTH, OSD_PNG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_PNG_PATH, data_path, OSD_PNG_WIDTH, OSD_PNG_HEIGHT)) << "图片解析不正确, filename=" << OSD_PNG_PATH;

}

CASE_F(TEST_OSD_PICTURE16,
	"调用decoder image BMP，参数为内存指针方式，输出宽高与原图一致，应成功；比较内存数据与预期一致") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_BMP_PATH, OSD_BMP_WIDTH, OSD_BMP_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_BMP_PATH, data_path, OSD_BMP_WIDTH, OSD_BMP_HEIGHT)) << "图片解析不正确, filename=" << OSD_BMP_PATH;
}

CASE_F(TEST_OSD_PICTURE17,
	"调用decoder image GIF，参数为路径方式，输出宽高是原图的三分之一，应成功；比较内存数据与预期一致") {
	char data_path[512];

	CHDRV_OSD_ImageData_t stru_ImgData;
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_GIF_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_GIF_COMMON_FILE");
}

CASE_F(TEST_OSD_PICTURE18,
	"调用decoder image JPEG，参数为路径方式，输出宽高是原图的三分之一，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_JPG_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_JPG_COMMON_FILE");


}

CASE_F(TEST_OSD_PICTURE19,
	"调用decoder image PNG，参数为路径方式，输出宽高是原图的三分之一，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_PNG_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_PNG_COMMON_FILE");


}
CASE_F(TEST_OSD_PICTURE20,
	"调用decoder image BMP，参数为路径方式，输出宽高是原图的三分之一，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_BMP_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_BMP_COMMON_FILE");


}
CASE_F(TEST_OSD_PICTURE21,
	"调用decoder image GIF，参数为路径方式，输出宽高是原图的三倍，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_GIF_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_GIF_LITTLE_FILE");
}
CASE_F(TEST_OSD_PICTURE22,
	"调用decoder image JPEG，参数为路径方式，输出宽高是原图的三倍，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_JPG_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_JPG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE23,
	"调用decoder image PNG，参数为路径方式，输出宽高是原图的三倍，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_PNG_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_PNG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE24,
	"调用decoder image BMP，参数为路径方式，输出宽高是原图的三倍，应成功；比较内存数据与预期一致") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_BMP_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "图片解析不正确, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE25,
	"调用decoder image GIF，参数为路径方式，输出宽高为0，应输出原图大小，应成功；比较内存数据与预期一致") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_LITTLE_FILE"), data_path, 0, 0)) << "图片解析不正确, filename=" << fw_getenv("OSD_GIF_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE26,
	"调用decoder image JPEG，参数为路径方式，输出宽高为0，应输出原图大小，应成功；比较内存数据与预期一致") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_LITTLE_FILE"), data_path, 0, 0)) << "图片解析不正确, filename=" << fw_getenv("OSD_JPG_LITTLE_FILE");

}
CASE_F(TEST_OSD_PICTURE27,
	"调用decoder image PNG，参数为路径方式，输出宽高为0，应输出原图大小，应成功；比较内存数据与预期一致") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_LITTLE_FILE"), data_path, 0, 0)) << "图片解析不正确, filename=" << fw_getenv("OSD_PNG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE28,
	"调用decoder image BMP，参数为路径方式，输出宽高为0，应输出原图大小，应成功；比较内存数据与预期一致") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	char data_path[512];
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_LITTLE_FILE"), data_path, 0, 0)) << "图片解析不正确, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE29,
	"循环调用decoder image jpeg，参数为路径方式，输出宽高为整屏，应成功；比较内存数据与预期一致") {
	
	char data_path[512], file_name[32];
	int i;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	for(i = 1; ; i++)
	{
		sprintf(file_name, "OSD_JPG_LARGE_FILE_%d", i);
		if(fw_getenv(file_name) == NULL)
		{
			break;
		}
		sprintf(data_path, "%s.%dX%d.argb", fw_getenv(file_name), fw_getenv_i("OSD_PIC_REGION_WIDTH"),fw_getenv_i("OSD_PIC_REGION_HEIGHT"));
		EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv(file_name), data_path, fw_getenv_i("OSD_PIC_REGION_WIDTH"),fw_getenv_i("OSD_PIC_REGION_HEIGHT"))) << "图片解析不正确, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
	}
}

#endif /* ENABLE_TESTCASE */

static int OsdDeleteRegion(void)
{
	int ret = 0;
	int i;
	CHDRV_OSD_RegionHandle handle, handle2;
	for(i = CH_LAYER_OSD1; i <= CH_LAYER_OSD2; i++)
	{
		do
		{
			handle = osd_GetHandle((CH_LAYER_e)i, CHDRV_OSD_REGION_DRAW);
			if(handle)
			{
				ret = CHDRV_OSD_ClearFullRegion(handle, 0);
				ret = CHDRV_OSD_UpdateRegion(handle);
				handle2 = osd_GetHandle((CH_LAYER_e)i, CHDRV_OSD_REGION_DISPLAY);
				if(handle2)
				{
					ret = CHDRV_OSD_UnlinkRegion( handle, handle2 );
					ret |= CHDRV_OSD_DeleteRegion(handle2);
				}
				ret |= CHDRV_OSD_DeleteRegion(handle);
				if(ret != CHDRV_OSD_OK ) 
				{
					LOG("ERROR: CHMID_GRP_DeleteRegion handle return %d!\n", ret);
				}
			}
		}while(handle != NULL);
	}
	
	gh_CacheRegion = NULL;
	gh_MainRegion = NULL;
	LOG("Complete\n");
	return 0;
}


int OsdCreateRegion(CH_LAYER_e layer, CHDRV_OSD_COLOR_TYPE_e color_type, int width, int height)
{
	CHDRV_OSD_RegionParam_t stru_RegionParams;
	int 	ret;
	CH_Rect_t  stru_Rect;

	delete_prompt_region();
	OsdDeleteRegion();
	stru_RegionParams.i_PosX	= 0;
	stru_RegionParams.i_PosY	= 0;
	stru_RegionParams.i_Width 				= width;
	stru_RegionParams.i_Height 				= height;
	stru_RegionParams.enm_ColorType 		= color_type;
	stru_RegionParams.enm_ClutColorType 	= CHDRV_OSD_COLOR_TYPE_CLUT8;
	stru_RegionParams.enm_Layer 			= layer;
	stru_RegionParams.enm_RegionType 		= CHDRV_OSD_REGION_DISPLAY;
	stru_RegionParams.h_MemPartion 			= gh_APP_HEAP;
	ret = CHDRV_OSD_CreateRegion(&stru_RegionParams,
								  		&gh_MainRegion);
	if ( CHDRV_OSD_OK != ret )
	{
		LOG("ERROR: CHDRV_OSD_CreateRegion gh_MainRegion return %d!\n", ret);
		return -1;
	}
	
	stru_RegionParams.enm_RegionType 		= CHDRV_OSD_REGION_DRAW;
	ret = CHDRV_OSD_CreateRegion ( &stru_RegionParams,
								  		&gh_CacheRegion);
	if ( CHDRV_OSD_OK != ret )
	{
		LOG("ERROR: CHDRV_OSD_CreateRegion gh_CacheRegion return %d!\n", ret);
		return -1;
	}


	ret = CHDRV_OSD_LinkRegion( gh_CacheRegion, gh_MainRegion );
	if ( CHDRV_OSD_OK != ret )
	{
		LOG("ERROR: CHDRV_OSD_LinkRegion return %d!\n", ret);
		return -1;
	}

	ret = CHDRV_OSD_ClearFullRegion( gh_CacheRegion, 0 );
	if ( CHDRV_OSD_OK != ret )
	{
		LOG("ERROR: CHDRV_OSD_ClearFullRegion return %d!\n", ret);
		return -1;
	}
	stru_Rect.s_X = stru_RegionParams.i_PosX;
	stru_Rect.s_Y = stru_RegionParams.i_PosY;
	stru_Rect.s_Width = stru_RegionParams.i_Width;
	stru_Rect.s_Height = stru_RegionParams.i_Height;
	ret = CHDRV_OSD_SetWindow( gh_MainRegion,  stru_Rect);
	if (CHDRV_OSD_OK != ret)
	{
		LOG("ERROR: CHDRV_OSD_SetWindow return %d!\n", ret);
		return -1;	
	}
	ret = CHDRV_OSD_ShowRegion( gh_MainRegion );
	if (CHDRV_OSD_OK != ret)
	{
		LOG("ERROR: CHDRV_OSD_ShowRegion return %d!\n", ret);
		return -1;	
	}
		

	LOG("Complete\n");
	return 0;
}


static int osd_rect(int argc, CMD_PARAM argv[])
{
	CHDRV_OSD_RegionAttrib_t stru_RegionAttrib;
	if(!gh_CacheRegion)
	{
		LOG("请先创建OSD!可使用osd.pic命令\n");
		return -1;
	}
	memset(&stru_RegionAttrib, 0, sizeof(stru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_GetRegionAttrib(gh_CacheRegion, &stru_RegionAttrib), CHDRV_OSD_OK);
	if(argv[1].get_i() != stru_RegionAttrib.i_PosX
		|| argv[2].get_i() != stru_RegionAttrib.i_PosY
		|| argv[3].get_i() != stru_RegionAttrib.i_Width
		|| argv[4].get_i() != stru_RegionAttrib.i_Height)
	{
		EXPECT_EQ(CHDRV_OSD_ClearArea(gh_CacheRegion, argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), argv[4].get_i(), argv[5].get_i()), CHDRV_OSD_OK);
	}
	else
	{
		EXPECT_EQ(CHDRV_OSD_ClearFullRegion(gh_CacheRegion, argv[5].get_i()), CHDRV_OSD_OK);
		LOG("CHDRV_OSD_ClearFullRegion \n");
	}
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(gh_CacheRegion));
	return 0;
}

static int osd_getrect(int argc, CMD_PARAM argv[])
{
	CHDRV_OSD_RegionAttrib_t stru_RegionAttrib;
	U8 *pic = NULL;
	int i;
	
	if(!gh_CacheRegion)
	{
		LOG("请先创建OSD!可使用osd.pic命令\n");
		return -1;
	}
	if(argv[3].get_i() <= 0 ||  argv[4].get_i() <= 0)
	{
		LOG("参数错误! x=%d,y=%d,w=%d,h=%d\n", argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), argv[4].get_i(), argv[5].get_i());
		return -1;
	}
	memset(&stru_RegionAttrib, 0, sizeof(stru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_GetRegionAttrib(gh_CacheRegion, &stru_RegionAttrib), CHDRV_OSD_OK);
	pic = (U8*)CHDRV_OSD_AllocateMemory(argv[3].get_i() * argv[4].get_i() * stru_RegionAttrib.i_bitPerPixel / 8);
	if(!pic)
	{
		LOG("内存分配失败! x=%d,y=%d,w=%d,h=%d,i_bitPerPixel=%d\n", 
			argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), argv[4].get_i(), stru_RegionAttrib.i_bitPerPixel);
		return -1;
	}
	EXPECT_EQ(CHDRV_OSD_GetAreaData(gh_CacheRegion, argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), argv[4].get_i(), pic), CHDRV_OSD_OK);
	for(i = 0; i < argv[3].get_i() * argv[4].get_i() * stru_RegionAttrib.i_bitPerPixel / 8; i++)
	{
		printf("%02x ", pic[i]);
		if(i > 0 && (i % 16) == 0)
		{
			printf("\r\n");
		}
	}
	CHDRV_OSD_FreeMemory(pic);
	return 0;
}

static int osd_pic(int argc, CMD_PARAM argv[])
{
	int len;
	char* pbuf = NULL;
	CHDRV_OSD_ImageData_t stru_ImgData;
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	CH_Rect_t stru_Rect;

	OsdDeleteRegion();
	EXPECT_GT(len = fsys_read_file(argv[5].get_str(), pbuf), 0);
	EXPECT_EQ(OsdCreateRegion((CH_LAYER_e)argv[1].get_i(), (CHDRV_OSD_COLOR_TYPE_e)argv[2].get_i(), argv[3].get_i(), argv[4].get_i()), 0);

	if(len > 0)
	{
		if(strstr(argv[5].get_str(), ".jpg") 
			|| strstr(argv[5].get_str(), ".gif")
			|| strstr(argv[5].get_str(), ".png")
			|| strstr(argv[5].get_str(), ".bmp")
			|| strstr(argv[5].get_str(), ".jpeg"))
		{
			stru_ImgData.puc_FileName = NULL;
			stru_ImgData.puc_ImgAddr = (U8*)pbuf;
			stru_ImgData.ui_Length = len;
			memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
			EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
			LOG("stru_ImgInfo ui_Width=%d,ui_Height=%d\n", stru_ImgInfo.ui_Width, stru_ImgInfo.ui_Height);
			memset(&stru_Rect, 0, sizeof(stru_Rect));
			stru_Rect.s_Width = MIN(argv[3].get_i(), stru_ImgInfo.ui_Width);
			stru_Rect.s_Height = MIN(argv[4].get_i(), stru_ImgInfo.ui_Height);
			LOG("stru_Rect ui_Width=%d,ui_Height=%d\n", stru_Rect.s_Width, stru_Rect.s_Height);
			EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_DecodeImage(gh_CacheRegion, stru_Rect, &stru_ImgData));
		}
		else
		{
			CHDRV_OSD_RegionAttrib_t stru_RegionAttrib;
			CHDRV_OSD_GetRegionAttrib(gh_CacheRegion, &stru_RegionAttrib);
			memcpy(stru_RegionAttrib.puc_RegionData, pbuf, stru_RegionAttrib.i_bitPerPixel / 8 * stru_RegionAttrib.i_Height * stru_RegionAttrib.i_Width);
		}
	}
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(gh_CacheRegion));
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	return 0;
}

static int osd_trans(int argc, CMD_PARAM argv[])
{
	
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_SetRegionAlpha(gh_MainRegion, argv[1].get_i()));
	return 0;
}

static int osd_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_OSD_Init();
	LOG("CHDRV_OSD_Init return %d\n", ret);
	return ret;
}

static int osd_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_OSD_Term();
	LOG("CHDRV_OSD_Term return %d\n", ret);
	return ret;
}

static int osd_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t stru_ver;
	
	ret = CHDRV_OSD_GetVersion(&stru_ver);
	LOG("CHDRV_OSD_GetVersion return %d\n", ret);
	return ret;
}

static int osd_delregion(int argc, CMD_PARAM argv[])
{
	int ret;
	
	ret = OsdDeleteRegion();
	LOG("OsdDeleteRegion return %d\n", ret);
	return ret;
}


REGISTER_CMD(osd_init, "初始化osd模块", "",  "");
REGISTER_CMD(osd_term, "销毁osd模块", "",  "");
REGISTER_CMD(osd_ver, "获取osd版本号", "",  "");
REGISTER_CMD(osd_pic, "解码并显示一副图片", "layer | color_type | width | height | pic_file",  "3 0 1280 720 ./1280_720.bmp");
REGISTER_CMD(osd_delregion, "删除显示图片时创建的region", "",  "");
REGISTER_CMD(osd_trans, "设置OSD透明度", "alpha(0-100)",  "100");
REGISTER_CMD(osd_rect, "在OSD上绘制方框", "x|y|w|h|color",  "0 0 720 576 0");
REGISTER_CMD(osd_getrect, "捕获OSD某一区域内的数据并打印", "x|y|w|h",  "0 0 720 2");
REGISTER_CMD(osd_info, "获得osd的相关信息，打印到串口", "",  "");



