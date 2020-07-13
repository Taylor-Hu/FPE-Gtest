/**
@file OSD_osd.cpp
@def group osd���Լ�

@author ���繫˾����������:	����ǿ
@date   2014��4��10��  
@version V4.0
@todo     ---ʱ��---����-------------�޸Ĳ��ּ�ԭ��----------
@todo     1 2014��4��10�� ����ǿ  ����


@{
*/

/*���Լ����ƶ���*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif

#define CASE_NAME TestOsd


#include "test_menu.h"
#include "test_modules.h"

static CHDRV_OSD_RegionHandle gh_MainRegion = NULL, gh_CacheRegion = NULL;	

#define OSD_USEEABLE					(1) //���Բ��ã�ֻΪ�������ʹ��
#define OSD_SMOOTH 						(1) //��������

#define OSD_ENV_WIDTH					((S32)fw_getenv_i("OSD_REGION_WIDTH"))
#define OSD_ENV_HEIGHT 					((S32)fw_getenv_i("OSD_REGION_HEIGHT"))

#define OSD_PIC_ENV_WIDTH				((S32)fw_getenv_i("OSD_PIC_REGION_WIDTH"))
#define OSD_PIC_ENV_HEIGHT 				((S32)fw_getenv_i("OSD_PIC_REGION_HEIGHT"))





#define OSD_HANDLE  					((void *)0x87650000) //bmphandle(the first region handle) һ��������Ч���
#define OSD_HANDLE_INVALID   			((void *)0x87650129)  //handle count max=128; ����������ֵ��һ������Ч����


#define OSD_PARAM_RIGHT					(200)     // x<1280 y<720
#define OSD_PARAM_MINUS    				(-1)		//����Ϊ������һ����Ϊ��Ч����
#define OSD_PARAM_OVER_WRANGE 	 		(OSD_PIC_ENV_WIDTH + 1) 	//  �������ڿ�ȵĸ�ֵ����Ϊ�����������֮һ
#define OSD_PARAM_OVER_HRANGE  			(OSD_PIC_ENV_HEIGHT + 1)  //  �������ڸ߶ȵĸ�ֵ����Ϊ�����������֮һ

#define OSD_GIFDEC_COUNT_INVALID   		(17)  //GIFDEC_COUNT=16
#define OSD_GIFDEC_RIGHT   				(1)   //GIFDEC_COUNT=16

#define OSD_BOOL_FLASE     				(1)   // bool flase
#define OSD_B00L_TRUE					(0)   // bool true 



#define OSD_PIC_PATH					(fw_getenv("OSD_BMP_FILEPATH"))  //I֡ͼƬ·��

#define OSD_GIF_PATH					(fw_getenv("OSD_GIF_FILEPATH"))  	 	//ԭͼ�����·��
#define OSD_GIF_WIDTH 					((S32)fw_getenv_i("OSD_GIF_WIDTH"))		//ԭͼ�Ŀ�
#define OSD_GIF_HEIGHT					((S32)fw_getenv_i("OSD_GIF_HEIGHT"))	//ԭͼ�ĸ�


#define OSD_JPEG_PATH					(fw_getenv("OSD_JPEG_FILEPATH"))
#define OSD_JPEG_WIDTH 					((S32)fw_getenv_i("OSD_JPG_WIDTH"))
#define OSD_JPEG_HEIGHT					((S32)fw_getenv_i("OSD_JPG_HEIGHT"))



#define OSD_PNG_PATH					(fw_getenv("OSD_PNG_FILEPATH"))
#define OSD_PNG_WIDTH 					((S32)fw_getenv_i("OSD_PNG_WIDTH"))
#define OSD_PNG_HEIGHT					((S32)fw_getenv_i("OSD_PNG_HEIGHT"))


#define OSD_BMP_PATH					(fw_getenv("OSD_BMP_FILEPATH"))
#define OSD_BMP_WIDTH 					((S32)fw_getenv_i("OSD_BMP_WIDTH"))
#define OSD_BMP_HEIGHT					((S32)fw_getenv_i("OSD_BMP_HEIGHT"))



#define OSD_MEM_WIDTH					(480)     	//���� bitblitʱ�������ڴ���region�Ŀ�
#define OSD_MEM_HEIGHT					(320)		//���� bitblitʱ�������ڴ���region�ĸ�





#define OSD_REGION_COLOR4444_WHITE		(0xFFFF)   //argb  4444����ɫ
#define OSD_REGION_COLOR4444_RED		(0xFF00)   //argb  4444����ɫ
#define OSD_REGION_COLOR4444_GREEN		(0xF0F0)   //argb  4444����ɫ
#define OSD_REGION_COLOR4444_BLUE		(0xF00F)   //argb  4444����ɫ
#define OSD_REGION_COLOR4444_BPP		(2)		   //argb  4444 region �� byte per pixel
#define OSD_REGION_COLOR_CLEAR			(0x0)      // ��ɫ


#define OSD_REGION_COLOR8888_WHITE		(0xFFFFFFFF)   //argb  8888����ɫ
#define OSD_REGION_COLOR8888_RED		(0xFFFF0000)   //argb  8888����ɫ
#define OSD_REGION_COLOR8888_GREEN		(0xFF00FF00)   //argb  8888����ɫ
#define OSD_REGION_COLOR8888_BLUE		(0xFF0000FF)   //argb  8888����ɫ


#define OSD_REGION_COLOR1555_WHITE		(0xFFFF)    //argb 1555 ��ɫ
#define OSD_REGION_COLOR1555_RED		(0xFC00)    //argb 1555 ��ɫ



static CHDRV_OSD_RegionHandle	     grh_BmpDrawHandle = NULL;	//������ʾI֡ͼƬ��draw region handle
static CHDRV_OSD_RegionHandle 	  grh_BmpDisplayHandle = NULL;	//������ʾI֡ͼƬ��display region handle

static CHDRV_OSD_RegionHandle	 	grh_DrawHandleOsd1 = NULL;   //osd1��draw region handle
static CHDRV_OSD_RegionHandle 	 grh_DisplayHandleOsd1 = NULL;	//osd1��display region handle

static CHDRV_OSD_RegionHandle	 	 grh_DrawHandleMem = NULL;	//��MEM�ĳ���Ϊ������region handle

static CHDRV_OSD_RegionHandle	    grh_DrawHandleOsd2 = NULL;    //osd2��draw region handle
static CHDRV_OSD_RegionHandle	 grh_DisplayHandleOsd2 = NULL;    //osd2��display region handle

static U8 *	 gpuc_Data = NULL;	//get areaʱ�����������Թ�put areaʱʹ�õ��ڴ��ָ��
static void * 	 gpuc_Mem = NULL;	//����bufbitblitʱ�������ڴ���ָ��

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

//���region�Ƿ񻹴��ڣ���������������´���,�����ڲ��Թ�������Ϊ���������¿վ��,Ӱ����Խ��
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
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
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
//�Ա�ĳ����������Ƿ����Ԥ��
/*
void *					rpAreaHandle,//�Ա������������region���
S32 					ri_PosX, 	//�Ա������x,y,w,h
S32 					ri_PosY,
S32 					ri_Width,
S32 					ri_Height,
void  					rp_Data		 //Ԥ������
CHDRV_OSD_COLOR_TYPE_e  enm_ColorType //Ԥ������

*/
//�Ա�ĳ����������Ƿ����Ԥ�ڣ�Ԥ������ֻ��һ��ֵ
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
void *					rpAreaHandle,//�Ա������������region���
S32 					ri_PosX, 	//�Ա������x,y,w,h
S32 					ri_PosY,
S32 					ri_Width,
S32 					ri_Height,
void * 					rp_Data		//Ԥ������
CHDRV_OSD_COLOR_TYPE_e  enm_ColorType //ͼ�ε���������

