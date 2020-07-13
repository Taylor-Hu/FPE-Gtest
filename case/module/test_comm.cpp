/**
@file test_comm.cpp
@def group common测试集

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
#define CASE_NAME TestComm


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp TestComm TestCase " << std::endl;
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner TestComm TestCase " << std::endl;
}

void CASE_NAME::SetUp()
{
//execute before testcase
//std::cout << "Chdrv TestComm SetUp" << std::endl;
}
void CASE_NAME::TearDown()
{
//execute before testcase
//std::cout << "Chdrv TestComm TearDown" << std::endl;
}

#define D_CnvMjdToDate(mjd, y, m, d, wd)		\
{	\
	S32  i_Mjd;			\
	S32  iy, im, ik;		\
	i_Mjd = mjd;		\
	if (i_Mjd < 15079)		\
	{	\
		i_Mjd += 0x10000;	\
	}		\
	iy = (i_Mjd * 100 - 1507820) / 36525;		\
	im = (i_Mjd * 10000 - 149561000 - ((iy * 36525) / 100) * 10000) / 306001;		\
	d= (U8)(i_Mjd - 14956 - ((iy * 36525) / 100) - ((im * 306001) / 10000));		\
	ik = (( im == 14) || (im == 15)) ? 1 : 0;		\
	y = (U16)(iy + ik) + 1900;		\
	m = (U8)(im - 1 - ik * 12);		\
	wd = (U8)(((i_Mjd + 2) % 7) + 1);		\
}

#define D_CnvDateToMjd(year,month,day)		\
	 (U16)(14956 + (S32)day +		\
			((((S32)(year - 1900) - (((month == 1) || (month == 2)) ? 1 : 0)) * 36525) / 100) +		\
			((((S32)month + 1 + (((month == 1) || (month == 2)) ? 1 : 0) * 12) * 306001) / 10000))
#define D_CnvBcdToVal(bcd) (U8)((bcd >> 4) * 10 + (bcd & 0x0f))
#define D_CnvValToBcd(val) (U8)(((val / 10) << 4) | (val % 10))

#if ENABLE_TESTCASE

CASE_F(TEST_COM_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CH_COM_OK, CH_COM_Init());
	EXPECT_EQ(CH_COM_OK, CH_COM_Init()) << "多次初始化应该返回OK";
}

CASE_F(TEST_COM_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CH_COM_OK, CH_COM_Term());
	EXPECT_EQ(CH_COM_OK, CH_COM_Init());
}


CASE_F(TEST_COM_INIT3,
	"测试CH_COM_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_COM_OK, CH_COM_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}
#if FOR_AUTO_RUN == 0

CASE_F(TEST_COM_RESTART1,
	"测试热重启函数调用是否正常")
{
	PROMPT("即将重启，回车继续...");
#if SUPPORT_TEST_BROKEN
	SetTestRestartFlag();
#endif /* SUPPORT_TEST_BROKEN */
	test_input_none();
	
	EXPECT_EQ(CH_COM_OK, CH_COM_Reboot(CH_COM_WARM_REBOOT));
}

CASE_F(TEST_COM_RESTART2,
	"测试热重启功能是否执行成功")
{
#if SUPPORT_TEST_BROKEN
	ASSERT_STREQ(gcsLastResult.c_str(), "RESTART") << "热重启未成功";
	ASSERT_STREQ(gcsLastCaseName.c_str(), "TEST_COM_RESTART1") << "热重启未成功";
#endif /* SUPPORT_TEST_BROKEN */
}

CASE_F(TEST_COM_RESTART3,
	"测试冷重启函数调用是否正常")
{
	PROMPT("即将重启，回车继续...");
#if SUPPORT_TEST_BROKEN
	SetTestRestartFlag();
#endif /* SUPPORT_TEST_BROKEN */
	test_input_none();
	
	EXPECT_EQ(CH_COM_OK, CH_COM_Reboot(CH_COM_COOL_REBOOT));
}

CASE_F(TEST_COM_RESTART4,
	"测试冷重启功能是否执行成功")
{
#if SUPPORT_TEST_BROKEN
	ASSERT_STREQ(gcsLastResult.c_str(), "RESTART") << "冷重启未成功";
	ASSERT_STREQ(gcsLastCaseName.c_str(), "TEST_COM_RESTART3") << "冷重启未成功";
#endif /* SUPPORT_TEST_BROKEN */
}

CASE_F(TEST_COM_STANDBY1,
	"测试真待机函数调用是否正常")
{
	PROMPT("即将进入真待机，待机时间为0s，待机成功后请人工执行唤醒操作,回车继续...");
#if SUPPORT_TEST_BROKEN
	SetTestRestartFlag();
#endif /* SUPPORT_TEST_BROKEN */
	test_input_none();

#if !(defined(CFG_CHSSA_CHIP_MSTAR5C69) ||defined(CFG_CHSSA_CHIP_MSTAR7C123D) ||defined(CFG_CHSSA_CHIP_MSTAR5C58))
	EXPECT_EQ(CH_COM_OK, CH_COM_Standby(CH_COM_REAL_STANDBY, 5));
#endif
}

CASE_F(TEST_COM_STANDBY2,
	"测试真待机功能是否执行成功")
{
#if SUPPORT_TEST_BROKEN
	ASSERT_STREQ(gcsLastResult.c_str(), "RESTART") << "真待机未成功";
	ASSERT_STREQ(gcsLastCaseName.c_str(), "TEST_COM_STANDBY1") << "真待机未成功";
#endif /* SUPPORT_TEST_BROKEN */
}

CASE_F(TEST_COM_STANDBY3,
	"测试片内待机函数调用是否正常")
{
	PROMPT("即将进入片内待机，待机时间为0s，待机成功后请人工执行唤醒操作,回车继续...");
#if SUPPORT_TEST_BROKEN
	SetTestRestartFlag();
#endif /* SUPPORT_TEST_BROKEN */
	test_input_none();
#if !(defined(CFG_CHSSA_CHIP_MSTAR5C69)||defined(CFG_CHSSA_CHIP_MSTAR7C123D)||defined(CFG_CHSSA_CHIP_MSTAR5C58))
	EXPECT_EQ(CH_COM_OK, CH_COM_Standby(CH_COM_IC_LOW_POWER, 5));
#endif
}

