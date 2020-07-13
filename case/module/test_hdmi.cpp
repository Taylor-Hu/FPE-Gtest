/**
@file test_hdmi.cpp
@def group hdmi测试集

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
#define CASE_NAME TestHdmi


#include "test_menu.h"
#include "test_modules.h"

/*********************************全局变量定义***********************************/

static char *event_name[] = 
	{
		"CH_HDMI_EVENT_HOTPLUG",			    ///< HDMI插入
    		"CH_HDMI_EVENT_NO_PLUG",			    ///< HDMI拔出
    		"CH_HDMI_EVENT_HDCP_ON_SUCCESS",      ///< HDCP功能打开并认证成功
    		"CH_HDMI_EVENT_HDCP_NOT_SUPPORT", 	///< HDCP功能不支持
    		"CH_HDMI_EVENT_HDCP_AUTH_FAIL",       ///< HDCP功能打开时认证错误
    		"CH_HDMI_EVENT_HDCP_AUTH_IN_PROCESS", ///< 正在认证中
    		"CH_HDMI_EVENT_HDCP_ON_OTHER_ERR", 	///< HDCP功能打开时其他错误
    		"CH_HDMI_EVENT_HDCP_OFF_SUCCESS",     ///< HDCP功能关闭时成功
    		"CH_HDMI_EVENT_HDCP_OFF_FAIL",        ///< HDCP功能关闭时错误
    		"CH_HDMI_EVENT_EVENT_MAX",
	};


static U16	g_count_event= 0;              //回调收到插入通知计数
static U16	gt_setHDCP_count= 0;        //回调收到DHCP认证成功计数
static U16	gf_setHDCP_count= 0;	  //回调收到DHCP认证失败计数
static U16	g_closeHDCP_count= 0;	 //回调收到DHCP关闭通知计数


static  char *VIDEO_FORMAT_name[] = 
	{
	"CH_HDMI_VIDEO_FMT_NONE",  						///< 无效值
	"CH_HDMI_VIDEO_FMT_640X480P_60_4_3",   		       ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_720X480P_60_4_3",                      ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9 
	"CH_HDMI_VIDEO_FMT_720X480P_60_16_9",                     ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27 
	"CH_HDMI_VIDEO_FMT_1280X720P_60_16_9",                   ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_1920X1080I_60_16_9",                   ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_720_1440X480I_60_4_3",               ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9 
	"CH_HDMI_VIDEO_FMT_720_1440X480I_60_16_9",              ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27 
	"CH_HDMI_VIDEO_FMT_720_1440X240P_60_4_3",               ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 4:9 
	"CH_HDMI_VIDEO_FMT_720_1440X240P_60_16_9",              ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 16:27 
	"CH_HDMI_VIDEO_FMT_2880X480I_60_4_3",                     ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 2:9~20:9^2
	"CH_HDMI_VIDEO_FMT_2880X480I_60_16_9",                    ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 8:27~80:27^2
	"CH_HDMI_VIDEO_FMT_2880X240P_60_4_3",                      ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 1:9~10:9^2
	"CH_HDMI_VIDEO_FMT_2880X240P_60_16_9",                     ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 4:27~40:27^2
	"CH_HDMI_VIDEO_FMT_1440X480P_60_4_3",                      ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 4:9 or 8:9^3    
	"CH_HDMI_VIDEO_FMT_1440X480P_60_16_9",                     ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 16:27 or 32:27^3
	"CH_HDMI_VIDEO_FMT_1920X1080P_60_16_9",                    ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_720X576P_50_4_3",                        ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15 
	"CH_HDMI_VIDEO_FMT_720X576P_50_16_9",                        ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45 
	"CH_HDMI_VIDEO_FMT_1280X720P_50_16_9",                        ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080I_50_16_9",                     ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_720_1440X576I_50_4_3",                  ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15 
	"CH_HDMI_VIDEO_FMT_720_1440X576I_50_16_9",                 ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45 
	"CH_HDMI_VIDEO_FMT_720_1440X288P_50_4_3",                 ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:15 
	"CH_HDMI_VIDEO_FMT_720_1440X288P_50_16_9",                ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 32:45 
	"CH_HDMI_VIDEO_FMT_2880X576I_50_4_3",                          ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 2:15~20:15^2
	"CH_HDMI_VIDEO_FMT_2880X576I_50_16_9",                          ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 16:45~160:45^2
	"CH_HDMI_VIDEO_FMT_2880X288P_50_4_3",                           ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 1:15 ~10:15^2
	"CH_HDMI_VIDEO_FMT_2880X288P_50_16_9",                         ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 8:45 ~80:45^2
	"CH_HDMI_VIDEO_FMT_1440X576P_50_4_3",                           ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:15 or 16:15^3
	"CH_HDMI_VIDEO_FMT_1440X576P_50_16_9",                          ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 32:45 or 64:45^3
	"CH_HDMI_VIDEO_FMT_1920X1080P_50_16_9",                        ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080P_24_16_9",                         ///< FiledRate: 23.97/24Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080P_25_16_9",                          ///< FiledRate: 25Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080P_30_16_9",                           ///< FiledRate: 29.97/30Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_2880X480P_60_4_3",                             ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 4:3,  Pixel AR(H:V): 2:9,4:9,or 8:9^4
	"CH_HDMI_VIDEO_FMT_2880X480P_60_16_9",                            ///< FiledRate: 59.94/60Hz, Picture AR(H:V): 16:9, Pixel AR(H:V): 8:27,16:27,or 32:27^4
	"CH_HDMI_VIDEO_FMT_2880X576P_50_4_3",                               ///< FiledRate: 50Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 4:15,8:15,or 16:15^4
	"CH_HDMI_VIDEO_FMT_2880X576P_50_16_9",                              ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 16:45,32:45,or 64:45^4
	"CH_HDMI_VIDEO_FMT_1920X1080I_1250_50_16_9",                     ///< FiledRate: 50Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080I_100_16_9",                            ///< FiledRate: 100Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1280X720P_100_16_9",                              ///< FiledRate: 100Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_720X576P_100_4_3",                                ///< FiledRate: 100Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15 
	"CH_HDMI_VIDEO_FMT_720X576P_100_16_9",                               ///< FiledRate: 100Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45
	"CH_HDMI_VIDEO_FMT_720_1440X576I_100_4_3",                        ///< FiledRate: 100Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15 
	"CH_HDMI_VIDEO_FMT_720_1440X576I_100_16_9",                        ///< FiledRate: 100Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45 
	"CH_HDMI_VIDEO_FMT_1920X1080I_120_16_9",                            ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1 
	"CH_HDMI_VIDEO_FMT_1280X720P_120_16_9",                             ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1    
	"CH_HDMI_VIDEO_FMT_720X480P_120_4_3",                                ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9
	"CH_HDMI_VIDEO_FMT_720X480P_120_16_9",                              ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27
	"CH_HDMI_VIDEO_FMT_720_1440X480I_120_4_3",                         ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9 
	"CH_HDMI_VIDEO_FMT_720_1440X480I_120_16_9",                       ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27
	"CH_HDMI_VIDEO_FMT_720X576P_200_4_3",                                 ///< FiledRate: 200Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15
	"CH_HDMI_VIDEO_FMT_720X576P_200_16_9",                             ///< FiledRate: 200Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45
	"CH_HDMI_VIDEO_FMT_720_1440X576I_200_4_3",                         ///< FiledRate: 200Hz,       Picture AR(H:V): 4:3,  Pixel AR(H:V): 16:15
	"CH_HDMI_VIDEO_FMT_720_1440X576I_200_16_9",                     ///< FiledRate: 200Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 64:45
	"CH_HDMI_VIDEO_FMT_720X480P_240_4_3",                             ///< FiledRate: 239.76/240Hz,Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9
	"CH_HDMI_VIDEO_FMT_720X480P_240_16_9",                             ///< FiledRate: 239.76/240Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27
	"CH_HDMI_VIDEO_FMT_720_1440X480I_240_4_3",                          ///< FiledRate: 239.76/240Hz,Picture AR(H:V): 4:3,  Pixel AR(H:V): 8:9
	"CH_HDMI_VIDEO_FMT_720_1440X480I_240_16_9",                        ///< FiledRate: 239.76/240Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 32:27
	"CH_HDMI_VIDEO_FMT_1280X720P_24_16_9",               		   ///< FiledRate: 23.97/24Hz,  Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1    
	"CH_HDMI_VIDEO_FMT_1280X720P_25_16_9",               			 ///< FiledRate: 25Hz,        Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1    
	"CH_HDMI_VIDEO_FMT_1280X720P_30_16_9",             		   ///< FiledRate: 29.97/30Hz,  Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1    
	"CH_HDMI_VIDEO_FMT_1920X1080P_120_16_9",            		    ///< FiledRate: 119.88/120Hz,Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_1920X1080P_100_16_9",            		   ///< FiledRate: 100Hz,       Picture AR(H:V): 16:9, Pixel AR(H:V): 1:1
	"CH_HDMI_VIDEO_FMT_MAX",
	};