*/
//�Ա�ĳ����������Ƿ����Ԥ��
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
			PROMPT("%s��ʾ�Ƿ����� pos=(%d,%d,%d,%d),�����ϲ�Ӧ������ɫ��,��������Y����������N", (const char *) rpuc_PicName, ri_PosX,ri_PosY,ri_Width,ri_Height);
			EXPECT_EQ(CH_TRUE, ret = test_input_yes()) << "����Ա��Ϊ��ʾ����ȷ,filename=" <<  (const char *) rpuc_PicName << ",size=" << ret;
			if(ret)
			{
				ret = fsys_write_file((const char*)rpv_Data, (char*)puc_RegionData, ri_Width*ri_Height*i_ColorType);
				if(ret == ri_Width*ri_Height*i_ColorType)
				{
					LOG("����ģ��%s\n", (const char*)rpv_Data);
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
			//EXPECT_EQ(*(puc_RegionData+i_Loop ) , *((U8 *)puc_ImgData+i_Loop)) << "��������ݲ�һ��, i_Loop=" << i_Loop << "pic name=" << rpuc_PicName << ",size=" << ri_Width*ri_Height*i_ColorType;
			if( *(puc_RegionData+i_Loop ) != *((U8 *)puc_ImgData+i_Loop))
			{
				PROMPT("%s�������ݶԱȲ����ϣ��˹��۲���ʾ�Ƿ�����? pos=(%d,%d,%d,%d), ��������Y����������N", (const char *) rpuc_PicName, ri_PosX,ri_PosY,ri_Width,ri_Height);
				EXPECT_EQ(CH_TRUE, ret = test_input_yes()) << "����Ա��Ϊ��ʾ����ȷ. pic name=" << rpuc_PicName << ",size=" << ri_Width*ri_Height*i_ColorType;
				if(ret)
				{
					ret = fsys_write_file((const char*)rpv_Data, (char*)puc_RegionData, ri_Width * ri_Height * i_ColorType);
					if(ret == ri_Width * ri_Height * i_ColorType)
					{
						LOG("����ģ��%s\n", rpv_Data);
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
		EXPECT_EQ(0, ret = Osd_Compare_Data(grh_DrawHandleOsd1,stru_Rect.s_X,stru_Rect.s_Y,w, h,(void*)data_name, stru_RegionAttrib.enm_ColorType, (char*)file_name)) << "�������filename=" << file_name;
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
	<<(len == -1 ? "�ļ�������" : "�ڴ����ʧ��" )<< "filename=" << file_name;
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
		EXPECT_EQ(0, ret = Osd_Compare_Data(grh_DrawHandleOsd1,stru_Rect.s_X ,stru_Rect.s_Y,w,h,(void*)data_name,stru_RegionAttrib.enm_ColorType, (char*)file_name)) << "�������filename=" << file_name;
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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init());
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}


CASE_F(TEST_OSD_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	//EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Term());
	//EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_Init());
	//test_av_ready();
}


CASE_F(TEST_OSD_INIT3,
	"����CHDRV_OSD_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_OSD_DELAY,
	"�ۿ���Ƶ�������") {
	//CH_RTOS_TaskDelay(2000);
}
 //OSD wrong paramters of functions




CASE_F(TEST_OSD_PARAMS1,
	"CHDRV_OSD_AddToMemBitMap�����������") {

	//CHDRV_OSD_AddToMemBitMap
	//CH_RTOS_TaskDelay(10000);
}

CASE_F(TEST_OSD_PARAMS2,
	"CHDRV_OSD_AllocateMemory�����������") {
	//U32 rui_Size=0; 
	//void * rpv_Memory;
	//EXPECT_EQ(NULL,CHDRV_OSD_AllocateMemory(rui_Size ));
	//����ֵΪ void * �ͣ���CHDRV_OSD_RESULT_e ��
	//CHDRV_OSD_FreeMemory(rpv_Memory);
}
 

CASE_F(TEST_OSD_PARAMS3,
	"CHDRV_OSD_BitBlit�����������") {

	CHDRV_OSD_BitBlitParam_t 		rstru_BitBlitParam;

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( NULL, OSD_HANDLE, &rstru_BitBlitParam));//�����1��ָ��
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE_INVALID, OSD_HANDLE, &rstru_BitBlitParam));//�����1��Ч���
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, NULL, &rstru_BitBlitParam));//�����2��ָ�룬
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, OSD_HANDLE_INVALID, &rstru_BitBlitParam));//�����2��Ч��� 
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_BitBlit( OSD_HANDLE, OSD_HANDLE, NULL));//�����3��ָ��,(����ֵ��ΪCHDRV_OSD_INVALID_PARAM)
}
 




CASE_F(TEST_OSD_PARAMS6,
	"CHDRV_OSD_ClearArea�����������") {


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
	"CHDRV_OSD_ClearFullRegion�����������") {
	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearFullRegion(NULL,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ClearFullRegion(OSD_HANDLE_INVALID,0));
}



CASE_F(TEST_OSD_PARAMS8,
	"CHDRV_OSD_CopyRect�����������") {

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
	"CHDRV_OSD_CreateRegion�����������") {

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
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_CreateRegion(&rstru_Params, &rh_Region));	//�ڲ��ڲ���δ��ȫ
}



CASE_F(TEST_OSD_PARAMS12,
	"CHDRV_OSD_DecodeImage�����������") {
	
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
	"CHDRV_OSD_DeleteRegion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DeleteRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_DeleteRegion(OSD_HANDLE_INVALID));
}


#if OSD_USEEABLE
CASE_F(TEST_OSD_PARAMS21,
	"CHDRV_OSD_FreeMemory�����������") {
	
	//void * rpv_Memory=NULL;
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_FreeMemory(rpv_Memory));
	//CHDRV_OSD_FreeMemory ����ֵ����Ϊvoid
}
#endif 

