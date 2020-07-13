/**
@file test_fp.cpp
@defgroup fp测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestFp
#include "test_menu.h"


#if ENABLE_TESTCASE

typedef struct
{
	int i_key;
	const char *pc_name;
}Test_KeyMap_t;

static Test_KeyMap_t gstru_KeyMap[] = 
{
	{(int)CHDRV_REM_KEY_0,			"CHDRV_REM_KEY_0"},			///< 数字0                         
	{(int)CHDRV_REM_KEY_1,			"CHDRV_REM_KEY_1"},				///< 数字1         
	{(int)CHDRV_REM_KEY_2,			"CHDRV_REM_KEY_2"},				///< 数字2         
	{(int)CHDRV_REM_KEY_3,			"CHDRV_REM_KEY_3"},				///< 数字3         
	{(int)CHDRV_REM_KEY_4,			"CHDRV_REM_KEY_4"},				///< 数字4         
	{(int)CHDRV_REM_KEY_5,			"CHDRV_REM_KEY_5"},				///< 数字5         
	{(int)CHDRV_REM_KEY_6,			"CHDRV_REM_KEY_6"},				///< 数字6         
	{(int)CHDRV_REM_KEY_7,			"CHDRV_REM_KEY_7"},				///< 数字7         
	{(int)CHDRV_REM_KEY_8,			"CHDRV_REM_KEY_8"},				///< 数字8         
	{(int)CHDRV_REM_KEY_9,			"CHDRV_REM_KEY_9"},				///< 数字9         
	{(int)CHDRV_REM_KEY_MENU,			"CHDRV_REM_KEY_MENU"},		    	///< 菜单按键              
	{(int)CHDRV_REM_KEY_ESC,			"CHDRV_REM_KEY_ESC"},		    	///< 退出按键              
	{(int)CHDRV_REM_KEY_RIGHT,			"CHDRV_REM_KEY_RIGHT"},		    ///< 向右箭头按键              
	{(int)CHDRV_REM_KEY_LEFT,			"CHDRV_REM_KEY_LEFT"},		    	///< 向左箭头按键          
	{(int)CHDRV_REM_KEY_UP,       		"CHDRV_REM_KEY_UP"},       		///< 向上箭头按键          
	{(int)CHDRV_REM_KEY_DOWN,			"CHDRV_REM_KEY_DOWN"},		    	///< 向下箭头按键          
	{(int)CHDRV_REM_KEY_VOLUP,			"CHDRV_REM_KEY_VOLUP"},		    ///< 声音+按键                 
	{(int)CHDRV_REM_KEY_VOLDOWN,			"CHDRV_REM_KEY_VOLDOWN"},			///< 声音-按键     
	{(int)CHDRV_REM_KEY_CHUP,			"CHDRV_REM_KEY_CHUP"},		    	///< 频道+按键             
	{(int)CHDRV_REM_KEY_CHDOWN,			"CHDRV_REM_KEY_CHDOWN"},			///< 频道-按键             
                                                                           
	{(int)CHDRV_REM_KEY_BS,               	"CHDRV_REM_KEY_BS"},                                                
	{(int)CHDRV_REM_KEY_ENTER,			"CHDRV_REM_KEY_ENTER"},		    ///< 确认按键                  
	{(int)CHDRV_REM_KEY_MUTE,			"CHDRV_REM_KEY_MUTE"},		    	///< 静音按键              
	{(int)CHDRV_REM_KEY_POWER,	    		"CHDRV_REM_KEY_POWER"},	    	///< 电源按键                      
	{(int)CHDRV_REM_KEY_AUDIO,	    		"CHDRV_REM_KEY_AUDIO"},	    	///< 声音按键                      
	{(int)CHDRV_REM_KEY_TVRADIO,	    		"CHDRV_REM_KEY_TVRADIO"},	    	///< 电视/广播切换按键     
	{(int)CHDRV_REM_KEY_I,			"CHDRV_REM_KEY_I"},		       	///< 信息快捷键            
   	{(int)CHDRV_REM_KEY_FAV,                 	"CHDRV_REM_KEY_FAV"},              ///< 喜爱节目快捷键              
	{(int)CHDRV_REM_KEY_TIME,	    		"CHDRV_REM_KEY_TIME"},	    		///< 时间按键              
	{(int)CHDRV_REM_KEY_VCR,	    		"CHDRV_REM_KEY_VCR"},	    		///< VCR按键               
	{(int)CHDRV_REM_KEY_SAT ,	    		"CHDRV_REM_KEY_SAT "},	    		///< 天线设置按键          
   	{(int)CHDRV_REM_KEY_REC,		   	"CHDRV_REM_KEY_REC"},				///< 返回按键      
	{(int)CHDRV_REM_KEY_PRGVOL,			"CHDRV_REM_KEY_PRGVOL"},			///< 节目号位数设置按键    
	{(int)CHDRV_REM_KEY_SEARCH,           	"CHDRV_REM_KEY_SEARCH"},           ///< 搜索快捷键                  
	{(int)CHDRV_REM_KEY_EPG,         		"CHDRV_REM_KEY_EPG"},         		///< EPG快捷键             
	{(int)CHDRV_REM_KEY_PICTURE,        		"CHDRV_REM_KEY_PICTURE"},        	///< Code Picture key      
  	{(int)CHDRV_REM_KEY_SUB,                	"CHDRV_REM_KEY_SUB"},              ///< Subtitle按键                
   	{(int)CHDRV_REM_KEY_TXT,         	   	"CHDRV_REM_KEY_TXT"},         		///< Teletext快捷键        
   	{(int)CHDRV_REM_KEY_MOVIE,               	"CHDRV_REM_KEY_MOVIE"},            ///< Movie                       
   	{(int)CHDRV_REM_KEY_VOD,		   	"CHDRV_REM_KEY_VOD"},				///< VOD快捷键     
   	{(int)CHDRV_REM_KEY_STOCK,               	"CHDRV_REM_KEY_STOCK"},            ///< STOCK快捷键                 
   	{(int)CHDRV_REM_KEY_PIP,                 	"CHDRV_REM_KEY_PIP"},              ///< PIP进入按键                 
   	{(int)CHDRV_REM_KEY_PIPUP,               	"CHDRV_REM_KEY_PIPUP"},            ///< PIP节目+按键                
   	{(int)CHDRV_REM_KEY_PIPDOWN,             	"CHDRV_REM_KEY_PIPDOWN"},          ///< PIP节目-按键                
   	{(int)CHDRV_REM_KEY_PIPSWAP,             	"CHDRV_REM_KEY_PIPSWAP"},          ///< 大小画面切换按键            
 	{(int)CHDRV_REM_KEY_F,		 	"CHDRV_REM_KEY_F"},			    ///< 功能键            
	{(int)CHDRV_REM_KEY_RED,         		"CHDRV_REM_KEY_RED"},         		///< 红色按键              
	{(int)CHDRV_REM_KEY_GREEN,          		"CHDRV_REM_KEY_GREEN"},          	///< 绿色按键              
	{(int)CHDRV_REM_KEY_YELLOW,           	"CHDRV_REM_KEY_YELLOW"},           ///< 黄色按键                    
	{(int)CHDRV_REM_KEY_BLUE,           		"CHDRV_REM_KEY_BLUE"},           	///< 兰色按键              
                                                                           
	{(int)CHDRV_REM_KEY_BACK,             	"CHDRV_REM_KEY_BACK"},             ///< 回退                        
	{(int)CHDRV_REM_KEY_PAGDOWN,          	"CHDRV_REM_KEY_PAGDOWN"},          ///< 下页                        
	{(int)CHDRV_REM_KEY_PAGUP,      		"CHDRV_REM_KEY_PAGUP"},      	    ///< 上页                      
	{(int)CHDRV_REM_KEY_CHANNEL, 	        	"CHDRV_REM_KEY_CHANNEL"}, 	        ///< 频道                  
	{(int)CHDRV_REM_KEY_STILLVEDIO,       	"CHDRV_REM_KEY_STILLVEDIO"},       ///< 静像                        
	{(int)CHDRV_REM_KEY_MAIL,	        	"CHDRV_REM_KEY_MAIL"},	            ///< 邮件                      
	{(int)CHDRV_REM_KEY_ZIXUN,			"CHDRV_REM_KEY_ZIXUN"},		    ///< 资讯                      
	{(int)CHDRV_REM_KEY_F1,			"CHDRV_REM_KEY_F1"},		     	///< F1                    
	{(int)CHDRV_REM_KEY_F2,			"CHDRV_REM_KEY_F2"},		        ///< F2                    
	{(int)CHDRV_REM_KEY_F3,			"CHDRV_REM_KEY_F3"},		      	///< F3                    
	{(int)CHDRV_REM_KEY_F4,			"CHDRV_REM_KEY_F4"},		        ///< F4                    
                                                                           
	{(int)CHDRV_REM_KEY_FILELIST,			"CHDRV_REM_KEY_FILELIST"},			///< 文件列表      
	{(int)CHDRV_REM_KEY_ASPECTSW,			"CHDRV_REM_KEY_ASPECTSW"},			///< 视频宽高比切换
	{(int)CHDRV_REM_KEY_FORMATSW,			"CHDRV_REM_KEY_FORMATSW"},			///< 制式切换      
	{(int)CHDRV_REM_KEY_LANGSW,			"CHDRV_REM_KEY_LANGSW"},			///< 语言切换              
	{(int)CHDRV_REM_KEY_HELP,			"CHDRV_REM_KEY_HELP"},				///< 帮助          
	{(int)CHDRV_REM_KEY_DESTINE,			"CHDRV_REM_KEY_DESTINE"},			///< 预定          
	{(int)CHDRV_REM_KEY_CC,			"CHDRV_REM_KEY_CC"},				///< CC*/          
	{(int)CHDRV_REM_KEY_POUND,			"CHDRV_REM_KEY_POUND"},			///< \#键                  
	{(int)CHDRV_REM_KEY_STAR,			"CHDRV_REM_KEY_STAR"},				///< *键           
	{(int)CHDRV_REM_KEY_PLUS,			"CHDRV_REM_KEY_PLUS"},				///< +键           
                                                                           
	{(int)CHDRV_REM_KEY_PVRRECORD,		"CHDRV_REM_KEY_PVRRECORD"},		///< 录制                  
	{(int)CHDRV_REM_KEY_PVRPAUSE,			"CHDRV_REM_KEY_PVRPAUSE"},			///< 暂停          
	{(int)CHDRV_REM_KEY_PVRPLAY,			"CHDRV_REM_KEY_PVRPLAY"},			///< 回放          
	{(int)CHDRV_REM_KEY_PVRSTOP,			"CHDRV_REM_KEY_PVRSTOP"},			///< 停止          
	{(int)CHDRV_REM_KEY_PVRFF,			"CHDRV_REM_KEY_PVRFF"},			///< 快进                  
	{(int)CHDRV_REM_KEY_PVRFW,			"CHDRV_REM_KEY_PVRFW"},			///< 快退                  
	{(int)CHDRV_REM_KEY_PVRSLOWPLAY,		"CHDRV_REM_KEY_PVRSLOWPLAY"},		///< 慢放                  
	{(int)CHDRV_REM_KEY_PVRTIMESHIFT,		"CHDRV_REM_KEY_PVRTIMESHIFT"},		///< 时移                  
	{(int)CHDRV_REM_KEY_PVRBEFORE,		"CHDRV_REM_KEY_PVRBEFORE"},		///< 上一个                
	{(int)CHDRV_REM_KEY_PVRNEXT,			"CHDRV_REM_KEY_PVRNEXT"},			///< 下一个        
                                                                           
                                                                           
	/*以下为面板按键*/              	/*以下为面板按键*/                                                 
	{(int)CHDRV_FP_KEY_UP,			"CHDRV_FP_KEY_UP"},			///< 向上箭头按键          
	{(int)CHDRV_FP_KEY_DOWN,			"CHDRV_FP_KEY_DOWN"},				///< 向下箭头按键  
	{(int)CHDRV_FP_KEY_LEFT,			"CHDRV_FP_KEY_LEFT"},				///< 向左箭头按键  
	{(int)CHDRV_FP_KEY_RIGHT,			"CHDRV_FP_KEY_RIGHT"},				///< 向右箭头按键  
	{(int)CHDRV_FP_KEY_MENU,			"CHDRV_FP_KEY_MENU"},				///< 菜单按键      
	{(int)CHDRV_FP_KEY_OK,			"CHDRV_FP_KEY_OK"},				///< 确定按键      
	{(int)CHDRV_FP_KEY_EXIT,			"CHDRV_FP_KEY_EXIT"},				///< 退出按键      
	{(int)CHDRV_FP_KEY_POWER,			"CHDRV_FP_KEY_POWER"},				///< 电源          
	{(int)CHDRV_FP_KEY_AUDIO,			"CHDRV_FP_KEY_AUDIO"},				///< 声道按键      
                                                                           
	/*组合键*/                      	/*组合键*/                                                         
	{(int)CHDRV_FP_KEY_COMBO_0,           	"CHDRV_FP_KEY_COMBO_0"},                                            
	{(int)CHDRV_FP_KEY_COMBO_1 ,          	"CHDRV_FP_KEY_COMBO_1 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_2 ,          	"CHDRV_FP_KEY_COMBO_2 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_3 ,          	"CHDRV_FP_KEY_COMBO_3 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_4 ,          	"CHDRV_FP_KEY_COMBO_4 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_5 ,          	"CHDRV_FP_KEY_COMBO_5 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_6 ,          	"CHDRV_FP_KEY_COMBO_6 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_7 ,          	"CHDRV_FP_KEY_COMBO_7 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_8 ,          	"CHDRV_FP_KEY_COMBO_8 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_9 ,          	"CHDRV_FP_KEY_COMBO_9 "},                                           
	{(int)CHDRV_FP_KEY_COMBO_10 ,         	"CHDRV_FP_KEY_COMBO_10 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_11 ,         	"CHDRV_FP_KEY_COMBO_11 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_12 ,         	"CHDRV_FP_KEY_COMBO_12 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_13 ,         	"CHDRV_FP_KEY_COMBO_13 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_14 ,         	"CHDRV_FP_KEY_COMBO_14 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_15 ,         	"CHDRV_FP_KEY_COMBO_15 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_16 ,         	"CHDRV_FP_KEY_COMBO_16 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_17 ,         	"CHDRV_FP_KEY_COMBO_17 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_18 ,         	"CHDRV_FP_KEY_COMBO_18 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_19 ,         	"CHDRV_FP_KEY_COMBO_19 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_20 ,         	"CHDRV_FP_KEY_COMBO_20 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_21 ,         	"CHDRV_FP_KEY_COMBO_21 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_22 ,         	"CHDRV_FP_KEY_COMBO_22 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_23 ,         	"CHDRV_FP_KEY_COMBO_23 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_24 ,         	"CHDRV_FP_KEY_COMBO_24 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_25 ,         	"CHDRV_FP_KEY_COMBO_25 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_26 ,         	"CHDRV_FP_KEY_COMBO_26 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_27 ,         	"CHDRV_FP_KEY_COMBO_27 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_28 ,         	"CHDRV_FP_KEY_COMBO_28 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_29 ,         	"CHDRV_FP_KEY_COMBO_29 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_30 ,         	"CHDRV_FP_KEY_COMBO_30 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_31 ,         	"CHDRV_FP_KEY_COMBO_31 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_32 ,         	"CHDRV_FP_KEY_COMBO_32 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_33 ,         	"CHDRV_FP_KEY_COMBO_33 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_34 ,         	"CHDRV_FP_KEY_COMBO_34 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_35 ,         	"CHDRV_FP_KEY_COMBO_35 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_36 ,         	"CHDRV_FP_KEY_COMBO_36 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_37 ,         	"CHDRV_FP_KEY_COMBO_37 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_38 ,         	"CHDRV_FP_KEY_COMBO_38 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_39 ,         	"CHDRV_FP_KEY_COMBO_39 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_40 ,         	"CHDRV_FP_KEY_COMBO_40 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_41 ,         	"CHDRV_FP_KEY_COMBO_41 "},                                          
	{(int)CHDRV_FP_KEY_COMBO_42 ,         	"CHDRV_FP_KEY_COMBO_42 "},                                          
	{(-1) ,         						"UNKOWN"}
};

