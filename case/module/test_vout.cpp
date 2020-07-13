/**
@file test_vout.cpp
@def group vout���Լ�

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
#define CASE_NAME TestVout


#include "test_menu.h"
#include "test_modules.h"



#define VOUT_PARAM_MINUS       		(-1)   	//����һ�����Ƿ�����
#define VOUT_PARAM_RIGHT			(200)	//
#define VOUT_PARAM_ZERO				(0)	//
#define VOUT_PARAM_OVER_HRANGE		(730)
#define VOUT_PARAM_OVER_WRANGE		(1290)

#define VOUT_PARAM_VALUE_DEFAULT	(50)//���öԱȶȣ���ȣ����ͶȵȲ�����Χ 0~100��ȡ50��Ĭ����ȷ���� 
#define VOUT_PARAM_VALUE_INVALID	(101)//���öԱȶȣ���ȣ����ͶȵȲ�����Χ 0~100��ȡ101��Ĭ����ȷ����    


#define VOUT_PATH_HD				(1)
#define VOUT_PATH_SD				(0)



#define VOUT_HANDLE_INVALID			((void*)0x87650002)	// 
#define VOUT_HANDLE					((void*)0x87650000)
#define VOUT_HANDLE_OSD_INVALID		((void*)0x87650129)

#define VOUT_BGCOLOR_WHITE			(0xFFFFFF)   //rgb  888����ɫ    
#define VOUT_BGCOLOR_RED			(0xFF0000)   //rgb  888����ɫ 
#define VOUT_BGCOLOR_GREEN			(0x00FF00)   //rgb  888����ɫ
#define VOUT_BGCOLOR_BLUE			(0x0000FF)   //rgb  888����ɫ


#define VOUT_FMT_NUM				(CHDRV_VOUT_FMT_MAX)//�ɹ�ѡ��ĸ�ʽ��Ŀ
#define VOUT_FMT_LENGTH				(50)//��ʽ���Ƴ���
#define VOUT_RESOLUTION_SURPORT		(1)//֧��ĳ�ֱ���



#define VOUT_PIC_PATH1				(fw_getenv("OSD_BASIC_HD"))
#define VOUT_PIC_PATH2				(fw_getenv("OSD_BASIC_SD"))
typedef struct OSD_SHOW_PARAM
{
	CH_LAYER_e 						 enm_Layer ;
	CH_Rect_t 					     stru_Rect ;
	const U8 *					  puc_FileName ;
	CHDRV_OSD_RegionHandle *   h_BmpDrawHandle ;
	CHDRV_OSD_RegionHandle *h_BmpDisplayHandle ;
}VOUT_Osd_Show;


static CHDRV_OSD_RegionHandle	     grh_BmpDrawHandle1 = NULL ;	//������ʾ��OSD1��ͼƬ��draw region handle
static CHDRV_OSD_RegionHandle 	  grh_BmpDisplayHandle1 = NULL ;	//������ʾ��OSD1��ͼƬ��display region handle

static CHDRV_OSD_RegionHandle	     grh_BmpDrawHandle2 = NULL ;	//������ʾ��OSD2��ͼƬ��draw region handle
static CHDRV_OSD_RegionHandle 	  grh_BmpDisplayHandle2 = NULL ;	//������ʾ��OSD2��ͼƬ��display region handle

static CHDRV_VOUT_PathHandle  	  grh_VideoOutputHandle  = NULL;
static CHDRV_VOUT_LayerEnable_t 	 grstru_LayerEnable ;	
static CHDRV_VOUT_Port_t              grstru_OutputPort ;
static S32 								 FMT_ENABLE[17] = {0};
char FMT_ENV[VOUT_FMT_NUM][VOUT_FMT_LENGTH] = {
	"VOUT_FMT_480I60_SUPPORT",
	"VOUT_FMT_576I50_SUPPORT",	
	"VOUT_FMT_576I60_SUPPORT",
	"VOUT_FMT_576I50_SECAM_SUPPORT",   
	"VOUT_FMT_HD_480P60_SUPPORT", 
	"VOUT_FMT_HD_576P50_SUPPORT", 
	"VOUT_FMT_HD_720P50_SUPPORT",  
	"VOUT_FMT_HD_720P60_SUPPORT",  
	"VOUT_FMT_HD_1080I50_SUPPORT",	   
	"VOUT_FMT_HD_1080I60_SUPPORT",	
	"VOUT_FMT_HD_1080P50_SUPPORT",	   
	"VOUT_FMT_HD_1080P60_SUPPORT",	   
	"VOUT_FMT_HD_2160P24_SUPPORT",	   
	"VOUT_FMT_HD_2160P25_SUPPORT",	   
	"VOUT_FMT_HD_2160P30_SUPPORT",	   
	"VOUT_FMT_HD_2160P50_SUPPORT",	   
	"VOUT_FMT_HD_2160P60_SUPPORT" 
};



static void test_osd1_ready(void)
{
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("osd.pic 3 0 1280 720 %s", fw_getenv("OSD_BASIC_HD"));
#else
	RUN_CMD("osd.pic 3 0 720 576 %s", fw_getenv("OSD_BASIC_SD"));
#endif

	EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_SetRegionAlpha(osd_GetHandle(CH_LAYER_OSD1, CHDRV_OSD_REGION_DRAW),70));	
	grh_BmpDrawHandle1 = osd_GetHandle(CH_LAYER_OSD1, CHDRV_OSD_REGION_DRAW);
	grh_BmpDisplayHandle1 = osd_GetHandle(CH_LAYER_OSD1, CHDRV_OSD_REGION_DISPLAY);
}


static void VOUT_Check_FmtEnable(void)
{
	S32 ri_Loop = 0;
	for(ri_Loop = 0;ri_Loop < VOUT_FMT_NUM;ri_Loop ++)
	{
		if((S32)fw_getenv_i(&(FMT_ENV[ri_Loop][0])) == VOUT_RESOLUTION_SURPORT)//VOUT_RESOLUTION_SURPORT
		{
			FMT_ENABLE[ri_Loop] = 1;
		}
			
	}

}
void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;

	RUN_CMD("route.create 0 0");
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.open 1 1 1 1 0 0 0 0\n");
	RUN_CMD("vout.set_format 6");
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_VOUT_FILE_HD"));
#else
	RUN_CMD("vout.set_format 1");
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_VOUT_FILE_SD"));
#endif
	
	delete_prompt_region();
	test_osd1_ready();
	VOUT_Check_FmtEnable();
	//RUN_CMD("osd.delregion");
	//test_osd2_ready();
	// init
	
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit voutn rtos TestCase " << std::endl;
	if(grh_BmpDrawHandle1)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_ClearFullRegion(grh_BmpDrawHandle1, 0));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_UpdateRegion(grh_BmpDrawHandle1));
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDrawHandle1));
	}
	if(grh_BmpDisplayHandle1)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDisplayHandle1));
	}
	if(grh_BmpDrawHandle2)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDrawHandle2));
	}
	if(grh_BmpDisplayHandle2)
	{
		EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDisplayHandle2));
	}
	delete_prompt_region();
	RUN_CMD("vid.ram_stop 0");
	RUN_CMD("vout.set_format 6");
	RUN_CMD("osd.delregion");
	RUN_CMD("vout.close 1\n");
	RUN_CMD("vout.close 0\n");
	grh_BmpDrawHandle1 = NULL;
	grh_BmpDisplayHandle1 = NULL;
	grh_BmpDrawHandle2 = NULL;
	grh_BmpDisplayHandle2 = NULL;
}

void CASE_NAME::SetUp()
{
//execute before testcase
//std::cout << "Chdrv SetUp" << std::endl;
}
void CASE_NAME::TearDown()
{
//execute before testcase
//std::cout << "Chdrv TearDown" << std::endl;
}
#if ENABLE_TESTCASE

CASE_F(VOUT_VOUT_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_VOUT_OK, CHDRV_VOUT_Init());
	EXPECT_EQ(CHDRV_VOUT_OK, CHDRV_VOUT_Init()) << "��γ�ʼ��Ӧ�÷���OK";
	
	
}

CASE_F(VOUT_VOUT_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	//EXPECT_EQ(CHDRV_VOUT_OK, CHDRV_VOUT_Term());//Ӱ���������
	EXPECT_EQ(CHDRV_VOUT_OK, CHDRV_VOUT_Init());
	//
}


CASE_F(VOUT_VOUT_INIT3,
	"����CHDRV_VOUT_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_VOUT_OK, CHDRV_VOUT_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(VOUT_VOUT_Open ,
	"����CHDRV_VOUT_Open�������") {
	S32 							  ri_PathIndex = VOUT_PATH_HD;
	CHDRV_VOUT_LayerEnable_t    rstru_LayerEnable1 ;
	CHDRV_VOUT_LayerEnable_t  * rstru_LayerEnable2 = NULL;
	CHDRV_VOUT_PathHandle    rh_VideoOutputHandle1 ;
	CHDRV_VOUT_PathHandle  * rh_VideoOutputHandle2 = NULL;
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Open(ri_PathIndex,&rstru_LayerEnable1,rh_VideoOutputHandle2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Open(ri_PathIndex,rstru_LayerEnable2,&rh_VideoOutputHandle1));

}


CASE_F(VOUT_VOUT_FORMAT,
	"����CHDRV_VOUT_SetFormat�����������") {
	CHDRV_VOUT_PathHandle  rh_VideoOutputHandle = NULL;
	CHDRV_VOUT_FORMAT_e 			renm_Format = CHDRV_VOUT_FMT_MAX;
	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetFormat(rh_VideoOutputHandle,renm_Format));
}

CASE_F(VOUT_VOUT_ENCODER2,
	"����CHDRV_VOUT_GetEncoderState�������") {
	

}

CASE_F(VOUT_VOUT_DISPLAY,
	"����CHDRV_VOUT_SetLayerDisplay��GetLayerDisplay�������") {
	CHDRV_VOUT_PathHandle    rh_VideoOutputHandle1 = NULL;
	CHDRV_VOUT_PathHandle    rh_VideoOutputHandle2 = VOUT_HANDLE;
	//CHDRV_VOUT_PathHandle    rh_VideoOutputHandle3 = VOUT_HANDLE_INVALID;
	CHDRV_VOUT_LayerEnable_t   rpstru_LayerEnable1 ;
	CHDRV_VOUT_LayerEnable_t * rpstru_LayerEnable2 = NULL;
	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerDisplay(rh_VideoOutputHandle1,&rpstru_LayerEnable1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerDisplay(rh_VideoOutputHandle2,rpstru_LayerEnable2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerDisplay(rh_VideoOutputHandle3,&rpstru_LayerEnable1));
	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerDisplay(rh_VideoOutputHandle1,&rpstru_LayerEnable1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerDisplay(rh_VideoOutputHandle2,rpstru_LayerEnable2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerDisplay(rh_VideoOutputHandle3,&rpstru_LayerEnable1));
}
CASE_F(VOUT_VOUT_PRIORITY,
	"����CHDRV_VOUT_SetLayerPriority��GetLayerPriority�����������") {
	
	CH_LAYER_e   rpenm_LayerList1 ;
	CH_LAYER_e * rpenm_LayerList2 = NULL;
	S32 			 ri_LayerNumb = 0 ;
	S32          ri_LayerNumbGet1 ;
	S32 *		 ri_LayerNumbGet2 = NULL;
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerPriority(rpenm_LayerList2, ri_LayerNumb));
	//GetLayerPriority�����������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerPriority(&rpenm_LayerList1, ri_LayerNumbGet2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerPriority(rpenm_LayerList2, &ri_LayerNumbGet1));
}
CASE_F(VOUT_VOUT_BRIGHTNESS,
	"����CHDRV_VOUT_SetBrightness��GetBrightness�������") {
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle1 = NULL;
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle2 = VOUT_HANDLE;
	//CHDRV_VOUT_PathHandle rh_VideoOutputHandle3 = VOUT_HANDLE_INVALID;
	
	S8 						rc_BrightnessValue1 = VOUT_PARAM_MINUS;
	S8 						rc_BrightnessValue2 = VOUT_PARAM_VALUE_DEFAULT;
	S8						rc_BrightnessValue3 = VOUT_PARAM_VALUE_INVALID;
	
	S8 					 rc_BrightnessValueGet1 ;
	S8*					 rc_BrightnessValueGet2 = NULL;
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(rh_VideoOutputHandle1,rc_BrightnessValue2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(rh_VideoOutputHandle3,rc_BrightnessValue2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(rh_VideoOutputHandle2,rc_BrightnessValue1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(rh_VideoOutputHandle2,rc_BrightnessValue3));
	//GetBrightness�������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetBrightness(rh_VideoOutputHandle1,&rc_BrightnessValueGet1));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetBrightness(rh_VideoOutputHandle3,&rc_BrightnessValueGet1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetBrightness(rh_VideoOutputHandle2,rc_BrightnessValueGet2));

}




CASE_F(VOUT_VOUT_CONTRAST,
	"����CHDRV_VOUT_SetContrast��GetContrast�������") {
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle1 = NULL;
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle2 = VOUT_HANDLE;
	//CHDRV_VOUT_PathHandle rh_VideoOutputHandle3 = VOUT_HANDLE_INVALID;
	S8 						  rc_ContrastValue1 = VOUT_PARAM_MINUS;
	S8 						  rc_ContrastValue2 = VOUT_PARAM_VALUE_DEFAULT;
	S8						  rc_ContrastValue3 = VOUT_PARAM_VALUE_INVALID;
	
	S8 				   	   rc_ContrastValueGet1 ; 
	S8*					   rc_ContrastValueGet2 = NULL;	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(rh_VideoOutputHandle1,rc_ContrastValue2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(rh_VideoOutputHandle3,rc_ContrastValue2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(rh_VideoOutputHandle2,rc_ContrastValue1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(rh_VideoOutputHandle2,rc_ContrastValue3));
	//GetContrast�������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetContrast(rh_VideoOutputHandle1,&rc_ContrastValueGet1));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetContrast(rh_VideoOutputHandle3,&rc_ContrastValueGet1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetContrast(rh_VideoOutputHandle2,rc_ContrastValueGet2));
	
}



CASE_F(VOUT_VOUT_SATURATION,
	"����CHDRV_VOUT_SetSaturation��GetSaturation�������") {
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle1 = NULL;
	CHDRV_VOUT_PathHandle rh_VideoOutputHandle2 = VOUT_HANDLE;
	//CHDRV_VOUT_PathHandle rh_VideoOutputHandle3 = VOUT_HANDLE_INVALID;
	S8 						rc_SaturationValue1 = VOUT_PARAM_MINUS;
	S8 						rc_SaturationValue2 = VOUT_PARAM_VALUE_DEFAULT;
	S8						rc_SaturationValue3 = VOUT_PARAM_VALUE_INVALID;

	S8 					 rc_SaturationValueGet1 ; 
	S8*					 rc_SaturationValueGet2 = NULL;	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(rh_VideoOutputHandle1,rc_SaturationValue2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(rh_VideoOutputHandle3,rc_SaturationValue2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(rh_VideoOutputHandle2,rc_SaturationValue1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(rh_VideoOutputHandle2,rc_SaturationValue3));
	//SetSaturation�������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetSaturation(rh_VideoOutputHandle1,&rc_SaturationValueGet1));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetSaturation(rh_VideoOutputHandle3,&rc_SaturationValueGet1));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetSaturation(rh_VideoOutputHandle2,rc_SaturationValueGet2));
}



CASE_F(VOUT_VOUT_BGLAYERCOLOR,
	"����CHDRV_VOUT_SetBGLayerColor��GetBGLayerColor�������") {
	//U32 	rui_RGB888Color = 0;
	U32* rui_RGB888ColorGet = NULL;
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBGLayerColor(rui_RGB888Color));//�޴�������ɲ�
	//GetBGLayerColor�������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetBGLayerColor(rui_RGB888ColorGet));
}



CASE_F(VOUT_VOUT_ALPHA,
	"����CHDRV_VOUT_SetLayerAlpha��GetLayerAlpha�������") {
	CH_LAYER_e renm_Layer1 = CH_LAYER_VIDEO1;
	U8* ruc_LayerAlphaGet2 = NULL;
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerAlpha(renm_Layer2,ruc_LayerAlpha1));//video layer������ԣ�porting ֻ������δ�����أ�
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerAlpha(renm_Layer1,ruc_LayerAlpha2));//δ��alphaֵ����Χ�ж�
	//GetLayerAlpha�������
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerAlpha(renm_Layer1,ruc_LayerAlphaGet2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_GetLayerAlpha(renm_Layer2,&ruc_LayerAlphaGet1));//δ��layer���ж�
}



#if 0 	/*sqzou20170925*/
CASE_F(VOUT_VOUT_CAPTURE,
	"����CHDRV_VOUT_Capture2OSD�������") {
	CH_LAYER_e 	    	  renm_SrcLayer1 = CH_LAYER_VIDEO1;
	CH_LAYER_e 			  renm_SrcLayer2 = CH_LAYER_MAX;
	
	CH_Rect_t  			  rstru_SrcArea1 = {VOUT_PARAM_MINUS,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT};
	CH_Rect_t  			  rstru_SrcArea2 = {VOUT_PARAM_RIGHT,VOUT_PARAM_MINUS,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT}; 
	CH_Rect_t  			  rstru_SrcArea3 = {VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_MINUS,VOUT_PARAM_RIGHT}; 
	CH_Rect_t  			  rstru_SrcArea4 = {VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_MINUS}; 
	CH_Rect_t  			  rstru_SrcArea5 = {VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT}; 
	
	CH_Rect_t  			  rstru_SrcArea6 = {VOUT_PARAM_OVER_WRANGE,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT}; 
	CH_Rect_t  			  rstru_SrcArea7 = {VOUT_PARAM_RIGHT,VOUT_PARAM_OVER_HRANGE,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT}; 
	CH_Rect_t  			  rstru_SrcArea8 = {VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_OVER_WRANGE,VOUT_PARAM_RIGHT}; 
	CH_Rect_t  			  rstru_SrcArea9 = {VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_OVER_HRANGE}; 
	
	CHDRV_OSD_RegionHandle rh_OSDHandle1 = NULL;
	CHDRV_OSD_RegionHandle rh_OSDHandle2 = VOUT_HANDLE;
	CHDRV_OSD_RegionHandle rh_OSDHandle3 = VOUT_HANDLE_OSD_INVALID;
	
	S16 				   rs_DstStartX1 = VOUT_PARAM_MINUS;
	S16 				   rs_DstStartX2 = VOUT_PARAM_RIGHT;
	S16 				   rs_DstStartX3 = VOUT_PARAM_OVER_WRANGE;
	
	S16 				   rs_DstStartY1 = VOUT_PARAM_MINUS;
	S16 				   rs_DstStartY2 = VOUT_PARAM_RIGHT;
	S16 				   rs_DstStartY3 = VOUT_PARAM_OVER_HRANGE;
	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer2,rstru_SrcArea5,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea1,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea2,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea3,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea4,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY2));
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle1,rs_DstStartX2,rs_DstStartY2));

	//δ��Խ���ж�
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea6,rh_OSDHandle1,rs_DstStartX2,rs_DstStartY2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea7,rh_OSDHandle1,rs_DstStartX2,rs_DstStartY2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea8,rh_OSDHandle1,rs_DstStartX2,rs_DstStartY2));
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea9,rh_OSDHandle1,rs_DstStartX2,rs_DstStartY2));

	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle3,rs_DstStartX2,rs_DstStartY2));//����osd handle �����Χ

	//δ��Ŀ�������x,y��������Χ�ж�
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle2,rs_DstStartX1,rs_DstStartY2));//Ŀ��x<0
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle2,rs_DstStartX3,rs_DstStartY2));//Ŀ��x>W   δ���ж�
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY1));//Ŀ������y<0
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_Capture2OSD(renm_SrcLayer1,rstru_SrcArea5,rh_OSDHandle2,rs_DstStartX2,rs_DstStartY1));//Ŀ������y>H  δ���ж�
}
#endif