CASE_F(TEST_OSD_PARAMS22,
	"CHDRV_OSD_GetAreaData�����������") {
		
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
	"CHDRV_OSD_GetDisplayRegionAttrib�����������") {

	CHDRV_OSD_RegionAttrib_t  rstru_RegionAttrib ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(NULL,&rstru_RegionAttrib,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(OSD_HANDLE_INVALID,&rstru_RegionAttrib,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetDisplayRegionAttrib(OSD_HANDLE,NULL,0));
	
}



CASE_F(TEST_OSD_PARAMS27,
	"CHDRV_OSD_GetImageInfo�����������") {

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
	"CHDRV_OSD_GetOsdInfo�����������") {

		
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
	"CHDRV_OSD_GetPixel�����������") {

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
	"CHDRV_OSD_GetRegionAlpha�����������") {
	
	S32   			  ri_AlphaValue ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(NULL,&ri_AlphaValue));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(OSD_HANDLE_INVALID,&ri_AlphaValue));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAlpha(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS31,
	"CHDRV_OSD_GetRegionAttrib�����������") {

	CHDRV_OSD_RegionAttrib_t   rstru_RegionAttrib ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(NULL,&rstru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(OSD_HANDLE_INVALID,&rstru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionAttrib(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS32,
	"CHDRV_OSD_GetRegionPalette�����������") {
	
	CHDRV_OSD_ColorValue   ui_ColorArray ;

	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(NULL,&ui_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(OSD_HANDLE_INVALID,&ui_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPalette(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS33,
	"CHDRV_OSD_GetRegionPaletteEntry�����������") {

	CHDRV_OSD_ColorValue   ui_Color ;

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(NULL,0,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(OSD_HANDLE_INVALID,0,&ui_Color));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetRegionPaletteEntry(OSD_HANDLE,0,NULL));
}

CASE_F(TEST_OSD_PARAMS34,
	"CHDRV_OSD_GetVersion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetVersion( NULL));
}

CASE_F(TEST_OSD_PARAMS35,
	"CHDRV_OSD_GetWindow�����������") {

	CH_Rect_t 			  stru_Rect ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(NULL,&stru_Rect));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(OSD_HANDLE_INVALID,&stru_Rect));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_GetWindow(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS36,
	"CHDRV_OSD_HideRegion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_HideRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_HideRegion(OSD_HANDLE_INVALID));
}
CASE_F(TEST_OSD_PARAMS40,
	"CHDRV_OSD_LinkRegion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(NULL,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE_INVALID,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE,NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_LinkRegion(OSD_HANDLE,OSD_HANDLE_INVALID));

}


CASE_F(TEST_OSD_PARAMS41,
	"CHDRV_OSD_PutAreaData�����������") {
	
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
	"CHDRV_OSD_SetPixel�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(NULL,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE_INVALID,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_OVER_WRANGE,OSD_PARAM_RIGHT,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,0,CHDRV_OSD_ALU_MAX));
	//EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetPixel(OSD_HANDLE,OSD_PARAM_RIGHT,OSD_PARAM_OVER_HRANGE,0,CHDRV_OSD_ALU_MAX));
}

CASE_F(TEST_OSD_PARAMS48,
	"CHDRV_OSD_SetRegionAlpha�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionAlpha(NULL,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionAlpha(OSD_HANDLE_INVALID,0));
}
 
CASE_F(TEST_OSD_PARAMS49,
	"CHDRV_OSD_SetRegionPalette�����������") {

	CHDRV_OSD_ColorValue   rp_ColorArray ;
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(NULL,&rp_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(OSD_HANDLE_INVALID,&rp_ColorArray));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPalette(OSD_HANDLE,NULL));
}

CASE_F(TEST_OSD_PARAMS50,
	"CHDRV_OSD_SetRegionPaletteEntry�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPaletteEntry(NULL,0,0));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_SetRegionPaletteEntry(OSD_HANDLE_INVALID,0,0));
}

CASE_F(TEST_OSD_PARAMS51,
	"CHDRV_OSD_SetWindow�����������") {

	
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
	"CHDRV_OSD_ShowRegion�����������") {

	
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ShowRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_ShowRegion(OSD_HANDLE_INVALID));
}
CASE_F(TEST_OSD_PARAMS56,
	"CHDRV_OSD_UnlinkRegion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(NULL,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE_INVALID,OSD_HANDLE));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE,NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UnlinkRegion(OSD_HANDLE,OSD_HANDLE_INVALID));
}

CASE_F(TEST_OSD_PARAMS57,
	"CHDRV_OSD_UpdateRegion�����������") {
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateRegion(OSD_HANDLE_INVALID));
}

CASE_F(TEST_OSD_PARAMS58,
	"CHDRV_OSD_UpdateRegionArea�����������") {
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
	"CHDRV_OSD_UpdateScrollRegion�����������") {

	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateScrollRegion(NULL));
	EXPECT_EQ(CHDRV_OSD_INVALID_PARAM,CHDRV_OSD_UpdateScrollRegion(OSD_HANDLE_INVALID));
}
#endif



CASE_F(TEST_OSD_CREATE_1,
	"�ԺϷ���������һ��osd1 draw region�����п�ߴ�env��ȡ����ɫ��ʽΪ4444��Ӧ���سɹ�") {
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
	"�ԺϷ���������һ��osd1 display region�����п�ߴ�env��ȡ����ɫ��ʽΪ4444��Ӧ���سɹ�") {
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB4444;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
}


CASE_F(TEST_OSD_GET_ATTRIB_1,
	"��Ըոմ�����2��region���ֱ����get attrib��Ӧ���سɹ�������ֵӦ��ȷ") {

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
		EXPECT_EQ(CH_LAYER_OSD1,rstru_RegionAttribGet[i_Loop].enm_Layer); //������û�л�ȡ��ֵ
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
	"������region link��һ��Ӧ����ok") {

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}

CASE_F(TEST_OSD_SHOWREGION_1,
	"����show region��Ӧ����ok") {

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}

CASE_F(TEST_OSD_CLEAREREGION_1,
	"����clear full draw regionΪ��ɫ��Ӧ����ok") {
	CHDRV_OSD_ColorValue 	 ui_Color = OSD_REGION_COLOR4444_WHITE;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,ui_Color));
}


CASE_F(TEST_OSD_CLEAREREGION_2,
	"����update region�������disp region��Ӧ����ok��ѯ�ʲ���Ա������Ӧ��ʾΪ��ȷ����ɫ") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_RED));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ӧ��ʾΪ��ɫ";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_GREEN));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ӧ��ʾΪ��ɫ";


	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_BLUE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ӧ��ʾΪ��ɫ";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ӧ��ʾΪ��ɫ";

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ӧ��ʾΪ��ɫ";
}


CASE_F(TEST_OSD_CLEAREREGION_3,
	"����clear full draw regionΪ��ɫ��Ӧ����ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR4444_RED));
	
}

CASE_F(TEST_OSD_UPDATE_1,
	"����update region area���Ͻǣ������disp region��Ӧ����ok��ѯ�ʲ���Ա������Ӧ�������Ͻ���ʾΪ��ɫ") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DisplayHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2));
	PROMPT("�����Ƿ�������Ͻ���ʾΪ��ɫ������Ϊ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "�ֲ����¹���δʵ��";
}

CASE_F(TEST_OSD_UPDATE_2,
	"����update regionӦ����ok�������draw region��ѯ�ʲ���Ա������Ӧ��ʾΪȫ����ɫ") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�������Ϊ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "ȫ�����¹���δʵ��";

}

CASE_F(TEST_OSD_CLEAREREGION_4,
	"����clear area�������Ͻ��ķ�֮һ������Ϊ͸��ɫ��Ӧ����ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,0));
}

CASE_F(TEST_OSD_UPDATE_3,
	"����update regionӦ����ok��ѯ�ʲ���Ա������Ӧ��ʾ���Ͻ�͸��ɫ��������ɫ") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�������Ͻ���ʾΪ͸��ɫ������Ϊ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_ClearArea͸��ɫ����δʵ��";

}


CASE_F(TEST_OSD_CLEAREREGION_5,
	"����clear area�������Ͻ��ķ�֮һ������Ϊ��ɫ��Ӧ����ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,OSD_REGION_COLOR4444_WHITE));
}

CASE_F(TEST_OSD_UPDATE_4,
	"����update regionӦ����ok��ѯ�ʲ���Ա������Ӧ��ʾ���Ͻǰ�ɫ��������ɫ") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�������Ͻ���ʾΪ��ɫ������Ϊ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_ClearArea����δʵ��";

}

CASE_F(TEST_OSD_GETDATA_1,
	"����get area data���Ͻ����ݣ�Ӧ����ok") {
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
	"����put area data��������put����Ļ�����Ͻǣ�Ӧ����ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	if(gpuc_Data != NULL)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_ENV_WIDTH/2,0,OSD_ENV_WIDTH/2,OSD_ENV_HEIGHT/2,gpuc_Data));
	}
}