CASE_F(TEST_COM_STANDBY4,
	"测试片内待机功能是否执行成功")
{
#if SUPPORT_TEST_BROKEN
	ASSERT_STREQ(gcsLastResult.c_str(), "RESTART") << "真待机未成功";
	ASSERT_STREQ(gcsLastCaseName.c_str(), "TEST_COM_STANDBY3") << "真待机未成功";
#endif /* SUPPORT_TEST_BROKEN */
}
#endif /* FOR_AUTO_RUN */





CASE_F(TEST_COM_UTC1,
	"测试CH_COM_CmpUtc功能是否正常") {
	CH_COM_UTCTime_t utc1[] = 
	{
		0,
		0,
		0,
		0,
		
		15078,
		0,
		0,
		1,

		15078,
		0,
		0,
		2,

		15012,
		0x23,
		0,
		3,

		15012,
		0x23,
		0,
		4,

		15012,
		0x23,
		0x59,
		5,

		15012,
		0x23,
		0x59,
		6,


		15012,
		0x23,
		0x59,
		7,		
	};
	CH_COM_UTCTime_t utc2[] = 
	{
		0,
		0,
		0,
		0,
		
		15079,
		0,
		0,
		1,

		15078,
		0,
		0,
		0,

		15012,
		0x21,
		0,
		0,

		15012,
		0x23,
		0,
		0,

		15012,
		0x23,
		0x58,
		0,

		15012,
		0x23,
		0x59,
		0x59,


		15012,
		0x23,
		0x59,
		0x42,		
	};
	int result[] = {0,1,1,1,1,1,-1,-1};
	int i;
	char str[64];
	for(i = 0; i < sizeof(utc1) / sizeof(utc1[0]); i++)
	{
		U16 y1, y2;
		U8 m1, d1,wd1;
		U8 m2, d2;
		char *res = "<=>";

		D_CnvMjdToDate(utc1[i].us_mjd, y1, m1, d1, wd1);
		D_CnvMjdToDate(utc2[i].us_mjd, y2, m2, d2, wd1);

		sprintf(str, "%d-%d-%d %02x:%02x:%02x %c %d-%d-%d %02x:%02x:%02x", 
			y1, m1, d1, utc1[i].uc_hours % 0x24, utc1[i].uc_mins % 0x60, utc1[i].uc_secs % 0x60,
			res[result[i] + 1], 
			y2, m2, d2, utc2[i].uc_hours % 0x24, utc2[i].uc_mins % 0x60, utc2[i].uc_secs % 0x60);
		
		EXPECT_EQ(result[i], CH_COM_CmpUtc(&utc1[i], &utc2[i])) << "test " << i << ".结果不对, 正确的应该是:" << str;
	}
	
}



CASE_F(TEST_COM_UTC2,
	"测试CH_COM_AddTimeToUtc功能是否正常") {
	CH_COM_UTCTime_t utc1 = {
		20920,
		0x22,
		0x55,
		0x42
	};

	CH_COM_UTCTime_t utc2;

	
	CH_COM_AddTimeToUtc(&utc1, 0x2, 0x4, 0x40, &utc2);
	LOG("utc2=%d,0x%x,0x%x,0x%x\n", utc2.us_mjd, utc2.uc_hours, utc2.uc_mins, utc2.uc_secs);
	EXPECT_EQ(utc2.us_mjd, 20921);
	EXPECT_EQ(utc2.uc_hours, 1);
	EXPECT_EQ(utc2.uc_mins, 0);
	EXPECT_EQ(utc2.uc_secs, 0x22);
}

CASE_F(TEST_COM_UTC3,
	"测试CH_COM_CalcLocalTime功能是否正常") {
	CH_COM_UTCTime_t utc1 = {
		20920,
		0x22,
		0x55,
		0x42
	};

	CH_COM_UTCTime_t utc2;

	
	CH_COM_CalcLocalTime(&utc1, -1380, &utc2);
	LOG("utc2=%d,0x%x,0x%x,0x%x\n", utc2.us_mjd, utc2.uc_hours, utc2.uc_mins, utc2.uc_secs);
	EXPECT_EQ(utc2.us_mjd, 20919);
	EXPECT_EQ(utc2.uc_hours, 0x23);
	EXPECT_EQ(utc2.uc_mins, 0x55);
	EXPECT_EQ(utc2.uc_secs, 0x42);
}

CASE_F(TEST_COM_UTC4,
	"测试CH_COM_CnvMjdToDate功能是否正常") {

	U16 	us_Mjd[] = {0, 20920, 15078, 15079, 15080};
	U16 	us_Year[2] = {0, 0};
	U8 	uc_Month[2] = {0, 0};
	U8 	uc_Day[2] = {0, 0};
	U8 	uc_WeekDay[2] = {0, 0};
	int i;

	for(i = 0; i < sizeof(us_Mjd) / sizeof(us_Mjd[0]); i++)
	{
		CH_COM_CnvMjdToDate(us_Mjd[i], &us_Year[0], &uc_Month[0], &uc_Day[0], &uc_WeekDay[0]);
		D_CnvMjdToDate(us_Mjd[i], us_Year[1], uc_Month[1], uc_Day[1], uc_WeekDay[1]);
		EXPECT_EQ(us_Year[0], us_Year[1]);
		EXPECT_EQ(uc_Month[0], uc_Month[1]);
		EXPECT_EQ(uc_Day[0], uc_Day[1]);
		EXPECT_EQ(uc_WeekDay[0], uc_WeekDay[1]);
	}
}

