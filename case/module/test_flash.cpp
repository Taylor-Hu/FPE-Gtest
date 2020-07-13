/**
@file test_flash.cpp
@defgroup flash测试集

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
#define CASE_NAME TestFlash
#include "test_menu.h"
#include "test_modules.h"

static CHDRV_FLASH_Handle gh_flashHdl;
#define DATA_SIZE 0x10000

#ifdef BLOCK_SIZE
#undef BLOCK_SIZE
#endif
#define BLOCK_SIZE 0x10000

static U8 *data_buffer = NULL, *guc_read_buffer = NULL;
static U8 *write_buffer = NULL;


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp flash TestCase " << std::endl;
	if(write_buffer == NULL)
	{
		data_buffer = new U8[DATA_SIZE];
		guc_read_buffer = new U8[DATA_SIZE];
		write_buffer = new U8[BLOCK_SIZE];
	}
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner flash TestCase " << std::endl;
	if(data_buffer)
	{
		delete []data_buffer;
		data_buffer = NULL;
		delete []guc_read_buffer;
		guc_read_buffer = NULL;
		delete []write_buffer;
		write_buffer = NULL;
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

//extern CHDRV_FLASH_RESULT_e  CHDRV_FLASH_OpenEx(const char * pc_MtdName,	S32 ri_Offset, S32 ri_Len, CHDRV_FLASH_Handle *rph_FlashHandle, U32 rui_OpenFlags,void * pv_RestParams);

static int memchar_cmp(void *p, char ch, int size)
{
	int i;
	if(!p || size <= 0)
	{
		return -1;
	}
	for(i = 0; i < size; i++)
	{
		if(((char *)p)[i] != ch)
		{
			return (((char *)p)[i] - ch);
		}
	}
	return 0;
}

int bufferCompare(U8 *dest_buf, U8 *src_buf, S32 size)
{
	int i;

	for(i=0;i<size;i++)
	{	
		if(*dest_buf == *src_buf)
		{
			dest_buf++;
			src_buf++;
			continue;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}
#if ENABLE_TESTCASE


CASE_F(TEST_FLASH_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_FLASH_OK, CHDRV_FLASH_Init());
	EXPECT_EQ(CHDRV_FLASH_OK, CHDRV_FLASH_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_FLASH_INIT2,
	"test flash can be term") {
	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
	EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Term());
	}
	EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Init());
}


CASE_F(TEST_FLASH_INIT3,
	"test CHDRV_FLASH_GetVersion if ok") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	//EXPECT_NE(CHDRV_FLASH_OK,  CHDRV_FLASH_GetVersion(NULL)) << "参数为空";
	EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_FLASH_PARAMS1,
	"CHDRV_FLASH_Open错误参数测试"){

	EXPECT_NE(CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_TYPE"), 0, 0, NULL), CHDRV_FLASH_OK);
}
/*
CASE__F(TEST_FLASH_PARAMS2.
	"CHDRV_FLASH_OpenEx错误参数测试"){

	EXPECT_EQ(CHDRV_FLASH_OpenEx(NULL, 0, 0, &gh_flashHdl, 0, NULL), CHDRV_FLASH_OK);
}
*/

