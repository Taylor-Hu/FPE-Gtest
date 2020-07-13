/**
@file test_perfo.cpp
@def group performance性能测试集

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
#define CASE_NAME TestPerformance



#include "test_menu.h"
#include "test_modules.h"
#include "math.h"

#define TimeInMillis int
#define PI 3.1415926535
#define RADIAN(angle) ((angle) * PI /180.0)

#define MAX_OF_GLOBULE			5
#define NUM_IMPACT				5

#define XSTEP				1		/*横坐标移动步长*/
#define YSTEP   				1		/*纵坐标移动步长*/
#define XSTART				40
#define YSTART				20
#define WEIGHT_SCREEN    		(fw_getenv_i("OSD_REGION_WIDTH") - XSTART * 2)
#define HIGHT_SCREEN  			(fw_getenv_i("OSD_REGION_HEIGHT") - YSTART * 2)

 typedef struct
{
	int  			mi_startx;
	int  			mi_starty;
	CH_BOOL	mb_xplus;
	CH_BOOL 	mb_yplus;
	int 			mi_pic_count;
	int  			mi_weight[4];
	int	 		mi_height[4];
}ImpactINFO_t;

 typedef struct
{
	ImpactINFO_t  mt_impact[NUM_IMPACT];
	char*  		mstr_pic_data[4];
	U8			*mu_back[NUM_IMPACT];
}Impact_t;

typedef struct
{
	CH_BOOL	mb_xstation[NUM_IMPACT];
	CH_BOOL 	mb_ystation[NUM_IMPACT];
}Impact_Station_t;

static CH_BOOL gp_beenatack[NUM_IMPACT];
static char *gpc_ReadBuffer = NULL;

using namespace testing;
using namespace internal;

static CH_RTOS_SemaphoreHandle gh_SemIframeCome = NULL;

#if defined(CFG_CHSSA_CHIP_MSTAR5C69) || defined(CFG_CHSSA_CHIP_MSTAR5C58) || defined(CFG_CHSSA_CHIP_MSTAR7C51D) ||defined(CFG_CHSSA_CHIP_MSTAR7C123D)
#define TimeInMillis int
#undef NULL
#define NULL (void*)0
#endif


static void ChannelVideoCallback(CHDRV_VIDEO_Handle rh_Handle ,
					CHDRV_VIDEO_EVENTTYPE_e renm_EventType,
					void * rp_ExtendData)
{
	if(renm_EventType == CHDRV_VIDEO_EVENT_DECODED)
	{
		if(gh_SemIframeCome)
		{
			CH_RTOS_SignalSemaphore(gh_SemIframeCome);
		}
	}
}

#if ENABLE_TESTCASE

static void Rotation(U8 *pSrc, int sw, int sh, U8* pDest, int dw, int dh, int angle)
{
    int bitCount = 32;
    int srcW = sw;
    int srcH = sh;
    int lineSize = bitCount * srcW / 8;
    U8* srcBuf = pSrc;
    int i, j;

    CH_Rect_t pLT, pRT, pLB, pRB;
    pLT.s_X = -srcW / 2;
    pLT.s_Y = srcH / 2;
    pRT.s_X = srcW / 2;
    pRT.s_Y = srcH / 2;
    pLB.s_X = -srcW / 2;
    pLB.s_Y = -srcH / 2;
    pRB.s_X = srcW / 2; 
    pRB.s_Y = -srcH / 2;
    
    CH_Rect_t pLTN, pRTN, pLBN, pRBN;
    double sina = sin(RADIAN(angle));
    double cosa = cos(RADIAN(angle));
    pLTN.s_X = pLT.s_X * cosa + pLT.s_Y * sina;    
    pLTN.s_Y = -pLT.s_X * sina + pLT.s_Y * cosa;
    pRTN.s_X = pRT.s_X * cosa + pRT.s_Y * sina;
    pRTN.s_Y = -pRT.s_X * sina + pRT.s_Y * cosa;
    pLBN.s_X = pLB.s_X * cosa + pLB.s_Y * sina;
    pLBN.s_Y = -pLB.s_X * sina + pLB.s_Y * cosa;
    pRBN.s_X = pRB.s_X * cosa + pRB.s_Y * sina;
    pRBN.s_Y = -pRB.s_X * sina + pRB.s_Y * cosa;
    int desWidth = MAX(ABS(pRBN.s_X - pLTN.s_X), ABS(pRTN.s_X - pLBN.s_X));
    int desHeight = MAX(ABS(pRBN.s_Y - pLTN.s_Y), ABS(pRTN.s_Y - pLBN.s_Y));
    U8 *desBuf = pDest;
    int dx = (dw - desWidth) / 2;
    int dy = (dh - desHeight) / 2;

    int desLineSize = ((dw * bitCount + 31) / 32) * 4;        
    int tX, tY, tXN, tYN;
    for (i = 0; i < desHeight; i++)
    {        
        for (j = 0; j < desWidth; j++)
        {
            tX = (j - desWidth / 2) * cos(RADIAN(360 - angle)) + (-i + desHeight / 2) * sin(RADIAN(360 - angle));
            tY = -(j - desWidth / 2) * sin(RADIAN(360 - angle)) + (-i + desHeight / 2) * cos(RADIAN(360 - angle));
            if (tX > srcW / 2 || tX < -srcW / 2 || tY > srcH / 2 || tY < -srcH / 2)
            {
                continue;
            }
            tXN = tX + srcW / 2; 
            tYN = ABS(tY - srcH / 2);
            if(i + dy >= 0 && i + dy < dh && j + dx >= 0 && j + dx < dw)
            {
	            memcpy(&desBuf[(i + dy) * desLineSize + (j + dx) * bitCount / 8], &srcBuf[tYN * lineSize + tXN * bitCount / 8], 4);       
            }
        }
    }
}