CASE_F(TEST_COM_UTC5,
	"测试CH_COM_CnvDateToMjd功能是否正常") {
	U16 	us_Year[] = {2016, 1900, 2089, 1901};
	U8 	uc_Month[] = {1, 2, 9, 12};
	U8 	uc_Day[ ] = {1, 28, 31, 31};
	int i;

	for(i = 0; i < sizeof(us_Year) / sizeof(us_Year[0]); i++)
	{
		EXPECT_EQ(CH_COM_CnvDateToMjd(us_Year[i], uc_Month[i], uc_Day[i]),
		D_CnvDateToMjd(us_Year[i], uc_Month[i], uc_Day[i]));
	}
}

CASE_F(TEST_COM_UTC6,
	"测试CH_COM_CmpUtcInterval功能是否正常") {
	CH_COM_UTCTime_t utc1[] = 
	{
		0,
		0,
		0,
		0,
		
		15078,
		0,
		0,
		1,

		15078,
		0,
		0,
		2,

		15012,
		0x23,
		0,
		3,

		15012,
		0x23,
		0,
		4,

		15012,
		0x23,
		0x41,
		5,

		15012,
		0x23,
		0x59,
		6,


		15012,
		0x23,
		0x59,
		7,		
	};
	CH_COM_UTCTime_t utc2[] = 
	{
		0,
		0,
		0,
		0,
		
		15078,
		0,
		0,
		1,

		15078,
		0,
		0,
		2,

		15012,
		0x23,
		0,
		3,

		15012,
		0x23,
		0,
		4,

		15012,
		0x23,
		0x41,
		5,

		15012,
		0x23,
		0x59,
		6,


		15012,
		0x23,
		0x59,
		7,		
	};
	int result[] = {0,1,0,1,0,0,0,1};
	int i;
	char str[64];
	for(i = 0; i < sizeof(utc1) / sizeof(utc1[0]); i++)
	{
		U16 y1, y2;
		U8 m1, d1,wd1;
		U8 m2, d2;
		char *res = "<=>";

		CH_COM_CalcLocalTime(&utc1[i], result[i], &utc1[i]);
		D_CnvMjdToDate(utc1[i].us_mjd, y1, m1, d1, wd1);
		D_CnvMjdToDate(utc2[i].us_mjd, y2, m2, d2, wd1);

		sprintf(str, "%d-%d-%d %02x:%02x:%02x %c %d-%d-%d %02x:%02x:%02x", 
			y1, m1, d1, utc1[i].uc_hours % 0x24, utc1[i].uc_mins % 0x60, utc1[i].uc_secs % 0x60,
			res[result[i] + 1], 
			y2, m2, d2, utc2[i].uc_hours % 0x24, utc2[i].uc_mins % 0x60, utc2[i].uc_secs % 0x60);
		
		EXPECT_EQ(result[i], CH_COM_CmpUtcInterval(&utc1[i], &utc2[i], result[i] * 60)) << "test" << i << ".结果不对, 正确的应该是:" << str;
	}
}

CASE_F(TEST_COM_UTC7,
	"测试CH_COM_SetRTC/CH_COM_GetRTC功能是否正常") {

	CH_COM_UTCTime_t utc1 = {15012,
		0x23,
		0,
		3};
	CH_COM_UTCTime_t utc2;

	
	EXPECT_EQ(CH_COM_SetRTC(utc1), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetRTC(&utc2), CH_COM_OK);

	
	EXPECT_EQ(utc1.us_mjd , utc2.us_mjd);
	EXPECT_EQ(utc1.uc_hours , utc2.uc_hours);
	EXPECT_EQ(utc1.uc_mins , utc2.uc_mins);
	EXPECT_EQ(utc1.uc_secs , utc2.uc_secs);
}


CASE_F(TEST_COM_UTC8,
	"测试CH_COM_CnvBcdToVal功能是否正常") {
	U8 bcd[] = {0, 0x12, 0x23, 0x34, 0x56, 0x78, 0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xdf, 0xff};
	int i;

	for(i = 0; i < sizeof(bcd) / sizeof(bcd[0]); i++)
	{
		EXPECT_EQ(CH_COM_CnvBcdToVal(bcd[i]), D_CnvBcdToVal(bcd[i]));
	}
}

CASE_F(TEST_COM_UTC9,
	"测试CH_COM_CnvValToBcd功能是否正常") {
	U8 val[] = {0, 12, 23, 34, 56, 78, 89, 128, 156, 179, 200, 245, 255};
	int i;

	for(i = 0; i < sizeof(val) / sizeof(val[0]); i++)
	{
		EXPECT_EQ(CH_COM_CnvValToBcd(val[i]), D_CnvValToBcd(val[i]));
	}
}

CASE_F(TEST_COM_CRC1,
	"测试CH_COM_MPEGCRC32功能是否正常") {
	U8 *p1 = (U8*)"abcd";

	EXPECT_EQ(CH_COM_MPEGCRC32(p1, strlen((char*)p1)), 0xc2b3ccb4);
}

CASE_F(TEST_COM_CRC2,
	"测试CH_COM_CRC32功能是否正常") {
	U8 *p1 =(U8*) "abcd";

	EXPECT_EQ(CH_COM_CRC32(p1, strlen((char*)p1)), 0xed82cd11);
}

CASE_F(TEST_COM_CRC3,
	"测试CH_COM_CRC16功能是否正常") {
	U8 *p1 = (U8*)"abcd";

	EXPECT_EQ(CH_COM_CRC16(p1, strlen((char*)p1)), 0x3997);
}