CASE_F(VOUT_VOUT_OPEN_1,
	"���Բ���Ϊpathid=0��osd1��osd2��video1Ϊ��Ч��CHDRV_VOUT_Open������ֵӦ��ȷ��handle��ΪNULL") {
#ifdef CFG_CHSSA_HD_SUPPORT	
	S32 	ri_PathIndex = VOUT_PATH_HD;  
#else
	S32 	ri_PathIndex = VOUT_PATH_SD;  

#endif	
	grstru_LayerEnable.b_Osd1 	= CH_TRUE;
	grstru_LayerEnable.b_Osd2 	= CH_TRUE;
	grstru_LayerEnable.b_Video1 = CH_TRUE;
#ifdef CFG_CHSSA_HD_SUPPORT	
	if(vout_GetHandleNums() > 1)
	{
		grh_VideoOutputHandle = vout_GetHandle(1);
	}
#else
	if(vout_GetHandleNums() > 0)
	{
		grh_VideoOutputHandle = vout_GetHandle(0);
	}
#endif
	else
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_Open(ri_PathIndex,&grstru_LayerEnable,&grh_VideoOutputHandle));
	}
	EXPECT_NE((CHDRV_VOUT_PathHandle)NULL, grh_VideoOutputHandle);

}
CASE_F(VOUT_VOUT_ONOFF_1,
	"����CHDRV_VOUT_OnOff��osd1��osd2��video1����Ϊon��Ӧ����OK") {
	CH_BOOL rb_Output = CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_OnOff(rb_Output));
}