static  char *CH_HDMI_SinkCapability[] = 
	{
	"设备是否正常连接",
    	"设备是否上电(TMDS)",
    	"EDID是否从显示设备获取标志",
    	"设备厂商标识",
  	"设备ID",
    	"设备序列号",
  	"设备生产日期(周)",
	"设备生产日期(年)",
    	"设备版本号",
    	"设备子版本号",
   	"EDID扩展块数目",
    	"是否支持YCBCR显示",
   	"是否支持xvYCC601颜色格式",
    	"是否支持xvYCC709颜色格式",
 	"xvYCC601支持的传输",
	"显示设备物理分辨率",
	"视频能力集",
    	"音频能力集",
	"PCM音频采样率能力集",
	"音频最大的PCM通道数",
	"扬声器位置",
 	"24比特的HDMI标志位",
  	"CEC物理地址是否有效",
 	"CEC物理地址A",
    	"CEC物理地址B",
   	"CEC物理地址C",
   	"CEC物理地址D",
	"是否支持 DVI dual-link",
	"是否支持 YCBCR 4:4:4",
	"是否支持Deep Color 30bit",
    	"是否支持Deep Color 36bit",
  	"是否支持Deep Color 48bit",
	"是否支持 Supports_AI 模式",
    	"最大TMDS时钟",
   	"延时标志位",
  	"视频和音频延时场",
    	"特殊的视频格式",
   	"视频延时	",
    	"音频延时",
  	"隔行视频下的视频延时",
   	"隔行视频下的音频延时",
	};