CASE_F(TEST_OSD_UPDATE_5,
	"����update regionӦ����ok��ѯ�ʲ���Ա��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�����ϰ벿����ʾΪ��ɫ������Ϊ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_PutAreaData����δʵ��";

}

CASE_F(TEST_OSD_COPYRECT,
	"����copy rect����draw regioin���ϰ벿�ֿ������°벿�֣�Ӧ����ok") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	//EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CopyRect(grh_DrawHandleOsd1,grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT/2,0,OSD_ENV_HEIGHT/2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CopyRect(grh_DrawHandleOsd1,grh_DisplayHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT/2,0,OSD_ENV_HEIGHT/2));
}

CASE_F(TEST_OSD_UPDATE_6,
	"����update regionӦ����ok��ѯ�ʲ���Ա������Ӧȫ����ʾΪ��ɫ") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	
	PROMPT("�����Ƿ�������ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "CHDRV_OSD_CopyRect����δʵ��";


}

CASE_F(TEST_OSD_CLEARAREA_1,
	"����clear area����������ֱ�����������Խ�磬���Է��ز��ɹ�����Ӧ����������Ļ��") {
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,OSD_REGION_COLOR4444_WHITE));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,OSD_REGION_COLOR4444_WHITE));
}


CASE_F(TEST_OSD_GETDATA_2,
	"����get area data����������ֱ�����������Խ�磬���Է��ز��ɹ�����Ӧ����������Ļ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,gpuc_Data));
}

CASE_F(TEST_OSD_PUTDATA_2,
	"����put area data����������ֱ�����������Խ�磬���Է��ز��ɹ�����Ӧ����������Ļ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT,gpuc_Data));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1,gpuc_Data));
}

CASE_F(TEST_OSD_UPDATA_AREA_1,
	"����update region area����������ֱ�����������Խ�磬���Է��ز��ɹ�����Ӧ����������Ļ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_MINUS,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_WIDTH+1,OSD_PARAM_RIGHT));
	EXPECT_NE(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegionArea(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_ENV_HEIGHT+1));
}



CASE_F(TEST_OSD_SETPIXEL,
	"����set pixel��������һ�����飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
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
		&i_Color_Set,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_SetPixelδ��ȷʵ��";
}

CASE_F(TEST_OSD_GETPIXEL,
	"����get pixel������ȡ���ݣ�Ӧ��ȷ") {
	
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
			//CHDRV_OSD_GetPixel�������и���ɫֵ��ȡд��
			EXPECT_EQ(i_Color, OSD_REGION_COLOR4444_GREEN) << "CHDRV_OSD_GetPixelδ��ȷʵ��";
			if(i_Color != OSD_REGION_COLOR4444_GREEN)
			{
				b_fail = CH_TRUE;
				break;
			}
		}
	}
}

CASE_F(TEST_OSD_POSITION_0,
	"OSDλ�ò��ԣ�Ӧ��I֡�ı��һ��") {
	
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
		PROMPT("��ɫ��λ��Ϊx=%d,y=%d,w=%d,h=%d,�Ƿ�ͱ��һ��?��������Y����������N", rect[i].s_X, rect[i].s_Y, rect[i].s_Width,rect[i].s_Height);
		EXPECT_EQ(CH_TRUE, test_input_yes()) << "OSD rectλ��(x="<<rect[i].s_X << ",y=" << rect[i].s_Y << ",w=" <<  rect[i].s_Width << ",h=" <<  rect[i].s_Height << ")���߲�����";
	}
	
}



#if 0 	/*sqzou20170807*/
CASE_F(TEST_OSD_DRAWLINE,
	"��ε���draw line�����Բ�ͬ�Ŀ�Ȼ��Ƴ�һ�����飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
//б�����ÿ�Ȳ������ã�������߻���ʱ��ӿ��û��Ч������Ҫ����mi�����޹��������
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

	//ÿ���߿��10 ����ʼ����(200,200),�յ�����(400,350),����xֵ���䣬���»��Ʒ��飬�����γ�һ�������߶�150�Ĵ󷽿�
	for(i_LineWidth = 10;i_LineWidth <= 50 && ret == CHDRV_OSD_OK;i_LineWidth += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawLine(h_DrawRegion,i_StartX,i_StartY,i_StartX,i_StartY + i_LineWidth,i_LineWidth,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,150,(U8 *)puc_Data));
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT*150;i_Loop ++)
	{
		EXPECT_EQ(OSD_REGION_COLOR4444_BLUE,*(puc_Data+i_Loop )) << "CHDRV_OSD_DrawLineδ��ȷʵ��";
		if(OSD_REGION_COLOR4444_BLUE != *(puc_Data+i_Loop ))
		{
			break;
		}
	}

	ret = CHDRV_OSD_OK;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR_CLEAR));
	//ÿ���߿��10 ����ʼ����(200,200),�յ�����(350,400),����yֵ���䣬���һ��Ʒ��飬�����γ�һ���������Ϊ150�Ĵ󷽿�
	for(i_LineWidth = 10;i_LineWidth <= 50 && ret == CHDRV_OSD_OK;i_LineWidth += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawLine(h_DrawRegion,i_StartX,i_StartY,i_StartX + i_LineWidth,i_StartY,i_LineWidth,i_LineColor,enm_AluMode));
		i_StartX += i_LineWidth;
	}

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,150,OSD_PARAM_RIGHT,(U8 *)puc_Data));
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT*150 * 2;i_Loop ++)
	{
		EXPECT_EQ(OSD_REGION_COLOR4444_BLUE,*(puc_Data+i_Loop )) << "CHDRV_OSD_DrawLineδ��ȷʵ��";
		if(OSD_REGION_COLOR4444_BLUE != *(puc_Data+i_Loop ))
		{
			break;
		}
	}
	
	CHDRV_OSD_FreeMemory(puc_Data);
}

CASE_F(TEST_OSD_DRAWHLINE,
	"��ε���draw H line�������Ƴ�һ�����飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
	//б�����ÿ�Ȳ������ã�������߻���ʱ��ӿ��û��Ч������Ҫ����mi�����޹��������
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

	//ÿ���߿��10 ����ʼ����(200,200),�յ�����(400,350),����xֵ���䣬���»��Ʒ��飬�����γ�һ�������߶�150�Ĵ󷽿�
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT && ret == CHDRV_OSD_OK;i_Loop++)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawHLine(h_DrawRegion,0, 0, i_StartX,i_StartY, OSD_PARAM_RIGHT,CHDRV_OSD_COLOR_TYPE_ARGB4444,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}
	

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,(U8 *)puc_Data));

	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,
		OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&i_LineColor,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_DrawHLineδ��ȷʵ��";
	
	CHDRV_OSD_FreeMemory(puc_Data);
}

CASE_F(TEST_OSD_DRAWVLINE,
	"��ε���draw V line�������Ƴ�һ�����飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
	//б�����ÿ�Ȳ������ã�������߻���ʱ��ӿ��û��Ч������Ҫ����mi�����޹��������
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

	//ÿ���߿��10 ����ʼ����(200,200),�յ�����(400,350),����xֵ���䣬���»��Ʒ��飬�����γ�һ�������߶�150�Ĵ󷽿�
	for(i_Loop = 0;i_Loop < OSD_PARAM_RIGHT && ret == CHDRV_OSD_OK;i_Loop++)
	{
		EXPECT_EQ(CHDRV_OSD_OK, ret = CHDRV_OSD_DrawVLine(h_DrawRegion,0, 0, i_StartX,i_StartY, OSD_PARAM_RIGHT,CHDRV_OSD_COLOR_TYPE_ARGB4444,i_LineColor,enm_AluMode));
		i_StartY += i_LineWidth;
	}
	

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(h_DrawRegion));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetAreaData(h_DrawRegion,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,(U8 *)puc_Data));

	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,
		OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&i_LineColor,CHDRV_OSD_COLOR_TYPE_ARGB4444)) << "CHDRV_OSD_DrawVLineδ��ȷʵ��";
	
	CHDRV_OSD_FreeMemory(puc_Data);
}
#endif