CASE_F(VOUT_VOUT_SETPORT_1,
	"����CHDRV_VOUT_SetPort��CVBS|HDMI|YPBPR������Ϊ�ɼ�������ֵӦ��ȷ���Ӷ�Ӧ����ӿ�Ӧ�ܿ������") {
	memset(&grstru_OutputPort, 0, sizeof(grstru_OutputPort));
	
	grstru_OutputPort.b_IsHDMI 	 = CH_TRUE;
	grstru_OutputPort.b_IsSDCvbs = CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
}

CASE_F(VOUT_VOUT_ONOFF_2,
	"����CHDRV_VOUT_OnOff��osd1��osd2��video1����1~2������Ϊoff��Ӧ����OK����Ļ�������̿�����off�Ĳ㲻�ɼ�") {
	
	CH_BOOL rb_Output = CH_FALSE;
	PROMPT("����OSD��ʾͼƬ,��OK������...");
	test_input_none();
	CHDRV_OSD_HideRegion(grh_BmpDrawHandle1);
	PROMPT("����I֡��ʾͼƬ,��OK������...");
	test_input_none();
	
	
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_OnOff(rb_Output));//CHDRV_VOUT_OnOffû��ָ���ر�ĳ��ڹ���
	PROMPT("�Ƿ�������Ƶ����˿ھ������?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "�ر������Ч��";
	rb_Output = CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_OnOff(rb_Output));//CHDRV_VOUT_OnOffû��ָ���ر�ĳ��ڹ���
	PROMPT("�Ƿ�������Ƶ����˿ھ��ѻָ����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "�ָ������Ч��";

	CHDRV_OSD_ShowRegion(grh_BmpDrawHandle1);
}