static  char *CH_HDMI_AUDIO_name[] = 
	{
    	"CH_HDMI_AUDIO_FMT_NONE",       			 ///< 无效值
     	"CH_HDMI_AUDIO_FMT_LINEAR_PCM",        ///< Linear PCM (e.g.,IEC 60958)
      	"CH_HDMI_AUDIO_FMT_AC3",      		  ///< AC-3
    	"CH_HDMI_AUDIO_FMT_MPEG1",      	  ///< MPEG1(Layer 1 & 2)
    	"CH_HDMI_AUDIO_FMT_MP3",       		 ///< MP3 (MPEG1 Layer 3)
    	"CH_HDMI_AUDIO_FMT_MPEG2",      	  ///< MPEG1(multichannel)
   	"CH_HDMI_AUDIO_FMT_AAC",       		 ///< AAC
   	"CH_HDMI_AUDIO_FMT_DTS",       		 ///< DTS
   	"CH_HDMI_AUDIO_FMT_ATRAC",      	  ///< ATRAC
    	"CH_HDMI_AUDIO_FMT_ONE_BIT_AUDIO",        ///< one bit audio
   	"CH_HDMI_AUDIO_FMT_DD_PLUS",     	  ///< Dolby Digital +
   	"CH_HDMI_AUDIO_FMT_DTS_HD",     	  ///< DTS-HD
   	"CH_HDMI_AUDIO_FMT_MAT_MLP",   	    ///< MAT(MLP)
   	"CH_HDMI_AUDIO_FMT_DST",      		 ///< DST
	"CH_HDMI_AUDIO_FMT_WMA_PRO",       ///< WMA Pro
	"CH_HDMI_AUDIO_FMT_MAX",
	};

/***************************** 全局变量定义结束****************************************/

void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;

int prognum = 0;

prognum = request_play_program("AV_STREAM_COMMON", "TUNER_PARAMS1");
RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));


RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);

#if defined(CFG_CHSSA_CHIP_MSTAR7SX5B)
EXPECT_EQ(CH_HDMI_OK, CHDRV_HDCP_Start());	
#endif
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit pvrn rtos TestCase " << std::endl;
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

CASE_F(TEST_HDMI_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CH_HDMI_OK, CH_HDMI_Init());
	EXPECT_EQ(CH_HDMI_OK, CH_HDMI_Init()) << "多次初始化应该返回OK";
}

CASE_F(TEST_HDMI_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CH_HDMI_OK, CH_HDMI_Term());
	EXPECT_EQ(CH_HDMI_OK, CH_HDMI_Init());
}


CASE_F(TEST_HDMI_INIT3,
	"测试CH_HDMI_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_HDMI_OK, CH_HDMI_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}


CASE_F(TEST_HDMI_OPEN,
	"测试调用OPEN HDMI是否成功"){
	EXPECT_EQ(CH_HDMI_Open(CH_HDMI_ID_0), CH_HDMI_OK);
}



static void test_hdmi_callback1(
		CH_HDMI_ID_e 			renm_HdmiID,
		CH_HDMI_EVENT_TYPE_e renm_event1, 
		void *rpv_EventData, 
		void *rpv_PrivateData)
{
	
	if(renm_event1 < CH_HDMI_EVENT_EVENT_MAX)
	{
		LOG("event=%s\n", event_name[renm_event1]);
		if(renm_event1== CH_HDMI_EVENT_HOTPLUG)
		{
			g_count_event ++;
			LOG("收到第一个插入通知\n");
		}
		if(renm_event1== CH_HDMI_EVENT_HDCP_ON_SUCCESS)
		{
			gt_setHDCP_count++;
			LOG("第一个HDCP设置成功\n");
		}
		if(renm_event1== CH_HDMI_EVENT_HDCP_AUTH_FAIL)
		{
			gf_setHDCP_count++;
			LOG("第一个HDCP设置失败通知\n");
		}
		if(renm_event1== CH_HDMI_EVENT_HDCP_OFF_SUCCESS)
		{
			g_closeHDCP_count++;
			LOG("第一个关闭DHCP 通知\n");
		}
	}
	else
	{
		LOG( "ERROR:test_hdmi_callback=%s\n" , renm_event1);
	}

}



static void test_hdmi_callback2(
		CH_HDMI_ID_e 			renm_HdmiID,
		CH_HDMI_EVENT_TYPE_e renm_event2, 
		void *rpv_EventData, 
		void *rpv_PrivateData)
{
	if(renm_event2 < CH_HDMI_EVENT_EVENT_MAX)
	{
		LOG("event=%s\n", event_name[renm_event2]);
		if(renm_event2== CH_HDMI_EVENT_HOTPLUG)
		{
		g_count_event ++;
		LOG("收到第2 个插入通知\n");
		}
		if(renm_event2== CH_HDMI_EVENT_HDCP_ON_SUCCESS)
		{
			gt_setHDCP_count++;
			LOG("第2 个HDCP设置成功\n");
		}
		if(renm_event2== CH_HDMI_EVENT_HDCP_AUTH_FAIL)
		{
			gf_setHDCP_count++;
			LOG("第二个HDCP设置失败通知\n");
		}
		if(renm_event2== CH_HDMI_EVENT_HDCP_OFF_SUCCESS)
		{
			g_closeHDCP_count++;
			LOG("第二个关闭DHCP 通知\n");
		}
	}
	else
	{
		LOG( "ERROR:test_hdmi_callback=%s\n" , renm_event2);
	}

}




static void test_hdmi_callback3(
		CH_HDMI_ID_e 			renm_HdmiID,
		CH_HDMI_EVENT_TYPE_e renm_event3, 
		void *rpv_EventData, 
		void *rpv_PrivateData)
{
	if(renm_event3 < CH_HDMI_EVENT_EVENT_MAX)
	{
		LOG("event=%s\n", event_name[renm_event3]);
		if(renm_event3== CH_HDMI_EVENT_HOTPLUG)
		{
			g_count_event ++;
			LOG("收到第3 个插入通知\n");
		}
		if(renm_event3== CH_HDMI_EVENT_HDCP_ON_SUCCESS)
		{
			gt_setHDCP_count++;
			LOG("第3 个HDCP设置成功\n");
		}
		if(renm_event3== CH_HDMI_EVENT_HDCP_AUTH_FAIL)
		{
			gf_setHDCP_count++;
			LOG("第三个HDCP设置失败通知\n");
		}
		if(renm_event3== CH_HDMI_EVENT_HDCP_OFF_SUCCESS)
		{
			g_closeHDCP_count++;
			LOG("第三个关闭DHCP 通知\n");
		}
	}
	else
	{
		LOG( "ERROR:test_hdmi_callback=%s\n" , renm_event3);
	}

}



static CH_BOOL  test_hdmi_SetAttribute1(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t  rpstru_SetAttrib;
	CH_HDMI_Attribute_t  rpstru_GetAttrib;

	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_FALSE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";
	return CH_TRUE;
}



static CH_BOOL  test_hdmi_SetAttribute2(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID = (CH_HDMI_ID_e)0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_TRUE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";

	return CH_TRUE;
}



static CH_BOOL  test_hdmi_SetAttribute3(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID = (CH_HDMI_ID_e)0;
	rpstru_SetAttrib.b_HdmiEnable = CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable = CH_FALSE;
	rpstru_SetAttrib.b_CareHdcpAuthFail = CH_TRUE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";

	return CH_TRUE;
}



static CH_BOOL  test_hdmi_SetAttribute4(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_FALSE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_TRUE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI是否无输出?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数b_HdmiEnable为FALSE，HDMI应关闭输出";

	return CH_TRUE;
}


static CH_BOOL  test_hdmi_SetAttribute5(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_FALSE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";
	return CH_TRUE;
}



static CH_BOOL  test_hdmi_SetAttribute6(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID = CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_TRUE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_HdcpEnable);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";
	return CH_TRUE;
}