static CH_RTOS_MessageQHandle gh_KeyMes = NULL;
static int gi_ir_task_run = 0;
static CHDRV_FP_LAMP_TYPE_e genm_lamp = CHDRV_FP_LED_RED;
static void test_fp_led_task(void *p)
{
	CHDRV_FP_LAMP_TYPE_e enum_lamp;
	
	CH_RTOS_TaskEnter(NULL);
	while(gi_ir_task_run)
	{
		enum_lamp = genm_lamp;
		if(enum_lamp < CHDRV_FP_LED_TYPE_MAX)
		{
			EXPECT_EQ(CHDRV_FP_CtrlLamp(enum_lamp, CHDRV_FP_LAMP_ON), CHDRV_FP_OK);
			CH_RTOS_TaskDelay(200);
			EXPECT_EQ(CHDRV_FP_CtrlLamp(enum_lamp, CHDRV_FP_LAMP_OFF) , CHDRV_FP_OK);
			CH_RTOS_TaskDelay(200);
		}
		else
		{
			CH_RTOS_TaskDelay(200);
		}
	}
	CH_RTOS_TaskExit();
}

static CH_BOOL test_fp_led_01(void)
{
 	CHDRV_FP_LAMP_TYPE_e enm_lamp[] = {CHDRV_FP_LED_RED, CHDRV_FP_LED_YELLOW, CHDRV_FP_LED_GREEN, CHDRV_FP_LED_COLON}; 
	char *pc_name[] = {"红灯", "黄灯", "绿灯", "冒号"}, *pc_user_input = NULL;
	int i;
	CH_RTOS_TaskHandle handle = 0;

	gi_ir_task_run = 1;
	genm_lamp = CHDRV_FP_LED_TYPE_MAX;
	EXPECT_EQ(CH_RTOS_CreateTask(test_fp_led_task, NULL, NULL, 1024 * 8, 200, (S8*)"test_fp_led_task", &handle),
		CH_RTOS_OK);
	for(i = 0; i < 4; i++)
	{
		genm_lamp = CHDRV_FP_LED_TYPE_MAX;
		switch(i)
		{
			case CHDRV_FP_LED_RED:
			if(fw_getenv_i("SUPPORT_RED_LED") == 0)
			{
				continue;
			}
			break;
			case CHDRV_FP_LED_YELLOW:
			if(fw_getenv_i("SUPPORT_YELLOW_LED") == 0)
			{
				continue;
			}
			break;
			case CHDRV_FP_LED_GREEN:
			if(fw_getenv_i("SUPPORT_GREEN_LED") == 0)
			{
				continue;
			}
			break;
			case CHDRV_FP_LED_COLON:
			if(fw_getenv_i("SUPPORT_8_SEGMENT_LED") == 0)
			{
				continue;
			}
			break;
		}
		genm_lamp = enm_lamp[i];
		PROMPT("观察前面板，%s是否在闪烁,如未装配请直接回车(y/n/q)?[y]", pc_name[i]);
		pc_user_input = test_get_input();
		EXPECT_NE(pc_user_input[0], 'n') << pc_name[i] << "未正确闪烁";
	}
	gi_ir_task_run = 0;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(handle, 2000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(handle), CH_RTOS_OK);
	return CH_TRUE;
}