CASE_F(VOUT_VOUT_SETPORT_2,
	"����CHDRV_VOUT_SetPort��CVBS|HDMI|YPBPR����1~2������Ϊoff��Ӧ����ok����off�Ķ˿�Ӧ���������;GET PORTӦ��set��һ��") {
	CHDRV_VOUT_Port_t  rstru_OutputPortGet;
	
	//off ypbpr
	if(fw_getenv_i("SUPPORT_YPbPr") == 1)
	{
		grstru_OutputPort.b_IsYPbPr = CH_FALSE;
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetPort(&rstru_OutputPortGet));
		EXPECT_EQ(CH_FALSE,rstru_OutputPortGet.b_IsYPbPr);
		PROMPT("�����Ƿ������?��������Y����������N");
		EXPECT_EQ(CH_TRUE,test_input_yes()) << "�رշ�����Ч��";
		grstru_OutputPort.b_IsYPbPr = CH_TRUE;
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
	}
	if(fw_getenv_i("SUPPORT_CVBS") == 1)
	{
		grstru_OutputPort.b_IsSDCvbs = CH_FALSE;
		//off cvbs 
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetPort(&rstru_OutputPortGet));
		EXPECT_EQ(CH_FALSE,rstru_OutputPortGet.b_IsSDCvbs);
		PROMPT("CVBS�Ƿ������?��������Y����������N");
		EXPECT_EQ(CH_TRUE,test_input_yes()) << "�ر�CVBS��Ч��";
		grstru_OutputPort.b_IsSDCvbs = CH_TRUE;
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
	}

	if(fw_getenv_i("SUPPORT_HDMI") == 1)
	{
		//off HDMI 
		grstru_OutputPort.b_IsHDMI = CH_FALSE;
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetPort(&rstru_OutputPortGet));
		EXPECT_EQ(CH_FALSE,rstru_OutputPortGet.b_IsHDMI);
		PROMPT("HDMI�Ƿ������?��������Y����������N");
		EXPECT_EQ(CH_TRUE,test_input_yes()) << "�ر�HDMI��Ч��";
		grstru_OutputPort.b_IsHDMI = CH_TRUE;
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetPort(&grstru_OutputPort));
	}
}


CASE_F(VOUT_VOUT_SETFORMAT,
	"����CHDRV_VOUT_SetFormat��֧�ֵĸ�ʽ��Ӧ����OK����Ļ���ܿ���ģʽ�л��ɹ���OSD������GetFormat�Ƿ񷵻��Ƿ�͵�ǰ��ʽһ��") {
	S32 i = 0;
	CHDRV_VOUT_FORMAT_e enm_Format;
	CHDRV_VOUT_FORMAT_e enm_FormatGet;
	CH_Rect_t stru_Rect;
	
	const char *puc_CvbsFMT[] = 
	{
		"NTSC",
		"PAL",
		"PAL-M",
		"SECAM"
	};
	const int region_wh[CHDRV_VOUT_FMT_MAX][2] =
	{
			{640, 480}, //CHDRV_VOUT_FMT_480I60,			///< NTSC��ʽ
			{720, 576}, //CHDRV_VOUT_FMT_576I50,			///< PAL��ʽ
			{720, 576}, //CHDRV_VOUT_FMT_576I60,			///< PAL-60��PAL-Mʹ��
			{720, 576}, //CHDRV_VOUT_FMT_576I50_SECAM,  	///< SECAM��ʽ
			{640, 480}, //CHDRV_VOUT_FMT_HD_480P60,		///< 480 ����  60����
			{720, 576}, //CHDRV_VOUT_FMT_HD_576P50,		///< 576  ���� 50Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_720P50,		///< 720  ���� 50Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_720P60,		///< 720  ���� 60Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_1080I50,     	///< 1080 ���� 50Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_1080I60,		///< 1080 ���� 60Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_1080P50,     	///< 1080 ���� 50Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_1080P60,     	///< 1080 ���� 60Hz
			{1280, 720}, //CHDRV_VOUT_FMT_HD_2160P24,     	/*2160 ���� 24Hz*/
			{1280, 720}, //CHDRV_VOUT_FMT_HD_2160P25,     	/*2160 ���� 25Hz*/
			{1280, 720}, //CHDRV_VOUT_FMT_HD_2160P30,     	/*2160 ���� 30Hz*/
			{1280, 720}, //CHDRV_VOUT_FMT_HD_2160P50,     	/*2160 ���� 50Hz*/
			{1280, 720}, //CHDRV_VOUT_FMT_HD_2160P60,     	/*2160 ���� 60Hz*/
	};
	for(i = 0;i < VOUT_FMT_NUM;i ++)
	{
		if(FMT_ENABLE[i] == 1)// ����֧��ö��˳��Ϊri_loop�ķֱ���
		{
			enm_Format = (CHDRV_VOUT_FORMAT_e)i;			///i = 0,��Ӧö�ٵ�һ��������
			EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetFormat(grh_VideoOutputHandle,enm_Format));
			EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetFormat(grh_VideoOutputHandle,&enm_FormatGet));
			EXPECT_EQ((CHDRV_VOUT_FORMAT_e)i, enm_FormatGet);
			RUN_CMD("osd.delregion");
			memset(&stru_Rect, 0, sizeof(stru_Rect));
			stru_Rect.s_Width = region_wh[i][0];
			stru_Rect.s_Height = region_wh[i][1] ;
			EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_SetRect(vwin_GetHandle(0), &stru_Rect));
