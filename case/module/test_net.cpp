/**
@file test_net.cpp
@defgroup net测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V1.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestNet
#include "test_menu.h"
#if TEST_NET && ENABLE_TESTCASE
extern int ch_httpget_download(unsigned char *rpuc_URL);



static char guc_netname[16] = "eth0";


static CH_BOOL test_net_ip(void)
{

	int ret;
	const char *str_status[] = {
	"CHMID_TCPIP_LINKDOWN",		/*没有插网线*/
	"CHMID_TCPIP_NOT_SET_IP",			/*没有设置IP*/
	"CHMID_TCPIP_RENEWING_IP",		/*更新IP中*/
	"CHMID_TCPIP_USABLE",				/*网络正常*/
	"CHMID_TCPIP_STATUS_MAX"
	};
	CHMID_TCPIP_STAUTS_e enm_status = CHMID_TCPIP_NOT_SET_IP, enm_status_bk = CHMID_TCPIP_NOT_SET_IP;
	U8 ip[32] = "", mask[32] = "", getway[32] = "";
	char *pc_user_input = NULL;
	U16 us_flags = 0;
	CHMID_TCPIP_DebugCtrl_t stru_DebugCtl = {1, 1};

	
	CHMID_TCPIP_Init();
	CHMID_TCPIP_DebugCtl(&stru_DebugCtl);
	
	PROMPT("请输入网络设备名称(eth0):\n");
	pc_user_input = test_get_input();
	if(pc_user_input[0])
	{
		strcpy((char*)guc_netname, pc_user_input);
	}

	LOG("使用网络设备:%s\n" ,guc_netname );
	EXPECT_EQ(CHMID_TCPIP_NICUp((S8*)guc_netname), CHMID_TCPIP_OK);

	EXPECT_EQ(CHMID_TCPIP_SetIPInfo((S8*)guc_netname, NULL, NULL, NULL), CHMID_TCPIP_OK);
	EXPECT_EQ(CHMID_TCPIP_Extend(CHMID_TCPIP_EX_SET_DHCP_BOOTP_FLAGS, (void *)&us_flags), CHMID_TCPIP_OK);

	while(1)
	{
		EXPECT_EQ(CHMID_TCPIP_GetNetStatusByEth(guc_netname, &enm_status), CHMID_TCPIP_OK);
		if(enm_status != enm_status_bk)
		{
			cout << "net status=" << str_status[enm_status] << "\n";
			enm_status_bk = enm_status;
		}
		if(enm_status == CHMID_TCPIP_USABLE)
		{
			break;
		}
		sleep(1);
	}

	EXPECT_EQ(CHMID_TCPIP_GetIPInfo((S8*)guc_netname, (S8*)ip, (S8*)mask, (S8*)getway), CHMID_TCPIP_OK);

	LOG("IP获取成功\n");
	LOG("IP=%s\n", ip);
	LOG("MASK=%s\n", mask);
	LOG("GATWAY=%s\n", getway);

	return CH_TRUE;
}

CASE(TEST_NET_IP,
	"自动获取IP") {
	EXPECT_EQ(CH_TRUE, test_net_ip());
}


static CH_BOOL test_net_ping(void)
{
	U8 ip[32] = "", mask[32] = "", gatway[32] = "";
	CHMID_TCPIP_Pingcfg_t stru_Pingcfg;
	char *pc_user_input = NULL;

	CHMID_TCPIP_Init();
	PROMPT("请输入网络设备名称(%s):", guc_netname);
	pc_user_input = test_get_input();
	if(pc_user_input[0])
	{
		strcpy(guc_netname, pc_user_input);
	}
	
	EXPECT_EQ(CHMID_TCPIP_GetIPInfo((S8*)guc_netname, (S8*)ip, (S8*)mask, (S8*)gatway), CHMID_TCPIP_OK);
	PROMPT"请输入PING目标的IP地址(" << gatway << "):\n");
	pc_user_input = test_get_input();
	if(pc_user_input[0])
	{
		strcpy((char*)gatway, pc_user_input);
	}
	
	stru_Pingcfg.ui_IP = CHMID_TCPIP_Addr((char*)gatway);
	stru_Pingcfg.i_Datalen = 56;
	stru_Pingcfg.i_TimeoutMs = 5000;
	stru_Pingcfg.i_RepeatTimes = 10;

	EXPECT_EQ(CHMID_TCPIP_Ping(&stru_Pingcfg), CHMID_TCPIP_OK);
	
	printf("Ping test--->ping successful!\n"
		"\tIP: 0x%x(%s)\n"
		"\tDatalen: %d\n"
		"\tTimeoutMs: %d\n"
		"\tTTL: %d\n"
		"\tSuccount: %d\n"
		"\tFailCount: %d\n"
		"\tAvgResTimeMs: %d\n"
		"\tMaxResTimeMs: %d\n"
		"\tMinResTimeMs: %d!\n",
		stru_Pingcfg.ui_IP,
		CHMID_TCPIP_Inetntoa(stru_Pingcfg.ui_IP),
		stru_Pingcfg.i_Datalen,
		stru_Pingcfg.i_TimeoutMs,
		stru_Pingcfg.i_Ttl,
		stru_Pingcfg.i_SuccessCount,
		stru_Pingcfg.i_FailureCount,
		stru_Pingcfg.i_AvgResponseTime,
		stru_Pingcfg.i_MaxResponseTime,
		stru_Pingcfg.i_MinResponseTime);

	return CH_TRUE;
}

CASE(TEST_NET_PING,
	"PING目标机测试") {
	EXPECT_EQ(CH_TRUE, test_net_ping());
}


static CH_BOOL test_net_httpget(void)
{
	char *pc_user_input = NULL;
	U8 url[1024] = "http://192.168.32.104/t.ts";
	U32 time1, time2;
	int size;

	CHMID_TCPIP_Init();
	PROMPT( "请输入目标文件的URL(" << url << "):\n");
	pc_user_input = test_get_input();
	if(pc_user_input[0])
	{
		strcpy((char*)url, pc_user_input);
	}

	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_GT(size = ch_httpget_download(url), 0) << "下载[" << url << "]失败";
	time2 = CH_RTOS_GetSysTimenow();

	printf("download [%s] size %d use time %d ms, speed = %d KByte/s\n",
		url,
		size,
		time2 - time1,
		size / (time2 - time1));
	return CH_TRUE;
}

CASE(TEST_NET_HTTPGET,
	"下载文件测试") {
	EXPECT_EQ(CH_TRUE, test_net_httpget());
}
#endif