CASE_F(TEST_FLASH_PARAMS3,
	"CHDRV_FLASH_Read错误参数测试"){

	S32 i_readNumber = 1;
	EXPECT_NE(CHDRV_FLASH_Read(gh_flashHdl, 0, 0, &i_readNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_PARAMS4,
	"CHDRV_FLASH_Erase错误参数测试"){

	EXPECT_NE(CHDRV_FLASH_Erase(gh_flashHdl, 0, 0), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_PARAMS5,
	"CHDRV_FLASH_Write错误参数测试"){

	S32 i_writeNumber = 1;
	EXPECT_NE(CHDRV_FLASH_Write(gh_flashHdl, 0, 0, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_PARAMS6,
	"CHDRV_FLASH_DirectWrite错误参数测试"){

	S32 i_writeNumber = 1;
	U8 *dataBuf = NULL;

	
	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		dataBuf = (U8*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 1024);
		if(!dataBuf)
		{
			GTEST_FAIL() << "分配内存失败";
			return;
		}
		memset(dataBuf, 0, sizeof(U8)*1024 );
		EXPECT_NE(CHDRV_FLASH_DirectWrite(gh_flashHdl, 0, dataBuf, &i_writeNumber), CHDRV_FLASH_OK);
		CH_RTOS_FreeMemory(gh_APP_HEAP, dataBuf);
	}
	else
	{
		GTEST_FAIL() << "应通过死亡测试";
	}
}

CASE_F(TEST_FLASH_PARAMS7,
	"CHDRV_FLASH_Lock错误参数测试"){

	//EXPECT_EQ(CHDRV_FLASH_Lock(gh_flashHdl, 0, 0, 0), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_PARAMS8,
	"CHDRV_FLASH_UnLock错误参数测试"){

	//CHDRV_FLASH_Lock(gh_flashHdl, 0, 10, 0);
	//EXPECT_EQ(CHDRV_FLASH_UnLock(gh_flashHdl, 0, 0), CHDRV_FLASH_OK);
}
/*
CASE_F(TEST_FLASH_PARAMS9,
	"CHDRV_FLASH_LockQuery错误参数测试"){

	EXPECT_EQ(CHDRV_FLASH_LockQueryEx(0, 0x00, NULL), CHDRV_FLASH_OK);
}
*/

CASE_F(TEST_FLASH_OPEN1,
	"CHDRV_FLASH_Open合法的参数是否成功,句柄不为NULL"){

	gh_flashHdl = NULL;
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0"), fw_getenv_i("FLASH_LENGTH_0"), &gh_flashHdl), CHDRV_FLASH_OK);
		EXPECT_NE(gh_flashHdl, (void *)NULL);
	}
	else if(strcmp(fw_getenv("FLASH_TYPE"), "NOR") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0"), fw_getenv_i("FLASH_LENGTH_0"), &gh_flashHdl), CHDRV_FLASH_OK);
		EXPECT_NE(gh_flashHdl, (void *)NULL);
	}
	else
		FAIL() << "not env flash_type found";
}


CASE_F(TEST_FLASH_OPEN2,
	"CHDRV_FLASH_OpenEx合法的参数是否成功，句柄不为NULL")
	{
	
	//EXPECT_EQ(CHDRV_FLASH_Open_Ex(CHDRV_FLASH_DEVICE_0, 0x100000, 0x100000, &gh_flashHdl, CHDRV_FLASH_O_SYNC), CHDRV_FLASH_OK);
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
	//	EXPECT_EQ(CHDRV_FLASH_Open_Ex((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0"), fw_getenv_i("FLASH_LENGTH_0"), &gh_flashHdl, CHDRV_FLASH_O_SYNC), CHDRV_FLASH_OK);
	}
}




CASE_F(TEST_FLASH_READ1,
	"CHDRV_FLASH_Read按BLOCK_SIZE对齐，应返回成功，数据暂存到内存中"){

	memset(guc_read_buffer, 0, sizeof(U8)*DATA_SIZE);
	S32 numRead = DATA_SIZE;
	EXPECT_EQ(CHDRV_FLASH_Read(gh_flashHdl, 0, guc_read_buffer, &numRead), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_ERASE1,
	"CHDRV_FLASH_Erase按BLOCK_SIZE对齐，应返回成功,依次擦除完全部FLASH_LENGTH_0"){

	int i;
	for(i=0;i<(fw_getenv_i("FLASH_LENGTH_0"));i=(i+fw_getenv_i("FLASH_BLOCK_SIZE")))
	{
		EXPECT_EQ(CHDRV_FLASH_Erase(gh_flashHdl,0+i,fw_getenv_i("FLASH_BLOCK_SIZE")), CHDRV_FLASH_OK);
	}
}




CASE_F(TEST_FLASH_WRITE1,
	"CHDRV_FLASH_Write按BLOCK_SIZE对齐，应返回成功"){

	memset(write_buffer, 1, sizeof(U8)*BLOCK_SIZE);

	S32 i_writeNumber = BLOCK_SIZE;
	EXPECT_EQ(CHDRV_FLASH_Write(gh_flashHdl, 0, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_READ2,
	"CHDRV_FLASH_Read按BLOCK_SIZE对齐应返回成功的数据一致"){

	U8 *read_buffer = (U8 *)data_buffer;
	S32 i_readNumber = BLOCK_SIZE;

	memset(read_buffer, 0, sizeof(U8)*BLOCK_SIZE);
	EXPECT_EQ(CHDRV_FLASH_Read(gh_flashHdl, 0, read_buffer, &i_readNumber), CHDRV_FLASH_OK);
	EXPECT_EQ(bufferCompare(write_buffer, read_buffer, i_readNumber), 0);
}

CASE_F(TEST_FLASH_WRITE2,
	"CHDRV_FLASH_DirectWrite按BLOCK_SIZE对齐，应返回成功"){

	S32 i_writeNumber = BLOCK_SIZE;
	EXPECT_EQ(CHDRV_FLASH_DirectWrite(gh_flashHdl, 0, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_READ3,
	"CHDRV_FLASH_Read按BLOCK_SIZE对齐，应返回成功，读出的数据与写入的数据一致"){

	U8 *read_buffer = (U8 *)data_buffer;
	S32 i_readNumber = BLOCK_SIZE;
	
	memset(read_buffer, 0, sizeof(U8)*BLOCK_SIZE);
	EXPECT_EQ(CHDRV_FLASH_Read(gh_flashHdl, 0, read_buffer, &i_readNumber), CHDRV_FLASH_OK);
	EXPECT_EQ(bufferCompare(write_buffer, read_buffer, i_readNumber), 0);
}

CASE_F(TEST_FLASH_CLOSE1,
	"CHDRV_FLASH_Close应返回成功"){

	EXPECT_EQ(CHDRV_FLASH_Close(gh_flashHdl), CHDRV_FLASH_OK);
	gh_flashHdl = NULL;
}

CASE_F(TEST_FLASH_OPEN3,
	"测试offset和length不按BLOCK_SIZE对齐，调用CHDRV_FLASH_Open是否成功"){
	
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0")+BLOCK_SIZE/4, fw_getenv_i("FLASH_LENGTH_0")-BLOCK_SIZE/4, &gh_flashHdl), CHDRV_FLASH_OK);
	}
	else if(strcmp(fw_getenv("FLASH_TYPE"), "NOR") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0") -BLOCK_SIZE/4, fw_getenv_i("FLASH_LENGTH_0") -BLOCK_SIZE/4, &gh_flashHdl), CHDRV_FLASH_OK);
		EXPECT_NE(gh_flashHdl, (void *)NULL);
	}
	else
		FAIL() << "not env flash_type found";
}

CASE_F(TEST_FLASH_OPEN4,
	"测试offset和length不按BLOCK_SIZE对齐，调用CHDRV_FLASH_OpenEx是否成功"){

	if(strcmp(fw_getenv("FLASH_TYPE"), "NOR") == 0)
	{
//		EXPECT_EQ(CHDRV_FLASH_Open_Ex((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0")+BLOCK_SIZE/4, fw_getenv_i("FLASH_LENGTH_0")-BLOCK_SIZE/4, &gh_flashHdl, CHDRV_FLASH_O_DEVID_IS_MTDID), CHDRV_FLASH_OK);
	}
}

CASE_F(TEST_FLASH_ERASE2,
	"测试offset和length不按BLOCK_SIZE对齐，调用CHDRV_FLASH_Erase应成功"){
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Erase(gh_flashHdl, 0+BLOCK_SIZE/4, BLOCK_SIZE/4), CHDRV_FLASH_OK);
	}
}

CASE_F(TEST_FLASH_WRITE3,
	"测试offset和length不按BLOCK_SIZE对齐，调用CHDRV_FLASH_Write应成功"){

	S32 i_writeNumber = BLOCK_SIZE;
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Write(gh_flashHdl, 0+BLOCK_SIZE/4, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
	}
}

CASE_F(TEST_FLASH_READ4,
	"测试offset和length不按BLOCK_SIZE对齐，CHDRV_FLASH_Read应返回成功，读出的数据与写入的数据一致"){

	U8 *read_buffer = (U8 *)data_buffer;
	S32 i_readNumber = BLOCK_SIZE;
	
	memset(read_buffer, 0, sizeof(U8)*BLOCK_SIZE);
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		EXPECT_EQ(CHDRV_FLASH_Read(gh_flashHdl, 0+BLOCK_SIZE/4, read_buffer, &i_readNumber), CHDRV_FLASH_OK);
		EXPECT_EQ(bufferCompare(write_buffer, read_buffer, i_readNumber), 0);
	}
}

CASE_F(TEST_FLASH_WRITE4,
	"测试offset和length不按BLOCK_SIZE对齐，调用CHDRV_FLASH_DirectWrite应成功"){

	S32 i_writeNumber = BLOCK_SIZE;
	
	EXPECT_EQ(CHDRV_FLASH_DirectWrite(gh_flashHdl, 0+BLOCK_SIZE/4, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_READ5,
	"CHDRV_FLASH_Read不按按BLOCK_SIZE对齐，应返回成功，读出的数据与写入的数据一致"){

	U8 *read_buffer = (U8 *)data_buffer;
	S32 i_readNumber = BLOCK_SIZE;

	memset(read_buffer, 0, sizeof(U8)*BLOCK_SIZE);
	EXPECT_EQ(CHDRV_FLASH_Read(gh_flashHdl, BLOCK_SIZE/4, read_buffer, &i_readNumber), CHDRV_FLASH_OK);
	EXPECT_EQ(bufferCompare(write_buffer, read_buffer, i_readNumber), 0);
}

CASE_F(TEST_FLASH_CLOSE2,
	"CHDRV_FLASH_Close应返回成功"){

	EXPECT_EQ(CHDRV_FLASH_Close(gh_flashHdl), CHDRV_FLASH_OK);
	gh_flashHdl = NULL;
}

#if 0
CASE_F(TEST_FLASH_LOCK1,
	"调用CHDRV_FLASH_Lock，应当成功"){
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0)
	{
		CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), 0, 0x100000, &gh_flashHdl);
	}
	else
	{
		EXPECT_EQ(CHDRV_FLASH_Open_Ex((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), 0, 0x100000, &gh_flashHdl, CHDRV_FLASH_O_DEVID_IS_MTDID), CHDRV_FLASH_OK);
	}
	EXPECT_EQ(CHDRV_FLASH_Lock(gh_flashHdl, 0, 0x100000, 0), CHDRV_FLASH_OK);
} 
/*
CASE_F(TEST_FLASH_LOCK2,
	"调用CHDRV_FLASH_LockQuery，查询结果应与预期一致"){

	EXPECT_EQ(CHDRV_FLASH_LockQueryEx(U32 ui_DevID,U32 ui_AbsAddr,CHDRV_FLASH_LOCK_INFO_t * pstr_LockInfo));
}
*/
CASE_F(TEST_FLASH_LOCK3,
	"测试在LOCK区域内擦除,应当不能被擦除"){

	EXPECT_NE(CHDRV_FLASH_Erase(gh_flashHdl, 0, 0x100000), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_LOCK4,
	"测试在LOCK区域内写入操作,数据应当不能被改写."){

	S32 i_writeNumber = BLOCK_SIZE;
	
	EXPECT_NE(CHDRV_FLASH_Write(gh_flashHdl, 0, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_LOCK5,
	"测试在LOCK区域内临近区域的写入,应当能正常操作."){

	S32 i_writeNumber = BLOCK_SIZE;

	EXPECT_EQ(CHDRV_FLASH_Write(gh_flashHdl, 0x100000, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_LOCK6,
	"测试CHDRV_FLASH_UnLock，应当返回成功"){

	EXPECT_EQ(CHDRV_FLASH_UnLock(gh_flashHdl , 0, 0x100000), CHDRV_FLASH_OK);
}

/*
CASE_F(TEST_FLASH_LOCK7,
	"调用CHDRV_FLASH_LockQuery，查询结果应与预期一致"){


	EXPECT_EQ(CHDRV_FLASH_LockQueryEx(U32 ui_DevID,U32 ui_AbsAddr,CHDRV_FLASH_LOCK_INFO_t * pstr_LockInfo));
}
*/

CASE_F(TEST_FLASH_LOCK8,
	"测试在LOCK区域内擦除,应当可以被擦除."){

	EXPECT_EQ(CHDRV_FLASH_Erase(gh_flashHdl, 0, 0x100000), CHDRV_FLASH_OK);
}

CASE_F(TEST_FLASH_LOCK9,
	"测试在LOCK区域内写入操作,数据应该可以被改写"){

	S32 i_writeNumber = BLOCK_SIZE;

	EXPECT_EQ(CHDRV_FLASH_Write(gh_flashHdl, 0, write_buffer, &i_writeNumber), CHDRV_FLASH_OK);
	EXPECT_EQ(CHDRV_FLASH_Close(gh_flashHdl), CHDRV_FLASH_OK);
	gh_flashHdl = NULL;
}
#endif
CASE_F(TEST_FLASH_WRITE5,
	"将内存内保存的临时数据写回FLASH，恢复成本测试之前的数据"){

	S32 i_writeNumber = DATA_SIZE;
	
	if(strcmp(fw_getenv("FLASH_TYPE"), "NAND") == 0
	|| strcmp(fw_getenv("FLASH_TYPE"), "NOR") == 0)
	{
		CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), fw_getenv_i("FLASH_OFFSET_0"), fw_getenv_i("FLASH_LENGTH_0"), &gh_flashHdl);
	}
	else
	{
//		EXPECT_EQ(CHDRV_FLASH_Open_Ex((CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID_0"), 0x100000, 0x100000, &gh_flashHdl, CHDRV_FLASH_O_DEVID_IS_MTDID), CHDRV_FLASH_OK);
	}

	EXPECT_EQ(CHDRV_FLASH_Write(gh_flashHdl, 0, guc_read_buffer, &i_writeNumber), CHDRV_FLASH_OK);
	EXPECT_EQ(CHDRV_FLASH_Close(gh_flashHdl), CHDRV_FLASH_OK);
	gh_flashHdl = NULL;
}
#endif /* ENABLE_TESTCASE */

/*
CASE_F(TEST_FLASH_READ,
	"test flash read if ok") {

	CHDRV_FLASH_Handle handle = NULL;
	int ret, i;
	int offset = 0, len = 0x400000;
	S32 read_len = 0x1000;
	CHDRV_FLASH_DEVICE_e device_id = CHDRV_FLASH_DEVICE_0;
	char *pstr = NULL, *mtd_name = NULL;
	char *flash_type = NULL;

	//get config value about flash
	device_id = (CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID");
	offset = fw_getenv_i("FLASH_READ_OFFSET");
	len = fw_getenv_i("FLASH_READ_LENGTH");

	ASSERT_LT(0x1, len);
	
	ASSERT_NE((void*)NULL, pstr = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, len));

	memset(pstr, 0xf3, len);
	flash_type = fw_getenv("FLASH_TYPE");
	if(strcmp(flash_type, "nand") == 0)
	{
		mtd_name = fw_getenv("FLASH_READ_MTDNAME");
		
		//EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_OpenEx(mtd_name, offset, len, &handle, CHDRV_FLASH_O_RAW_MODE, NULL));
	}
	else
	{
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Open(device_id, offset, len, &handle));
	}

	//check diffrent read length if ok
	i = 1;
	while(1)
	{
		if(i > len)i = len;
		read_len = i;
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Read(handle, 0, (U8*)pstr, (S32*)&read_len)) << "the read length not equal open size";
		EXPECT_EQ(i, read_len);
		EXPECT_NE(0, memchar_cmp(pstr, 0xf3, i)) << "When read flash, the memory must be modified.";
		if(i == len)
		{
			break;
		}
		i <<= 1;
	}
	CH_RTOS_FreeMemory(gh_APP_HEAP, pstr);
	EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Close(handle));
}

CASE_F(TEST_FLASH_WRITE,
	"test flash write if ok") {

	CHDRV_FLASH_Handle handle = NULL;
	int ret, i;
	int offset = 0, len = 0x400000,  block_size = 0x1000;
	S32 read_len = 0;
	CHDRV_FLASH_DEVICE_e device_id = CHDRV_FLASH_DEVICE_0;
	char *pstr = NULL, *pbak = NULL;
	char  *mtd_name = NULL;
	char *flash_type = NULL;

	//get config value about flash
	device_id = (CHDRV_FLASH_DEVICE_e)fw_getenv_i("FLASH_DEVICE_ID");
 	offset = fw_getenv_i("FLASH_WRITE_OFFSET");
 	len = fw_getenv_i("FLASH_WRITE_LENGTH");
 	block_size = fw_getenv_i("FLASH_BLOCK_SIZE");
 

	

	ASSERT_NE((void*)NULL, pstr = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, block_size)) << "alloc memory size=" << block_size << "fail";
	ASSERT_NE((void*)NULL, pbak = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, block_size))<< "alloc memory size=" << block_size << "fail";
	memset(pstr, 0xf2, block_size);
	flash_type = fw_getenv("FLASH_TYPE");
	if(strcmp(flash_type, "nand") == 0)
	{
		mtd_name = fw_getenv("FLASH_READ_MTDNAME");
		
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_OpenEx(mtd_name, offset, len, &handle, CHDRV_FLASH_O_RAW_MODE, NULL));
	}
	else
	{
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Open(device_id, offset, len, &handle));
	}

	i = 1;
	for(i = 0; i < len; i += block_size)
	{

		read_len = block_size;
		//backup the flash content
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Read(handle, i, (U8*)pbak, &read_len)) << "the read length not equal open size";
		EXPECT_EQ(block_size, read_len);
		
		//write new content to flash
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Erase(handle, i, block_size));
		read_len = block_size;
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Write(handle, i, (U8*)pstr, &read_len));
		EXPECT_EQ(block_size, read_len);
		read_len = block_size;
		
		//read the flash content after write
		memset(pstr, 0, block_size);
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Read(handle, i, (U8*)pstr, &read_len));
		EXPECT_EQ(block_size, read_len);
		
		//compare the content if ok
		EXPECT_NE(0, memchar_cmp(pstr, 0xf2, i)) << "check the buffer if really write to flash";
		//restore the backup to flash
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Erase(handle, i, block_size));
		read_len = block_size;
		EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Write(handle, i, (U8*)pbak, &read_len));
		EXPECT_EQ(block_size, read_len);
		
	}
	CH_RTOS_FreeMemory(gh_APP_HEAP, pstr);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbak);
	EXPECT_EQ(CHDRV_FLASH_OK,  CHDRV_FLASH_Close(handle));
}
*/
CHDRV_FLASH_Handle gh_cmd_flash = NULL;
static int flash_open(int argc, CMD_PARAM argv[])
{
	int ret;

	if(gh_cmd_flash)
	{
		CHDRV_FLASH_Close(gh_cmd_flash);
		gh_cmd_flash = NULL;
	}

	ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), argv[2].get_i(), argv[3].get_i(), &gh_cmd_flash);

	LOG("CHDRV_FLASH_Open %s, handle=0x%x\n", ret == CHDRV_FLASH_OK ? "ok" : "fail", gh_cmd_flash);
	return ret;
}

static int flash_open_ex(int argc, CMD_PARAM argv[])
{
	//int ret;

	if(gh_cmd_flash)
	{
		CHDRV_FLASH_Close(gh_cmd_flash);
		gh_cmd_flash = NULL;
	}
	//ret = CHDRV_FLASH_OpenEx(argv[1].get_str(), argv[2].get_i(), argv[3].get_i(), &gh_cmd_flash, argv[4].get_i(), NULL);
	//LOG("flash_open_ex %s, handle=0x%x\n", ret == CHDRV_FLASH_OK ? "ok" : "fail", gh_cmd_flash);
	return 0;
}


static int flash_close(int argc, CMD_PARAM argv[])
{
	int ret = -1;

	if(gh_cmd_flash)
	{
		ret = CHDRV_FLASH_Close(gh_cmd_flash);
		gh_cmd_flash = NULL;
	}
	else
	{
		LOG("flash not open\n");
		return -1;
	}

	LOG("CHDRV_FLASH_Close %s\n", ret == CHDRV_FLASH_OK ? "ok" : "fail");
	return ret;
}

static int flash_read(int argc, CMD_PARAM argv[])
{
	int ret, i;
	S32 read_len;
	char *pbuf = NULL;
	CH_FSYS_FileHandle fp = 0;
	
	if(!gh_cmd_flash)
	{
		LOG("ERORR: please open flash first\n");
		return -1;
	}

	read_len = argv[2].get_i();

	pbuf = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, read_len);
	if(!pbuf)
	{
		LOG("error: can't alloc memory, size=%d\n", read_len);
		return -1;
	}
	ret = CHDRV_FLASH_Read(gh_cmd_flash, argv[1].get_i(), (U8*)pbuf, &read_len);
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHDRV_FLASH_Read return %d\n", ret);
		goto iERROR;
	}
	else if(read_len != argv[2].get_i())
	{
		LOG("read_len(%d)!= argv[3](%d)\n", read_len, argv[3].get_i());
		ret = -1;
		goto iERROR;
	}

	//print content
	LOG("read buffer content is:\n");
	for(i = 0; i < 64; i++)
	{
		printf("%02x ", (U8)pbuf[i]);
		if(i && i % 8 == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	//save file
	fp = CH_FSYS_OpenFile((const S8 *)argv[3].get_str(), (const S8 *)"wb");
	if(!fp)
	{
		LOG("open %s fail\n", argv[3].get_str());
		goto iERROR;
	}
	ret = CH_FSYS_WriteFile(fp, pbuf, 1, read_len);
	if(!fp)
	{
		LOG("CH_FSYS_WriteFile %s size=%d return %d\n", argv[3].get_str(), read_len, ret);
		goto iERROR;
	}
	ret = CH_FSYS_CloseFile(fp);
	if(ret != CH_FSYS_OK)
	{
		LOG("CH_FSYS_CloseFile %s return %d\n", argv[3].get_str(), ret);
		goto iERROR;
	}
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	LOG("flash read ok, write content to %s\n", argv[3].get_str());
	return 0;

iERROR:
	if(fp)
		CH_FSYS_CloseFile(fp);
	if(pbuf)
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	return -1;
}


static int flash_write(int argc, CMD_PARAM argv[])
{
	int ret;
	S32 read_len;
	char *pbuf = NULL;
	CH_FSYS_FileHandle fp = 0;
	
	
	if(!gh_cmd_flash)
	{
		LOG("ERORR: please open flash first\n");
		return -1;
	}

	//read file
	read_len = fsys_read_file(argv[1].get_str(), pbuf);
	if(read_len <= 0)
	{
		LOG("ERROR: open %s fail\n", (const S8 *)argv[1].get_str());
		return -1;
	}

	ret = CHDRV_FLASH_Erase(gh_cmd_flash, argv[2].get_i(), (read_len + 0xfff) & (~0xfff));
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHDRV_FLASH_Erase 0x%x-0x%x return %d\n", argv[2].get_i(), (read_len + 0xfff) & (~0xfff), ret);
		goto iERROR;
	}
	ret = CHDRV_FLASH_Write(gh_cmd_flash, argv[2].get_i(), (U8*)pbuf, &read_len);
	if(ret != CHDRV_FLASH_OK)
	{
		LOG("CHDRV_FLASH_Write 0x%x-0x%x return %d\n", argv[2].get_i(), (read_len + 0xfff) & (~0xfff), ret);
		goto iERROR;
	}
	
	
	ret = CH_FSYS_CloseFile(fp);
	if(ret != CH_FSYS_OK)
	{
		LOG("CH_FSYS_CloseFile %s return %d\n", argv[3].get_str(), ret);
		goto iERROR;
	}
	//CH_FSYS_CloseFile(fp);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	LOG("flash write ok, write offset = 0x%x, length=0x%x\n", argv[2].get_i(), read_len);
	return 0;

iERROR:
	if(fp)
		CH_FSYS_CloseFile(fp);
	if(pbuf)
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	return -1;
}

static int flash_init(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CHDRV_FLASH_Init();
	if(ret != 0)
	{
		PROMPT("Flash init Fail!\n");
		return -1;
	}
	return 0;
}

static int flash_term(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CHDRV_FLASH_Term();
	if(ret != 0)
	{
		PROMPT("Flash Term Fail!\n");
		return -1;
	}
	return 0;

}

static int flash_ver(int argc, CMD_PARAM argv[])
{
	CHDRV_Version_t stru_Ver;
	int ret;
	ret = CHDRV_FLASH_GetVersion(&stru_Ver);
	if(ret != 0)
	{
		PROMPT("Flash Get Version Fail!\n");
		return -1;
	}
	PROMPT("version = %d\n",stru_Ver);
	return 0;
}

static int flash_rwtest(int argc, CMD_PARAM argv[])
{
	int ret, i;
	S32 read_len, loop_times, test_len, size;
	int flash_offset;
	U8 *read_buffer = NULL, *buffer = NULL;
	CHDRV_FLASH_Handle h_flash = NULL;

	if(write_buffer == NULL)
	{
		guc_read_buffer = new U8[DATA_SIZE];
		write_buffer = new U8[BLOCK_SIZE];
	}
#if 1	
	flash_offset = argv[2].get_i();
	test_len = argv[3].get_i();
	loop_times = argv[4].get_i();

	if(test_len > 0)
	{
		read_buffer = new U8[test_len];
	}
	for(i = 0; i < loop_times || loop_times <= 0; i++)
	{
		EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), flash_offset, test_len, &h_flash));
		if(ret != CHDRV_FLASH_OK)
		{
			break;
		}

		size = test_len;
		EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Read(h_flash, 0, read_buffer, &size));
		if(ret != CHDRV_FLASH_OK || size != test_len)
		{
			LOG("ERROR : CHDRV_FLASH_Read return %d, size=%d\n", ret, size);
			ret = -1;
			break;
		}
		buffer = read_buffer;
		
		memset(write_buffer, i & 0xff, BLOCK_SIZE);
		for(read_len = 0; read_len < test_len; read_len += BLOCK_SIZE)
		{
			
			EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Erase(h_flash, read_len, BLOCK_SIZE));
			if(ret != CHDRV_FLASH_OK)
			{
				break;
			}
			size = BLOCK_SIZE;
			EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Write(h_flash, read_len, write_buffer, &size));
			if(ret != CHDRV_FLASH_OK || size != BLOCK_SIZE)
			{
				LOG("ERROR : CHDRV_FLASH_Write return %d, size=%d\n", ret, size);
				ret = -1;
				break;
			}
			size = BLOCK_SIZE;
			memset(guc_read_buffer, 0, BLOCK_SIZE);
			EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Read(h_flash, read_len, guc_read_buffer, &size));
			if(ret != CHDRV_FLASH_OK || size != BLOCK_SIZE)
			{
				LOG("ERROR : CHDRV_FLASH_Read return %d, size=%d\n", ret, size);
				ret = -1;
				break;
			}
			EXPECT_EQ(0, ret = memcmp(guc_read_buffer, write_buffer, BLOCK_SIZE));
			if(ret != 0)
			{
				LOG("ERROR : memcmp return %d\n", ret);
				ret = -1;
				break;
			}
			LOG("###   read_len = 0x%x\n", read_len);
			EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Erase(h_flash, read_len, BLOCK_SIZE));
			if(ret != CHDRV_FLASH_OK)
			{
				break;
			}
			size = BLOCK_SIZE;
			EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Write(h_flash, read_len, buffer, &size));
			if(ret != CHDRV_FLASH_OK || size != BLOCK_SIZE)
			{
				LOG("ERROR : CHDRV_FLASH_Write return %d, size=%d\n", ret, size);
				ret = -1;
				break;
			}
			buffer += BLOCK_SIZE;
		}
		if(ret != 0)
		{
			break;
		}
		EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Close(h_flash));
		if(ret != CHDRV_FLASH_OK)
		{
			LOG("ERROR : CHDRV_FLASH_Close return %d\n", ret);
			ret = -1;
			break;
		}
		h_flash = NULL;
		LOG("test times = %d\n", i);
	}

	if(h_flash)
	{
		EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Close(h_flash));
	}