CASE_F(TEST_OSD_ALLOCMEM_1,
	"����osd memory����һ��480X320Xcolor bytes��С���ڴ�") {	
	EXPECT_NE((void*)NULL, gpuc_Mem = CHDRV_OSD_AllocateMemory(OSD_MEM_WIDTH*OSD_MEM_HEIGHT*2))<<"error";
	if(gpuc_Mem != NULL)
	{
		memset(gpuc_Mem, 0, sizeof(U8)*OSD_MEM_WIDTH*OSD_MEM_HEIGHT*2);
	}
	
}
#if 0 	/*sqzou20170925*/

CASE_F(TEST_OSD_DRAWRECT_1,
	"����draw rect������draw memory��ʽ���Ʒ��飬Ȼ��������ݱȽϣ�Ӧ����Ԥ��") {
	CH_BOOL 				b_IsUseAlpha = OSD_BOOL_FLASE;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect = {0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue   ui_Color = OSD_REGION_COLOR4444_GREEN, ui_ColorTemp;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB4444;	///< һ������ռ16bit��ARGBÿ����ռ4bit������ַ�ɸ���������	
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
		EXPECT_EQ(pus_Temp[i], ui_ColorTemp) << "CHDRV_OSD_DrawRectδ��ȷʵ��";
		if(pus_Temp[i] != ui_ColorTemp)
		{
			break;
		}
	}

}


CASE_F(TEST_OSD_BUFBITBLIT_1,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ���COPY��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK, Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_2,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ���Ŵ�draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data���Ŵ������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK, Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_4,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����С��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data����С������Ӧ����Ԥ��") {
	
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

	ui_Color = 0;
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_6,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����alpha blend��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data��alpha blend������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_8,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����alpha blend��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data��alpha blend������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_GREEN;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}
#endif

CASE_F(TEST_OSD_BUFBITBLIT_10,
	"�ٴ���һ��480X320����ͬ��ɫģʽ��draw region2��Ӧ�ɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_MEM_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_MEM_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB4444;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;	
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleMem)) ;
	
}

CASE_F(TEST_OSD_BUFBITBLIT_11,
	"����draw rect������draw region handle��ʽ���Ʒ��鵽region2�ϣ�Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB4444;	///< һ������ռ16bit��ARGBÿ����ռ4bit������ַ�ɸ���������

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleMem,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleMem,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,enm_ColorType));
	
}

CASE_F(TEST_OSD_BUFBITBLIT_12,
	"��clear draw region��Ȼ�����bitblit���ոջ��Ƶķ����region2 COPY��draw region�ϣ�Ӧ�ɹ�") {
	
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
	"����get area data������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_14,
	"��clear draw region��Ȼ�����bitblit���ոջ��Ƶķ���Ŵ�draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data���Ŵ������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_16,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����С��draw region�ϣ�Ӧ�ɹ�") {

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
	"����get area data����С������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

	ui_Color = 0;
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,0,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,OSD_PARAM_RIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));

}

CASE_F(TEST_OSD_BUFBITBLIT_18,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����alpha blend��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data��alpha blend������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_BUFBITBLIT_20,
	"��clear draw region��Ȼ�����buf bitblit���ոջ��Ƶķ�����alpha blend��draw region�ϣ�Ӧ�ɹ�") {
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
	"����get area data��alpha blend������Ӧ����Ԥ��") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR4444_RED;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_ENV_WIDTH,OSD_ENV_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB4444));
}

CASE_F(TEST_OSD_DELETEREGIOIN_1,
	"ɾ������������region��Ӧ�ɹ�") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB4444, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR4444_RED);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd1));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleMem));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd1));
	grh_DrawHandleOsd1     = NULL;
	grh_DrawHandleMem 	   = NULL;
	grh_DisplayHandleOsd1  = NULL;
}

CASE_F(TEST_OSD_CREATEREGION_1,
	"�ԺϷ���������һ��osd1 draw region�����п��Ϊ720X576����ɫ��ʽΪ1555��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB1555;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd1)) ;

}


CASE_F(TEST_OSD_CREATEREGION_2,
	"�ԺϷ���������һ��osd1 display region�����п��Ϊ720X576����ɫ��ʽΪ1555��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX 			= 0;
	rstru_Params.i_PosY 			= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType		= CHDRV_OSD_COLOR_TYPE_ARGB1555;
	rstru_Params.enm_RegionType 	= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType	= CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer			= CH_LAYER_OSD1;
		
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
		
}
CASE_F(TEST_OSD_GETATTRIB_1,
	"��Ըոմ�����2��region���ֱ����get attrib��Ӧ���سɹ�������ֵӦ��ȷ") {
	
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
	"������region link��һ��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}
CASE_F(TEST_OSD_SHOWREGION_2,
	"����show region��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}
CASE_F(TEST_OSD_CLEARREGION_1,
	"����clear full draw regionΪ��ɫ��Ӧ����ok") {
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR1555_WHITE;

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR1555_WHITE));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555));

}

CASE_F(TEST_OSD_DRAWRECT_2,
	"����draw rect������draw region handle��ʽ���Ʒ��飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = OSD_REGION_COLOR1555_RED;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB1555;	///
	CHDRV_OSD_ColorValue 		  ui_32Color = OSD_REGION_COLOR1555_RED;
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_32Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555)) << "CHDRV_OSD_DrawRectδ��ȷʵ��";
}

CASE_F(TEST_OSD_DRAWRECT_3,
	"����update regionӦ����ok��ѯ�ʲ���Ա��������ʾӦ����Ԥ��") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	PROMPT("�����Ƿ�Ϊ��ɫ�������и�һ��480X320�ĺ�ɫ��?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRectδʵ��";
	
}

CASE_F(TEST_OSD_DRAWRECT_2_1,
	"����draw rect������draw region handle��ʽ����͸�����飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
	CH_BOOL 				b_IsUseAlpha = 0;
	U8 					        uc_Alpha = 0;
	CH_Rect_t   			   stru_Rect ={0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT};
	CHDRV_OSD_ColorValue 		  ui_Color = 0;
	CHDRV_OSD_COLOR_TYPE_e enm_ColorType = CHDRV_OSD_COLOR_TYPE_ARGB1555;	///
	CHDRV_OSD_ColorValue 		  ui_32Color = 0;
	//
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DrawRect(grh_DrawHandleOsd1,0,0,&stru_Rect,enm_ColorType,b_IsUseAlpha,uc_Alpha,ui_32Color));
	EXPECT_EQ(CHDRV_OSD_OK,Osd_Compare_SingleData(grh_DrawHandleOsd1,0,0,OSD_MEM_WIDTH,OSD_MEM_HEIGHT,&ui_Color,CHDRV_OSD_COLOR_TYPE_ARGB1555)) << "CHDRV_OSD_DrawRectδ��ȷʵ��";
}

CASE_F(TEST_OSD_DRAWRECT_3_1,
	"����update regionӦ����ok��ѯ�ʲ���Ա��������ʾӦ����Ԥ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	PROMPT("�����Ƿ�Ϊ��ɫ�������и�һ��480X320��͸����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect͸��ɫδʵ��";
	
}

