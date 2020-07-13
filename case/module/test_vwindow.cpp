/**
@file test_vwindow.cpp
@def group vwindow测试集

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
#define CASE_NAME TestVwindow


#include "test_menu.h"
#include "test_modules.h"

#define VWINDOW_PARAM_MINUS       	(-1)   	//负数一般作非法参数
#define VWINDOW_PARAM_RIGHT			(200)	//该测试为防止越界一般取200作合法参数  
#define VWINDOW_PARAM_ZERO			(0)		//

#define VWINDOW_TEST_LITTLE_PIXEL	(3)		//测试像素小于5的值设定

#define VWINDOW_PATH_HD				(1)//高清
#define VWINDOW_PATH_SD				(0)//标清

#define VWINDOW_CHANNEL_MAIN		(0)//主通道

#define VWINDOW_PARAM_OVER_HRANGE	(1090)//超出最大高度，作越界非法参数
#define VWINDOW_PARAM_OVER_WRANGE	(1930)//超出最大宽度，作越界非法参数


#define VWINDOW_HANDLE 				((void *)0x87650000)//在vwindow handle 范围内 ，作正确参数
#define VWINDOW_HANDLE_INVALID 		((void *)0x87650002)//超出最大vwindow handle值 ，作错误参数



extern int vwin_info(int argc, CMD_PARAM argv[]);


/**
@brief 函数功能: 设置窗口大小并调用函数读取窗口大小对比值是否吻合
@param [in] rh_WinHandle 	视频窗口句柄
@param [in] rpstru_Rect 	视频显示窗口坐标
@param [in] ri_PathIndex	高标清索引(0: 标清；1:高清)
@retval CHDRV_VWINDOW_OK  操作成功
@retval CHDRV_VWINDOW_FAIL  操作失败
**/
static CHDRV_VWINDOW_RESULT_e TEST_VWINDOW_SetGetRectEx(CHDRV_VWINDOW_Handle  rh_WinHandle, CH_Rect_t  rpstru_SetRect,S32 ri_PathIndex)
{
	CH_Rect_t  rstru_RectG ={0};
	
	if(CHDRV_VWINDOW_FAIL == CHDRV_VWINDOW_SetRectEx(rh_WinHandle,&rpstru_SetRect,ri_PathIndex))
	{
		LOG("CHDRV_VWINDOW_SetRect error \n");
		return CHDRV_VWINDOW_FAIL;
	}
	if(CHDRV_VWINDOW_FAIL == CHDRV_VWINDOW_GetRectEx(rh_WinHandle,&rstru_RectG,ri_PathIndex))
	{
		LOG("CHDRV_VWINDOW_GetRect error \n");
		return CHDRV_VWINDOW_FAIL;
	}
	if(
#if CFG_CHSSA_HD_SUPPORT		
		rpstru_SetRect.s_X >= 0 && rpstru_SetRect.s_X + rpstru_SetRect.s_Width <= 1920
		&& rpstru_SetRect.s_Y >= 0 && rpstru_SetRect.s_Y + rpstru_SetRect.s_Height <= 1080
#else
		rpstru_SetRect.s_X >= 0 && rpstru_SetRect.s_X + rpstru_SetRect.s_Width <= 720
		&& rpstru_SetRect.s_Y >= 0 && rpstru_SetRect.s_Y + rpstru_SetRect.s_Height <= 576
#endif
		&& (rstru_RectG.s_Height != rpstru_SetRect.s_Height 
		|| rstru_RectG.s_Width != rpstru_SetRect.s_Width 
		|| rstru_RectG.s_X != rpstru_SetRect.s_X 
		|| rstru_RectG.s_Y != rpstru_SetRect.s_Y)) 
	{
		LOG("get rect 与 set rect 的值不同\n");
		return CHDRV_VWINDOW_FAIL;
	}
	
	return CHDRV_VWINDOW_OK;
}