#endif
	if(test_len > 0)
	{
		delete []read_buffer;
		read_buffer = NULL;
	}
	
	if(guc_read_buffer)
	{
		delete []guc_read_buffer;
		guc_read_buffer = NULL;
		delete []write_buffer;
		write_buffer = NULL;
	}
	return 0;
}

#if 0
static int flash_lock(int argc, CMD_PARAM argv[])
{
	int ret;
	S32 flash_offset, test_len;
	CHDRV_FLASH_Handle h_flash = NULL;

	flash_offset = argv[2].get_i();
	test_len = argv[3].get_i();
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), flash_offset, test_len, &h_flash));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Open Fail!\n");
		return -1;
	}
	
	ret = CHDRV_FLASH_Lock((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), NULL, flash_offset, test_len, 0);
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Lock Fail!\n");
		return -1;
	}

	return 0;

}

#else
static int flash_lock(int argc, CMD_PARAM argv[])
{
	int ret;
	S32 flash_offset, test_len;
	CHDRV_FLASH_Handle h_flash = NULL;

	flash_offset = argv[2].get_i();
	test_len = argv[3].get_i();
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), flash_offset, test_len, &h_flash));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Open Fail!\n");
		goto iERROR;
	}
#if (defined(CFG_CHSSA_CHIP_MSTAR5019) || defined(CFG_CHSSA_CHIP_MSTAR5029))
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_SPI_ChangeToBottom((CHDRV_FLASH_DEVICE_e)argv[1].get_i()));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Change Bottom Mode Fail!\n");
		goto iERROR;
	}