CASE_F(TEST_OSD_DELETEREGIOIN_2,
	"ɾ������������region��Ӧ�ɹ�") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB1555, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR1555_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd1));
	grh_DrawHandleOsd1 = NULL;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd1));
	grh_DisplayHandleOsd1 = NULL;
	
}
CASE_F(TEST_OSD_CREATEREGION_3,
	"�ԺϷ���������һ��osd1 draw region�����п��Ϊ720X576����ɫ��ʽΪ8888��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;	
	delete_prompt_region();
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd1)) ;
}
CASE_F(TEST_OSD_CREATEREGION_4,
	"�ԺϷ���������һ��osd1 display region�����п��Ϊ720X576����ɫ��ʽΪ8888��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DISPLAY;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD1;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DisplayHandleOsd1)) ;
}
CASE_F(TEST_OSD_GETATTRIB_2,
	"��Ըոմ�����2��region���ֱ����get attrib��Ӧ���سɹ�������ֵӦ��ȷ") {
	
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
	"������region link��һ��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd1,grh_DisplayHandleOsd1));
	
}
CASE_F(TEST_OSD_SHOWREGION_3,
	"����show region��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd1));

}
CASE_F(TEST_OSD_CLEARREGION_2,
	"����clear full draw regionΪ��ɫ��Ӧ����ok") {
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
		
}

CASE_F(TEST_OSD_DRAWRECT_4,
	"����draw rect������draw region handle��ʽ���Ʒ��飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
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
	"����update regionӦ����ok��ѯ�ʲ���Ա��������ʾӦ����Ԥ��") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�Ϊ��ɫ�������и�һ��480X320����ɫ��?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRectδʵ��";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	

}
CASE_F(TEST_OSD_DRAWRECT_4_1,
	"����draw rect������draw region handle��ʽ���Ʒ��飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
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
	"����update regionӦ����ok��ѯ�ʲ���Ա��������ʾӦ����Ԥ��") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	PROMPT("�����Ƿ�Ϊ��ɫ�������и�һ��480X320��͸����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_DrawRect͸��ɫδʵ��";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1,OSD_REGION_COLOR8888_WHITE));
	

}

CASE_F(TEST_OSD_CREATEREGION_5,
	"�ԺϷ���������һ��osd2 draw region�����п��Ϊ720X576����ɫ��ʽΪ8888��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
	rstru_Params.h_MemPartion		= NULL;
	rstru_Params.enm_ColorType 		= CHDRV_OSD_COLOR_TYPE_ARGB8888;
	rstru_Params.enm_RegionType		= CHDRV_OSD_REGION_DRAW;
	rstru_Params.enm_ClutColorType  = CHDRV_OSD_COLOR_TYPE_MAX;
	rstru_Params.enm_Layer 			= CH_LAYER_OSD2;
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_CreateRegion(&rstru_Params,&grh_DrawHandleOsd2)) ;
}
CASE_F(TEST_OSD_CREATEREGION_6,
	"�ԺϷ���������һ��osd2 display region�����п��Ϊ720X576����ɫ��ʽΪ8888��Ӧ���سɹ�") {
	
	CHDRV_OSD_RegionParam_t rstru_Params;
	CH_Rect_t stru_rect;
	
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
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
	"��Ըոմ�����2��region���ֱ����get attrib��Ӧ���سɹ�������ֵӦ��ȷ") {
	
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
	"������region link��һ��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_LinkRegion(grh_DrawHandleOsd2,grh_DisplayHandleOsd2));
	
}
CASE_F(TEST_OSD_SHOWREGION_4,
	"����show region��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DrawHandleOsd2));

}
CASE_F(TEST_OSD_CLEARAREA_2,
	"����clear full draw regionΪ��ɫ��Ӧ����ok") {
	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd2,0));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearArea(grh_DrawHandleOsd2, 0, 0, OSD_ENV_WIDTH, OSD_ENV_HEIGHT / 2, OSD_REGION_COLOR8888_BLUE));
	
}

CASE_F(TEST_OSD_DRAWRECT_6,
	"����draw rect������draw region handle��ʽ���Ʒ��飬Ȼ�����get area data��ȡ���ݽ��бȽϣ�Ӧ�ɹ�") {
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
	"����update regionӦ����ok��ѯ�ʲ���Ա��������ʾӦ����Ԥ�ڣ���osd1�ĵ����ڵ�Ч������") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd2));
		
	PROMPT("�����Ƿ�Ϊ�°벿��ɫ���ϰ벿Ϊ��ɫ��������һ��200X200����ɫ����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "osd2δ��ȷʵ��";
}

CASE_F(TEST_OSD_SETALPHA_1,
	"����set osd alpha����osd1��͸���ȷֱ�Ϊ100,80,40,0��osd2��͸����Ӧ����Ӱ�죬ѯ�ʲ���ԱЧ���Ƿ�OK") {
	int a;
	PROMPT("����OSD1��͸���ȣ�ȫ���İ�ɫ��ɫ����ΪOSD1����ע��۲죬�����������..");
	test_input_none();
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	for(a = 0; a <= 100; a += 10)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1,a));	
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
		PROMPT("OSD1͸����Ϊ%d\n", a);
		CH_RTOS_TaskDelay(200);
		clear_promt();
	}
	PROMPT("����仯�Ƿ�����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "osd1͸��������δ��ȷʵ��";
}


CASE_F(TEST_OSD_HIDE_1,
	"����hide region 2��Ӧֻ�ɼ�osd1��ѯ�ʲ���ԱЧ���Ƿ�һ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_HideRegion(grh_DisplayHandleOsd2));
	PROMPT("��Ļ���Ƿ�ֻ����ɫȫ������?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_HideRegionδ��ȷʵ��";
}

CASE_F(TEST_OSD_HIDE_2,
	"����hide region 1��region 1��2Ӧ�����ɼ���ѯ�ʲ���ԱЧ���Ƿ�һ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_HideRegion(grh_DisplayHandleOsd1));
	PROMPT("��Ļ���Ƿ���OSD��ʾ��ֻ�������I֡����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_HideRegionδ��ȷʵ��";
}

CASE_F(TEST_OSD_SHOWREGION_5,
	"����show region 2��Ӧֻ�ɼ�osd2��ѯ�ʲ���ԱЧ���Ƿ�һ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd2, &grh_DrawHandleOsd2, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DisplayHandleOsd2));
	PROMPT("��Ļ���Ƿ��°벿��ΪI֡�ķ����棬�ϰ벿��Ϊ��ɫ��������ɫС����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_ShowRegionδ��ȷʵ��";
}

CASE_F(TEST_OSD_SHOWREGION_6,
	"����show region 1��osd 1��2���ɼ���ѯ�ʲ���ԱЧ���Ƿ�һ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ShowRegion(grh_DisplayHandleOsd1));
	PROMPT("��Ļ���Ƿ��°벿��Ϊ��ɫ���棬�ϰ벿��Ϊ��ɫ��������ɫС����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_OSD_ShowRegionδ��ȷʵ��";

}

CASE_F(TEST_OSD_POSITION_1,
	"osd1��osd2�ϵķ���λ�á���С���뱳���ϵ�I֡����Ƿ�һ�£�ѯ�ʲ���Ա�������") {

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1, 60));	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd2, 60));	
	PROMPT("��Ļ���Ƿ��°벿��Ϊ��ɫ���棬�ϰ벿��Ϊ��ɫ������200X200��ɫС���飬λ���Ƿ����߷���?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "��ʾλ����I֡������";
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd1, 100));	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(grh_DisplayHandleOsd2, 100));

	
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UnlinkRegion(grh_DrawHandleOsd2, grh_DisplayHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DrawHandleOsd2));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_DisplayHandleOsd2));
	grh_DrawHandleOsd2 = NULL;
	grh_DisplayHandleOsd2 = NULL;


	
}

