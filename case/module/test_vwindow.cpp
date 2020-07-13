/**
@file test_vwindow.cpp
@def group vwindow���Լ�

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
#define CASE_NAME TestVwindow


#include "test_menu.h"
#include "test_modules.h"

#define VWINDOW_PARAM_MINUS       	(-1)   	//����һ�����Ƿ�����
#define VWINDOW_PARAM_RIGHT			(200)	//�ò���Ϊ��ֹԽ��һ��ȡ200���Ϸ�����  
#define VWINDOW_PARAM_ZERO			(0)		//

#define VWINDOW_TEST_LITTLE_PIXEL	(3)		//��������С��5��ֵ�趨

#define VWINDOW_PATH_HD				(1)//����
#define VWINDOW_PATH_SD				(0)//����

#define VWINDOW_CHANNEL_MAIN		(0)//��ͨ��

#define VWINDOW_PARAM_OVER_HRANGE	(1090)//�������߶ȣ���Խ��Ƿ�����
#define VWINDOW_PARAM_OVER_WRANGE	(1930)//��������ȣ���Խ��Ƿ�����


#define VWINDOW_HANDLE 				((void *)0x87650000)//��vwindow handle ��Χ�� ������ȷ����
#define VWINDOW_HANDLE_INVALID 		((void *)0x87650002)//�������vwindow handleֵ �����������



extern int vwin_info(int argc, CMD_PARAM argv[]);


/**
@brief ��������: ���ô��ڴ�С�����ú�����ȡ���ڴ�С�Ա�ֵ�Ƿ��Ǻ�
@param [in] rh_WinHandle 	��Ƶ���ھ��
@param [in] rpstru_Rect 	��Ƶ��ʾ��������
@param [in] ri_PathIndex	�߱�������(0: ���壻1:����)
@retval CHDRV_VWINDOW_OK  �����ɹ�
@retval CHDRV_VWINDOW_FAIL  ����ʧ��
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
		LOG("get rect �� set rect ��ֵ��ͬ\n");
		return CHDRV_VWINDOW_FAIL;
	}
	
	return CHDRV_VWINDOW_OK;
}



/**
@brief ��������: ���ô��ڴ�С�����ú�����ȡ���ڴ�С�Ա�ֵ�Ƿ��Ǻ�
@param [in] rh_WinHandle 	��Ƶ���ھ��
@param [in] rpstru_Rect 	��Ƶ��ʾ��������
@param [in] ri_PathIndex	�߱�������(0: ���壻1:����)
@retval CHDRV_VWINDOW_OK  �����ɹ�
@retval CHDRV_VWINDOW_FAIL  ����ʧ��
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
		LOG("get rect �� set rect ��ֵ��ͬ\n");
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
	//RUN_CMD("av.stop 0");			//ֹͣ����
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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init());
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}



CASE_F(TEST_VWINDOW_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Term());
	EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_Init());
}


CASE_F(TEST_VWINDOW_INIT3,
	"����CHDRV_VWINDOW_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	//EXPECT_EQ(CHDRV_VWINDOW_OK, CHDRV_VWINDOW_GetVersion(&stru_Ver));
	//EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_VWINDOW_PARAMS_OPEN,
	"����CHDRV_VWINDOW_Open�����������") {
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
	"����CHDRV_VWINDOW_SetRect/GetRect�����������") {
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
	"����CHDRV_VWINDOW_SetRect/GetRect�����������") {
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
	"����CHDRV_VWINDOW_SetAlphaGamma/GetAlphaGamma�����������") {

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
	"CHDRV_VWINDOW_SetIOArea�����������") {
#define RECT_INFO(name, rect) 	name << "(" << rect.s_X << "," <<rect.s_Y << "," << rect.s_Width << "," << rect.s_Height << ")" 
	CHDRV_VWINDOW_Handle rh_WinHandle1 = VWINDOW_HANDLE;
	CHDRV_VWINDOW_Handle rh_WinHandle2 = NULL;
	CHDRV_VWINDOW_Handle rh_WinHandle3 = VWINDOW_HANDLE_INVALID;
	
	CH_Rect_t   	 rpstru_Area1;
	
	INIT_RECT(rpstru_Area1,0,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//���뷶Χ�ڵ����򣬺Ϸ�
	CH_Rect_t   	 rpstru_Area2 ;
	INIT_RECT(rpstru_Area2, VWINDOW_PARAM_MINUS,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//xΪ��,���������Ϸ�
	CH_Rect_t   	 rpstru_Area3;//yΪ�������������Ϸ�
	INIT_RECT(rpstru_Area3, 0,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);//xΪ��,���������Ϸ�
	CH_Rect_t   	 rpstru_Area4 ;//wΪ�������������Ϸ�
	INIT_RECT(rpstru_Area4, 0,0,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area5;//hΪ�������������Ϸ�
	INIT_RECT(rpstru_Area5,0,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS);
	CH_Rect_t		 rpstru_Area6 ;//x>WԽ�磬���������Ϸ�
	INIT_RECT(rpstru_Area6,VWINDOW_PARAM_OVER_WRANGE,0,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	CH_Rect_t		 rpstru_Area7;//y>HԽ�磬���������Ϸ�
	INIT_RECT(rpstru_Area7,0,VWINDOW_PARAM_OVER_HRANGE,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area8;//w>WԽ�磬���������Ϸ�
	INIT_RECT(rpstru_Area8,0,0,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	CH_Rect_t   	 rpstru_Area9 ;//h<HԽ�磬���������Ϸ�
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
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area4);//w<0δ������
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_SetIOArea(rh_WinHandle1,&rpstru_Area1,&rpstru_Area5))
		<< RECT_INFO("input", rpstru_Area1) << "," << RECT_INFO("output", rpstru_Area5);//h<0δ������
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
	"����CHDRV_VWINDOW_Show/Hide�����������") {
	CHDRV_VWINDOW_Handle rh_WinHandle = NULL;
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Show(rh_WinHandle)) << "handle is null";
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Hide(rh_WinHandle))<< "handle is null";
}

CASE_F(TEST_VWINDOW_PARAMS_CLOSE,
	"����CHDRV_VWINDOW_Close�����������") {
	CHDRV_VWINDOW_Handle rh_WinHandle = NULL;
	EXPECT_EQ(CHDRV_VWINDOW_INVALID_PARAM,CHDRV_VWINDOW_Close(rh_WinHandle)) << "handle is null";

}

CASE_F(TEST_VWINDOW_OPEN,
	"����CHDRV_VWINDOW_Open�����Ƿ����������ؾ����ӦΪ��") {
	//CH_LAYER_e 				renm_Layer = CH_LAYER_VIDEO1 ;
	
	//EXPECT_EQ(CHDRV_VIDEO_OK,CHDRV_VIDEO_Open(VWINDOW_CHANNEL_MAIN,&grh_Handle));//
	//EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Open(renm_Layer,grh_Handle,&grh_WinHandle));
	//CH_RTOS_TaskDelay(2000);
	CHDRV_VWINDOW_Handle grh_Handle = NULL;
	EXPECT_EQ(CHDRV_ROUTE_OK,CHDRV_ROUTE_GetVWindowHandle(route_GetHandle(0),&grh_Handle));

}

CASE_F(TEST_VWINDOW_SHOW,
	"����CHDRV_VWINDOW_Show��Ӧ���سɹ�����Ƶ���ڿɼ�") {
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
	PROMPT("��ǰvwinΪ�ɼ�״̬,����Ƿ�����?���밴Y�������밴N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "���ڲ��ɼ�";
}

CASE_F(TEST_VWINDOW_SET_RECT_EX,
	"����SetRectEx��0,0,400,300���Ƿ�ɹ�����Ƶλ���Ƿ���ȷ") {
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
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

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
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	rstru_Rect.s_Width = 400;
	rstru_Rect.s_Height = 300;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRectEx(vwin_GetHandle(0),&rstru_Rect,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;


}

CASE_F(TEST_VWINDOW_GET_RECT_EX,
	"����GetRectEx��ȡ�������Ƿ������õ�һ��") {
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
	"���Ե�����λ�ó������ϡ��ҡ���ʱ��SetRectEx�Ƿ����óɹ�����Ƶλ���Ƿ���ȷ��GetRectEx�Ƿ������õ�һ��") {
	
	S32 	  ri_PathIndex = VWINDOW_PATH_HD;	
	CH_Rect_t  rstru_RectS;	

	INIT_RECT(rstru_RectS, VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
#else
	ri_PathIndex = VWINDOW_PATH_SD;
#endif

	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_HRANGE);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
}

CASE_F(TEST_VWINDOW_RECT_EX_LITTLE,
	"����rect width��hight�ֱ�С��5����ʱ��SetRectEx�Ƿ�ɹ�����Ƶ��ʾ�Ƿ�������GetRectEx�Ƿ������õ�һ��") {
	S32 	  ri_PathIndex = VWINDOW_PATH_HD;
	CH_Rect_t  rstru_RectS = {VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL,VWINDOW_PARAM_RIGHT};//	

#ifdef CFG_CHSSA_HD_SUPPORT	
	ri_PathIndex = VWINDOW_PATH_HD;
#else
	ri_PathIndex = VWINDOW_PATH_SD;
#endif

	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;


#ifdef CFG_CHSSA_HD_SUPPORT	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,1280,720);
#else
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,720,576);
#endif

	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRectEx(vwin_GetHandle(0),rstru_RectS,ri_PathIndex));
	
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

}


//=========

CASE_F(TEST_VWINDOW_SET_RECT1,
	"����SetRect��0,0,400,300���Ƿ�ɹ�����Ƶλ���Ƿ���ȷ") {
	CH_Rect_t  rstru_Rect = {0,0,400,300};


	rstru_Rect.s_X = 100;
	rstru_Rect.s_Y = 100;
	rstru_Rect.s_Width = 300;
	rstru_Rect.s_Height = 200;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

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
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;

	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	rstru_Rect.s_Width = 400;
	rstru_Rect.s_Height = 300;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_Rect.s_X, rstru_Rect.s_Y, rstru_Rect.s_Width, rstru_Rect.s_Height);
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_Rect.s_X<< ",y="<< rstru_Rect.s_Y << ",w=" << rstru_Rect.s_Width<< ",h=" << rstru_Rect.s_Height;


}

CASE_F(TEST_VWINDOW_GET_RECT1,
	"����GetRect��ȡ�������Ƿ������õ�һ��") {
	CH_Rect_t rstru_Rect ;
	

		
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_GetRect(vwin_GetHandle(0),&rstru_Rect));
	EXPECT_EQ(300,rstru_Rect.s_Height);
	EXPECT_EQ(400,rstru_Rect.s_Width);
	EXPECT_EQ(0,rstru_Rect.s_X);
	EXPECT_EQ(0,rstru_Rect.s_Y);
}



CASE_F(TEST_VWINDOW_SET_RECT2,
	"���Ե�����λ�ó������ϡ��ҡ���ʱ��SetRect�Ƿ����óɹ�����Ƶλ���Ƿ���ȷ��GetRect�Ƿ������õ�һ��") {
	
	CH_Rect_t  rstru_RectS;	
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_MINUS,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_WRANGE,VWINDOW_PARAM_RIGHT);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_OVER_HRANGE);
	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),�����Ƿ���Ч�򰴱߽�ü�?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;
}

CASE_F(TEST_VWINDOW_RECT_LITTLE1,
	"����rect width��hight�ֱ�С��5����ʱ��SetRect�Ƿ�ɹ�����Ƶ��ʾ�Ƿ�������GetRect�Ƿ������õ�һ��") {
	CH_Rect_t  rstru_RectS ;//	

	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL,VWINDOW_PARAM_RIGHT);
	
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_PARAM_RIGHT,VWINDOW_TEST_LITTLE_PIXEL);
	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;


#ifdef CFG_CHSSA_HD_SUPPORT	
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,1280,720);
#else
	INIT_RECT(rstru_RectS, VWINDOW_PARAM_ZERO,VWINDOW_PARAM_ZERO,720,576);
#endif

	EXPECT_EQ(CHDRV_VWINDOW_OK,TEST_VWINDOW_SetGetRect(vwin_GetHandle(0),rstru_RectS));
	
	PROMPT("��ǰvwinλ�ñ�����Ϊ(%d, %d, %d, %d),���λ����ʾ�Ƿ���ȷ?���밴Y�������밴N:",
		rstru_RectS.s_X, rstru_RectS.s_Y, rstru_RectS.s_Width, rstru_RectS.s_Height);	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��Ƶλ����ʾ����ȷ,x=" << rstru_RectS.s_X<< ",y="<< rstru_RectS.s_Y << ",w=" << rstru_RectS.s_Width<< ",h=" << rstru_RectS.s_Height;

}
//=========
CASE_F(TEST_VWINDOW_HIDE1,
	"����CHDRV_VWINDOW_Hide,Ӧ���سɹ�����ƵӦ���ɼ�") {
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Hide(vwin_GetHandle(0)));
	PROMPT("��ǰvwinΪ����״̬,����Ƿ񲻿ɼ�?���밴Y�������밴N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "����������Ч";
}

CASE_F(TEST_VWINDOW_SHOW1,
	"����CHDRV_VWINDOW_SHOW,Ӧ���سɹ�����ƵӦ�ɼ�") {
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_Show(vwin_GetHandle(0)));
	PROMPT("��ǰvwinΪ�ɼ�״̬,����Ƿ�ָ���ʾ?���밴Y�������밴N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "��������Ϊ�ɼ���Ч";
}


CASE_F(TEST_VWINDOW_SETIO_AREA,
	"����SetIOArea�����Ƿ�����") {
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
	PROMPT("����������I֡���棬��OK������...");	
	test_input_none();
	
	stru_InRect.s_Width /= 4;
	stru_InRect.s_Height /= 4;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));
	PROMPT("��ǰ����ӦΪ���ϽǷŴ���ʾ?���밴Y�������밴N:");	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "SetIOArea�Ŵ���ʾ��Ч";
	
	stru_InRect.s_X += stru_InRect.s_Width * 3 /4;
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));
	PROMPT("��ǰ����ӦΪ���ϽǷŴ���ʾ?���밴Y�������밴N:");	
	
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "SetIOArea�Ŵ�����������ʾ��Ч";
	memcpy(&stru_InRect, &stru_Rect, sizeof(CH_Rect_t));
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetIOArea(vwin_GetHandle(0),&stru_InRect, &stru_OutRect));

	
}


CASE_F(TEST_VWINDOW_CLOSE,
	"����CHDRV_VWINDOW_Close����ֵ�Ƿ���ȷ") {
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


REGISTER_CMD(vwin_open, "����vwin���", "video_no(0:video1,1:video2)",  "0");
REGISTER_CMD(vwin_close, "ɾ��win���", "video_no",  "0");
REGISTER_CMD(vwin_show, "������Ƶ���Ƿ�ɼ�,0���أ�1��ʾ", "video_no|show",  "0 1");
REGISTER_CMD(vwin_info, "���vwin�������Ϣ����ӡ������", "",  "");
REGISTER_CMD(vwin_set_rect, "������Ƶ����λ��", "vwin_no | x | y | w | h",  "0 0 0 720 576");
REGISTER_CMD(vwin_set_rectex, "������Ƶ����λ��", "vwin_no | x | y | w | h | path_index",  "0 0 0 720 576 0");
REGISTER_CMD(vwin_set_iorect, "���������������", "vwin_no | in_x|in_y|in_w|in_h|out_x|out_y|out_w|out_h",  "0 0 0 720 576 0 0 720 576");