/**
@brief 函数功能: 设置窗口大小并调用函数读取窗口大小对比值是否吻合
@param [in] rh_WinHandle 	视频窗口句柄
@param [in] rpstru_Rect 	视频显示窗口坐标
@param [in] ri_PathIndex	高标清索引(0: 标清；1:高清)
@retval CHDRV_VWINDOW_OK  操作成功
@retval CHDRV_VWINDOW_FAIL  操作失败
**/
static CHDRV_VWINDOW_RESULT_e TEST_VWINDOW_SetGetRect(CHDRV_VWINDOW_Handle  rh_WinHandle, CH_Rect_t  rpstru_SetRect)
{
	CH_Rect_t  rstru_RectG ={0};
	
	if(CHDRV_VWINDOW_FAIL == CHDRV_VWINDOW_SetRect(rh_WinHandle,&rpstru_SetRect))
	{
		LOG("CHDRV_VWINDOW_SetRect error \n");
		return CHDRV_VWINDOW_FAIL;
	}
	if(CHDRV_VWINDOW_FAIL == CHDRV_VWINDOW_GetRect(rh_WinHandle,&rstru_RectG))
	{
		LOG("CHDRV_VWINDOW_GetRect error \n");
		return CHDRV_VWINDOW_FAIL;
	}
	if(
#if CFG_CHSSA_HD_SUPPORT		
		rpstru_SetRect.s_X >= 0 && rpstru_SetRect.s_X + rpstru_SetRect.s_Width <= 1920
		&& rpstru_SetRect.s_Y >= 0 && rpstru_SetRect.s_Y + rpstru_SetRect.s_Height <= 1080
#else
		rpstru_SetRect.s_X >= 0 && rpstru_SetRect.s_X + rpstru_SetRect.s_Width <= 720
		&& rpstru_SetRect.s_Y >= 0 && rpstru_SetRect.s_Y + rpstru_SetRect.s_Height <= 576
#endif
		&& (rstru_RectG.s_Height != rpstru_SetRect.s_Height 
		|| rstru_RectG.s_Width != rpstru_SetRect.s_Width 
		|| rstru_RectG.s_X != rpstru_SetRect.s_X 
		|| rstru_RectG.s_Y != rpstru_SetRect.s_Y)) 
	{
		LOG("get rect 与 set rect 的值不同\n");
		return CHDRV_VWINDOW_FAIL;
	}
	
	return CHDRV_VWINDOW_OK;
}



void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;

	RUN_CMD("route.create 0 0");
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.open 1 1 1 1 0 0 0 0\n");
	RUN_CMD("vout.set_format 6");
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("VIDEO_POSITION_HD"));
#else
	RUN_CMD("vout.set_format 1");
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("VIDEO_POSITION_SD"));
#endif
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit vwindown rtos TestCase " << std::endl;

	//EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDrawHandleW));
	//EXPECT_EQ(CHDRV_OSD_OK,CHDRV_OSD_DeleteRegion(grh_BmpDisplayHandleW));
	//RUN_CMD("av.stop 0");			//停止播放
	//EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Term());
	RUN_CMD("vid.ram_stop 0");
	RUN_CMD("osd.delregion");
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
dbg_checkmem(0);
}
#if ENABLE_TESTCASE

CASE_F(TEST_VWINDOW_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init());
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init()) << "多次初始化应该返回OK";
}



CASE_F(TEST_VWINDOW_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Term());
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init());
}


CASE_F(TEST_VWINDOW_INIT3,
	"测试CHDRV_VWINDOW_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	//EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_GetVersion(&stru_Ver));
	//EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_VWINDOW_PARAMS_OPEN,
	"测试CHDRV_VWINDOW_Open错误参数测试") {
	CH_LAYER_e        			renm_Layer1 = CH_LAYER_MAX;
	CH_LAYER_e 		  			renm_Layer2 = CH_LAYER_VIDEO1;
	
	CHDRV_VIDEO_Handle  		 rh_Handle1 = NULL;
	CHDRV_VIDEO_Handle  		 rh_Handle2 = NULL;
	
	CHDRV_VWINDOW_Handle *   rph_WinHandle1 = NULL;
	CHDRV_VWINDOW_Handle  	 rph_WinHandle2  = NULL;
	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Open(renm_Layer1,rh_Handle2,&rph_WinHandle2));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Open(renm_Layer2,rh_Handle1,&rph_WinHandle2));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Open(renm_Layer2,rh_Handle2, rph_WinHandle1));
}