static CH_BOOL  test_hdmi_SetAttribute7(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_FALSE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_FALSE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI输出正常?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数设置正确，HDMI应正常输出";
	return CH_TRUE;
}


static CH_BOOL  test_hdmi_SetAttribute8(void)
{
	CH_HDMI_ID_e renm_HdmiID;
	CH_HDMI_Attribute_t 	rpstru_SetAttrib;
	CH_HDMI_Attribute_t 	rpstru_GetAttrib;

	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_FALSE;
	rpstru_SetAttrib.b_HdcpEnable= CH_FALSE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_FALSE;

	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_SetAttrib.b_HdmiEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_SetAttrib.b_HdcpEnable);
	EXPECT_EQ(rpstru_GetAttrib.b_CareHdcpAuthFail, rpstru_SetAttrib.b_CareHdcpAuthFail);
	PROMPT("请确认当前HDMI是否无输出?是请按Y否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "参数b_HdmiEnable为FALSE，HDMI应关闭输出";

	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_FALSE;
	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	return CH_TRUE;
}




static CH_BOOL   test_hdmi_GetSinkCapability()
{
	CH_HDMI_SinkCapability_t 	rpstru_Capability;
	int i = 0;
	
       memset(&rpstru_Capability, 0, sizeof(rpstru_Capability));
	EXPECT_EQ(CH_HDMI_GetSinkCapability(CH_HDMI_ID_0, &rpstru_Capability), CH_HDMI_OK) <<"获取能力集应该成功返回CH_HDMI_OK";


	PROMPT(" 以下为获取到的能力集列表；请仔细核对，如果无误请按y \n");
	
	
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[0], rpstru_Capability.b_Connected);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[1], rpstru_Capability.b_IsSinkPowerOn);
    	LOG(" %-30s %d\n",  CH_HDMI_SinkCapability[2], rpstru_Capability.b_IsRealEDID);
    	LOG(" %-30s %c%c%c%c \n", CH_HDMI_SinkCapability[3], 
		 rpstru_Capability.uc_IDManufactureName[0], rpstru_Capability.uc_IDManufactureName[1],
		 rpstru_Capability.uc_IDManufactureName[2], rpstru_Capability.uc_IDManufactureName[3]);
  	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[4], rpstru_Capability.ui_IDProductCode);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[5], rpstru_Capability.ui_IDSerialNumber);
  	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[6], rpstru_Capability.ui_WeekOfManufacture);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[7], rpstru_Capability.ui_YearOfManufacture);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[8], rpstru_Capability.uc_Version);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[9], rpstru_Capability.uc_Revision);
   	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[10], rpstru_Capability.uc_EDIDExternBlockNum);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[11], rpstru_Capability.b_SupportYCbCr);
   	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[12], rpstru_Capability.b_SupportxvYCC601);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[13], rpstru_Capability.b_SupportxvYCC709);
 	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[14], rpstru_Capability.uc_MDBit);
	LOG(" %-30s %s \n", CH_HDMI_SinkCapability[15], VIDEO_FORMAT_name[rpstru_Capability.enm_NativeVideoFormat]);
	while(rpstru_Capability.enm_VideoFmtSupported[i]!= CH_HDMI_VIDEO_FMT_NONE)
	{
		LOG(" %-30s %s\n", CH_HDMI_SinkCapability[16], VIDEO_FORMAT_name[rpstru_Capability.enm_VideoFmtSupported[i]]);
		i++;
	};
	i= 0;
    	while(rpstru_Capability.enm_AudioFmtSupported[i]!= CH_HDMI_AUDIO_FMT_NONE)
	{
		 LOG(" %-30s%s \n", CH_HDMI_SinkCapability[17], CH_HDMI_AUDIO_name[rpstru_Capability.enm_AudioFmtSupported[i]]);
		 i++;
	};
   	i= 0;
	for(i=0; i<CH_HDMI_AUDIO_SMPRATE_COUNT_MAX; i++)
	{
		LOG(" %-30s%d \n", CH_HDMI_SinkCapability[18], rpstru_Capability.ui_AudioSampleRateSupported[i]);
		i++;
	};

	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[19], rpstru_Capability.ui_MaxPcmChannels);
	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[20], rpstru_Capability.uc_Speaker);
       LOG(" %-*s %d%d%d \n", 30, CH_HDMI_SinkCapability[21], rpstru_Capability.uc_IEERegId[0],rpstru_Capability.uc_IEERegId[1],rpstru_Capability.uc_IEERegId[2]);
  	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[22], rpstru_Capability.b_IsPhyAddrValid);
 	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[23], rpstru_Capability.uc_PhyAddr_A);
    	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[24], rpstru_Capability.uc_PhyAddr_B);
   	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[25], rpstru_Capability.uc_PhyAddr_C);
   	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[26], rpstru_Capability.uc_PhyAddr_D);
	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[27], rpstru_Capability.b_SupportDVIDual);
	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[28], rpstru_Capability.b_SupportDeepColorYCBCR444);
	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[29], rpstru_Capability.b_SupportDeepColor30Bit);
    	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[30], rpstru_Capability.b_SupportDeepColor36Bit);
  	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[31], rpstru_Capability.b_SupportDeepColor48Bit);
	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[32], rpstru_Capability.b_SupportAI);
    	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[33], rpstru_Capability.ui_MaxTMDSClock);
   	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[34], rpstru_Capability.b_I_Latency_Fields_Present);
  	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[35], rpstru_Capability.b_Latency_Fields_Present);
    	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[36], rpstru_Capability.b_HDMI_Video_Present);
   	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[37], rpstru_Capability.uc_Video_Latency);
    	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[38], rpstru_Capability.uc_Audio_Latency);
  	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[39], rpstru_Capability.uc_Interlaced_Video_Latency);
   	LOG(" %-*s %d \n", 30, CH_HDMI_SinkCapability[40], rpstru_Capability.uc_Interlaced_Audio_Latency);

	EXPECT_EQ(test_input_yes(), CH_TRUE)  <<  "获取的能力集不正确";
	
	EXPECT_NE(rpstru_Capability.enm_VideoFmtSupported[0],  CH_HDMI_VIDEO_FMT_NONE) << "视频格式不能为无效值";
	EXPECT_NE(rpstru_Capability.enm_AudioFmtSupported[0],  CH_HDMI_AUDIO_FMT_NONE) << "音频格式不能为无效值";
	return CH_TRUE;
}