#ifdef CFG_CHSSA_HD_SUPPORT	
			RUN_CMD("osd.pic 3 0 %d %d %s", 
				region_wh[i][0],
				region_wh[i][1],
				fw_getenv("OSD_BASIC_HD"));
#else
			RUN_CMD("osd.pic 3 0 %d %d %s", 
				region_wh[i][0],
				region_wh[i][1],
				fw_getenv("OSD_BASIC_SD"));
#endif
			
			PROMPT("HDMI�ֱ����л�Ϊ%s�Ƿ�ɹ�?��������Y����������N", FMT_ENV[i]);
			EXPECT_EQ(CH_TRUE,test_input_yes()) << "HDMI�л��ֱ���" << FMT_ENV[i] << "ʧ��";
		}
	}

#ifdef CFG_CHSSA_HD_SUPPORT
	PROMPT("��ʼ�л�CVBS�ķֱ��ʣ���ע��۲�HDMI��������棬׼�����Ժ��밴�س�");
	test_input_none();
	for(i = CHDRV_VOUT_FMT_480I60; i < CHDRV_VOUT_FMT_576I50_SECAM; i++)
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetFormat(vout_GetHandle(0), (CHDRV_VOUT_FORMAT_e)i));
		LOG("CVBS��ʽ�л�Ϊ%s(%s)...\n", FMT_ENV[i], puc_CvbsFMT[i]);
		CH_RTOS_TaskDelay(1000);
	}
	PROMPT("CVBS�л������У�HDMI��������̶��Ƿ�ɽ���?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ա��Ϊ�ڱ����л��ֱ��ʹ����У�HDMI���Ч������";
	
	PROMPT("�����CVBS�����֧����ʾ������ʽ�ĵ��ӣ�׼�����Ժ���س�");
	test_input_none();

	for(i = CHDRV_VOUT_FMT_480I60; i < CHDRV_VOUT_FMT_576I50_SECAM; i++)
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetFormat(vout_GetHandle(0), (CHDRV_VOUT_FORMAT_e)i));
		PROMPT("CVBS�����ʽ�л�Ϊ%s(%s)�Ƿ�ɹ�?��������Y����������N", FMT_ENV[i], puc_CvbsFMT[i]);
		EXPECT_EQ(CH_TRUE,test_input_yes()) << "CVBS�л��ֱ���" << FMT_ENV[i] << "("<< puc_CvbsFMT[i] <<  ")ʧ��";
	}
	PROMPT("��ʼ�л�HDMI�ķֱ��ʣ���ע��۲�CVBS��������棬׼�����Ժ��밴�س�");
	test_input_none();

	for(i = 0;i < VOUT_FMT_NUM;i ++)
	{
		if(FMT_ENABLE[i] == 1)// ����֧��ö��˳��Ϊri_loop�ķֱ���
		{
			CH_Rect_t stru_Rect;
			enm_Format = (CHDRV_VOUT_FORMAT_e)i;			///i = 0,��Ӧö�ٵ�һ��������
			EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetFormat(grh_VideoOutputHandle,enm_Format));
			RUN_CMD("osd.delregion");
			memset(&stru_Rect, 0, sizeof(stru_Rect));
			stru_Rect.s_Width = region_wh[i][0];
			stru_Rect.s_Height = region_wh[i][1] ;
			EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_SetRect(vwin_GetHandle(0), &stru_Rect));
			RUN_CMD("osd.pic 3 0 %d %d %s", 
				region_wh[i][0],
				region_wh[i][1],
				fw_getenv("OSD_BASIC_HD"));
		
			LOG("HDMI�ֱ����л�Ϊ%s...\n", FMT_ENV[i]);
			CH_RTOS_TaskDelay(1000);
		}
	}

	PROMPT("HDMI�л������У�CVBS��������̶��Ƿ�ɽ���?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "����Ա��Ϊ��HDMI�л��ֱ��ʹ����У�CVBS���Ч������";

	//�ָ��ֳ�
	RUN_CMD("vout.set_format 6");	
	RUN_CMD("osd.delregion");
	memset(&stru_Rect, 0, sizeof(stru_Rect));
	stru_Rect.s_Width = region_wh[CHDRV_VOUT_FMT_HD_720P50][0];
	stru_Rect.s_Height = region_wh[CHDRV_VOUT_FMT_HD_720P50][1] ;
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_SetRect(vwin_GetHandle(0), &stru_Rect));
	RUN_CMD("osd.pic 3 0 %d %d %s", 
				region_wh[CHDRV_VOUT_FMT_HD_720P50][0],
				region_wh[CHDRV_VOUT_FMT_HD_720P50][1],
				fw_getenv("OSD_BASIC_HD"));
#else
	//�ָ��ֳ�

	RUN_CMD("vout.set_format 1");	
	RUN_CMD("osd.delregion");
	memset(&stru_Rect, 0, sizeof(stru_Rect));
	stru_Rect.s_Width = region_wh[CHDRV_VOUT_FMT_576I50][0];
	stru_Rect.s_Height = region_wh[CHDRV_VOUT_FMT_576I50][1] ;
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_SetRect(vwin_GetHandle(0), &stru_Rect));
	RUN_CMD("osd.pic 3 0 %d %d %s", 
		region_wh[CHDRV_VOUT_FMT_576I50][0],
		region_wh[CHDRV_VOUT_FMT_576I50][1],
		fw_getenv("OSD_BASIC_SD"));
#endif
	

}






