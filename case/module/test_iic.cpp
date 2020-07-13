/**
@file test_iic.cpp
@def group iic测试集

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
#define CASE_NAME TestIIC


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner rtos TestCase " << std::endl;
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
CASE_F(TEST_IIC_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Init());
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Init()) << "多次初始化应该返回OK";
}



CASE_F(TEST_IIC_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Term());
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Init());
}


CASE_F(TEST_IIC_INIT3,
	"测试CHDRV_IIC_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}
#endif /* ENABLE_TESTCASE */

#if 0//很多平台都没有使用IIC模块了
CASE_F(TEST_IIC_DETECT,
	"获取所有通道iic（0~3），地址从0x2~0xff，扫描可用iic地址，可用地址不小于1个，不大于20个") {
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	 U32                 Dev,Loop,Length;
	 CHDRV_IIC_RESULT_e ret;
	 U8 data[4];
	 int i_valid_addr = 0;
	memset(&stru_iic_info, 0, sizeof(stru_iic_info));
	 /* Check all the i2c peripherals */
	 /* ----------------------------- */
	 for (Dev=0;Dev<3;Dev++)
	  {
	   for (Loop=0x2;Loop<0xFF;Loop+=2)
	    {	
	    	LOG("write	%d		0x%2x		+", Dev,Loop);
	    	data[0] = 0;
	    	
		 stru_iic_info.ui_BusId = Dev;
		 stru_iic_info.uc_SlaveAddr = Loop;
		 stru_iic_info.ui_SubAddr = 0;
		 stru_iic_info.puc_Data = &data[0];
		 stru_iic_info.ui_Length = 1;
		 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
		 stru_iic_info.enm_SubAddrLen = CHDRV_IIC_ONE_SUBADDR;
	       ret = CHDRV_IIC_Write(&stru_iic_info);
	       printf("%s\n", ret ? "fail" : "  OK");	
	       if(ret == 0)
		 {
			i_valid_addr ++;
		 }
		 LOG("read	%d		0x%2x		+", Dev,Loop);
	       ret = CHDRV_IIC_Read(&stru_iic_info);
		printf("%s\n", ret ? "fail" : "  OK");			
	     }
	  } 

	  EXPECT_GT(i_valid_addr, 0) << "可用的IIC地址为0";
}

CASE_F(TEST_IIC_READ1,
	"测试iic读一个字节是否成功	") {
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 data = 0;
	 stru_iic_info.ui_BusId = 0;
	 stru_iic_info.uc_SlaveAddr = fw_getenv_i("IIC_CHIP_ADDR");
	 stru_iic_info.ui_SubAddr = 0;
	 stru_iic_info.puc_Data = &data;
	 stru_iic_info.ui_Length = 1;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Read(&stru_iic_info));
}
CASE_F(TEST_IIC_READ2,
	"测试iic读四个字节是否成功	") {
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 Data[4]={0};
	 stru_iic_info.ui_BusId = 0;
	 stru_iic_info.uc_SlaveAddr = fw_getenv_i("IIC_CHIP_ADDR");
	 stru_iic_info.ui_SubAddr = 0;
	 stru_iic_info.puc_Data = Data;
	 stru_iic_info.ui_Length = 4;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Read(&stru_iic_info));
}

CASE_F(TEST_IIC_write1,
	"测试iic写一个字节是否成功	") {
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 Data=0;
	 stru_iic_info.ui_BusId = 0;
	 stru_iic_info.uc_SlaveAddr = fw_getenv_i("IIC_CHIP_ADDR");
	 stru_iic_info.ui_SubAddr = 0;
	 stru_iic_info.puc_Data = &Data;
	 stru_iic_info.ui_Length = 1;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Write(&stru_iic_info));
}
CASE_F(TEST_IIC_write2,
	"测试iic写一个字节是否成功	") {
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 Data[4]={0};
	 stru_iic_info.ui_BusId = 0;
	 stru_iic_info.uc_SlaveAddr = fw_getenv_i("IIC_CHIP_ADDR");
	 stru_iic_info.ui_SubAddr = 0;
	 stru_iic_info.puc_Data = Data;
	 stru_iic_info.ui_Length = 1;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	
	EXPECT_EQ(CHDRV_IIC_OK, CHDRV_IIC_Write(&stru_iic_info));
}
#endif
static int iic_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_IIC_Init();
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Init return %d\n", ret);
		return ret;
	}
	
	return 0;
}
static int iic_term(int argc, CMD_PARAM argv[])
{
		int ret;

	ret = CHDRV_IIC_Term();
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Term return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int iic_ver(int argc, CMD_PARAM argv[])
{

	int ret;
	CHDRV_Version_t stru_Ver;
	
	ret = CHDRV_IIC_GetVersion(&stru_Ver);
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_GetVersion return %d\n", ret);
		return ret;
	}
	LOG("version=0x%x\n",stru_Ver);
	return 0;
}
static int iic_read_1(int argc, CMD_PARAM argv[])
{
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 data = 0;
	int ret;
	
	 stru_iic_info.ui_BusId = argv[1].get_i();
	 stru_iic_info.uc_SlaveAddr = argv[2].get_i();
	 stru_iic_info.ui_SubAddr = argv[3].get_i();
	 stru_iic_info.puc_Data = &data;
	 stru_iic_info.ui_Length = 1;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	  ret = CHDRV_IIC_Read(&stru_iic_info);
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Read return %d\n", ret);
		return ret;
	}
	LOG("iic_read_1 data=0x%x\n",data);
	return 0;
}