CASE_F(TEST_VWINDOW_PARAMS_RECT,
	"测试CHDRV_VWINDOW_SetRect/GetRect错误参数测试") {
	CHDRV_VWINDOW_Handle  rh_WinHandle1 = VWINDOW_HANDLE;
	CHDRV_VWINDOW_Handle  rh_WinHandle2 = NULL;
	CHDRV_VWINDOW_Handle  rh_WinHandle3 = VWINDOW_HANDLE_INVALID;
	
	CH_Rect_t  			   rpstru_Rect1 ;
	CH_Rect_t *			   rpstru_Rect2 =  NULL;//
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRect( rh_WinHandle2, &rpstru_Rect1));	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRect( rh_WinHandle3, &rpstru_Rect1));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRect( rh_WinHandle1, rpstru_Rect2));

	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRect( rh_WinHandle3, &rpstru_Rect1));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRect( rh_WinHandle2, &rpstru_Rect1));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRect( rh_WinHandle1, rpstru_Rect2));

}

CASE_F(TEST_VWINDOW_PARAMS_RECT_EX,
	"测试CHDRV_VWINDOW_SetRect/GetRect错误参数测试") {
	CHDRV_VWINDOW_Handle  rh_WinHandle1 = VWINDOW_HANDLE;
	CHDRV_VWINDOW_Handle  rh_WinHandle2 = NULL;
	CHDRV_VWINDOW_Handle  rh_WinHandle3 = VWINDOW_HANDLE_INVALID;
	
	CH_Rect_t  			   rpstru_Rect1 ;
	CH_Rect_t *			   rpstru_Rect2 =  NULL;//
	S32 	 ri_PathIndex = VWINDOW_PATH_SD;


	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRectEx( rh_WinHandle2, &rpstru_Rect1, ri_PathIndex));	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRectEx( rh_WinHandle3, &rpstru_Rect1, ri_PathIndex));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetRectEx( rh_WinHandle1, rpstru_Rect2, ri_PathIndex));

	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRectEx( rh_WinHandle3, &rpstru_Rect1, ri_PathIndex));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRectEx( rh_WinHandle2, &rpstru_Rect1, ri_PathIndex));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetRectEx( rh_WinHandle1, rpstru_Rect2, ri_PathIndex));

}

CASE_F(TEST_VWINDOW_PARAMS_GAMMA,
	"测试CHDRV_VWINDOW_SetAlphaGamma/GetAlphaGamma错误参数测试") {

	U8 						 ruc_Alpha = 0;
	S8 						  rc_Gamma = 0;
	
	U8  				ruc_Alpha_Get1 = 0;
	U8* 				ruc_Alpha_Get2 = NULL; 
	
	S8  				 rc_Gamma_Get1 ;
	S8*  				 rc_Gamma_Get2 = NULL; 
	

	CHDRV_VWINDOW_Handle rh_WinHandle1 = NULL; 
	CHDRV_VWINDOW_Handle rh_WinHandle2 = VWINDOW_HANDLE_INVALID;
	CHDRV_VWINDOW_Handle rh_WinHandle3 = VWINDOW_HANDLE; 

	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetAlphaGamma(rh_WinHandle1 , ruc_Alpha, rc_Gamma));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetAlphaGamma(rh_WinHandle2 , ruc_Alpha, rc_Gamma));
	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetAlphaGamma(rh_WinHandle3, ruc_Alpha_Get2, &rc_Gamma_Get1));	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetAlphaGamma(rh_WinHandle3, &ruc_Alpha_Get1, rc_Gamma_Get2));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetAlphaGamma(rh_WinHandle2, &ruc_Alpha_Get1, &rc_Gamma_Get1));
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_GetAlphaGamma(rh_WinHandle1, &ruc_Alpha_Get1, &rc_Gamma_Get1));
}