static void test_scale_pic(U32 &time)
{
	U32 use_time = 0;
	char* psrc = NULL, *pDest = NULL;
	int len, i, sw, sh, dw, dh;

	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, 0);

	len = fsys_read_file((const char *)fw_getenv("OSD_SCALE_TEST"), psrc);
	ASSERT_GT(len, 0) << "读取文件" << fw_getenv("OSD_SCALE_TEST") << "失败";

	CHDRV_OSD_RegionAttrib_t stru_RegionAttrib;
	ASSERT_EQ(CHDRV_OSD_OK, CHDRV_OSD_GetRegionAttrib(grh_DrawHandleOsd1, &stru_RegionAttrib));
	ASSERT_EQ(CHDRV_OSD_OK, CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,
			(OSD_ENV_WIDTH - fw_getenv_i("OSD_SCALE_WIDTH")) / 2, 
			(OSD_ENV_HEIGHT - fw_getenv_i("OSD_SCALE_HEIGHT")) / 2,
			fw_getenv_i("OSD_SCALE_WIDTH"), 
			fw_getenv_i("OSD_SCALE_HEIGHT"), 
			(U8*)psrc));
	ASSERT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));


	pDest =(char*) stru_RegionAttrib.puc_RegionData;
	sw = fw_getenv_i("OSD_SCALE_WIDTH");
	sh = fw_getenv_i("OSD_SCALE_HEIGHT");
	dw = OSD_ENV_WIDTH;
	dh = OSD_ENV_HEIGHT;
	LOG("start scale\n");
	use_time = CH_RTOS_GetSysTimenow();
	for(i = 0; i < 36 * 4; i++)
	{
		ASSERT_EQ(CHDRV_OSD_OK, CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1, 0));

		Rotation((U8*)psrc, 
			sw, 
			sh,	
			(U8*)pDest, 
			dw, 
			dh, 
			(i * 10 + 10) % 360);
		ASSERT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));
	}
	use_time = CH_RTOS_GetSysTimenow() - use_time;
	CH_RTOS_FreeMemory(gh_APP_HEAP, psrc);

	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1, 0));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	time = use_time;
}


static ImpactINFO_t *CheckOneStation(ImpactINFO_t t_globule,
													int i_stepx,int i_stepy)
{
	int 	i_xp=XSTART ,i_yp =YSTART;
	CH_BOOL b_xplus = CH_TRUE,b_yplus = CH_TRUE;
	ImpactINFO_t *t_Temp_globule = NULL;
	int   i_screenH = 0;

	i_screenH = HIGHT_SCREEN;
	t_Temp_globule = (ImpactINFO_t*)malloc(sizeof(ImpactINFO_t));
	if(NULL != t_Temp_globule)
	{
		memset(t_Temp_globule,0,sizeof(ImpactINFO_t));
	}
	else
	{
		LOG ( ">>[CheckOneStation]=erro memory malloc\n");
		return NULL;
	}
	b_xplus		= t_globule.mb_xplus;
	b_yplus 	= t_globule.mb_yplus;
	i_xp 		= t_globule.mi_startx;
	i_yp		= t_globule.mi_starty;
	if(i_xp >= WEIGHT_SCREEN-t_globule.mi_weight[t_globule.mi_pic_count])
	{
		b_xplus = CH_FALSE;
	}
	else if(i_xp <= XSTART)
	{
		b_xplus = CH_TRUE;
	}
	if(i_yp >= i_screenH-t_globule.mi_height[t_globule.mi_pic_count])
	{
		b_yplus = CH_FALSE;
	}
	else if(i_yp <= YSTART)
	{
		b_yplus = CH_TRUE;
	}
	//LOG("i_xp=%d,i_yp=%d,WEIGHT_SCREEN=%d,HIGHT_SCREEN=%d,mi_weight=%d,mi_height=%d\n",i_xp,  i_yp, WEIGHT_SCREEN, HIGHT_SCREEN,t_globule.mi_weight, t_globule.mi_height);
	if(b_xplus)
	{
		i_xp += i_stepx;
	}
	else
	{
		i_xp -= i_stepx;
	}
	if(b_yplus)
	{
		i_yp +=i_stepy;
	}
	else
	{
		i_yp -=i_stepy;
	}
	 t_Temp_globule->mi_startx = i_xp;
	 t_Temp_globule->mi_starty = i_yp;
	 if(t_globule.mb_xplus != b_xplus || t_globule.mb_yplus != b_yplus)
	 {
		t_Temp_globule->mi_pic_count = (t_globule.mi_pic_count + 1) % 4;
	 }
	 else
	 {
		t_Temp_globule->mi_pic_count = t_globule.mi_pic_count;
	 }
	 t_Temp_globule->mb_xplus = b_xplus;
	 t_Temp_globule->mb_yplus = b_yplus;
	 memcpy(&t_Temp_globule->mi_weight[0], &t_globule.mi_weight[0], sizeof(t_globule.mi_weight));	 
	 memcpy(&t_Temp_globule->mi_height[0], &t_globule.mi_height[0], sizeof(t_globule.mi_height));

	 return t_Temp_globule;
}

static void CheckStation(Impact_t *t_globule)
{
	int i;
	ImpactINFO_t *t_tempImpact = NULL;	

	for(i=0;i<NUM_IMPACT;i++)
	{
		if(t_globule->mu_back[i] == NULL)
		{
			t_globule->mu_back[i] = (U8*)1;
		}
		else
		{
			CHDRV_OSD_ClearArea(grh_DrawHandleOsd1,
				t_globule->mt_impact[i].mi_startx, 
				t_globule->mt_impact[i].mi_starty,
				t_globule->mt_impact[i].mi_weight[t_globule->mt_impact[i].mi_pic_count], 
				t_globule->mt_impact[i].mi_height[t_globule->mt_impact[i].mi_pic_count],
				0x0);
		}
		t_tempImpact = CheckOneStation(t_globule->mt_impact[i],XSTEP,YSTEP);
		if(t_tempImpact !=NULL)
		{
			t_globule->mt_impact[i].mb_xplus= t_tempImpact->mb_xplus;
			t_globule->mt_impact[i].mb_yplus= t_tempImpact->mb_yplus;
			t_globule->mt_impact[i].mi_startx= t_tempImpact->mi_startx;
			t_globule->mt_impact[i].mi_starty= t_tempImpact->mi_starty;
		
			CHDRV_OSD_PutAreaData(grh_DrawHandleOsd1,
					t_globule->mt_impact[i].mi_startx,
					t_globule->mt_impact[i].mi_starty,
					t_globule->mt_impact[i].mi_weight[t_tempImpact->mi_pic_count], 
					t_globule->mt_impact[i].mi_height[t_tempImpact->mi_pic_count],
					(U8*)t_globule->mstr_pic_data[t_tempImpact->mi_pic_count]);
			t_globule->mt_impact[i].mi_pic_count = t_tempImpact->mi_pic_count;
			free(t_tempImpact);
			t_tempImpact = NULL;
		}		
	}
}