static CH_BOOL  test_hdmi_plug(void)
{
	U16 i;
	g_count_event= 0;              //重置HDMI回调通知计数

	for(i= 0; i<1; i++)
	{
		g_count_event= 0;              //重置HDMI回调通知计数
		PROMPT("请您拔出HDMI连接线，等待1S后插入,然后回车");
		test_get_input();
		EXPECT_EQ(g_count_event,3) << "HDMI连接电视应该收到三个插入回调通知";
	}
	return CH_TRUE;
}




CASE_F(TEST_HDMI_CALL1,
	"测试安装3个不同的回调函数是否成功"){
	EXPECT_EQ(CH_HDMI_InstallCallback(test_hdmi_callback1, NULL), CH_HDMI_OK)
	 << "回调函数1 注册成功应返回CH_HDMI_OK";
	EXPECT_EQ(CH_HDMI_InstallCallback(test_hdmi_callback2, NULL), CH_HDMI_OK)
	 << "回调函数2 注册成功应返回CH_HDMI_OK";
	EXPECT_EQ(CH_HDMI_InstallCallback(test_hdmi_callback3, NULL), CH_HDMI_OK)
	 << "回调函数3 注册成功应返回CH_HDMI_OK";
}



CASE_F(TEST_HDMI_CALL2,
	"提示测试员插入HDMI连接电视，是否3个回调均收到了插入通知"){
	int retry=0;
	PROMPT("请重新插拔HDMI连接到电视，准备好后回车\n");
	test_get_input();
	while(gt_setHDCP_count < 3 && retry++ < 20)
	{
		CH_RTOS_TaskDelay(500);
	}	
	EXPECT_EQ(g_count_event,3 ) << "重新拔插后应该收到三个插入通知";

}