CASE_F(TEST_VWINDOW_PARAMS_IOAREA,
	"CHDRV_VWINDOW_SetIOArea错误参数测试") {
#define RECT_INFO(name, rect) 	name << "(" << rect.s_X << "," <<rect.s_Y << "," << rect.s_Width << "," << rect.s_Height << ")" 
	CHDRV_VWINDOW_Handle rh_WinHandle1 = VWINDOW_HANDLE;
	CHDRV_VWINDOW_Handle rh_WinHandle2 = NULL;
	CHDRV_VWINDOW_Handle rh_WinHandle3 = VWINDOW_HANDLE_INVALID;
	
	CH_Rect_t   	 rpstru_Area1;
	
	INIT_RECT(rpstru_Area1,0,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//输入范围内的区域，合法
	CH_Rect_t   	 rpstru_Area2 ;
	INIT_RECT(rpstru_Area2, VWINDOW_PARAM_MINUS,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//x为负,其他参数合法
	CH_Rect_t   	 rpstru_Area3;//y为负，其他参数合法
	INIT_RECT(rpstru_Area3, 0,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//x为负,其他参数合法
	CH_Rect_t   	 rpstru_Area4 ;//w为负，其他参数合法
	INIT_RECT(rpstru_Area4, 0,0,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area5;//h为负，其他参数合法
	INIT_RECT(rpstru_Area5,0,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS);
	CH_Rect_t		 rpstru_Area6 ;//x>W越界，其他参数合法
	INIT_RECT(rpstru_Area6,VWINDOW_PARAM_OVER_WRANGE,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	CH_Rect_t		 rpstru_Area7;//y>H越界，其他参数合法
	INIT_RECT(rpstru_Area7,0,VWINDOW_PARAM_OVER_HRANGE,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area8;//w>W越界，其他参数合法
	INIT_RECT(rpstru_Area8,0,0,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area9 ;//h<H越界，其他参数合法
	INIT_RECT(rpstru_Area9,0,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_HRANGE);


	

	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle2,&rpstru_Area1,&rpstru_Area1)) 
		<< "handle=" << rh_WinHandle2 << "," << RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle3,&rpstru_Area1,&rpstru_Area1))
		<< "handle=" << rh_WinHandle3 << "," << RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area2,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area2) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area3,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area3) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area4,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area4) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area5,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area5) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area6,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area6) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area7,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area7) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area8,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area8) << "," << RECT_INFO("output", rpstru_Area1);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area9,&rpstru_Area1))
		<< RECT_INFO("input", rpstru_Area9) << "," << RECT_INFO("output", rpstru_Area1);
	
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area2))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area2);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area3))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area3);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area4))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area4);//w<0未做处理
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area5))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area5);//h<0未做处理
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area6))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area6);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area7))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area7);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area8))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area8);
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area9))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area9);
}

CASE_F(TEST_VWINDOW_PARAMS_SHOW,
	"测试CHDRV_VWINDOW_Show/Hide错误参数测试") {
	CHDRV_VWINDOW_Handle rh_WinHandle = NULL;
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Show(rh_WinHandle)) << "handle is null";
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Hide(rh_WinHandle))<< "handle is null";
}

CASE_F(TEST_VWINDOW_PARAMS_CLOSE,
	"测试CHDRV_VWINDOW_Close错误参数测试") {
	CHDRV_VWINDOW_Handle rh_WinHandle = NULL;
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Close(rh_WinHandle)) << "handle is null";

}