CASE_F(TEST_COM_CODE1,
	"测试CH_COM_CodingConvert功能是否正常") {
	U8 *p1;
	U8 *p2;
	U32 len;
	p2 = new U8[1024];

	p1 = (U8*)"\x54\xc8\x54\xc8\xff\x0c\x8f\xd9\x66\x2f\x4e\x00\x4e\x2a\x00\x73\x00\x61\x00\x6d\x00\x70\x00\x6c\x00\x65\x00\x21\x00\x00";

	len = 30;
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_UTF16_BIG_ENDIAN, p1, CH_COM_TEXT_CODETYPE_GB2312, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xb9\xfe\xb9\xfe\xa3\xac\xd5\xe2\xca\xc7\xd2\xbb\xb8\xf6\x73\x61\x6d\x70\x6c\x65\x21\x00", len), 0);
	
	
	len = 30;
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_UTF16_BIG_ENDIAN, p1, CH_COM_TEXT_CODETYPE_GB18030, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xb9\xfe\xb9\xfe\xa3\xac\xd5\xe2\xca\xc7\xd2\xbb\xb8\xf6\x73\x61\x6d\x70\x6c\x65\x21\x00", len), 0);
	
	
	len = 30;
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_UTF16_BIG_ENDIAN, p1, CH_COM_TEXT_CODETYPE_UTF8, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xe5\x93\x88\xe5\x93\x88\xef\xbc\x8c\xe8\xbf\x99\xe6\x98\xaf\xe4\xb8\x80\xe4\xb8\xaa\x73\x61\x6d\x70\x6c\x65\x21\x00", len), 0);
	
	p1 = (U8*)"\xE5\x93\x88\xe5\x93\x88\xef\xbc\x8c\xe8\xbf\x99\xe4\xbb\x85\xe4\xbb\x85\xe6\x98\xaf\xe4\xb8\x80\xe4\xb8\xaa\x73\x61\x6d\x70\x6c\x65\xef\xbc\x3f\x20\x00\x00";

	len = strlen((char*)p1);
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_UTF8, p1, CH_COM_TEXT_CODETYPE_GB18030, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xb9\xfe\xb9\xfe\xa3\xac\xd5\xe2\xbd\xf6\xbd\xf6\xca\xc7\xd2\xbb\xb8\xf6\x73\x61\x6d\x70\x6c\x65\xa3\xdf\x20\x00", len), 0);

	len = strlen((char*)p1);
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_UTF8, p1, CH_COM_TEXT_CODETYPE_GB2312, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xb9\xfe\xb9\xfe\xa3\xac\xd5\xe2\xbd\xf6\xbd\xf6\xca\xc7\xd2\xbb\xb8\xf6\x73\x61\x6d\x70\x6c\x65\xa3\xdf\x20\x00", len), 0);

	p1 = (U8*)"哈哈,这是一个sample!";
	len = strlen((char*)p1);
	EXPECT_EQ(CH_COM_CodingConvert(CH_COM_TEXT_CODETYPE_GB2312, p1, CH_COM_TEXT_CODETYPE_UTF8, p2, &len), CH_COM_OK);
	EXPECT_EQ(memcmp(p2, "\xe5\x93\x88\xe5\x93\x88\x2c\xe8\xbf\x99\xe6\x98\xaf\xe4\xb8\x80\xe4\xb8\xaa\x00", len), 0);

	delete p2;
	
}
#if 0 	/*sqzou20170424*/

CASE_F(TEST_COM_LOADERINFO,
	"测试CH_COM_SetLoaderInfo和CH_COM_GetLoaderInfo功能是否正常") {
	CH_LoaderInfo_t stru_Info0, stru_Info1, stru_Info2;

	stru_Info1.i_length= sizeof(stru_Info1);				///< 信息大小
	stru_Info1.ui_OperatorNumber = 0x32;		///< 运营商ID
	stru_Info1.ui_LoaderType = 0x43;			///< loader类型，具体由loader组分配
	stru_Info1.ui_Manufacturer = 0x34;		///< 厂商代码
	stru_Info1.ui_Hardware = 0x23332333;		///< 硬件版本号
	stru_Info1.ui_Software = 0x11112222;		///< 软件版本号
	stru_Info1.ui_DateCode = 0x33445566;			///< 软件日期
	stru_Info1.ui_TimeCode = 0x55443322;			///< 软件时间
	stru_Info1.i_CaSpecailDataOffset = 0x120000;	///< CA私有数据存放地址
	stru_Info1.ui_crc = 0;					///< CRC信息

	stru_Info0.i_length= sizeof(stru_Info0);
	EXPECT_EQ(CH_COM_GetLoaderInfo(&stru_Info0), CH_COM_OK);//备份
	strcpy((char*)stru_Info1.uc_name, (char*)stru_Info0.uc_name);
	
	EXPECT_EQ(CH_COM_SetLoaderInfo(&stru_Info1), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetLoaderInfo(&stru_Info2), CH_COM_OK);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_name[0], (char*)&stru_Info2.uc_name[0]), 0);
	EXPECT_EQ(stru_Info1.i_length, stru_Info2.i_length);	
	EXPECT_EQ(stru_Info1.ui_OperatorNumber,stru_Info2.ui_OperatorNumber);		
	EXPECT_EQ(stru_Info1.ui_LoaderType,stru_Info2.ui_LoaderType);		
	EXPECT_EQ(stru_Info1.ui_Manufacturer,stru_Info2.ui_Manufacturer);		
	EXPECT_EQ(stru_Info1.ui_Hardware, stru_Info2.ui_Hardware);	
	EXPECT_EQ(stru_Info1.ui_Software, stru_Info2.ui_Software);	
	EXPECT_EQ(stru_Info1.ui_DateCode, stru_Info2.ui_DateCode);	
	EXPECT_EQ(stru_Info1.ui_TimeCode, stru_Info2.ui_TimeCode);	
	EXPECT_EQ(stru_Info1.i_CaSpecailDataOffset, stru_Info2.i_CaSpecailDataOffset);	
	EXPECT_NE(stru_Info2.ui_crc, 0);		

	EXPECT_EQ(CH_COM_SetLoaderInfo(&stru_Info0), CH_COM_OK);//恢复
}