CASE_F(TEST_HDMI_KEY1,
	"调用LoadHDCPKey设置一组正确的HDCP KEY，应成功，回调收到认证成功通知"){

	char *pbuf = NULL;
	int len;
	CH_HDMI_Attribute_t  rpstru_SetAttrib;
	CH_HDMI_Attribute_t  rpstru_GetAttrib;
	CH_HDMI_ID_e renm_HdmiID;
	int retry=0;
	/*设置b_HdmiEnable enable和b_HdcpEnable enable*/
	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_TRUE;
	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);

	EXPECT_EQ(CH_HDMI_GetAttribute(renm_HdmiID, &rpstru_GetAttrib), CH_HDMI_OK);
	EXPECT_EQ(rpstru_GetAttrib.b_HdmiEnable, rpstru_GetAttrib.b_HdmiEnable)<<"b_HdmiEnable未设置正确";
	EXPECT_EQ(rpstru_GetAttrib.b_HdcpEnable, rpstru_GetAttrib.b_HdcpEnable)<<"b_HdcpEnable未设置正确";
	
	len = fsys_read_file(fw_getenv("DHCPKEY"), pbuf);
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << fw_getenv("DHCPKEY");
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	EXPECT_GT(len, 0)<< "读取DHCPKEY失败，请先配置";
	EXPECT_NE(pbuf, (void*)NULL)<< "读取DHCPKEY失败，请先配置";
	if(len > 0 && pbuf)
	{
		CH_HDMI_HdcpKey_t 	rpstru_HdcpKey;
		rpstru_HdcpKey.puc_HdcpKey=(U8*) pbuf;
		rpstru_HdcpKey.ui_KeyLength= len;
		
	        EXPECT_EQ(CH_HDMI_LoadHDCPKey(CH_HDMI_ID_0 , &rpstru_HdcpKey), CH_HDMI_OK)
			<< "设置正确HDCP KEY失败" ;
#if defined(CFG_CHSSA_CHIP_MSTAR7SX5B)
		EXPECT_EQ(CH_HDMI_OK, CHDRV_HDCP_Start());
#endif /* CFG_CHSSA_CHIP_MSTAR7CX5 */
		while(gt_setHDCP_count < 3 && retry++<20)
		{
			CH_RTOS_TaskDelay(500);
		}
		LOG("gt_setHDCP_count = %d \r\n",gt_setHDCP_count);
		EXPECT_EQ(gt_setHDCP_count, 3) << "应该收到三个HDCP KEY认证成功通知";
		CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	 }
}


CASE_F(TEST_HDMI_HDCP1,
	"确认HDMI输出应正常，获取属性与设置的一致"){
	EXPECT_EQ(test_hdmi_SetAttribute1(), CH_TRUE);
}


CASE_F(TEST_HDMI_HDCP2,
	"确认HDMI输出应正常，获取属性与设置的一致"){
	EXPECT_EQ(test_hdmi_SetAttribute2(), CH_TRUE);
}


CASE_F(TEST_HDMI_HDCP3,
	"确认HDMI输出应正常并收到回调通知，获取属性与设置的一致"){
	g_closeHDCP_count = 0;
	EXPECT_EQ(test_hdmi_SetAttribute3(), CH_TRUE);
	int retry=0;
	while(gt_setHDCP_count < 3 && retry++<20)
	{
		CH_RTOS_TaskDelay(500);
	}	
	EXPECT_EQ(g_closeHDCP_count,3) << "应该收到三个DHCP关闭通知";	
}


CASE_F(TEST_HDMI_HDCP4,
	"确认HDMI 无输出，获取属性与设置的一致"){
	EXPECT_EQ(test_hdmi_SetAttribute4(), CH_TRUE);
}



CASE_F(TEST_HDMI_KEY2,
	"调用LoadHDCPKey设置一组错误的HDCP KEY，应成功，回调收到认证失败通知"){
	
	char *pbuf = NULL;
	int len;
	int retry = 0;
	len = fsys_read_file(fw_getenv("DHCPKEY2"), pbuf);
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << fw_getenv("DHCPKEY2");
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	EXPECT_LT(0, len)<< "读取DHCPKEY2失败，请先配置";
	EXPECT_NE(pbuf, (void*)NULL)<< "读取DHCPKEY2失败，请先配置";
	if(len > 0 && pbuf)
	{
		CH_HDMI_HdcpKey_t 	rpstru_HdcpKey;
		rpstru_HdcpKey.puc_HdcpKey= (U8*)pbuf;
		rpstru_HdcpKey.ui_KeyLength= len;

		 EXPECT_EQ(CH_HDMI_LoadHDCPKey(CH_HDMI_ID_0 , &rpstru_HdcpKey), CH_HDMI_OK)
			<< "设置错误HDCP KEY失败" ;
		PROMPT("请重新插拔HDMI连接到电视，准备好后回车\n");
		while(gf_setHDCP_count < 3 && retry++<20)
		{
			CH_RTOS_TaskDelay(500);
		}		 
		 EXPECT_EQ(gf_setHDCP_count, 3) << "应该收到三个HDCP KEY设置失败通知";
		 CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	}
}



CASE_F(TEST_HDMI_HDCP5,
	"确认HDMI输出应正常，获取属性与设置的一致"){
	EXPECT_EQ(test_hdmi_SetAttribute5(), CH_TRUE);
}


CASE_F(TEST_HDMI_HDCP6,
	"确认HDMI 无输出，获取属性与设置的一致"){
	char *pbuf = NULL;
	int len;
	CH_HDMI_Attribute_t  rpstru_SetAttrib;
	CH_HDMI_Attribute_t  rpstru_GetAttrib;
	CH_HDMI_ID_e renm_HdmiID;
	int retry=0;
	renm_HdmiID= CH_HDMI_ID_0;
	rpstru_SetAttrib.b_HdmiEnable= CH_TRUE;
	rpstru_SetAttrib.b_HdcpEnable= CH_TRUE;
	rpstru_SetAttrib.b_CareHdcpAuthFail= CH_TRUE;
	EXPECT_EQ(CH_HDMI_SetAttribute(renm_HdmiID, &rpstru_SetAttrib), CH_HDMI_OK);
	len = fsys_read_file(fw_getenv("DHCPKEY"), pbuf);
	if(len == -1)
	{
		FAIL() << "file not exsit,file=" << fw_getenv("DHCPKEY");
	}
	else if(len < -1)
	{
		FAIL() << "can not malloc memory,size=" << -len;
	}
	EXPECT_GT(len, 0)<< "读取DHCPKEY失败，请先配置";
	EXPECT_NE(pbuf, (void*)NULL)<< "读取DHCPKEY失败，请先配置";
	if(len > 0 && pbuf)
	{
		CH_HDMI_HdcpKey_t	rpstru_HdcpKey;
		rpstru_HdcpKey.puc_HdcpKey=(U8*) pbuf;
		rpstru_HdcpKey.ui_KeyLength= len;
		EXPECT_EQ(CH_HDMI_LoadHDCPKey(CH_HDMI_ID_0 , &rpstru_HdcpKey), CH_HDMI_OK)
			<< "设置正确HDCP KEY失败" ;
#if defined(CFG_CHSSA_CHIP_MSTAR7SX5B)
		EXPECT_EQ(CH_HDMI_OK, CHDRV_HDCP_Start());
#endif /* CFG_CHSSA_CHIP_MSTAR7CX5 */
		CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	 }
	EXPECT_EQ(test_hdmi_SetAttribute6(), CH_TRUE);
}