CASE_F(TEST_VWINDOW_OPEN,
	"测试CHDRV_VWINDOW_Open功能是否正常，返回句柄不应为空") {
	//CH_LAYER_e 				renm_Layer = CH_LAYER_VIDEO1 ;
	
	//EXPECT_EQ(CHDRV_VIDEO_OK,CHDRV_VIDEO_Open(VWINDOW_CHANNEL_MAIN,&grh_Handle));//
	//EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Open(renm_Layer,grh_Handle,&grh_WinHandle));
	//CH_RTOS_TaskDelay(2000);
	CHDRV_VWINDOW_Handle grh_Handle = NULL;
	EXPECT_EQ(CHDRV_ROUTE_OK,CHDRV_ROUTE_GetVWindowHandle(route_GetHandle(0),&grh_Handle));

}

CASE_F(TEST_VWINDOW_SHOW,
	"测试CHDRV_VWINDOW_Show，应返回成功，视频窗口可见") {
	CH_Rect_t  rstru_Rect = {0,0,400,300};

	LOG("size=%d\n", vwin_GetHandleNums());
	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	
#ifdef CFG_CHSSA_HD_SUPPORT	
	rstru_Rect.s_Width = 1280;
	rstru_Rect.s_Height = 720;
#else
	rstru_Rect.s_Width = 720;
	rstru_Rect.s_Height = 576;
#endif
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Show(vwin_GetHandle(0)));
	PROMPT("当前vwin为可见状态,输出是否正常?是请按Y，否则请按N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "窗口不可见";
}

CASE_F(TEST_VWINDOW_SET_RECT_EX,
	"测试SetRectEx（0,0,400,300）是否成功，视频位置是否正确") {
	S32 	 ri_PathIndex = VWINDOW_PATH_HD;
	CH_Rect_t  rstru_Rect = {0,0,400,300};

#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
	RUN_CMD("osd.pic 3 0 1280 720 %s", fw_getenv("OSD_BASIC_HD"));
#else
	RUN_CMD("osd.pic 3 0 720 576 %s", fw_getenv("OSD_BASIC_SD"));
	ri_PathIndex = VWINDOW_PATH_SD;
#endif
	RUN_CMD("osd.trans 40");

	rstru_Rect.s_X = 100;
	rstru_Rect.s_Y = 100;
	rstru_Rect.s_Width = 300;
	rstru_Rect.s_Height = 200;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRectEx(vwin_GetHandle(0),&rstru_Rect,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	
#ifdef CFG_CHSSA_HD_SUPPORT	
	rstru_Rect.s_Width = 1280/2;
	rstru_Rect.s_Height = 720/2;
#else
	rstru_Rect.s_Width = 720/2;
	rstru_Rect.s_Height = 576/2;
#endif
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRectEx(vwin_GetHandle(0),&rstru_Rect,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	rstru_Rect.s_Width = 400;
	rstru_Rect.s_Height = 300;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRectEx(vwin_GetHandle(0),&rstru_Rect,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;


}

CASE_F(TEST_VWINDOW_GET_RECT_EX,
	"测试GetRectEx获取的坐标是否与设置的一致") {
	CH_Rect_t rstru_Rect ;
	S32 ri_PathIndex = VWINDOW_PATH_HD;

#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
#else
	ri_PathIndex = VWINDOW_PATH_SD;
#endif
		
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_GetRectEx(vwin_GetHandle(0),&rstru_Rect,ri_PathIndex));
	EXPECT_EQ(300,rstru_Rect.s_Height);
	EXPECT_EQ(400,rstru_Rect.s_Width);
	EXPECT_EQ(0,rstru_Rect.s_X);
	EXPECT_EQ(0,rstru_Rect.s_Y);
}




CASE_F(TEST_VWINDOW_RECT_EX_ERROR,
	"测试当坐标位置超出左、上、右、下时，SetRectEx是否设置成功，视频位置是否正确，GetRectEx是否与设置的一致") {
	
	S32 	  ri_PathIndex = VWINDOW_PATH_HD;	
	CH_Rect_t  rstru_RectS;	

	INIT_RECT(rstru_RectS, VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
#else
	ri_PathIndex = VWINDOW_PATH_SD;
#endif

	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_HRANGE);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
}

CASE_F(TEST_VWINDOW_RECT_EX_LITTLE,
	"测试rect width和hight分别小于5像素时，SetRectEx是否成功，视频显示是否正常，GetRectEx是否与设置的一致") {
	S32 	  ri_PathIndex = VWINDOW_PATH_HD;
	CH_Rect_t  rstru_RectS = {VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL,VWINDOW_PARAM_RIGHT};//	

#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
#else
	ri_PathIndex = VWINDOW_PATH_SD;
#endif

	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;


#ifdef CFG_CHSSA_HD_SUPPORT	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,1280,720);
#else
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,720,576);
#endif

	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

}


//=========

CASE_F(TEST_VWINDOW_SET_RECT1,
	"测试SetRect（0,0,400,300）是否成功，视频位置是否正确") {
	CH_Rect_t  rstru_Rect = {0,0,400,300};


	rstru_Rect.s_X = 100;
	rstru_Rect.s_Y = 100;
	rstru_Rect.s_Width = 300;
	rstru_Rect.s_Height = 200;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	
#ifdef CFG_CHSSA_HD_SUPPORT	
	rstru_Rect.s_Width = 1280/2;
	rstru_Rect.s_Height = 720/2;
#else
	rstru_Rect.s_Width = 720/2;
	rstru_Rect.s_Height = 576/2;
#endif
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	rstru_Rect.s_Width = 400;
	rstru_Rect.s_Height = 300;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;


}

CASE_F(TEST_VWINDOW_GET_RECT1,
	"测试GetRect获取的坐标是否与设置的一致") {
	CH_Rect_t rstru_Rect ;
	

		
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_GetRect(vwin_GetHandle(0),&rstru_Rect));
	EXPECT_EQ(300,rstru_Rect.s_Height);
	EXPECT_EQ(400,rstru_Rect.s_Width);
	EXPECT_EQ(0,rstru_Rect.s_X);
	EXPECT_EQ(0,rstru_Rect.s_Y);
}