CASE_F(TEST_COM_OTAINFO,
	"测试CH_COM_SetOTAInfo和CH_COM_GetOTAInfo功能是否正常") {
	CH_UpdateInfo_t stru_Info0, stru_Info1, stru_Info2;

	stru_Info1.i_length = sizeof(stru_Info1);		
	stru_Info1.enm_AntType = CH_DEMOD_DVB_C;	
	stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_FreqKHZ = 0x43;		
	stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_SymbKbps = 0x56;		
	stru_Info1.un_FreqInfo.stru_CableFreqInfo.uc_QAMMode = 0x78;		
	stru_Info1.us_DataPid = 0x12;
	stru_Info1.us_DataGid = 0x23;
	stru_Info1.uc_UpdaeFlag = 0x34;
	stru_Info1.ui_crc = 0;

	stru_Info0.i_length = sizeof(stru_Info0);
	EXPECT_EQ(CH_COM_GetOTAInfo(&stru_Info0), CH_COM_OK);//备份
	strcpy((char*)stru_Info1.uc_name, (char*)stru_Info0.uc_name);
	
	EXPECT_EQ(CH_COM_SetOTAInfo(&stru_Info1), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetOTAInfo(&stru_Info2), CH_COM_OK);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_name[0],(char*) &stru_Info2.uc_name[0]), 0);
	EXPECT_EQ(stru_Info1.i_length, stru_Info2.i_length);	
	EXPECT_EQ(stru_Info1.enm_AntType,stru_Info2.enm_AntType);		
	EXPECT_EQ(stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_FreqKHZ,stru_Info2.un_FreqInfo.stru_CableFreqInfo.ui_FreqKHZ);		
	EXPECT_EQ(stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_SymbKbps,stru_Info2.un_FreqInfo.stru_CableFreqInfo.ui_SymbKbps);		
	EXPECT_EQ(stru_Info1.un_FreqInfo.stru_CableFreqInfo.uc_QAMMode, stru_Info2.un_FreqInfo.stru_CableFreqInfo.uc_QAMMode);	
	EXPECT_EQ(stru_Info1.us_DataPid,stru_Info2.us_DataPid);		
	EXPECT_EQ(stru_Info1.us_DataGid,stru_Info2.us_DataGid);		
	EXPECT_EQ(stru_Info1.uc_UpdaeFlag,stru_Info2.uc_UpdaeFlag);		
	EXPECT_NE(stru_Info2.ui_crc, 0);		

	EXPECT_EQ(CH_COM_SetOTAInfo(&stru_Info0), CH_COM_OK);//恢复
}

CASE_F(TEST_COM_SYSTEMINFO,
	"测试CH_COM_SetSystemInfo和CH_COM_GetSystemInfo功能是否正常") {
	CH_SystemInfo_t stru_Info0, stru_Info1, stru_Info2;

				
	stru_Info1.i_length = sizeof(stru_Info1);		
	strcpy((char*)stru_Info1.uc_STBSN, "\x01\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x03\x00");
	strcpy((char*)stru_Info1.uc_MacAddress, "\x01\x02\x02\x02\x02\x02\x02\x00");
	strcpy((char*)stru_Info1.uc_HDCPKey, "\x01\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x03\x00");

	stru_Info1.ui_CRC = 0;

	stru_Info0.i_length= sizeof(stru_Info0);
	EXPECT_EQ(CH_COM_GetSystemInfo(&stru_Info0), CH_COM_OK);//备份

	strcpy((char*)stru_Info1.uc_name, (char*)stru_Info0.uc_name);
	EXPECT_EQ(CH_COM_SetSystemInfo(&stru_Info1), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetSystemInfo(&stru_Info2), CH_COM_OK);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_name[0], (char*)&stru_Info2.uc_name[0]), 0);
	EXPECT_EQ(stru_Info1.i_length, stru_Info2.i_length);	
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_name[0], (char*)&stru_Info2.uc_name[0]), 0);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_STBSN[0], (char*)&stru_Info2.uc_STBSN[0]), 0);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_MacAddress[0],(char*)&stru_Info2.uc_MacAddress[0]), 0);
	EXPECT_EQ(strcmp((char*)&stru_Info1.uc_HDCPKey[0], (char*)&stru_Info2.uc_HDCPKey[0]), 0);
	EXPECT_NE(stru_Info2.ui_CRC, 0);		
	
	EXPECT_EQ(CH_COM_SetSystemInfo(&stru_Info0), CH_COM_OK);//恢复
}

CASE_F(TEST_COM_FACTFLAG,
	"测试CH_COM_SetFactBoot和CH_COM_GetFactBoot功能是否正常") {
	CH_BOOL b_Flag, b_Boot = CH_FALSE;

	EXPECT_EQ(CH_COM_GetFactBoot(&b_Boot), CH_COM_OK);//备份

	b_Flag = CH_TRUE;
	EXPECT_EQ(CH_COM_SetFactBoot(b_Flag), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetFactBoot(&b_Flag), CH_COM_OK);
	EXPECT_EQ(b_Flag, CH_TRUE);

	b_Flag = CH_FALSE;
	EXPECT_EQ(CH_COM_SetFactBoot(b_Flag), CH_COM_OK);
	EXPECT_EQ(CH_COM_GetFactBoot(&b_Flag), CH_COM_OK);
	EXPECT_EQ(b_Flag, CH_FALSE);

	
	EXPECT_EQ(CH_COM_SetFactBoot(b_Boot), CH_COM_OK);//恢复
				
}
#endif

CASE_F(TEST_COM_CHIPID,
	"测试CH_COM_GetChipInfo功能是否正常") {
	CH_ChipInfo_t stru_Info;

	memset(&stru_Info, 0, sizeof(stru_Info));

	EXPECT_EQ(CH_COM_GetChipInfo(&stru_Info), CH_COM_OK);
	EXPECT_NE(stru_Info.ui_ChipID, 0);
}
#endif /* ENABLE_TESTCASE */


#define MAX_LEN 65535   // 2字节最大数
static unsigned short big_buffer[MAX_LEN] = {0};
static int isMapOK = 0;