CASE_F(TEST_HDMI_HDCP7,
	"确认HDMI输出应正常并收到回调函数，获取属性与设置的一致"){
	int retry=0;
	g_closeHDCP_count = 0;
	EXPECT_EQ(test_hdmi_SetAttribute7(), CH_TRUE);
		while(gf_setHDCP_count < 3 && retry++ < 20)
		{
			CH_RTOS_TaskDelay(500);
		}
	EXPECT_EQ(g_closeHDCP_count,3) << "应该收到三个DHCP关闭通知";	
}


CASE_F(TEST_HDMI_HDCP8,
	"确认HDMI 无输出，获取属性与设置的一致"){
	EXPECT_EQ(test_hdmi_SetAttribute8(), CH_TRUE);
}


CASE_F(TEST_HDMI_HDCP9,
	"测试GetSinkCapability应成功，能力集获取正确"){
	EXPECT_EQ(test_hdmi_GetSinkCapability(), CH_TRUE);
}

CASE_F(TEST_HDMI_PLUG,
	"测试反复拔插4次，HDMI均应工作正常，回调正常"){
	EXPECT_EQ(test_hdmi_plug(), CH_TRUE);
}


CASE_F(TEST_HDMI_CALL3,
	"测试卸载所有回调，应成功"){
	EXPECT_EQ(CH_HDMI_UnInstallCallback(test_hdmi_callback1), CH_HDMI_OK)
	 << "回调函数1卸载成功返回CH_HDMI_OK";
	EXPECT_EQ(CH_HDMI_UnInstallCallback(test_hdmi_callback2), CH_HDMI_OK)
	 << "回调函数2 卸载成功返回CH_HDMI_OK";
	EXPECT_EQ(CH_HDMI_UnInstallCallback(test_hdmi_callback3), CH_HDMI_OK)
	 << "回调函数3 卸载成功返回CH_HDMI_OK";
}


CASE_F(TEST_HDMI_CLOSE,
	"测试CLOSE HDMI，应成功"){
	EXPECT_EQ(CH_HDMI_Close(CH_HDMI_ID_0), CH_HDMI_OK);
}
#endif /* ENABLE_TESTCASE */



int hdmi_init(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CH_HDMI_Init();
	LOG("CH_HDMI_Init return %d\n", ret);
	return ret;
}


int hdmi_term(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CH_HDMI_Term();
	LOG("CH_HDMI_Term return %d\n", ret);
	return ret;
}


int hdmi_ver(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = 0x20102;     //CH_HDMI_VERSION;
	LOG("HDMI版本号为: 0x%x\n", ret);
	return ret;
}


int hdmi_open(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CH_HDMI_Open(CH_HDMI_ID_0);
	LOG("CH_HDMI_Open return: %d\n", ret);
	return ret;
}



int hdmi_close(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CH_HDMI_Close(CH_HDMI_ID_0);
	LOG("CH_HDMI_Close return: %d\n", ret);
	return ret;
}


int hdmi_set_hdcp(int argc, CMD_PARAM argv[])
{
 	int ret;
	CH_HDMI_Attribute_t 	rpstru_Attrib;

	rpstru_Attrib.b_HdmiEnable= argv[2].get_b();
	rpstru_Attrib.b_HdcpEnable= argv[3].get_b();
	rpstru_Attrib.b_CareHdcpAuthFail= argv[4].get_b();

	ret = CH_HDMI_SetAttribute((CH_HDMI_ID_e)argv[1].get_i(), &rpstru_Attrib);
	LOG("hdmi_set_hdcp return: %d\n", ret);
	return ret;
}

static int  hdmi_load_key(int argc, CMD_PARAM argv[])
{
	int ret;
	char *pbuf = NULL;
	int len;
	len = fsys_read_file(fw_getenv("DHCPKEY"), pbuf);
	CH_HDMI_HdcpKey_t 	rpstru_HdcpKey;
	rpstru_HdcpKey.puc_HdcpKey= (U8*)pbuf;
	rpstru_HdcpKey.ui_KeyLength= len;

	ret= CH_HDMI_LoadHDCPKey((CH_HDMI_ID_e)argv[1].get_i(), &rpstru_HdcpKey);
	LOG("CH_HDMI_LoadHDCPKey return: %d\n", ret);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	return ret;
}