CASE_F(VOUT_VOUT_SETLAYER_DISPLAY_1,
	"����CHDRV_VOUT_SetLayerDisplay��������osd1��osd2��video1Ϊ�ɼ���Ӧ����OK��GetLayerDisplay��ȡ����vout״̬Ӧ�ú͵�ǰ����һ��") {
	grstru_LayerEnable.b_Osd1 	= CH_TRUE;
	grstru_LayerEnable.b_Osd2 	= CH_TRUE;
	grstru_LayerEnable.b_Video1 = CH_TRUE;
	CHDRV_VOUT_LayerEnable_t  rstru_LayerEnable;
	//
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetLayerDisplay(grh_VideoOutputHandle,&rstru_LayerEnable));
	EXPECT_EQ(CH_TRUE,rstru_LayerEnable.b_Osd1);
	EXPECT_EQ(CH_TRUE,rstru_LayerEnable.b_Osd2);
	EXPECT_EQ(CH_TRUE,rstru_LayerEnable.b_Video1);

	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_HideRegion(grh_BmpDrawHandle1));
}



CASE_F(VOUT_VOUT_SETBRIGHTNESS,
	"����CHDRV_VOUT_SetBrightness���ò�ͬ�������ܹ��۲���Ƶ���ȱ仯������������Ȳ��仯����ȷ���õĲ������ᱻ�ı䣻GetBrightness��ȡ��ȷ����ֵ") {
	S8 rc_BrightnessValueGet = 0;
	
	S8    rc_BrightnessValue = VOUT_PARAM_MINUS;//��������������ޱ仯	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(grh_VideoOutputHandle,rc_BrightnessValue));
	CH_RTOS_TaskDelay(200);
	
	rc_BrightnessValue = VOUT_PARAM_VALUE_INVALID;//��������������ޱ仯
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetBrightness(grh_VideoOutputHandle,rc_BrightnessValue));
	CH_RTOS_TaskDelay(200);
	

	PROMPT("��׼���ù۲���Ƶ���ȱ仯,��OK������...");
	test_input_none();
	for(rc_BrightnessValue = 0 ; rc_BrightnessValue <= 100 ; rc_BrightnessValue+=20 )
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBrightness(grh_VideoOutputHandle,rc_BrightnessValue));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetBrightness(grh_VideoOutputHandle,&rc_BrightnessValueGet));
		EXPECT_EQ(rc_BrightnessValue,rc_BrightnessValueGet);
		CH_RTOS_TaskDelay(200);
	}
	PROMPT("��Ƶ���ȱ仯�Ƿ�����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetBrightness��Ч��";

	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBrightness(grh_VideoOutputHandle, 50));
	
}


CASE_F(VOUT_VOUT_SETCONTRAST,
	"����CHDRV_VOUT_SetContrast���ò�ͬ�ĶԱȶ�ֵ�ܹ��۲���Ƶ�仯������������仯����ȷ���õĲ������ᱻ�ı䣻GetContrast�ܹ���ȷ��ȡ�Աȶ�ֵ") {

	S8 rc_ContrastValueGet = 0;
	S8    c_ContrastValue = VOUT_PARAM_MINUS;//����������Աȶ��ޱ仯	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(grh_VideoOutputHandle,c_ContrastValue));
	
	c_ContrastValue = VOUT_PARAM_VALUE_INVALID;//����������Աȶ��ޱ仯	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetContrast(grh_VideoOutputHandle,c_ContrastValue));

	PROMPT("��׼���ù۲���Ƶ�Աȶȱ仯,��OK������...");
	test_input_none();
	for(c_ContrastValue = 0;c_ContrastValue <= 100;c_ContrastValue += 20 )
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetContrast(grh_VideoOutputHandle,c_ContrastValue));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetContrast(grh_VideoOutputHandle,&rc_ContrastValueGet));
		EXPECT_EQ(c_ContrastValue,rc_ContrastValueGet);
		CH_RTOS_TaskDelay(200);
	}
	PROMPT("��Ƶ�Աȶȱ仯�Ƿ�����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetContrast��Ч��";

	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetContrast(grh_VideoOutputHandle,50));
}


CASE_F(VOUT_VOUT_SETSATURATION,
	"����CHDRV_VOUT_SetSaturation���ò�ͬ�ı��Ͷ��ܹ��۲���Ƶ�仯������������仯����ȷ���õĲ������ᱻ�ı�;GetSaturation��ȡ���ı��Ͷ�ֵ��ȷ") {
	S8 rc_SaturationValueGet ;
	S8    rc_SaturationValue = VOUT_PARAM_MINUS;//����������Աȶ��ޱ仯	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(grh_VideoOutputHandle,rc_SaturationValue));
	CH_RTOS_TaskDelay(200);
	rc_SaturationValue = VOUT_PARAM_VALUE_INVALID;//����������Աȶ��ޱ仯	
	EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetSaturation(grh_VideoOutputHandle,rc_SaturationValue));

	PROMPT("��׼���ù۲���Ƶ���Ͷȱ仯,��OK������...");
	test_input_none();
	for(rc_SaturationValue = 0;rc_SaturationValue <= 100;rc_SaturationValue += 20 )
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetSaturation(grh_VideoOutputHandle,rc_SaturationValue));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetSaturation(grh_VideoOutputHandle,&rc_SaturationValueGet));
		EXPECT_EQ(rc_SaturationValue,rc_SaturationValueGet);
		CH_RTOS_TaskDelay(200);
	}
	PROMPT("��Ƶ���Ͷȱ仯�Ƿ�����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetSaturation��Ч��";
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetSaturation(grh_VideoOutputHandle,50));


	grstru_LayerEnable.b_Osd1 = CH_TRUE;
	grstru_LayerEnable.b_Osd2 = CH_TRUE;
	grstru_LayerEnable.b_Video1 = CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));

	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_ShowRegion(grh_BmpDrawHandle1));
}


CASE_F(VOUT_VOUT_SETLAYER_DISPLAY_2,
	"����CHDRV_VOUT_SetLayerDisplay��������osd1��osd2��video1����1~2��Ϊ���ɼ���Ӧ����OK����Ļ��Ӧ�����̿������ɼ��Ĳ㱻���ء�GetLayerDisplay��ȡ����vout״̬Ӧ�ú͵�ǰ����һ��") {
	CHDRV_VOUT_LayerEnable_t  rstru_LayerEnable;
	
	grstru_LayerEnable.b_Osd2 = CH_FALSE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));
	grstru_LayerEnable.b_Osd1 = CH_FALSE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));
	PROMPT("��Ļ���Ƿ�ֻ��I֡����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "SetLayerDisplay osdΪfalse��Ч��";

	grstru_LayerEnable.b_Video1 = CH_FALSE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));
	PROMPT("��Ļ���Ƿ�Ϊ����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "SetLayerDisplay video1Ϊfalse��Ч��";
	
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetLayerDisplay(grh_VideoOutputHandle,&rstru_LayerEnable));
	EXPECT_EQ(CH_FALSE,rstru_LayerEnable.b_Osd1);
	EXPECT_EQ(CH_FALSE,rstru_LayerEnable.b_Osd2);
	EXPECT_EQ(CH_FALSE,rstru_LayerEnable.b_Video1);
	CH_RTOS_TaskDelay(200);


	grstru_LayerEnable.b_Osd1 = CH_TRUE;
	grstru_LayerEnable.b_Osd2 = CH_TRUE;
	grstru_LayerEnable.b_Video1 = CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));

	PROMPT("��Ļ��OSD��ʾ�Ƿ��ѻָ�?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "SetLayerDisplay�ָ�OSD��Ч��";

}
#endif /* ENABLE_TESTCASE */