CASE(TEST_FP_LED1,
	"测试LED灯闪烁") {
	
	if(fw_getenv_i("SUPPORT_RED_LED") 
		||fw_getenv_i("SUPPORT_YELLOW_LED") 
		||fw_getenv_i("SUPPORT_GREEN_LED")  
		|| fw_getenv_i("SUPPORT_8_SEGMENT_LED"))
	{
		EXPECT_EQ(CH_TRUE, test_fp_led_01());
	}
}


static CH_BOOL test_fp_led_02(char *pstr)
{
	char *pc_user_input = NULL;
 	EXPECT_EQ(CHDRV_FP_SetDispString(1, strlen(pstr), (U8 *) pstr), CHDRV_FP_OK)
		<< "显示[" << pstr << "]失败";

	PROMPT("观察前面板，数码管显示[%s]是否正确,如未装配数码管请直接回车(y/n/q)?[y]", pstr);
	pc_user_input = test_get_input();
	EXPECT_NE(pc_user_input[0], 'n') << pstr << "未正确显示";
	
	return CH_TRUE;
}


CASE(TEST_FP_LED2,
	"测试数码管显示88:88") {
	if(fw_getenv_i("SUPPORT_8_SEGMENT_LED"))
	{
		EXPECT_EQ(CHDRV_FP_CtrlLamp(CHDRV_FP_LED_COLON, CHDRV_FP_LAMP_ON), CHDRV_FP_OK);
		EXPECT_EQ(CH_TRUE, test_fp_led_02("8888"));
	}
}