/***************************************
l两个小球相撞时处理函数
void  CHAPP_AUDIOCheckAtack(int Startx1,int Starty1,int Startx2,int Starty2,
									int   Weight1,int Height1,int Weight2,int Height2,
									CH_BOOL *bx_plus1,CH_BOOL *by_plus1,
									CH_BOOL *bx_plus2,CH_BOOL *by_plus2)
输入:各自的位置与图片大小，路径参数；
相撞后更改路径；

*****************************/
static Impact_Station_t *CheckTwoStack(ImpactINFO_t      t_impact_globule1,ImpactINFO_t t_impact_globule2)
{
	CH_BOOL bx_plus1,by_plus1;
	CH_BOOL bx_plus2,by_plus2;
	int i_distancex=0,i_distancey=0;
	Impact_Station_t *t_temp_station=NULL;	

	t_temp_station = (Impact_Station_t*)malloc (sizeof(Impact_Station_t));
	if(NULL != t_temp_station)
	{
		memset(t_temp_station,0,sizeof(Impact_Station_t));
	}
	else
	{
		LOG ( ">>[CheckTwoStack]=erro memory malloc\n");
		return NULL;
	}
	bx_plus1 = t_impact_globule1.mb_xplus;
	by_plus1 = t_impact_globule1.mb_yplus;
	bx_plus2 = t_impact_globule2.mb_xplus;
	by_plus2 = t_impact_globule2.mb_yplus;
	
	if(t_impact_globule1.mi_startx > t_impact_globule2.mi_startx)
	{
		i_distancex = t_impact_globule1.mi_startx-t_impact_globule2.mi_startx -t_impact_globule2.mi_weight[t_impact_globule2.mi_pic_count];
	}
	else
	{
		i_distancex = t_impact_globule2.mi_startx-t_impact_globule1.mi_startx-t_impact_globule1.mi_weight[t_impact_globule1.mi_pic_count];
	}
	if(t_impact_globule1.mi_starty > t_impact_globule2.mi_starty)
	{
		i_distancey = t_impact_globule1.mi_starty-t_impact_globule2.mi_starty-t_impact_globule2.mi_height[t_impact_globule2.mi_pic_count];
	}
	else
	{
		i_distancey = t_impact_globule2.mi_starty-t_impact_globule1.mi_starty-t_impact_globule1.mi_height[t_impact_globule1.mi_pic_count];
	}
	i_distancex -=XSTEP;
	i_distancey -= YSTEP;
	if(0<= i_distancex||0<= i_distancey)
	{
		free( t_temp_station);
		t_temp_station = NULL;	
		return NULL;
	}
	i_distancex = 0- i_distancex;
	i_distancey = 0-i_distancey;
	/*x ,y轴谁剩下的距离长就是在先在该轴上相遇，则实际碰撞后另外一个轴就反向*/
	if((float)(i_distancey /XSTEP)> (float)(i_distancex /YSTEP) )
	{
		bx_plus1 =bx_plus1^1;
		bx_plus2 = bx_plus2^1;
		if(bx_plus1==bx_plus2)
		{
			if(t_impact_globule1.mi_startx > t_impact_globule2.mi_startx && bx_plus1==0)
			{
				bx_plus1 = bx_plus1^1;
			}
			else
			{				
				bx_plus2 = bx_plus2^1;
			}
		}			
	}
	else
	{
		by_plus1 =by_plus1^1;
		by_plus2 = by_plus2^1;
		if(by_plus1== by_plus2)
		{ 
			if(t_impact_globule1.mi_starty > t_impact_globule2.mi_starty && by_plus1==0)
			{
				by_plus1 = by_plus1^1;
			}
			else
			{				
				by_plus2 = by_plus2^1;
			}
		}	
	}
	t_temp_station->mb_xstation[1] = bx_plus1;
	t_temp_station->mb_ystation[1] = by_plus1;
	t_temp_station->mb_xstation[2] = bx_plus2;
	t_temp_station->mb_ystation[2] = by_plus2;

	return (t_temp_station);
}