int make_charmap_gb18030(void)
{
	char *buffer = NULL, *pTemp = NULL;
	int len = 0;
	int x1 = 0;
	int x2 = 0;
	int max_num = 0;

	if(isMapOK == 1){

		return 0;
	}

	memset(big_buffer, 0, sizeof(big_buffer));
	len = fsys_read_file((const char *)fw_getenv("GB2312_UNICABLE_PATH"), buffer);
	if (len == 0 || buffer == NULL)
	{
		LOG("open file error!!\n");
		return -1;
	}

	LOG("file len: %d\n", len);

	pTemp = strtok(buffer, "\n");
	while (pTemp)
	{
		sscanf(pTemp, "%x:%x\n", &x1, &x2);
		big_buffer[x2] = x1;
		if (x2 > max_num)
		max_num = x2;
		pTemp = strtok(NULL, "\n");
	}

	LOG("max num: %d 0x%x\n", max_num, max_num);
	//len = fsys_write_file("c:/resource/unicode2gbk.bin", (char*)big_buffer, max_num  * 2);
	CH_RTOS_FreeMemory(gh_APP_HEAP, buffer);
	isMapOK = 1;

    return 0;
}



int gbk_to_unicode(unsigned short int* unicode, const char* gb, int len)
{
	int i = 0,j;
	unsigned short int temp[2] = {0, 0};

	unsigned char* gb_temp = (unsigned char *)gb;   //必须转换成无符号

	unicode[0] = 0x11;

	for(j = 1; i < len; j++)
	{
		if (gb_temp[i] <= 0x80)
		{
		    unicode[j] = gb_temp[i];
		    i++;
		}
		else
		{
		    temp[0] = (gb_temp[i] << 8) + gb_temp[i+1];

		    unicode[j] = ((big_buffer[temp[0]] & 0xff) << 8) + ((big_buffer[temp[0]] >> 8) & 0xff);
		    i += 2;
		}
	}
	unicode[j++] = 0;
    return j;
}