CASE(TEST_FP_LED3,
	"测试数码管显示0000") {
	if(fw_getenv_i("SUPPORT_8_SEGMENT_LED"))
	{
		EXPECT_EQ(CHDRV_FP_CtrlLamp(CHDRV_FP_LED_COLON, CHDRV_FP_LAMP_OFF), CHDRV_FP_OK);
		EXPECT_EQ(CH_TRUE, test_fp_led_02("0000"));
	}
}

CASE(TEST_FP_LED4,
	"测试数码管无显示") {
	if(fw_getenv_i("SUPPORT_8_SEGMENT_LED"))
	{
		EXPECT_EQ(CHDRV_FP_CtrlLamp(CHDRV_FP_LED_COLON, CHDRV_FP_LAMP_OFF), CHDRV_FP_OK);
		EXPECT_EQ(CH_TRUE, test_fp_led_02("    "));
	}
}

static CH_BOOL test_fp_led_03(void)
{
	char *pc_user_input = NULL;
	char string[5], i;
	
	while(1)
	{
		printf("1. LED测试通过;\n");
		printf("2. LED字符串显示失败;\n");
		printf("3. 输入任意字符串并显示在前面板上;\n");
		PROMPT("请选择(1/2/3):[1]");
		pc_user_input = test_get_input();
		if(pc_user_input[0] == 0
			|| pc_user_input[0] == '1')
		{
			return CH_TRUE;
		}
		else if(pc_user_input[0] == '2')
		{
			EXPECT_NE(pc_user_input[0], '2') << "测试员认为显示字符串[" << string << "]错误";
			return CH_FALSE;
		}
		PROMPT("请输入4个测试字符后回车:");
		pc_user_input = test_get_input();
		memset(string, ' ', sizeof(string));
		string[4] = 0;

		for(i = 0; i < 4 && pc_user_input[i] != 0; i++)
		{
			string[i] = pc_user_input[i];
		}
		test_fp_led_02(string);
	}

	return CH_TRUE;
}