static void CheckAtack(Impact_t * t_Globule)
{
	int i_index,i_index2=NUM_IMPACT;
	Impact_Station_t *T_temp_station = NULL;
	static CH_BOOL i_been=CH_FALSE;

	for(i_index=0;i_index<NUM_IMPACT;i_index++)
	{
		i_index2 = i_index+1;
		while(NUM_IMPACT>i_index2)
		{
			T_temp_station = CheckTwoStack(t_Globule->mt_impact[i_index],t_Globule->mt_impact[i_index2]);
			if(T_temp_station !=NULL)
			{
				if(CH_TRUE==gp_beenatack[i_index] && CH_TRUE== gp_beenatack[i_index2])
				{	
					//printf("[CHAPP-Impact->>>]-one= [%d],another = [%d]\n",i_index,i_index2);
					if(i_been == CH_FALSE)/*特殊情况全部反向*/
					{
						t_Globule->mt_impact[i_index].mb_xplus = t_Globule->mt_impact[i_index].mb_xplus^1;
						t_Globule->mt_impact[i_index2].mb_xplus = t_Globule->mt_impact[i_index2].mb_xplus^1;
						t_Globule->mt_impact[i_index].mb_yplus = t_Globule->mt_impact[i_index].mb_yplus^1;
						t_Globule->mt_impact[i_index2].mb_yplus = t_Globule->mt_impact[i_index2].mb_yplus^1;
						i_been= CH_TRUE;
					}
					free(T_temp_station);
					T_temp_station = NULL;					
					i_index2++;
					continue;
				}
				t_Globule->mt_impact[i_index].mb_xplus	= T_temp_station->mb_xstation[1];
				t_Globule->mt_impact[i_index].mb_yplus	= T_temp_station->mb_ystation[1];
				t_Globule->mt_impact[i_index2].mb_xplus	= T_temp_station->mb_xstation[2];
				t_Globule->mt_impact[i_index2].mb_yplus	= T_temp_station->mb_ystation[2];				
				gp_beenatack[i_index] =CH_TRUE;
				gp_beenatack[i_index2] = CH_TRUE; 
				free(T_temp_station);
				T_temp_station = NULL;
			}				
			else
			{
				if(CH_TRUE == gp_beenatack[i_index]&& CH_TRUE == gp_beenatack[i_index2])
				{
					gp_beenatack[i_index] =CH_FALSE;
					gp_beenatack[i_index2] = CH_FALSE; 
					i_been=CH_FALSE;
				}
			}
			i_index2++;
		}
	}
}


static void put_multi_pic(U32 &time)
{
	U32 use_time = 0;
	int i;
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, 0);

#if 0 	/*生成原始图像 sqzou20171121*/
	len = fsys_read_file((const char *)("c:/resource/fish1.png"), psrc);
	ASSERT_GT(len, 0) << "读取文件" << "c:/resource/fish1.png" << "失败";

	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic("c:/resource/fish1.png", 
		"c:/resource/fish1.152X124.argb", 
		152, 
		124, 
		&use_time)) << "图片解码失败, filename=" << "c:/resource/fish1.png";
	CH_RTOS_FreeMemory(gh_APP_HEAP, psrc);

	len = fsys_read_file((const char *)("c:/resource/fish2.png"), psrc);
	ASSERT_GT(len, 0) << "读取文件" << "c:/resource/fish21.png" << "失败";

	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic("c:/resource/fish2.png", 
		"c:/resource/fish2.164X120.argb", 
		164, 
		120 , &use_time)) << "图片解码失败, filename=" << "c:/resource/fish2.png";
	CH_RTOS_FreeMemory(gh_APP_HEAP, psrc);
	
	len = fsys_read_file((const char *)("c:/resource/fish3.png"), psrc);
	ASSERT_GT(len, 0) << "读取文件" << "c:/resource/fish3.png" << "失败";

	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic("c:/resource/fish3.png", 
		"c:/resource/fish3.168X117.argb", 
		168, 
		117 , &use_time)) << "图片解码失败, filename=" << "c:/resource/fish3.png";
	CH_RTOS_FreeMemory(gh_APP_HEAP, psrc);
	
	len = fsys_read_file((const char *)("c:/resource/fish4.png"), psrc);
	ASSERT_GT(len, 0) << "读取文件" << "c:/resource/fish4.png" << "失败";

	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic("c:/resource/fish4.png", 
		"c:/resource/fish4.171X125.argb", 
		171, 
		125, &use_time)) << "图片解码失败, filename=" << "c:/resource/fish4.png";
	CH_RTOS_FreeMemory(gh_APP_HEAP, psrc);
#endif

	Impact_t t_globule;

	memset(&t_globule, 0, sizeof(t_globule));
	ASSERT_GT(fsys_read_file(fw_getenv("OSD_UPDATE_PIC1"), t_globule.mstr_pic_data[0]), 0);
	ASSERT_GT(fsys_read_file(fw_getenv("OSD_UPDATE_PIC2"), t_globule.mstr_pic_data[1]), 0);
	ASSERT_GT(fsys_read_file(fw_getenv("OSD_UPDATE_PIC3"), t_globule.mstr_pic_data[2]), 0);
	ASSERT_GT(fsys_read_file(fw_getenv("OSD_UPDATE_PIC4"), t_globule.mstr_pic_data[3]), 0);
	
	t_globule.mt_impact[0].mi_startx = XSTART;
	t_globule.mt_impact[0].mi_starty = YSTART;
	t_globule.mt_impact[0].mb_xplus = CH_TRUE;
	t_globule.mt_impact[0].mb_yplus = CH_FALSE;
	
	t_globule.mt_impact[0].mi_weight[0] = fw_getenv_i("OSD_PUDATE_PIC1_WIDTH");
	t_globule.mt_impact[0].mi_height[0] = fw_getenv_i("OSD_PUDATE_PIC1_HEIGHT");
	t_globule.mt_impact[0].mi_weight[1] = fw_getenv_i("OSD_PUDATE_PIC2_WIDTH");
	t_globule.mt_impact[0].mi_height[1] = fw_getenv_i("OSD_PUDATE_PIC2_HEIGHT");
	t_globule.mt_impact[0].mi_weight[2]= fw_getenv_i("OSD_PUDATE_PIC3_WIDTH");
	t_globule.mt_impact[0].mi_height[2] = fw_getenv_i("OSD_PUDATE_PIC3_HEIGHT");
	t_globule.mt_impact[0].mi_weight[3] = fw_getenv_i("OSD_PUDATE_PIC4_WIDTH");
	t_globule.mt_impact[0].mi_height[3] = fw_getenv_i("OSD_PUDATE_PIC4_HEIGHT");
	
	t_globule.mt_impact[1]= t_globule.mt_impact[0];
	t_globule.mt_impact[2]= t_globule.mt_impact[0];
	t_globule.mt_impact[3]= t_globule.mt_impact[0];
	t_globule.mt_impact[4]= t_globule.mt_impact[0];
	
	t_globule.mt_impact[1].mi_startx = WEIGHT_SCREEN - 100;
	t_globule.mt_impact[1].mi_starty = HIGHT_SCREEN - 100;
	t_globule.mt_impact[1].mb_xplus = CH_FALSE;
	t_globule.mt_impact[1].mb_yplus = CH_TRUE;

	t_globule.mt_impact[2].mi_startx = WEIGHT_SCREEN / 2;
	t_globule.mt_impact[2].mi_starty = HIGHT_SCREEN / 2;
	t_globule.mt_impact[2].mb_xplus = CH_FALSE;
	t_globule.mt_impact[2].mb_yplus = CH_FALSE;


	t_globule.mt_impact[3].mi_startx = WEIGHT_SCREEN / 2;;
	t_globule.mt_impact[3].mi_starty = YSTART;
	t_globule.mt_impact[3].mb_xplus = CH_TRUE;
	t_globule.mt_impact[3].mb_yplus = CH_TRUE;

	t_globule.mt_impact[4].mi_startx = 0;
	t_globule.mt_impact[4].mi_starty = HIGHT_SCREEN / 2 - 100;
	t_globule.mt_impact[4].mb_xplus = CH_TRUE;
	t_globule.mt_impact[4].mb_yplus = CH_FALSE;
	
	
	memset(&gp_beenatack, 0, sizeof(gp_beenatack));
	use_time = CH_RTOS_GetSysTimenow();
	for(i = 0; i < 5000; i++)
	{
		CheckStation(&t_globule);
		CHDRV_OSD_UpdateRegion( grh_DrawHandleOsd1);
		CheckAtack(&t_globule);
	}
	use_time = CH_RTOS_GetSysTimenow() - use_time;

	CH_RTOS_FreeMemory(gh_APP_HEAP, t_globule.mstr_pic_data[0]);
	CH_RTOS_FreeMemory(gh_APP_HEAP, t_globule.mstr_pic_data[1]);
	CH_RTOS_FreeMemory(gh_APP_HEAP, t_globule.mstr_pic_data[2]);
	CH_RTOS_FreeMemory(gh_APP_HEAP, t_globule.mstr_pic_data[3]);

	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_ClearFullRegion(grh_DrawHandleOsd1, 0));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(grh_DrawHandleOsd1));

	time = use_time;
	return;
	
}