CASE_F(TEST_VWINDOW_SET_RECT2,
	"测试当坐标位置超出左、上、右、下时，SetRect是否设置成功，视频位置是否正确，GetRect是否与设置的一致") {
	
	CH_Rect_t  rstru_RectS;	
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_HRANGE);
	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),设置是否无效或按边界裁剪?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
}

CASE_F(TEST_VWINDOW_RECT_LITTLE1,
	"测试rect width和hight分别小于5像素时，SetRect是否成功，视频显示是否正常，GetRect是否与设置的一致") {
	CH_Rect_t  rstru_RectS ;//	

	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL,VWINDOW_PARAM_RIGHT);
	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;


#ifdef CFG_CHSSA_HD_SUPPORT	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,1280,720);
#else
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,720,576);
#endif

	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	
	PROMPT("当前vwin位置被设置为(%d, %d, %d, %d),输出位置显示是否正确?是请按Y，否则请按N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "视频位置显示不正确,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

}
//=========
CASE_F(TEST_VWINDOW_HIDE1,
	"测试CHDRV_VWINDOW_Hide,应返回成功，视频应不可见") {
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Hide(vwin_GetHandle(0)));
	PROMPT("当前vwin为隐藏状态,输出是否不可见?是请按Y，否则请按N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "窗口隐藏无效";
}

CASE_F(TEST_VWINDOW_SHOW1,
	"测试CHDRV_VWINDOW_SHOW,应返回成功，视频应可见") {
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Show(vwin_GetHandle(0)));
	PROMPT("当前vwin为可见状态,输出是否恢复显示?是请按Y，否则请按N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "窗口设置为可见无效";
}


CASE_F(TEST_VWINDOW_SETIO_AREA,
	"测试SetIOArea功能是否正常") {
	//EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Close(vwin_GetHandle(0)));
	
	//CH_RTOS_TaskDelay(2000);
CH_Rect_t stru_InRect, stru_OutRect, stru_Rect;

	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_ClearFullRegion(osd_GetHandle(CH_LAYER_OSD1, CHDRV_OSD_REGION_DRAW),0));
	EXPECT_EQ(CHDRV_OSD_OK, CHDRV_OSD_UpdateRegion(osd_GetHandle(CH_LAYER_OSD1, CHDRV_OSD_REGION_DRAW)));
	RUN_CMD("osd.trans 100");

	RUN_CMD("vid.ram_stop 0");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("VIDEO_BASIC_HD"));