CASE_F(TEST_OSD_CREATE_3,
	"һ������OSD1�ϴ���2��OSD_REGION_WH��С����ɫ��ʽΪ8888��draw region��Ӧ���سɹ���Ȼ���ͷ�") {

	CHDRV_OSD_RegionHandle rh_DrawRegionOSD1 = NULL;
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD2 = NULL;
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
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
	"һ������OSD2�ϴ���2��OSD_REGION_WH��С����ɫ��ʽΪ8888��draw region��Ӧ���سɹ���Ȼ���ͷ�") {
	
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD1 = NULL;
	CHDRV_OSD_RegionHandle rh_DrawRegionOSD2 = NULL;
	CHDRV_OSD_RegionParam_t rstru_Params;
	rstru_Params.i_PosX				= 0;
	rstru_Params.i_PosY				= 0;
	rstru_Params.i_Width			= OSD_ENV_WIDTH;//(env��ȡ�� 
	rstru_Params.i_Height			= OSD_ENV_HEIGHT;//��env��ȡ��
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
	"osd malloc 4 Mbytes�ڴ棬Ӧ���سɹ�") {
	void *  rpv_Memory = NULL;

	EXPECT_NE((void*)NULL, rpv_Memory = CHDRV_OSD_AllocateMemory(0x400000)) << "����4M OSD MEM���ɹ�";
	if(rpv_Memory != NULL)
	{
		CHDRV_OSD_FreeMemory(rpv_Memory);
	}

}