static int FLASH_ReadSpeedTest(unsigned int offset, char *p1, int len, int &time)
{
	S32 ret, size = fw_getenv_i("FLASH_BLOCK_SIZE");
	CHDRV_FLASH_Handle h_FlashHandle = NULL;
	unsigned int time1, time2;
	
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), offset, len, &h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashOpen fail, addr=%#x,len=%#x, ret=%d!\n", offset, len, ret);
		return - 1;
	}

	time1 = CH_RTOS_GetSysTimenow();
	
	size = len;
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Read(h_FlashHandle, 0, (U8*)p1, &size));
	if(ret != CHDRV_FLASH_OK || size != len)
	{
		LOG("CHMID_NVM_FlashRead fail, addr=%#x, ret=%d!\n", offset, ret);
		return -1;
	}

	time2 = CH_RTOS_GetSysTimenow();
	time = time2 - time1;

	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Close(h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashClose fail ,ret=%d!\n", ret);
		return -1;
	}

	return 0;
}

static int FLASH_EraseSpeedTest(unsigned int offset, char *p1, int len, int &time)
{
	S32 ret, size = fw_getenv_i("FLASH_BLOCK_SIZE");
	CHDRV_FLASH_Handle h_FlashHandle = NULL;
	unsigned int time1, time2;
	
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), offset, len, &h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashOpen fail, addr=%#x,len=%#x, ret=%d!\n", offset, len, ret);
		return - 1;
	}

	time1 = CH_RTOS_GetSysTimenow();
	size = len;
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Erase(h_FlashHandle, 0, len));
	if(ret != CHDRV_FLASH_OK || size != len)
	{
		LOG("CHMID_NVM_FlashErase fail, addr=%#x, ret=%d!\n", offset, ret);
		CHDRV_FLASH_Close(h_FlashHandle);
		return -1;
	}
	time2 = CH_RTOS_GetSysTimenow();
	time = time2 - time1;
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Close(h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashClose fail ,ret=%d!\n", ret);
		return -1;
	}

	return 0;
}

static int FLASH_WriteSpeedTest(unsigned int offset, char *p1, int len, int &time)
{
	S32 ret, size = fw_getenv_i("FLASH_BLOCK_SIZE");
	CHDRV_FLASH_Handle h_FlashHandle = NULL;
	unsigned int time1, time2;
	
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), offset, len, &h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashOpen fail, addr=%#x,len=%#x, ret=%d!\n", offset, len, ret);
		return - 1;
	}

	time1 = CH_RTOS_GetSysTimenow();
	size = len;
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Write(h_FlashHandle, 0, (U8*)p1, &size));
	if(ret != CHDRV_FLASH_OK || size != len)
	{
		LOG("CHMID_NVM_FlashWrite fail, addr=%#x, ret=%d!\n", offset, ret);
		return -1;
	}

	time2 = CH_RTOS_GetSysTimenow();
	time = time2 - time1;

	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Close(h_FlashHandle));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHMID_NVM_FlashClose fail ,ret=%d!\n", ret);
		return -1;
	}

	return 0;
}


static int fsys_write_fileBlK(const char *file_path, char * file_buf, int buffer_len, int total_len)
{
	CH_FSYS_FileHandle hfile = 0;
	int ret = 1, len;
	S64 ul_Size;
	
	hfile = CH_FSYS_OpenFile((const S8 *)file_path,(const S8 *)"w+");
	if(hfile == 0)
	{
		LOG("ERROR: CH_FSYS_OpenFile %s fail\n", file_path);
		return 0;
	}
	CHS64_SetValue32(0, 0, &ul_Size);
	CH_FSYS_ResizeFile((CH_FSYS_FileHandle)hfile, ul_Size);
	for(len = 0 ; len < total_len && ret > 0; len += ret)
	{
		ret = CH_FSYS_WriteFile(hfile, file_buf, 1, buffer_len);
	}
	CH_FSYS_CloseFile(hfile);
	return len;
}