int hdmi_get_sink(int argc, CMD_PARAM argv[])
{
 	int ret;
	int i= 0;
	CH_HDMI_SinkCapability_t rpstru_Capability;

	memset(&rpstru_Capability, 0, sizeof(rpstru_Capability));

	EXPECT_EQ(CH_HDMI_OK, ret = CH_HDMI_GetSinkCapability((CH_HDMI_ID_e)argv[1].get_i(), &rpstru_Capability));
	LOG( "以下为获取到的能力集列表；请仔细核对\n");

	
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[0], rpstru_Capability.b_Connected);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[1], rpstru_Capability.b_IsSinkPowerOn);
    	LOG(" %-30s %d\n",  CH_HDMI_SinkCapability[2], rpstru_Capability.b_IsRealEDID);
    	LOG(" %-30s %c%c%c%c \n", CH_HDMI_SinkCapability[3], 
		 rpstru_Capability.uc_IDManufactureName[0], rpstru_Capability.uc_IDManufactureName[1],
		 rpstru_Capability.uc_IDManufactureName[2], rpstru_Capability.uc_IDManufactureName[3]);
  	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[4], rpstru_Capability.ui_IDProductCode);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[5], rpstru_Capability.ui_IDSerialNumber);
  	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[6], rpstru_Capability.ui_WeekOfManufacture);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[7], rpstru_Capability.ui_YearOfManufacture);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[8], rpstru_Capability.uc_Version);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[9], rpstru_Capability.uc_Revision);
   	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[10], rpstru_Capability.uc_EDIDExternBlockNum);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[11], rpstru_Capability.b_SupportYCbCr);
   	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[12], rpstru_Capability.b_SupportxvYCC601);
    	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[13], rpstru_Capability.b_SupportxvYCC709);
 	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[14], rpstru_Capability.uc_MDBit);
	LOG(" %-30s %s \n", CH_HDMI_SinkCapability[15], VIDEO_FORMAT_name[rpstru_Capability.enm_NativeVideoFormat]);
	while(rpstru_Capability.enm_VideoFmtSupported[i]!= CH_HDMI_VIDEO_FMT_NONE)
	{
		LOG("  %-30s %s \n", CH_HDMI_SinkCapability[16], VIDEO_FORMAT_name[rpstru_Capability.enm_AudioFmtSupported[i]]);
		i++;
	};
	i= 0;
    	while(rpstru_Capability.enm_AudioFmtSupported[i]!= CH_HDMI_AUDIO_FMT_NONE)
	{
		LOG("  %-30s %s \n", CH_HDMI_SinkCapability[17], CH_HDMI_AUDIO_name[rpstru_Capability.enm_AudioFmtSupported[i]]);
		i++;
	};
   	i= 0;
	for(i=0; i<CH_HDMI_AUDIO_SMPRATE_COUNT_MAX; i++)
	{
		LOG("  %-30s%d \n", CH_HDMI_SinkCapability[18], rpstru_Capability.ui_AudioSampleRateSupported[i]);
		i++;
	};
 	
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[19], rpstru_Capability.ui_MaxPcmChannels);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[20], rpstru_Capability.uc_Speaker);
	LOG(" %-30s %d%d%d \n", CH_HDMI_SinkCapability[21], rpstru_Capability.uc_IEERegId[0],rpstru_Capability.uc_IEERegId[1],rpstru_Capability.uc_IEERegId[2]);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[22], rpstru_Capability.b_IsPhyAddrValid);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[23], rpstru_Capability.uc_PhyAddr_A);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[24], rpstru_Capability.uc_PhyAddr_B);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[25], rpstru_Capability.uc_PhyAddr_C);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[26], rpstru_Capability.uc_PhyAddr_D);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[27], rpstru_Capability.b_SupportDVIDual);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[28], rpstru_Capability.b_SupportDeepColorYCBCR444);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[29], rpstru_Capability.b_SupportDeepColor30Bit);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[30], rpstru_Capability.b_SupportDeepColor36Bit);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[31], rpstru_Capability.b_SupportDeepColor48Bit);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[32], rpstru_Capability.b_SupportAI);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[33], rpstru_Capability.ui_MaxTMDSClock);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[34], rpstru_Capability.b_I_Latency_Fields_Present);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[35], rpstru_Capability.b_Latency_Fields_Present);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[36], rpstru_Capability.b_HDMI_Video_Present);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[37], rpstru_Capability.uc_Video_Latency);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[38], rpstru_Capability.uc_Audio_Latency);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[39], rpstru_Capability.uc_Interlaced_Video_Latency);
	LOG(" %-30s %d \n", CH_HDMI_SinkCapability[40], rpstru_Capability.uc_Interlaced_Audio_Latency);



	EXPECT_NE(rpstru_Capability.enm_VideoFmtSupported[0],  CH_HDMI_VIDEO_FMT_NONE);
	EXPECT_NE(rpstru_Capability.enm_AudioFmtSupported[0],  CH_HDMI_AUDIO_FMT_NONE);		   
	return ret;
}



REGISTER_CMD(hdmi_init,  "打开HDMI", "", "");
REGISTER_CMD(hdmi_term, "销毁hdmi模块", "",  "");
REGISTER_CMD(hdmi_ver,"打印hdmi模块版本号","","");
REGISTER_CMD(hdmi_open,"打开hdmi模块","","");
REGISTER_CMD(hdmi_close,"关闭hdmi模块","","");
REGISTER_CMD(hdmi_set_hdcp,"设置hdmi是否允许输出，是否打开HDCP，认证失败后是否允许输出","device_id|hdmi.on|hdcp_on|output_when_fail","0 1 1 1");
REGISTER_CMD(hdmi_load_key,"导入hdcp key","device_id|file_path","0 c:/mstar_7c75_hdcp_key.bin");
REGISTER_CMD(hdmi_get_sink,"打印hdmi输出设备的能力集","device_id","0");