#else
	RUN_CMD("vid.ram_start 0 %s", fw_getenv("VIDEO_BASIC_SD"));
#endif
	stru_Rect.s_X = 0;
	stru_Rect.s_Y = 0;
	
	stru_Rect.s_Width = fw_getenv_i("VIDEO_REGION_WIDTH");
	stru_Rect.s_Height = fw_getenv_i("VIDEO_REGION_HEIGHT");

	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&stru_Rect));

	memcpy(&stru_OutRect, &stru_Rect, sizeof(CH_Rect_t));
	memcpy(&stru_InRect, &stru_Rect, sizeof(CH_Rect_t));

	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));
	PROMPT("这是正常的I帧画面，按OK键继续...");	
	test_input_none();
	
	stru_InRect.s_Width /= 4;
	stru_InRect.s_Height /= 4;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));
	PROMPT("当前画面应为左上角放大显示?是请按Y，否则请按N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "SetIOArea放大显示无效";
	
	stru_InRect.s_X += stru_InRect.s_Width * 3 /4;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));
	PROMPT("当前画面应为右上角放大显示?是请按Y，否则请按N:");	
	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "SetIOArea放大并右移坐标显示无效";
	memcpy(&stru_InRect, &stru_Rect, sizeof(CH_Rect_t));
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));

	
}


CASE_F(TEST_VWINDOW_CLOSE,
	"测试CHDRV_VWINDOW_Close返回值是否正确") {
	//EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Close(vwin_GetHandle(0)));
	
	//CH_RTOS_TaskDelay(2000);

}
#endif /* ENABLE_TESTCASE */


static int vwin_open(int argc, CMD_PARAM argv[])
{
	int ret  = 0;
	CHDRV_VIDEO_Handle handle = NULL;

	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		if(video_GetHandleNums() == 0)
		{
			LOG("error: please open video first!\n");
			return -1;
		}
		switch(argv[1].get_i())
		{
			case 0:
				ret = CHDRV_VWINDOW_Open(CH_LAYER_VIDEO1, video_GetHandle(0), &handle);
				break;
			case 1:
				ret = CHDRV_VWINDOW_Open(CH_LAYER_VIDEO2, video_GetHandle(0), &handle);
				break;	
			default:
				LOG("error: can not support video %d!\n", argv[1].get_i());
				break;
			return -1;

		}
		if(ret != CHDRV_VWINDOW_OK)
		{
			LOG("error: CHDRV_VWINDOW_Open return %d!\n", ret);
			return -1;
		}
	}
	LOG("ok, handle no = %d\n", vwin_GetHandleNums());
	return 0;
}