static int fsys_read_fileBlK(const char *file_path, char * file_buf, int buffer_len, int total_len)
{
	CH_FSYS_FileHandle hfile = 0;
	int ret = 1, len;
	
	hfile = CH_FSYS_OpenFile((const S8 *)file_path, (const S8 *)"r+");
	if(hfile == 0)
	{
		LOG("ERROR: CH_FSYS_OpenFile %s fail\n", file_path);
		return 0;
	}
	for(len = 0 ; len < total_len && ret > 0; len += ret)
	{
		ret = CH_FSYS_ReadFile(hfile, file_buf, 1, buffer_len);
	}
	CH_FSYS_CloseFile(hfile);
	return len;
}

static int channel_switch_test(int b_same_freq)
{
	
	int max_times = 20;
	

	Test_ChannelRecordTime(0);
	stop_av();

	Test_ChannelRecordTime(-1);

	int i, ret;


	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_SemIframeCome), CH_RTOS_OK);
	EXPECT_EQ(CHDRV_VIDEO_InstallCallback(video_GetHandle(0), ChannelVideoCallback), CHDRV_VIDEO_OK);
	for(i = 0; i < max_times; i++)
	{
		Test_ChannelRecordTime(TEST_KEY_START);
		
		while(CH_RTOS_WaitSemaphore(gh_SemIframeCome, 1) == CH_RTOS_OK);

		
		if(b_same_freq == 0)
		{
			RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
		}
		else
		{
			Test_ChannelRecordTime(TEST_SET_FREQ);
			Test_ChannelRecordTime(TEST_TUNER_LOCK);
		}

		start_av();
		EXPECT_EQ(ret = CH_RTOS_WaitSemaphore(gh_SemIframeCome, 10000), CH_RTOS_OK) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
		if(ret == CH_RTOS_OK)
		{
			CH_RTOS_TaskDelay(200);
		}
		else
		{
			stop_av();
			break;
		}
		if(b_same_freq == 0)
		{
			RUN_CMD("tuner.lock %s", fw_getenv("TUNER_INVALID_PARAMS1"));
		}		
		stop_av();
	}

	EXPECT_EQ(CHDRV_VIDEO_UnInstallCallback(video_GetHandle(0), ChannelVideoCallback), CHDRV_VIDEO_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(gh_SemIframeCome), CH_RTOS_OK);
	gh_SemIframeCome = NULL;
	RUN_CMD("av.stop %d\n",get_demux_id("TUNER_PARAMS1"));
	return 0;
}
void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
	CaseTime time;
	time.Clear();

	OsdDeleteRegion();
	grh_DrawHandleOsd1 = NULL;
	grh_DisplayHandleOsd1 = NULL;

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit pvrn rtos TestCase " << std::endl;
	CaseTime time;
	time.Clear();

	OsdDeleteRegion();
	grh_DrawHandleOsd1 = NULL;
	grh_DisplayHandleOsd1 = NULL;	

	if(gpc_ReadBuffer)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, gpc_ReadBuffer);
		gpc_ReadBuffer = NULL;
	}
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


#if 1
CASE_F(REBOOT_TIME_TEST1,
	"测试重启并记录时间"){

	int SUPPORT_TRUE_STANDY = fw_getenv_i("SUPPORT_TRUE_STANDBY");

	if(SUPPORT_TRUE_STANDY)
	{
	PROMPT("请在securCRT工具内Run脚本secureCRT_time.py,准备好以后按回车");
	test_input_none();

	LOG("get_PcRealTime()\n");
	char *p = test_get_input();
	LOG("%s\n", p);
	PROMPT("即将重启，回车继续...");
#if SUPPORT_TEST_BROKEN
	SetTestRestartFlag();
#endif /* SUPPORT_TEST_BROKEN */
	test_input_none();

	LOG("get_PcRealTime()\n");
	p = test_get_input();
	LOG("%s\n", p);
	RWLastTestTime(p, 1);
	EXPECT_EQ(CH_COM_OK, CH_COM_Reboot(CH_COM_WARM_REBOOT));
	}

}


CASE_F(REBOOT_TIME_TEST2,
	"计算重启耗费时间"){

	int SUPPORT_TRUE_STANDY = fw_getenv_i("SUPPORT_TRUE_STANDBY");

	if(SUPPORT_TRUE_STANDY)
	{
	LOG("get_PcRealTime()\n");
	char *p = test_get_input();
	LOG("%s\n", p);

	char time[32] = "";

	RWLastTestTime(time, 0);
	ASSERT_STRNE(time, "") << "重启未成功";
	int sec1, sec2;
	int h = 0, m = 0, s = 0;
	sscanf(time, "%d:%d:%d", &h, &m, &s);
	sec1 = h * 3600 + m * 60 + s;
	sscanf(p, "%d:%d:%d", &h, &m, &s);
	sec2 = h * 3600 + m * 60 + s;
	sec2 = (sec2 - sec1 - 1) * 1000;//减去保存时间时延时的1S
	
	LOG("重启耗时%ds sec=%d\n", sec2 / 1000, sec2);
	CaseTime case_time;
	case_time.Set("REBOOT_TIME_TEST2", sec2);//设置测试结果的时间

	PROMPT("测试完毕，现在可以停止运行脚本secureCRT_time.py了,准备好以后按回车");
	test_input_none();

	}
	
}
#endif