#endif	
	ret = CHDRV_FLASH_Lock(0, NULL, flash_offset, test_len, 0);
	if(ret != 0)
	{
		PROMPT("Flash Lock Fail!\n");
		goto iERROR;
	}
	if(h_flash)
	{
		CHDRV_FLASH_Close(h_flash);
	}
	return 0;
iERROR:
	if(h_flash)
	{
		CHDRV_FLASH_Close(h_flash);
	}
	return -1;

}
#endif

#if 0
static int flash_unlock(int argc, CMD_PARAM argv[])
{
	int ret;
	S32 flash_offset, test_len;
	CHDRV_FLASH_Handle h_flash;

	flash_offset = argv[2].get_i();
	test_len = argv[3].get_i();
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), flash_offset, test_len, &h_flash));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Open Fail!\n");
		return -1;
	}
	
	ret = CHDRV_FLASH_UnLock((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), NULL, flash_offset, test_len);
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Unlock Fail!\n");
		return -1;
	}
	return 0;

}

#else
static int flash_unlock(int argc, CMD_PARAM argv[])
{
	int ret;
	S32 flash_offset, test_len;
	CHDRV_FLASH_Handle h_flash;

	flash_offset = argv[2].get_i();
	test_len = argv[3].get_i();
	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_Open((CHDRV_FLASH_DEVICE_e)argv[1].get_i(), flash_offset, test_len, &h_flash));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Open Fail!\n");
		goto iERROR;
	}
	
	ret = CHDRV_FLASH_UnLock(0, NULL, flash_offset, test_len);
	if(ret != 0)
	{
		PROMPT("Flash Unlock Fail!\n");
		goto iERROR;
	}
	if(h_flash)
	{
		CHDRV_FLASH_Close(h_flash);
	}
	return 0;