CASE(TEST_FP_LED5,
	"测试数码管显示任意字符串") {
	if(fw_getenv_i("SUPPORT_8_SEGMENT_LED"))
	{
		EXPECT_EQ(CHDRV_FP_CtrlLamp(CHDRV_FP_LED_COLON, CHDRV_FP_LAMP_OFF), CHDRV_FP_OK);
		EXPECT_EQ(CH_TRUE, test_fp_led_03());
	}
}


static void cb_func(CHDRV_FP_Key_t rstru_key)
{
	if(gh_KeyMes)
	{
		EXPECT_EQ(CH_RTOS_SendMessage(gh_KeyMes, &rstru_key, 1000), CH_RTOS_OK);
	}
}

static CHDRV_FP_RESULT_e test_fp_GetKey(CHDRV_FP_Key_t *rpstru_KeyInfo,S32 ri_TimeOutMS )
{
	if(!gh_KeyMes)
	{
		CH_RTOS_TaskDelay(100);
		return CHDRV_FP_FAIL;
	}
	if(CH_RTOS_ReceiveMessage(gh_KeyMes, rpstru_KeyInfo, ri_TimeOutMS) == CH_RTOS_OK)
	{
		return CHDRV_FP_OK;
	}

	return CHDRV_FP_FAIL;
}