#if 1
CASE_F(JPEG_DECODER_TIME,
	"计算解码一幅1920X1080的JPEG图片所耗费的时间")
{
	int use_time = 0;
	int ret = 0;

	int prognum = 0;

#if CFG_CHSSA_HD_SUPPORT
	prognum = request_play_program("HD_CHANNEL_TEST", "TUNER_PARAMS1");
#else
	prognum = request_play_program("SD_CHANNEL_TEST", "TUNER_PARAMS1");
#endif /* CFG_CHSSA_HD_SUPPORT */
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"), prognum);
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic(fw_getenv("OSD_JPG_COMMON_FILE"), 
		NULL, OSD_ENV_WIDTH, OSD_ENV_HEIGHT, &use_time)) << "图片解码失败, filename=" << fw_getenv("OSD_JPG_COMMON_FILE");;
	CaseTime case_time;
	case_time.Set("JPEG_DECODER_TIME", use_time);//设置测试结果的时间
	if(ret == CHDRV_OSD_OK)
	{
		PROMPT("图片显示是否正常?正常输入Y，否则输入N");
		EXPECT_EQ(CH_TRUE, test_input_yes()) << "JPEG解码数据错误";
	}
}

CASE_F(GIF_DECODER_TIME,
	"计算解码一幅1920X1080的GIF图片所耗费的时间")
{
	U32 use_time = 0;
	int ret = 0;
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(0, ret = osd_decode_pic(fw_getenv("OSD_GIF_COMMON_FILE"), 
		NULL, OSD_ENV_WIDTH, OSD_ENV_HEIGHT, &use_time)) << "图片解码失败, filename=" << fw_getenv("OSD_GIF_COMMON_FILE");;
	CaseTime case_time;
	case_time.Set("GIF_DECODER_TIME", use_time);//设置测试结果的时间

	if(ret == CHDRV_OSD_OK)
	{
		PROMPT("图片显示是否正常?正常输入Y，否则输入N");
		EXPECT_EQ(CH_TRUE, test_input_yes()) << "GIF解码数据错误";
	}
}

CASE_F(PNG_DECODER_TIME,
	"计算解码一幅1920X1080的PNG图片所耗费的时间")
{
	U32 use_time = 0;
	int ret = 0;
	
	InitRegion(CHDRV_OSD_COLOR_TYPE_ARGB8888, &grh_DisplayHandleOsd1, &grh_DrawHandleOsd1, OSD_REGION_COLOR8888_WHITE);
	EXPECT_EQ(CHDRV_OSD_OK, ret = osd_decode_pic(fw_getenv("OSD_PNG_COMMON_FILE"), 
		NULL, OSD_ENV_WIDTH, OSD_ENV_HEIGHT, (int*)&use_time)) << "图片解码失败, filename=" << fw_getenv("OSD_PNG_COMMON_FILE");;
	CaseTime case_time;
	case_time.Set("PNG_DECODER_TIME", use_time);//设置测试结果的时间
	if(ret == CHDRV_OSD_OK)
	{
		PROMPT("图片显示是否正常?正常输入Y，否则输入N");
		EXPECT_EQ(CH_TRUE, test_input_yes()) << "PNG解码数据错误";
	}
}


CASE_F(PIC_SCALE_TIME1,
	"在播放节目的同时计算图片旋转4圈所耗费的时间")
{
	U32 use_time = 0;

	test_scale_pic(use_time);
	
	CaseTime case_time;
	case_time.Set("PIC_SCALE_TIME1", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);
}

CASE_F(PIC_SCALE_TIME2,
	"不播放节目时计算图片旋转4圈所耗费的时间")
{
	U32 use_time = 0;

	stop_av();
	test_scale_pic(use_time);
	
	CaseTime case_time;
	case_time.Set("PIC_SCALE_TIME2", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);
	start_av();
	CH_RTOS_TaskDelay(2000);
}




CASE_F(PIC_PUTEARA_TIME1,
	"在播放节目的同时计算输出图形5000次所耗费的时间")
{
	U32 use_time = 0;
	put_multi_pic(use_time);

	CaseTime case_time;
	case_time.Set("PIC_PUTEARA_TIME1", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);
}

CASE_F(PIC_PUTEARA_TIME2,
	"停止播放节目后计算输出图形5000次所耗费的时间")
{
	U32 use_time = 0;

	stop_av();
	put_multi_pic(use_time);

	CaseTime case_time;
	case_time.Set("PIC_PUTEARA_TIME2", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

	//start_av();
	//CH_RTOS_TaskDelay(2000);
}


CASE_F(FLASH_READ_TIME,
	"测试从FLASH读取1MB数据所耗费的时间")
{
	int offset = fw_getenv_i("FLASH_OFFSET_0");
	int size = fw_getenv_i("FLASH_LENGTH_0");
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, size), NULL);
	}
	ASSERT_EQ(FLASH_ReadSpeedTest(offset, gpc_ReadBuffer, size, use_time), 0);

	use_time = use_time * (size / 0x100000);
	
	CaseTime case_time;
	case_time.Set("FLASH_READ_TIME", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);
}

CASE_F(FLASH_ERASE_TIME,
	"测试从FLASH擦除1MB数据所耗费的时间")
{
	int offset = fw_getenv_i("FLASH_OFFSET_0");
	int size = fw_getenv_i("FLASH_LENGTH_0");
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, size), NULL);
	}
	ASSERT_EQ(FLASH_EraseSpeedTest(offset, gpc_ReadBuffer, size, use_time), 0);

	use_time = use_time * (size / 0x100000);
	
	CaseTime case_time;
	case_time.Set("FLASH_ERASE_TIME", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);
}

CASE_F(FLASH_WRITE_TIME,
	"测试从FLASH写入1MB数据所耗费的时间")
{
	int offset = fw_getenv_i("FLASH_OFFSET_0");
	int size = fw_getenv_i("FLASH_LENGTH_0");
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, size), NULL);
	}
	ASSERT_EQ(FLASH_WriteSpeedTest(offset, gpc_ReadBuffer, size, use_time), 0);

	use_time = use_time * (size / 0x100000);
	
	CaseTime case_time;
	case_time.Set("FLASH_WRITE_TIME", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);


}