#if 0 	/*sqzou20170425*/
CASE_F(VOUT_VOUT_SETCOLOR,
	"�ص���Ƶ���ţ�����CHDRV_VOUT_SetBGLayerColor���ò�ͬ�ı�����ɫ�ܹ��۲�ͼ�㱳��ɫ�仯������������仯����ȷ���õĲ������ᱻ�ı䣻GetBGLayerColor��ȡ����ֵ��ȷ") {
	U32 rui_RGB888Color;
	U32 rui_RGB888ColorGet = 0;

	CHDRV_OSD_HideRegion(grh_BmpDisplayHandle1);

	RUN_CMD("vid.ram_stop 0");	
	CHDRV_VIDEO_ClearVideoBuffer(video_GetHandle(0));
	
	rui_RGB888Color = VOUT_BGCOLOR_WHITE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBGLayerColor(rui_RGB888Color));
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetBGLayerColor(&rui_RGB888ColorGet));
	EXPECT_EQ(VOUT_BGCOLOR_WHITE,rui_RGB888ColorGet);
	PROMPT("��Ļ�Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetBGLayerColor��Ч��";
	
	rui_RGB888Color = VOUT_BGCOLOR_RED;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBGLayerColor(rui_RGB888Color));
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetBGLayerColor(&rui_RGB888ColorGet));
	EXPECT_EQ(VOUT_BGCOLOR_RED,rui_RGB888ColorGet);
	PROMPT("��Ļ�Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetBGLayerColor��Ч��";
	
	rui_RGB888Color = VOUT_BGCOLOR_GREEN;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBGLayerColor(rui_RGB888Color));
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetBGLayerColor(&rui_RGB888ColorGet));
	EXPECT_EQ(VOUT_BGCOLOR_GREEN,rui_RGB888ColorGet);
	PROMPT("��Ļ�Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetBGLayerColor��Ч��";

	rui_RGB888Color = VOUT_BGCOLOR_BLUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetBGLayerColor(rui_RGB888Color));
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetBGLayerColor(&rui_RGB888ColorGet));
	EXPECT_EQ(VOUT_BGCOLOR_BLUE,rui_RGB888ColorGet);
	PROMPT("��Ļ�Ƿ���ʾΪ��ɫ?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetBGLayerColor��Ч��";
	
	CHDRV_OSD_ShowRegion(grh_BmpDisplayHandle1);

#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_VOUT_FILE_HD"));
#else
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_VOUT_FILE_HD"));
#endif
	
	
}
#if 0 	/*sqzou20170714*/

#endif

CASE_F(VOUT_VOUT_SETALPHA,
	"����CHDRV_VOUT_SetLayerAlpha���ò�ͬ��͸�����ܹ��۲�ͼ��͸���ȱ仯������������仯����ȷ���õĲ������ᱻ�ı䣻GetLayerAlpha�ܹ���ȷ��ȡͼ��͸����") {
	U8  ruc_LayerAlphaGet ;
	U8     uc_LayerAlpha ;
	CH_LAYER_e renm_Layer = CH_LAYER_OSD1;
	
	//EXPECT_EQ(CHDRV_VOUT_INVALID_PARAM,CHDRV_VOUT_SetLayerAlpha(renm_Layer,uc_LayerAlpha));
	PROMPT("��׼���ù۲�OSD͸���ȱ仯,alpha��0�𲽱仯��100,��OK������...");
	test_input_none();

	CH_RTOS_TaskDelay(200);

	for(uc_LayerAlpha = 0;uc_LayerAlpha <= 100;uc_LayerAlpha += 20 )
	{
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerAlpha(renm_Layer,uc_LayerAlpha));
		EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_GetLayerAlpha(renm_Layer,&ruc_LayerAlphaGet));
		EXPECT_EQ(uc_LayerAlpha,ruc_LayerAlphaGet);
		CH_RTOS_TaskDelay(200);
	}
	PROMPT("osd͸���ȱ仯�Ƿ�����?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_SetLayerAlpha��Ч��";
	
}
#endif
#if 0 	/*sqzou20170706*/
CASE_F(VOUT_VOUT_CAPTURE2OSD,
	"����CHDRV_VOUT_Capture2OSD����Ƶͼ��׽��ʾ��osdָ�����꣬��ͬ�����ܹ��������Ա仯�����������������Ƿ���ȷ����") {
	S16 				   rs_DstStartX = 0 ;
	S16		    		   rs_DstStartY = 0 ;
	CH_LAYER_e 			  renm_SrcLayer = CH_LAYER_VIDEO1;
	CH_Rect_t  			  rstru_SrcArea ={VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT,VOUT_PARAM_RIGHT};
	
	//����osd1ͼ��ok
	grstru_LayerEnable.b_Osd1 	= CH_TRUE;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_SetLayerDisplay(grh_VideoOutputHandle,&grstru_LayerEnable));

	
	CHDRV_OSD_RegionHandle rh_OSDHandle = grh_BmpDrawHandle1 ;
	PROMPT("OSD����,��(%d,%d,%d,%d)����(%d,%d),��OK������...", 
		rstru_SrcArea.s_X, rstru_SrcArea.s_Y, rstru_SrcArea.s_Width, rstru_SrcArea.s_Height, rs_DstStartX, rs_DstStartY);
	test_input_none();
	rs_DstStartX = 0;
	rs_DstStartY = 0;
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_Capture2OSD(renm_SrcLayer,rstru_SrcArea,rh_OSDHandle,rs_DstStartX,rs_DstStartY));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(rh_OSDHandle));
	PROMPT("osd�����Ƿ�ɹ�?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_Capture2OSD��Ч��";
	
	rs_DstStartX = 0;
	rs_DstStartY = VOUT_PARAM_RIGHT;
	PROMPT("OSD����,��(%d,%d,%d,%d)����(%d,%d),��OK������...", 
		rstru_SrcArea.s_X, rstru_SrcArea.s_Y, rstru_SrcArea.s_Width, rstru_SrcArea.s_Height, rs_DstStartX, rs_DstStartY);
	test_input_none();
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_Capture2OSD(renm_SrcLayer,rstru_SrcArea,rh_OSDHandle,rs_DstStartX,rs_DstStartY));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(rh_OSDHandle));
	PROMPT("osd�����Ƿ�ɹ�?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_Capture2OSD��Ч��";
	
	rs_DstStartX = VOUT_PARAM_RIGHT;
	rs_DstStartY = 0;
	PROMPT("OSD����,��(%d,%d,%d,%d)����(%d,%d),��OK������...", 
		rstru_SrcArea.s_X, rstru_SrcArea.s_Y, rstru_SrcArea.s_Width, rstru_SrcArea.s_Height, rs_DstStartX, rs_DstStartY);
	test_input_none();
	EXPECT_EQ(CHDRV_VOUT_OK,CHDRV_VOUT_Capture2OSD(renm_SrcLayer,rstru_SrcArea,rh_OSDHandle,rs_DstStartX,rs_DstStartY));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(rh_OSDHandle));
	PROMPT("osd�����Ƿ�ɹ�?��������Y����������N");
	EXPECT_EQ(CH_TRUE,test_input_yes()) << "CHDRV_VOUT_Capture2OSD��Ч��";
}
#endif