static int iic_read_4(int argc, CMD_PARAM argv[])
{
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 data[4] = {0};
	int ret;
	
	 stru_iic_info.ui_BusId = argv[1].get_i();
	 stru_iic_info.uc_SlaveAddr = argv[2].get_i();
	 stru_iic_info.ui_SubAddr = argv[3].get_i();
	 stru_iic_info.puc_Data = data;
	 stru_iic_info.ui_Length = 4;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	  ret = CHDRV_IIC_Read(&stru_iic_info);
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Read return %d\n", ret);
		return ret;
	}
	LOG("iic_read_4 data[0]=0x%x  data[1]=0x%x   data[2]=0x%x  data[3]=0x%x\n",data[0],data[1],data[2],data[3]);
	return 0;
}
static int iic_write_1(int argc, CMD_PARAM argv[])
{
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 data = 0x20;
	int ret;
	
	 stru_iic_info.ui_BusId = argv[1].get_i();
	 stru_iic_info.uc_SlaveAddr = argv[2].get_i();
	 stru_iic_info.ui_SubAddr = argv[3].get_i();
	 stru_iic_info.puc_Data = &data;
	 stru_iic_info.ui_Length = 1;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	  ret = CHDRV_IIC_Write(&stru_iic_info);
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Write return %d\n", ret);
		return ret;
	}
	LOG("iic_write_1 data=0x%x\n",data);
	return 0;
}

static int iic_write_4(int argc, CMD_PARAM argv[])
{
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	U8 data[4] = {0};
	int ret;
	data[0] = (U8)(argv[4].get_i()>>24)&0xff;
	data[1] = (U8)(argv[4].get_i()>>16)&0xff;
	data[2] = (U8)(argv[4].get_i()>>8)&0xff;
	data[3] = (U8)(argv[4].get_i())&0xff;	
	
	 stru_iic_info.ui_BusId = argv[1].get_i();
	 stru_iic_info.uc_SlaveAddr = argv[2].get_i();
	 stru_iic_info.ui_SubAddr = argv[3].get_i();
	 stru_iic_info.puc_Data = data;
	 stru_iic_info.ui_Length = 4;
	 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	  ret = CHDRV_IIC_Write(&stru_iic_info);
	if(ret != CHDRV_IIC_OK)
	{
		LOG("ERROR: CHDRV_IIC_Write return %d\n", ret);
		return ret;
	}
	LOG("iic_write_4 data[0]=0x%x  data[1]=0x%x   data[2]=0x%x  data[3]=0x%x\n",data[0],data[1],data[2],data[3]);	
	return 0;
}
static int iic_detect(int argc, CMD_PARAM argv[])
{
	CHDRV_IIC_SET_INFO_t  stru_iic_info;
	 U32                 Dev,Loop;
	 CHDRV_IIC_RESULT_e ret;
	 U8 data = 0;
	 int i = 0;
 	U8 addr_ok[256][2];

	memset(addr_ok, 0, sizeof(addr_ok));
	memset(&stru_iic_info, 0, sizeof(stru_iic_info));
	 /* Check all the i2c peripherals */
	 /* ----------------------------- */
	 for (Dev=0;Dev<3;Dev++)
	  {
	   for (Loop=0x2;Loop<0xFF;Loop+=2)
	    {	
		 stru_iic_info.ui_BusId = Dev;
		 stru_iic_info.uc_SlaveAddr = Loop;
		 stru_iic_info.ui_SubAddr = 0;
		 stru_iic_info.puc_Data = &data;
		 stru_iic_info.ui_Length = 1;
		 stru_iic_info.ui_Timeout = 1000;/*CH_RTOS_TIMEOUT_INFINITY;*/
	       ret = CHDRV_IIC_Write(&stru_iic_info);
		  if(!ret)  
		{
			LOG("+	%d		0x%2x		Write OK		+", Dev,Loop);	
			addr_ok[i][0] = Dev;
			addr_ok[i][1] = Loop;
			i++;
		 }
/*		 stru_iic_info.ui_BusId = Dev;
		 stru_iic_info.uc_SlaveAddr = Loop;
		 stru_iic_info.ui_SubAddr = 0;
		 stru_iic_info.puc_Data = 0;
		 stru_iic_info.ui_Length = 1;
		 stru_iic_info.ui_Timeout = 1000; yys20161102*/
	       ret = CHDRV_IIC_Read(&stru_iic_info);
		 if(!ret)  
		{
			LOG("+	%d		0x%2x		read OK		+\n", Dev,Loop);	
		 }	
	     }
	  }

	 LOG("all useable iic addr is below:");
	 for(Loop = 0; Loop < i; Loop++)
	 {
		LOG("Dev=%d, addr=0x%x", addr_ok[Loop][0], addr_ok[Loop][1]);
	 }
	 return 0;
}
REGISTER_CMD(iic_init, "初始化iic模块", "",  "");
REGISTER_CMD(iic_term, "销毁iic模块", "",  "");
REGISTER_CMD(iic_ver, "打印iic模块版本号", "",  "");
REGISTER_CMD(iic_read_1, "读一个字节并打印结果", "channel_no| iic.addr |reg_addr",  "0 0xc0 0x00");
REGISTER_CMD(iic_read_4, "读四个字节并打印结果", "channel_no| iic.addr |reg_addr",  "0 0xc0 0x00");
REGISTER_CMD(iic_write_1, "写一个字节", "channel_no| iic.addr |reg_addr |data",  "0 0xc0  0x20 0x01");
REGISTER_CMD(iic_write_4, "写四个字节", "channel_no| iic.addr |reg_addr |data",  "0 0xc0  0x20 0x01020304");
REGISTER_CMD(iic_detect, "iic扫描所有可用通道和地址并打印结果", "",  "");