static int com_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_COM_Init();
	if(ret != CH_COM_OK)
	{
		LOG("ERROR: CH_COM_Init return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int com_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_COM_Term();
	if(ret != CH_COM_OK)
	{
		LOG("ERROR: CH_COM_Term return %d\n", ret);
		return ret;
	}
	
	return 0;
}


static int com_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t stru_Ver;
	
	ret = CH_COM_GetVersion(&stru_Ver);
	if(ret != CH_COM_OK)
	{
		LOG("ERROR: CH_COM_GetVersion return %d\n", ret);
		return ret;
	}
	LOG("version=0x%x\n",stru_Ver);
	return 0;
}
static int com_mpegcrc32(int argc, CMD_PARAM argv[])
{
	U32 ui_crc;
	
	if(strcmp(argv[1].get_str(), "-f")== 0)
	{
		char *pbuf = NULL;
		int len;

		len = fsys_read_file(argv[2].get_str(), pbuf);
		if(len <= 0)
		{
			LOG("ERROR: open %s fail\n", (const S8 *)argv[2].get_str());
			return -1;
		}		
		ui_crc = CH_COM_MPEGCRC32((U8 *)pbuf, len);
		LOG("FILE[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), len, ui_crc);
		delete []pbuf;
	}
	else
	{
		ui_crc = CH_COM_MPEGCRC32((U8 *)argv[2].get_str(), strlen(argv[2].get_str()));
		LOG("str[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), strlen(argv[2].get_str()), ui_crc);
	}
	return 0;
}

static int com_crc32(int argc, CMD_PARAM argv[])
{
	U32 ui_crc;
	
	if(strcmp(argv[1].get_str(), "-f")== 0)
	{
		char *pbuf = NULL;
		int len;

		len = fsys_read_file(argv[2].get_str(), pbuf);
		if(len <= 0)
		{
			LOG("ERROR: open %s fail\n", (const S8 *)argv[2].get_str());
			return -1;
		}	

		ui_crc = CH_COM_CRC32((U8 *)pbuf, len);
		LOG("FILE[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), len, ui_crc);
		CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	}
	else
	{
		ui_crc = CH_COM_CRC32((U8 *)argv[2].get_str(), strlen(argv[2].get_str()));
		LOG("str[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), strlen(argv[2].get_str()), ui_crc);
	}
	return 0;
}

static int com_crc16(int argc, CMD_PARAM argv[])
{
	U16 ui_crc;
	
	if(strcmp(argv[1].get_str(), "-f")== 0)
	{
		char *pbuf = NULL;
		int len;
		
		len = fsys_read_file(argv[2].get_str(), pbuf);
		if(len <= 0)
		{
			LOG("ERROR: open %s fail\n", (const S8 *)argv[2].get_str());
			return -1;
		}	
		ui_crc = CH_COM_CRC16((U8 *)pbuf, len);
		CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
		LOG("FILE[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), len, ui_crc);
	}
	else
	{
		ui_crc = CH_COM_CRC16((U8 *)argv[2].get_str(), strlen(argv[2].get_str()));
		LOG("str[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), strlen(argv[2].get_str()), ui_crc);
	}
	return 0;
}

static int com_ccittcrc16(int argc, CMD_PARAM argv[])
{
	U16 ui_crc;
	
	if(strcmp(argv[1].get_str(), "-f")== 0)
	{
		char *pbuf = NULL;
		int len;
		len = fsys_read_file(argv[2].get_str(), pbuf);
		if(len <= 0)
		{
			LOG("ERROR: open %s fail\n", (const S8 *)argv[2].get_str());
			return -1;
		}
		ui_crc = CH_COM_CCITTCRC16((U8 *)pbuf, len);
		CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
		LOG("FILE[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), len, ui_crc);
	}
	else
	{
		ui_crc = CH_COM_CCITTCRC16((U8 *)argv[2].get_str(), strlen(argv[2].get_str()));
		LOG("str[%s] len = %dB, crc=0x%x\n", argv[2].get_str(), strlen(argv[2].get_str()), ui_crc);
	}
	return 0;
}

static int com_get_loaderinfo(int argc, CMD_PARAM argv[])
{
	CH_LoaderInfo_t stru_Info1;

	EXPECT_EQ(CH_COM_OK, CH_COM_GetLoaderInfo(&stru_Info1));
	LOG("CH_COM_GetLoaderInfo:i_length=0x%x\n"
		"ui_OperatorNumber=0x%x\n"
		"ui_LoaderType=0x%x\n"
		"ui_Manufacturer=0x%x\n"
		"ui_Hardware=0x%x\n"
		"ui_Software=0x%x\n"
		"ui_DateCode=0x%x\n"
		"ui_TimeCode=0x%x\n"
		"i_CaSpecailDataOffset=0x%x\n"
		"ui_crc=0x%x\n",
		stru_Info1.i_length,	
		stru_Info1.ui_OperatorNumber,	
		stru_Info1.ui_LoaderType,		
		stru_Info1.ui_Manufacturer,		
		stru_Info1.ui_Hardware,	
		stru_Info1.ui_Software,		
		stru_Info1.ui_DateCode,		
		stru_Info1.ui_TimeCode,		
		stru_Info1.i_CaSpecailDataOffset,	
		stru_Info1.ui_crc);
	return 0;
}

static int com_get_otainfo(int argc, CMD_PARAM argv[])
{
	CH_UpdateInfo_t stru_Info1;

	EXPECT_EQ(CH_COM_OK, CH_COM_GetOTAInfo(&stru_Info1));


	if(stru_Info1.enm_AntType == CH_DEMOD_DVB_C
		|| stru_Info1.enm_AntType == CH_DEMOD_DVB_C2)
	{
		LOG("i_length=%d\n"
			"enm_AntType=%d\n"
			"ui_FreqKHZ=%d\n"
			"ui_SymbKbps=%d\n"
			"uc_QAMMode=%d\n"
			"us_DataPid=%d\n"
			"us_DataGid=%d\n"
			"uc_UpdaeFlag=%d\n"
			"ui_crc=%d\n",
			stru_Info1.i_length,		
			stru_Info1.enm_AntType,
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_FreqKHZ,	
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_SymbKbps,	
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.uc_QAMMode,	
			stru_Info1.us_DataPid,
			stru_Info1.us_DataGid,
			stru_Info1.uc_UpdaeFlag,
			stru_Info1.ui_crc);
	}
	else if(stru_Info1.enm_AntType == CH_DEMOD_DVB_S
		|| stru_Info1.enm_AntType == CH_DEMOD_DVB_S2)
	{
		LOG("i_length=%d\n"
			"enm_AntType=%d\n"
			"ui_FreqKHZ=%d\n"
			"ui_SymbKbps=%d\n"
			"uc_QPSKMode=%d\n"
			"ui_LNBHighFreKhz=%d\n"
			"ui_LNBLowFreKhz=%d\n"
			"uc_LNBPower=%d\n"
			"uc_Polarization=%d\n"
			"uc_22KType=%d\n"
			"uc_DiseqcVer=%d\n"
			"uc_DieseqcPort=%d\n"
			"uc_DieseqcPos=%d\n"
			"i_SatelliteLongitude=%d\n"
			"i_LocalLongitude=%d\n"
			"i_LocalLatitude=%d\n"
			"us_DataPid=%d\n"
			"us_DataGid=%d\n"
			"uc_UpdaeFlag=%d\n"
			"ui_crc=%d\n",
			stru_Info1.i_length,		
			stru_Info1.enm_AntType,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.ui_FreqKHz,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.ui_SymbKbps,	
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_QPSKMode,	
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.ui_LNBHighFreKhz,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.ui_LNBLowFreKhz,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_LNBPower,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_Polarization,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_22KType,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_DiseqcVer,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_DieseqcPort,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.uc_DieseqcPos,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.i_SatelliteLongitude,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.i_LocalLongitude,
			stru_Info1.un_FreqInfo.stru_SatelliteFreqInfo.i_LocalLatitude,
			stru_Info1.us_DataPid,
			stru_Info1.us_DataGid,
			stru_Info1.uc_UpdaeFlag,
			stru_Info1.ui_crc);
	}
	else if(stru_Info1.enm_AntType == CH_DEMOD_DVB_T
		|| stru_Info1.enm_AntType == CH_DEMOD_DVB_T2
		|| stru_Info1.enm_AntType == CH_DEMOD_DMB_TH)
	{
		LOG("i_length=%d\n"
			"enm_AntType=%d\n"
			"ui_FreqKHZ=%d\n"
			"ui_SymbKbps=%d\n"
			"us_DataPid=%d\n"
			"us_DataGid=%d\n"
			"uc_UpdaeFlag=%d\n"
			"ui_crc=%d\n",
			stru_Info1.i_length,		
			stru_Info1.enm_AntType,
			stru_Info1.un_FreqInfo.stru_TerrestrialFreqInfo.ui_FreqKHZ,	
			stru_Info1.un_FreqInfo.stru_TerrestrialFreqInfo.ui_BandWithKHz,	
			stru_Info1.us_DataPid,
			stru_Info1.us_DataGid,
			stru_Info1.uc_UpdaeFlag,
			stru_Info1.ui_crc);
	}
	else
	{
		LOG("i_length=%d\n"
			"enm_AntType=%d\n"
			"ui_FreqKHZ=%d\n"
			"ui_SymbKbps=%d\n"
			"uc_QAMMode=%d\n"
			"us_DataPid=%d\n"
			"us_DataGid=%d\n"
			"uc_UpdaeFlag=%d\n"
			"ui_crc=%d\n",
			stru_Info1.i_length,		
			stru_Info1.enm_AntType,
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_FreqKHZ,	
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.ui_SymbKbps,	
			stru_Info1.un_FreqInfo.stru_CableFreqInfo.uc_QAMMode,	
			stru_Info1.us_DataPid,
			stru_Info1.us_DataGid,
			stru_Info1.uc_UpdaeFlag,
			stru_Info1.ui_crc);
	}
	return 0;
}

static int com_get_systeminfo(int argc, CMD_PARAM argv[])
{
	CH_SystemInfo_t stru_Info1;

	EXPECT_EQ(CH_COM_OK, CH_COM_GetSystemInfo(&stru_Info1));
	int i;
	
	stru_Info1.uc_name[sizeof(stru_Info1.uc_name) - 1] = 0;
	LOG("CH_COM_GetSystemInfo:i_length=0x%x\n"
		"uc_name=%s\n"
		"ui_CRC=0x%x\n",
		stru_Info1.i_length,	
		stru_Info1.uc_name,
		stru_Info1.ui_CRC);
	LOG("uc_STBSN = [");
	for(i = 0; i < sizeof(stru_Info1.uc_STBSN); i++)
	{
		if(i && i % 8 == 0)CH_COM_DebugPrintf((const S8 *)"\n");
		CH_COM_DebugPrintf((const S8 *)"%02x ", stru_Info1.uc_STBSN[i]);
	}
	CH_COM_DebugPrintf((const S8 *)"]\n");
	
	LOG("uc_MacAddress = [");
	for(i = 0; i < sizeof(stru_Info1.uc_MacAddress); i++)
	{
		if(i && i % 8 == 0)CH_COM_DebugPrintf((const S8 *)"\n");
		CH_COM_DebugPrintf((const S8 *)"%02x ", stru_Info1.uc_MacAddress[i]);
	}
	CH_COM_DebugPrintf((const S8 *)"]\n");

	
	LOG("uc_HDCPKey = [");
	for(i = 0; i < sizeof(stru_Info1.uc_HDCPKey); i++)
	{
		if(i && i % 8 == 0)CH_COM_DebugPrintf((const S8 *)"\n");
		CH_COM_DebugPrintf((const S8 *)"%02x ", stru_Info1.uc_HDCPKey[i]);
	}
	CH_COM_DebugPrintf((const S8 *)"]\n");
	return 0;
}

static int com_get_chipinfo(int argc, CMD_PARAM argv[])
{
	CH_ChipInfo_t stru_Info1;

	EXPECT_EQ(CH_COM_OK, CH_COM_GetChipInfo(&stru_Info1));

	
	LOG("CH_COM_GetChipInfo:enm_ChipType=0x%x\n"
		"ui_ChipID=0x%x\n"
		"ui_JTAGStatus=0x%x\n"
		"ui_BootRSAStatus=0x%x\n"
		"ui_CWStatus=0x%x\n",
		stru_Info1.enm_ChipType,	
		stru_Info1.ui_ChipID,
		stru_Info1.ui_JTAGStatus,
		stru_Info1.ui_BootRSAStatus,	
		stru_Info1.ui_CWStatus);
	return 0;
}


static int com_get_factboot(int argc, CMD_PARAM argv[])
{
	CH_BOOL b_Info1;

	EXPECT_EQ(CH_COM_OK, CH_COM_GetFactBoot(&b_Info1));

	
	LOG("CH_COM_GetFactBoot:flag=0x%x\n",
		&b_Info1);
	return 0;
}
static int com_set_factboot(int argc, CMD_PARAM argv[])
{
	CH_BOOL b_Info1;

	b_Info1 = (CH_BOOL)argv[1].get_i();
	EXPECT_EQ(CH_COM_OK, CH_COM_SetFactBoot(b_Info1));
	return 0;
}

static int com_reboot(int argc, CMD_PARAM argv[])
{
	CH_COM_REBOOT_MODE_e enm_RebootMode;

	enm_RebootMode = (CH_COM_REBOOT_MODE_e)argv[1].get_i();
	EXPECT_EQ(CH_COM_OK, CH_COM_Reboot(enm_RebootMode));
	return 0;
}

static int com_standby(int argc, CMD_PARAM argv[])
{
	CH_COM_STANDBY_REALIZE_MODE_e enm_StandbyMode;
	S32 i_timeout;
	
	enm_StandbyMode = (CH_COM_STANDBY_REALIZE_MODE_e)argv[1].get_i();
	i_timeout = argv[2].get_i();
#if !(defined(CFG_CHSSA_CHIP_MSTAR5C69)||defined(CFG_CHSSA_CHIP_MSTAR7C123D)||defined(CFG_CHSSA_CHIP_MSTAR5C58))
	EXPECT_EQ(CH_COM_OK, CH_COM_Standby(enm_StandbyMode, i_timeout));
#else
	LOG("NOT SUPPORT\n");
#endif /* CFG_CHSSA_CHIP_MSTAR5C58*/
	return 0;
}

static int com_active_sercure(int argc, CMD_PARAM argv[])
{
	U32 ui_secItem;
	
	ui_secItem = argv[1].get_i();
	CH_COM_ActiveSecure(ui_secItem);
	return 0;
}


REGISTER_CMD(com_init, "初始化com模块", "",  "");
REGISTER_CMD(com_term, "销毁com模块", "",  "");
REGISTER_CMD(com_ver, "打印com模块版本号", "",  "");
REGISTER_CMD(com_mpegcrc32, "计算一串字符串或者一个文件的crc", "-s(-f) | data_string(file_path)",  "-s \"hello,world!\"");
REGISTER_CMD(com_crc32, "计算一串字符串或者一个文件的crc", "-s(-f) | data_string(file_path)",  "-s \"hello,world!\"");
REGISTER_CMD(com_crc16, "计算一串字符串或者一个文件的crc", "-s(-f) | data_string(file_path)",  "-s \"hello,world!\"");
REGISTER_CMD(com_ccittcrc16, "计算一串字符串或者一个文件的crc", "-s(-f) | data_string(file_path)",  "-s \"hello,world!\"");
REGISTER_CMD(com_get_loaderinfo, "打印出loader info的值", "",  "");
REGISTER_CMD(com_get_otainfo, "打印出ota info的值", "",  "");
REGISTER_CMD(com_get_systeminfo, "打印出system info的值", "",  "");
REGISTER_CMD(com_get_chipinfo, "打印出chip info的值", "",  "");
REGISTER_CMD(com_get_factboot, "打印出fact boot的值", "",  "");
REGISTER_CMD(com_set_factboot, "设置fact boot的值", "",  "0");
REGISTER_CMD(com_reboot, "重启机顶盒", "mode",  "0");
REGISTER_CMD(com_standby, "进入待机", "mode | timeout",  "0 0");
REGISTER_CMD(com_active_sercure, "开启高安选项", "item",  "0x20");