static int vout_open(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_PathHandle handle = NULL;
	CHDRV_VOUT_LayerEnable_t stru_LayerEnable;

	if(vout_GetHandleNums() > argv[1].get_i() )
	{
		LOG("error:  handle no=%d was open yet\n", argv[1].get_i() );
		return 0;
	}
	stru_LayerEnable.b_Osd1 = argv[2].get_i();				///< OSD1�Ƿ���ʾ
	stru_LayerEnable.b_Osd2 = argv[3].get_i();				///< OSD2�Ƿ���ʾ
	stru_LayerEnable.b_Video1 = argv[4].get_i();			///< 1����Ƶ���Ƿ���ʾ
	stru_LayerEnable.b_Video2 = argv[5].get_i();			///< 2����Ƶ���Ƿ���ʾ
	stru_LayerEnable.b_Background = argv[6].get_i();		///< �������Ƿ���ʾ
	stru_LayerEnable.b_Still = argv[7].get_i();			///< ��̬���Ƿ���ʾ
	stru_LayerEnable.b_Cursor = argv[8].get_i();			///< �����Ƿ���ʾ

	ret = CHDRV_VOUT_Open(argv[1].get_i(), &stru_LayerEnable, &handle);
	if(ret != CHDRV_VOUT_OK)
	{
		LOG("error: CHDRV_VOUT_Open return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vout_close(int argc, CMD_PARAM argv[])
{
	int ret;

	if(argv[1].get_i() >= vout_GetHandleNums())
	{
		LOG("error: please open vout first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	
	ret = CHDRV_VOUT_Close(vout_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VOUT_OK)
	{
		LOG("error: CHDRV_VOUT_Open return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vout_set_layer_display(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_LayerEnable_t stru_LayerEnable;

	if(argv[1].get_i() >= vout_GetHandleNums())
	{
		LOG("error: please open vout first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	
	
	stru_LayerEnable.b_Osd1 = argv[2].get_i();				///< OSD1�Ƿ���ʾ
	stru_LayerEnable.b_Osd2 = argv[3].get_i();				///< OSD2�Ƿ���ʾ
	stru_LayerEnable.b_Video1 = argv[4].get_i();			///< 1����Ƶ���Ƿ���ʾ
	stru_LayerEnable.b_Video2 = argv[5].get_i();			///< 2����Ƶ���Ƿ���ʾ
	stru_LayerEnable.b_Background = argv[6].get_i();		///< �������Ƿ���ʾ
	stru_LayerEnable.b_Still = argv[7].get_i();			///< ��̬���Ƿ���ʾ
	stru_LayerEnable.b_Cursor = argv[8].get_i();			///< �����Ƿ���ʾ

	ret = CHDRV_VOUT_SetLayerDisplay(vout_GetHandle(argv[1].get_i()), &stru_LayerEnable);
	if(ret != CHDRV_VOUT_OK)
	{
		LOG("error: CHDRV_VOUT_SetLayerDisplay return %d!\n", ret);
		return -1;
	}
	return 0;
}


static int vout_get_layer_display(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_LayerEnable_t stru_LayerEnable;



	for(int i = 0; i < vout_GetHandleNums(); i++)
	{
		ret = CHDRV_VOUT_GetLayerDisplay(vout_GetHandle(i), &stru_LayerEnable);
		if(ret != CHDRV_VOUT_OK)
		{
			LOG("error: CHDRV_VOUT_SetLayerDisplay return %d!\n", ret);
			return -1;
		}
		LOG("vout %d: b_Osd1=%d,b_Osd2=%d,b_Video1=%d,b_Video2=%d,b_Background=%d,b_Still=%d,b_Cursor=%d\n",
			i,
			stru_LayerEnable.b_Osd1,
			stru_LayerEnable.b_Osd2,
			stru_LayerEnable.b_Video1,
			stru_LayerEnable.b_Video2,
			stru_LayerEnable.b_Background,
			stru_LayerEnable.b_Still,
			stru_LayerEnable.b_Cursor);
	}
	return 0;
}

static int vout_set_port(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_Port_t stru_port;

	stru_port.b_IsSDCvbs=argv[1].get_i();	
	stru_port.b_IsYCrCb=argv[2].get_i();		
	stru_port.b_IsSDYC=argv[3].get_i();		
	stru_port.b_IsVGA=argv[4].get_i();		
	stru_port.b_IsYPbPr=argv[5].get_i();		
	stru_port.b_IsRGB=argv[6].get_i();		
	stru_port.b_IsDVI=argv[7].get_i();		
	stru_port.b_IsHDMI=argv[8].get_i();		
	stru_port.b_IsBT656=argv[10].get_i();		
	

	ret = CHDRV_VOUT_SetPort(&stru_port);
	if(ret != CHDRV_VOUT_OK)
	{
		LOG("error: CHDRV_VOUT_SetPort return %d!\n", ret);
		return -1;
	}
	return 0;
}


static int vout_set_format(int argc, CMD_PARAM argv[])
{
	int ret, i;

	if(vout_GetHandleNums() > 0)
	{
		for(i = 0; i < vout_GetHandleNums() ; i++)
		{
			ret = CHDRV_VOUT_SetFormat(vout_GetHandle(i), (CHDRV_VOUT_FORMAT_e) argv[1].get_i());
			if(ret != CHDRV_VOUT_OK)
			{
				LOG("error: CHDRV_VOUT_SetFormat return %d!\n", ret);
				return -1;
			}
		}
	}
	else
	{
		LOG("error: please create vout first!\n");
		return -1;
	}
	return 0;
}

REGISTER_CMD(vout_open, "����vout���", "path_id|show_osd1|show_osd2|show_video1|show_video2|show_background|show_still|show_cursor",  "0 1 1 1 0 0 0 0");
REGISTER_CMD(vout_close, "ɾ��vout���", "path_id",  "0");
REGISTER_CMD(vout_set_layer_display, "����ĳ�����Ƿ�ɼ�", "path_id|show_osd1|show_osd2|show_video1|show_video2|show_background|show_still|show_cursor",  "0 1 1 1 0 0 0 0");
REGISTER_CMD(vout_get_layer_display, "��ȡ���в�Ŀɼ�����", "",  "");
REGISTER_CMD(vout_set_port, "���ö˿��Ƿ��", "b_Cvbs|b_YCrCb|b_Yc|bVGA|bYpbPr|bRGB|bDVI|bHDMI|bBT656",  "1 0 0 0 0 0 0 1 0");
REGISTER_CMD(vout_set_format, "����HDMI����ֱ���", "format",  "6");