CASE_F(USB_WRITE_TIME_16K,
	"测试以16KB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 16 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_16K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_WRITE_TIME_64K,
	"测试以64KB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 64 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_64K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_WRITE_TIME_128K,
	"测试以128KB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 128 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_128K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_WRITE_TIME_256K,
	"测试以256KB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 256 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_256K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_WRITE_TIME_512K,
	"测试以512KB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 512 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_512K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_WRITE_TIME_1M,
	"测试以1MB为单位写入100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer =(char*) CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_write_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 1024 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_WRITE_TIME_1M", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}



CASE_F(USB_READ_TIME_16K,
	"测试以16KB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer =(char*) CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 16 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_16K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_READ_TIME_64K,
	"测试以64KB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 64 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_64K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_READ_TIME_128K,
	"测试以128KB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 128 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_128K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_READ_TIME_256K,
	"测试以256KB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 256 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_256K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_READ_TIME_512K,
	"测试以512KB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 512 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_512K", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}

CASE_F(USB_READ_TIME_1M,
	"测试以1MB为单位读出100M数据的耗费的时间")
{
	int use_time = 0;

	if(!gpc_ReadBuffer)
	{
		ASSERT_NE(gpc_ReadBuffer = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x100000), NULL);
	}
	use_time = CH_RTOS_GetSysTimenow();
	ASSERT_EQ(fsys_read_fileBlK("C:/speed_test.bin", gpc_ReadBuffer, 1024 * 1024, 0x6400000), 0x6400000);
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("USB_READ_TIME_1M", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

	CH_FSYS_DeleteFile("C:/speed_test.bin", NULL, NULL);
}

CASE_F(MEMTEST_5M,
	"测试5MB内存测试所耗费的时间")
{
	int use_time = 0;


	use_time = CH_RTOS_GetSysTimenow();

	RUN_CMD("mem.test 0x500000 1");
	
	use_time = CH_RTOS_GetSysTimenow() -use_time;
	
	CaseTime case_time;
	case_time.Set("MEMTEST_5M", use_time);//设置测试结果的时间
	LOG("use time = %dms\n", use_time);

}


CASE_F(SD_CHANNEL_SWITCH_1,
	"同频点标清节目静帧切换20次测试"){
	TimeInMillis time;
	int prognum = 0;

	
	Test_ChannelRecordTime(0);
	prognum = request_play_program("SD_CHANNEL_TEST", "TUNER_PARAMS1");
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);

	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	EXPECT_EQ(CHDRV_VIDEO_GetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	stru_attr.b_BlankOnStop= CH_FALSE;
	EXPECT_EQ(CHDRV_VIDEO_SetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	CH_RTOS_TaskDelay(2000);
	
	
	channel_switch_test(1);//换台测试

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("SD_CHANNEL_SWITCH_1", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}

CASE_F(SD_CHANNEL_SWITCH_2,
	"同频点标清节目黑屏切换20次测试"){
	TimeInMillis time;

	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	EXPECT_EQ(CHDRV_VIDEO_GetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	stru_attr.b_BlankOnStop = CH_TRUE;
	EXPECT_EQ(CHDRV_VIDEO_SetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	
	channel_switch_test(1);//换台测试

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("SD_CHANNEL_SWITCH_2", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}


CASE_F(SD_CHANNEL_SWITCH_3,
	"测试TUNER锁定时间"){
	TimeInMillis time;
	
	channel_switch_test( 0);//换台测试 

	//取tuner锁定时间
	EXPECT_GT(time = get_channel_switch_time(TEST_TUNER_LOCK), 0) << "chdrv_tuner未正确实现CHDRV_TUNER_LOCK_CHANGE";
	CaseTime case_time;
	case_time.Set("SD_CHANNEL_SWITCH_3", time);//设置测试结果的时间
}

CASE_F(SD_CHANNEL_SWITCH_4,
	"不同频点标清节目切换20次测试"){
	TimeInMillis time;

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("SD_CHANNEL_SWITCH_4", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}

#ifdef CFG_CHSSA_HD_SUPPORT
CASE_F(HD_CHANNEL_SWITCH_1,
	"同频点高清节目静帧切换20次测试"){
	TimeInMillis time;

	int prognum = 0;
	
	Test_ChannelRecordTime(0);
	prognum = request_play_program("HD_CHANNEL_TEST", "TUNER_PARAMS1");
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);
	CH_RTOS_TaskDelay(2000);
	
	channel_switch_test(1);//换台测试

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("HD_CHANNEL_SWITCH_1", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}

CASE_F(HD_CHANNEL_SWITCH_2,
	"同频点高清节目黑屏切换20次测试"){
	TimeInMillis time;

	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	EXPECT_EQ(CHDRV_VIDEO_GetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	stru_attr.b_BlankOnStop= CH_TRUE;
	EXPECT_EQ(CHDRV_VIDEO_SetWinAttrib(video_GetHandle(0), &stru_attr), CHDRV_VIDEO_OK);
	
	channel_switch_test(1);//换台测试

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("HD_CHANNEL_SWITCH_2", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}

CASE_F(HD_CHANNEL_SWITCH_3,
	"测试TUNER锁定时间"){
	TimeInMillis time;
	
	channel_switch_test(0);//换台测试

	//取tuner锁定时间
	EXPECT_GT(time = get_channel_switch_time(TEST_TUNER_LOCK), 0) << "chdrv_tuner未正确实现CHDRV_TUNER_LOCK_CHANGE";
	CaseTime case_time;
	case_time.Set("HD_CHANNEL_SWITCH_3", time);//设置测试结果的时间
}

CASE_F(HD_CHANNEL_SWITCH_4,
	"不同频点高清节目切换20次测试"){
	TimeInMillis time;

	//取I帧到来时间
	EXPECT_GT(time = get_channel_switch_time(TEST_IFRAME_COME), 0) << "chdrv_video未正确实现CHDRV_VIDEO_EVENT_DECODED";
	CaseTime case_time;
	case_time.Set("HD_CHANNEL_SWITCH_4", time);//设置测试结果的时间
	
	RUN_CMD("channel.time");//清除测试结果
}
#endif

#endif

#endif /* ENABLE_TESTCASE */