static int vwin_close(int argc, CMD_PARAM argv[])
{
	int ret;
	
	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		LOG("error: please open vwindow %d first!\n", argv[1].get_i());
		return -1;
	}
	ret = CHDRV_VWINDOW_Close(vwin_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VWINDOW_OK)
	{
		LOG("error: CHDRV_VWINDOW_Close  return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vwin_show(int argc, CMD_PARAM argv[])
{
	int ret;
	
	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		LOG("error: please open vwindow %d first!\n", argv[1].get_i());
		return -1;
	}
	if(argv[2].get_i())
	{
		ret = CHDRV_VWINDOW_Show(vwin_GetHandle(argv[1].get_i()));
	}
	else
	{
		ret = CHDRV_VWINDOW_Hide(vwin_GetHandle(argv[1].get_i()));
	}
	if(ret != CHDRV_VWINDOW_OK)
	{
		LOG("error: CHDRV_VWINDOW_Show CHDRV_VWINDOW_Hide  return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vwin_set_rect(int argc, CMD_PARAM argv[])
{
	int ret;
	CH_Rect_t stru_Rect;
	
	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		LOG("error: please open vwindow %d first!\n", argv[1].get_i());
		return -1;
	}
	stru_Rect.s_X = argv[2].get_i() & 0xffff;
	stru_Rect.s_Y= argv[3].get_i() & 0xffff;
	stru_Rect.s_Width = argv[4].get_i() & 0xffff;
	stru_Rect.s_Height = argv[5].get_i() & 0xffff;
	ret = CHDRV_VWINDOW_SetRect(vwin_GetHandle(argv[1].get_i()), &stru_Rect);
	
	if(ret != CHDRV_VWINDOW_OK)
	{
		LOG("error: CHDRV_VWINDOW_SetRect  return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vwin_set_rectex(int argc, CMD_PARAM argv[])
{
	int ret;
	CH_Rect_t stru_Rect;
	
	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		LOG("error: please open vwindow %d first!\n", argv[1].get_i());
		return -1;
	}
	stru_Rect.s_X = argv[2].get_i() & 0xffff;
	stru_Rect.s_Y= argv[3].get_i() & 0xffff;
	stru_Rect.s_Width = argv[4].get_i() & 0xffff;
	stru_Rect.s_Height = argv[5].get_i() & 0xffff;
	ret = CHDRV_VWINDOW_SetRectEx(vwin_GetHandle(argv[1].get_i()), &stru_Rect, argv[6].get_i());
	
	if(ret != CHDRV_VWINDOW_OK)
	{
		LOG("error: CHDRV_VWINDOW_SetRectEx  return %d!\n", ret);
		return -1;
	}
	return 0;
}

static int vwin_set_iorect(int argc, CMD_PARAM argv[])
{
	int ret;
	CH_Rect_t stru_input, stru_out;
	
	if(vwin_GetHandleNums() <= argv[1].get_i())
	{
		LOG("error: please open vwindow %d first!\n", argv[1].get_i());
		return -1;
	}
	stru_input.s_X = argv[2].get_i() & 0xffff;
	stru_input.s_Y= argv[3].get_i() & 0xffff;
	stru_input.s_Width = argv[4].get_i() & 0xffff;
	stru_input.s_Height = argv[5].get_i() & 0xffff;

	stru_out.s_X = argv[6].get_i() & 0xffff;
	stru_out.s_Y= argv[7].get_i() & 0xffff;
	stru_out.s_Width = argv[8].get_i() & 0xffff;
	stru_out.s_Height = argv[9].get_i() & 0xffff;
	LOG("in [%d %d %d %d] out[%d %d %d %d]\n", stru_input.s_X, stru_input.s_Y, stru_input.s_Width, stru_input.s_Width,
		stru_out.s_X, stru_out.s_Y, stru_out.s_Width, stru_out.s_Height);
	ret = CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(argv[1].get_i()), &stru_input, &stru_out);
	
	if(ret != CHDRV_VWINDOW_OK)
	{
		LOG("error: CHDRV_VWINDOW_SetIOArea  return %d!\n", ret);
		return -1;
	}
	return 0;
}


REGISTER_CMD(vwin_open, "创建vwin句柄", "video_no(0:video1,1:video2)",  "0");
REGISTER_CMD(vwin_close, "删除win句柄", "video_no",  "0");
REGISTER_CMD(vwin_show, "设置视频层是否可见,0隐藏，1显示", "video_no|show",  "0 1");
REGISTER_CMD(vwin_info, "获得vwin的相关信息，打印到串口", "",  "");
REGISTER_CMD(vwin_set_rect, "设置视频窗口位置", "vwin_no | x | y | w | h",  "0 0 0 720 576");
REGISTER_CMD(vwin_set_rectex, "设置视频窗口位置", "vwin_no | x | y | w | h | path_index",  "0 0 0 720 576 0");
REGISTER_CMD(vwin_set_iorect, "设置输入输出区域", "vwin_no | in_x|in_y|in_w|in_h|out_x|out_y|out_w|out_h",  "0 0 0 720 576 0 0 720 576");