CASE_F(TEST_OSD_PICTURE1,
	"����get image info������Ϊ·����ʽ����ȡGIFͼƬ����Ϣ��Ӧ�ɹ�") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_GIF_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_GIF_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
	EXPECT_EQ(fw_getenv_i("OSD_GIF_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";

}

CASE_F(TEST_OSD_PICTURE2,
	"����get image info������Ϊ·����ʽ����ȡJPEGͼƬ����Ϣ��Ӧ�ɹ�") {

	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_JPEG_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_JPG_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
	EXPECT_EQ(fw_getenv_i("OSD_JPG_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";

	
}

CASE_F(TEST_OSD_PICTURE3,
	"����get image info������Ϊ·����ʽ����ȡPNGͼƬ����Ϣ��Ӧ�ɹ�") {
	

	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_PNG_PATH;
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_PNG_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
	EXPECT_EQ(fw_getenv_i("OSD_PNG_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";

}

CASE_F(TEST_OSD_PICTURE4,
	"����get image info������Ϊ·����ʽ����ȡBMPͼƬ����Ϣ��Ӧ�ɹ�") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	memset(&stru_ImgData,0,sizeof(CHDRV_OSD_ImageData_t));
	stru_ImgData.puc_FileName = (const U8*)OSD_BMP_PATH;
	
	CHDRV_OSD_ImageInfo_t  stru_ImgInfo;
	memset(&stru_ImgInfo,0,sizeof(CHDRV_OSD_ImageInfo_t));
	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	EXPECT_EQ(fw_getenv_i("OSD_BMP_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
	EXPECT_EQ(fw_getenv_i("OSD_BMP_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";
}

CASE_F(TEST_OSD_PICTURE5,
	"����get image info������Ϊ�ڴ�ָ�뷽ʽ����ȡGIFͼƬ����Ϣ��Ӧ�ɹ�") {

	int len;
	CHDRV_OSD_ImageData_t  stru_ImgData ;

	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_GIF_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "�ڴ治����ļ�������,filename=" << OSD_GIF_PATH;
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
		EXPECT_EQ(fw_getenv_i("OSD_GIF_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
		EXPECT_EQ(fw_getenv_i("OSD_GIF_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}
	
}

CASE_F(TEST_OSD_PICTURE6,
	"����get image info������Ϊ�ڴ�ָ�뷽ʽ����ȡJPEGͼƬ����Ϣ��Ӧ�ɹ�") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_JPEG_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "�ڴ治����ļ�������,filename=" << OSD_JPEG_PATH;
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
		EXPECT_EQ(fw_getenv_i("OSD_JPG_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
		EXPECT_EQ(fw_getenv_i("OSD_JPG_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}

CASE_F(TEST_OSD_PICTURE7,
	"����get image info������Ϊ�ڴ�ָ�뷽ʽ����ȡPNGͼƬ����Ϣ��Ӧ�ɹ�") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_PNG_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "�ڴ治����ļ�������,filename=" << OSD_PNG_PATH;
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
		EXPECT_EQ(fw_getenv_i("OSD_PNG_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
		EXPECT_EQ(fw_getenv_i("OSD_PNG_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}

CASE_F(TEST_OSD_PICTURE8,
	"����get image info������Ϊ�ڴ�ָ�뷽ʽ����ȡBMPͼƬ����Ϣ��Ӧ�ɹ�") {
	
	CHDRV_OSD_ImageData_t  stru_ImgData ;
	int len;
	memset(&stru_ImgData,0,sizeof(stru_ImgData));
	EXPECT_LT(0, len = fsys_read_file(OSD_BMP_PATH, (char*&)stru_ImgData.puc_ImgAddr)) << "�ڴ治����ļ�������,filename=" << OSD_BMP_PATH;
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
		EXPECT_EQ(fw_getenv_i("OSD_BMP_WIDTH"),stru_ImgInfo.ui_Width) << "��ȡͼƬ��Ϣ��ȴ���";
		EXPECT_EQ(fw_getenv_i("OSD_BMP_HEIGHT"),stru_ImgInfo.ui_Height) << "��ȡͼƬ��Ϣ�߶ȴ���";
	}
	if(stru_ImgData.puc_ImgAddr)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, stru_ImgData.puc_ImgAddr);
	}

}









CASE_F(TEST_OSD_PICTURE9,
	"����decoder image GIF������Ϊ·����ʽ����������ԭͼһ�£�Ӧ�ɹ�;�Ƚ��ڴ�������Ԥ��һ��") {


	char data_path[512];

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_GIF_PATH, OSD_GIF_WIDTH, OSD_GIF_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_GIF_PATH, data_path, OSD_GIF_WIDTH, OSD_GIF_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_GIF_PATH;

}

CASE_F(TEST_OSD_PICTURE10,
	"����decoder image JPEG������Ϊ·����ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {

	char data_path[512];

	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_JPEG_PATH, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_JPEG_PATH, data_path, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_JPEG_PATH;
	
}

CASE_F(TEST_OSD_PICTURE11,
	"����decoder image PNG������Ϊ·����ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {


	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_PNG_PATH, OSD_PNG_WIDTH, OSD_PNG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_PNG_PATH, data_path, OSD_PNG_WIDTH, OSD_PNG_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_PNG_PATH;
}

CASE_F(TEST_OSD_PICTURE12,
	"����decoder image BMP������Ϊ·����ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {


	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_BMP_PATH, OSD_BMP_WIDTH, OSD_BMP_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(OSD_BMP_PATH, data_path, OSD_BMP_WIDTH, OSD_BMP_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_BMP_PATH;
}

CASE_F(TEST_OSD_PICTURE13,
	"����decoder image GIF������Ϊ�ڴ�ָ�뷽ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_GIF_PATH, OSD_GIF_WIDTH, OSD_GIF_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_GIF_PATH, data_path, OSD_GIF_WIDTH, OSD_GIF_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_GIF_PATH;

}

CASE_F(TEST_OSD_PICTURE14,
	"����decoder image JPEG������Ϊ�ڴ�ָ�뷽ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_JPEG_PATH, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_JPEG_PATH, data_path, OSD_JPEG_WIDTH, OSD_JPEG_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_JPEG_PATH;

}

CASE_F(TEST_OSD_PICTURE15,
	"����decoder image PNG������Ϊ�ڴ�ָ�뷽ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	

	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_PNG_PATH, OSD_PNG_WIDTH, OSD_PNG_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_PNG_PATH, data_path, OSD_PNG_WIDTH, OSD_PNG_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_PNG_PATH;

}

CASE_F(TEST_OSD_PICTURE16,
	"����decoder image BMP������Ϊ�ڴ�ָ�뷽ʽ����������ԭͼһ�£�Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", OSD_BMP_PATH, OSD_BMP_WIDTH, OSD_BMP_HEIGHT);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_data(OSD_BMP_PATH, data_path, OSD_BMP_WIDTH, OSD_BMP_HEIGHT)) << "ͼƬ��������ȷ, filename=" << OSD_BMP_PATH;
}

CASE_F(TEST_OSD_PICTURE17,
	"����decoder image GIF������Ϊ·����ʽ����������ԭͼ������֮һ��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	char data_path[512];

	CHDRV_OSD_ImageData_t stru_ImgData;
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_GIF_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_GIF_COMMON_FILE");
}

CASE_F(TEST_OSD_PICTURE18,
	"����decoder image JPEG������Ϊ·����ʽ����������ԭͼ������֮һ��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_JPG_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_JPG_COMMON_FILE");


}

CASE_F(TEST_OSD_PICTURE19,
	"����decoder image PNG������Ϊ·����ʽ����������ԭͼ������֮һ��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_PNG_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_PNG_COMMON_FILE");


}
CASE_F(TEST_OSD_PICTURE20,
	"����decoder image BMP������Ϊ·����ʽ����������ԭͼ������֮һ��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_BMP_COMMON_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_COMMON_FILE"), stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_COMMON_FILE"), data_path, stru_ImgInfo.ui_Width / 3, stru_ImgInfo.ui_Height / 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_BMP_COMMON_FILE");


}
CASE_F(TEST_OSD_PICTURE21,
	"����decoder image GIF������Ϊ·����ʽ����������ԭͼ��������Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_GIF_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_GIF_LITTLE_FILE");
}
CASE_F(TEST_OSD_PICTURE22,
	"����decoder image JPEG������Ϊ·����ʽ����������ԭͼ��������Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_JPG_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_JPG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE23,
	"����decoder image PNG������Ϊ·����ʽ����������ԭͼ��������Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_PNG_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_PNG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE24,
	"����decoder image BMP������Ϊ·����ʽ����������ԭͼ��������Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	CHDRV_OSD_ImageData_t stru_ImgData;
	char data_path[512];
	CHDRV_OSD_ImageInfo_t stru_ImgInfo;
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	memset(&stru_ImgData, 0, sizeof(stru_ImgData));
	memset(&stru_ImgInfo, 0, sizeof(stru_ImgInfo));
	stru_ImgData.puc_FileName = (const U8*)fw_getenv("OSD_BMP_LITTLE_FILE");
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetImageInfo(&stru_ImgData, &stru_ImgInfo));
	
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_LITTLE_FILE"), stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_LITTLE_FILE"), data_path, stru_ImgInfo.ui_Width * 3, stru_ImgInfo.ui_Height * 3)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE25,
	"����decoder image GIF������Ϊ·����ʽ��������Ϊ0��Ӧ���ԭͼ��С��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_GIF_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_GIF_LITTLE_FILE"), data_path, 0, 0)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_GIF_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE26,
	"����decoder image JPEG������Ϊ·����ʽ��������Ϊ0��Ӧ���ԭͼ��С��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_JPG_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_JPG_LITTLE_FILE"), data_path, 0, 0)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_JPG_LITTLE_FILE");

}
CASE_F(TEST_OSD_PICTURE27,
	"����decoder image PNG������Ϊ·����ʽ��������Ϊ0��Ӧ���ԭͼ��С��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	char data_path[512];
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_PNG_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_PNG_LITTLE_FILE"), data_path, 0, 0)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_PNG_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE28,
	"����decoder image BMP������Ϊ·����ʽ��������Ϊ0��Ӧ���ԭͼ��С��Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	char data_path[512];
	sprintf(data_path, "%s.%dX%d.argb", fw_getenv("OSD_BMP_LITTLE_FILE"), 0, 0);
	EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv("OSD_BMP_LITTLE_FILE"), data_path, 0, 0)) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
}

CASE_F(TEST_OSD_PICTURE29,
	"ѭ������decoder image jpeg������Ϊ·����ʽ��������Ϊ������Ӧ�ɹ����Ƚ��ڴ�������Ԥ��һ��") {
	
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
		EXPECT_EQ(CHDRV_OSD_OK, osd_decode_pic(fw_getenv(file_name), data_path, fw_getenv_i("OSD_PIC_REGION_WIDTH"),fw_getenv_i("OSD_PIC_REGION_HEIGHT"))) << "ͼƬ��������ȷ, filename=" << fw_getenv("OSD_BMP_LITTLE_FILE");
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
		LOG("���ȴ���OSD!��ʹ��osd.pic����\n");
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
		LOG("���ȴ���OSD!��ʹ��osd.pic����\n");
		return -1;
	}
	if(argv[3].get_i() <= 0 ||  argv[4].get_i() <= 0)
	{
		LOG("��������! x=%d,y=%d,w=%d,h=%d\n", argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), argv[4].get_i(), argv[5].get_i());
		return -1;
	}
	memset(&stru_RegionAttrib, 0, sizeof(stru_RegionAttrib));
	EXPECT_EQ(CHDRV_OSD_GetRegionAttrib(gh_CacheRegion, &stru_RegionAttrib), CHDRV_OSD_OK);
	pic = (U8*)CHDRV_OSD_AllocateMemory(argv[3].get_i() * argv[4].get_i() * stru_RegionAttrib.i_bitPerPixel / 8);
	if(!pic)
	{
		LOG("�ڴ����ʧ��! x=%d,y=%d,w=%d,h=%d,i_bitPerPixel=%d\n", 
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


REGISTER_CMD(osd_init, "��ʼ��osdģ��", "",  "");
REGISTER_CMD(osd_term, "����osdģ��", "",  "");
REGISTER_CMD(osd_ver, "��ȡosd�汾��", "",  "");
REGISTER_CMD(osd_pic, "���벢��ʾһ��ͼƬ", "layer | color_type | width | height | pic_file",  "3 0 1280 720 ./1280_720.bmp");
REGISTER_CMD(osd_delregion, "ɾ����ʾͼƬʱ������region", "",  "");
REGISTER_CMD(osd_trans, "����OSD͸����", "alpha(0-100)",  "100");
REGISTER_CMD(osd_rect, "��OSD�ϻ��Ʒ���", "x|y|w|h|color",  "0 0 720 576 0");
REGISTER_CMD(osd_getrect, "����OSDĳһ�����ڵ����ݲ���ӡ", "x|y|w|h",  "0 0 720 2");
REGISTER_CMD(osd_info, "���osd�������Ϣ����ӡ������", "",  "");