static char *test_fp_key_name(CHDRV_FP_KEYCODE_e renm_KeyValu)
{
	int i;

	for(i = 0; gstru_KeyMap[i].i_key >= 0; i++)
	{
		if(gstru_KeyMap[i].i_key == (int)renm_KeyValu)
		{
			break;
		}
	}
	return (char*)gstru_KeyMap[i].pc_name;
}

static void test_fp_task1(void *p)
{
	CHDRV_FP_Key_t stru_KeyInfo;
	
	CH_RTOS_TaskEnter(NULL);
	
	while(gi_ir_task_run)
	{
		if(test_fp_GetKey(&stru_KeyInfo, 200) == CHDRV_FP_OK)
		{
			printf("enm_KeyCode=%s[0x%x], ui_RepeatCounter=%d\n", 
				test_fp_key_name(stru_KeyInfo.enm_KeyCode), 
				stru_KeyInfo.enm_KeyCode, 
				stru_KeyInfo.ui_RepeatCounter);
			printf("所有按键测试完毕后，再输入本项的测试结果(y/n)?[y]\n");
		}
	}
	
	CH_RTOS_TaskExit();
}


static CH_BOOL test_fp_ir_01(void)
{
	CH_RTOS_TaskHandle handle = NULL;
	char *pc_user_input = NULL;
	
	gi_ir_task_run = 1;
	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(CHDRV_FP_Key_t), 20, &gh_KeyMes), CH_RTOS_OK);
	EXPECT_EQ(CHDRV_FP_InstallCallback(CHDRV_FP_INPUT_KEY, cb_func), CHDRV_FP_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_fp_task1, NULL, NULL, 1024 * 8, 200, (S8*)"test_fp_ir_01", &handle),
		CH_RTOS_OK);
	PROMPT("[按键回调]请按前面板按键或遥控器按键，观察输出按键值是否正确\n");
	PROMPT("所有按键测试完毕后，再输入本项的测试结果(y/n)?[y]\n");
	pc_user_input = test_get_input();
	EXPECT_NE(pc_user_input[0], 'n')
		<< "测试员认为此项不合格";
	
	gi_ir_task_run = 0;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(handle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(handle), CH_RTOS_OK);
	EXPECT_EQ(CHDRV_FP_UninstallCallback(CHDRV_FP_INPUT_KEY, cb_func), CHDRV_FP_OK);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(gh_KeyMes), CH_RTOS_OK);
	return CH_TRUE;
}