iERROR:
	if(h_flash)
	{
		CHDRV_FLASH_Close(h_flash);
	}
	return -1;
}
#endif

static int flash_lock_status(int argc, CMD_PARAM argv[])
{
	int ret, cnt;
	CHDRV_FLASH_LOCK_INFO_t *stru_lockStatus;

	EXPECT_EQ(CHDRV_FLASH_OK, ret = CHDRV_FLASH_LockQuery(0, NULL, 0, -1, &cnt, &stru_lockStatus));
	if(ret != CHDRV_FLASH_OK)
	{
		PROMPT("Flash Query Fail!\n");
		return -1;
	}
	PROMPT("ret=%d,b_Locked=%d,addr=0x%x,size=0x%x",ret,stru_lockStatus[0].b_Locked,stru_lockStatus[0].ui_StarAddr,stru_lockStatus[0].ui_Size);
	return 0;	
}

REGISTER_CMD(flash_open,  "打开FLASH以供读写", "device_id| offset|length", "0 0x300000 0x100000");
REGISTER_CMD(flash_open_ex,  "以指定的模式打开FLASH以供读写", "mtd_name|offset|length|open_flag", "ch_flash_data 0x0 0x100000 2");
REGISTER_CMD(flash_close,  "关闭FLASH句柄", "", "");
REGISTER_CMD(flash_read,  "读取FLASH并保存成文件", "offset|length|filename", "0 0x10000 c:/flash.bin");
REGISTER_CMD(flash_write,  "将一个文件写到FLASH的指定位置", "filename|offset", "c:/flash.bin 0");
REGISTER_CMD(flash_init,  "初始化flash模块", "", "");
REGISTER_CMD(flash_ver,  "打印flash版本号", "", "");
REGISTER_CMD(flash_term,  "销毁flash模块", "", "");
REGISTER_CMD(flash_rwtest,  "长时间测试flash读写,rw_times为0表示一直读写", "device_id|offset|length|rw_times", "0 0x1a0000 0x320000 5");
REGISTER_CMD(flash_lock,  "锁定flash", "device_id(mtd_name)|offset|length", "0 0 0x100000");
REGISTER_CMD(flash_unlock,"解锁flash", "device_id(mtd_name)|offset|length", "0 0 0x100000");//解锁一部分就可以解锁全部
REGISTER_CMD(flash_lock_status,"查询flash锁定状态", "device_id(mtd_name)", "0");