CASE(TEST_FP_IR1,
	"测试FP按键回调") {
	EXPECT_EQ(CH_TRUE, test_fp_ir_01());
}

static void test_fp_task2(void *p)
{
	CHDRV_FP_Key_t stru_KeyInfo;
	
	CH_RTOS_TaskEnter(NULL);
	
	while(gi_ir_task_run)
	{
		if(CHDRV_FP_GetKey(&stru_KeyInfo, 200) == CHDRV_FP_OK)
		{
			printf("enm_KeyCode=%s[0x%x], ui_RepeatCounter=%d\n", 
				test_fp_key_name(stru_KeyInfo.enm_KeyCode), 
				stru_KeyInfo.enm_KeyCode, 
				stru_KeyInfo.ui_RepeatCounter);
			printf("所有按键测试完毕后，再输入本项的测试结果(y/n)?[y]\n");
		}
	}
	
	CH_RTOS_TaskExit();
}

static CH_BOOL test_fp_ir_02(void)
{
	CH_RTOS_TaskHandle handle = NULL;
	char *pc_user_input = NULL;
	
	gi_ir_task_run = 1;
	EXPECT_EQ(CH_RTOS_CreateTask(test_fp_task2, NULL, NULL, 1024 * 8, 200, (S8*)"test_fp_ir_02", &handle),
		CH_RTOS_OK);
	PROMPT("[主动GET按键]请按前面板按键或遥控器按键，观察输出按键值是否正确\n");
	PROMPT("所有按键测试完毕后，再输入本项的测试结果(y/n)?[y]\n");
	
	pc_user_input = test_get_input();
	EXPECT_NE(pc_user_input[0], 'n')
		<< "测试员认为此项不合格";
	
	gi_ir_task_run = 0;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(handle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(handle), CH_RTOS_OK);
	return CH_TRUE;
}


CASE(TEST_FP_IR2,
	"测试主动读取按键CHDRV_FP_GetKey()") {
	EXPECT_EQ(CH_TRUE, test_fp_ir_02());
}
#endif /* ENABLE_TESTCASE */









