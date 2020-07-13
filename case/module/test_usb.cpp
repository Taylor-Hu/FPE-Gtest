/**
@file test_usb.cpp
@defgroup usb测试集

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
#define CASE_NAME TestUsb
#include "test_menu.h"
#include "test_usb.h"



void TestUsb::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
}
void TestUsb::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner rtos TestCase " << std::endl;
}

void TestUsb::SetUp()
{
//execute before testcase
//std::cout << "Chdrv SetUp" << std::endl;
}
void TestUsb::TearDown()
{
//execute before testcase
//std::cout << "Chdrv TearDown" << std::endl;
}

char* test_usb_ReadDir(char *ruc_ext, char *ruc_FilePath, int ri_MaxLen)
{
	S8 uc_Path[1024] = "c:";
	U32 rui_FileNums = 0;
	CH_FSYS_DirFileInfo_t * pstru_DirInfo = NULL;
	int i, num;
	char *pc_user_input = NULL;

	
	memset(ruc_FilePath, 0, ri_MaxLen);
	CH_FSYS_GetFileAndDirNumber(&uc_Path[0], &rui_FileNums);

	EXPECT_NE(rui_FileNums, 0) << "未检测到U盘";

	if(rui_FileNums == 0)
	{
		return NULL;
	}

	while(1)
	{
		pstru_DirInfo = (CH_FSYS_DirFileInfo_t*)CH_RTOS_AllocateAndClearMemory(gh_APP_HEAP, sizeof(CH_FSYS_DirFileInfo_t) * rui_FileNums);
		EXPECT_NE(pstru_DirInfo, (CH_FSYS_DirFileInfo_t*)NULL) << "分配内存失败";
		
		CH_FSYS_GetDirInfo(&uc_Path[0], 0, pstru_DirInfo, &rui_FileNums);

		for(i = 0; i < rui_FileNums; i++)
		{
			printf("%d[%s]. %s\n", 
				i, 
				pstru_DirInfo[i].enm_FileType == CH_FSYS_FILE ? "FILE" : "DIR",
				pstru_DirInfo[i].uc_FileLongName);
		}
		PROMPT("q. 退出测试\n");
		do
		{
			PROMPT("请输入文件或目录序号(0~" << (rui_FileNums > 0 ? (rui_FileNums - 1) : 0 )<< "):[0]\n");
			num = 0;
			pc_user_input = test_get_input();
			if(pc_user_input[0] == 'q')
			{
				return NULL;
			}
			sscanf((char*)pc_user_input, "%d", &num);
		}while(num < 0 || num >= rui_FileNums);

			
		if(pstru_DirInfo[num].enm_FileType == CH_FSYS_FILE)
		{
			sprintf(ruc_FilePath, "%s/%s",
				uc_Path,
				pstru_DirInfo[num].uc_FileLongName);
			CH_RTOS_FreeMemory(gh_APP_HEAP, pstru_DirInfo);
			pstru_DirInfo = NULL;
			return ruc_FilePath;
			//return CH_TRUE;
		}
		else
		{
			if(strcmp((char*)pstru_DirInfo[num].uc_FileLongName, ".") == 0)
			{

			}
			else if(strcmp((char*)pstru_DirInfo[num].uc_FileLongName, "..") == 0)
			{
				i = strlen((char*)uc_Path) - 1;
				for(; i > 0; i--)
				{
					if(uc_Path[i] == '/')
					{
						uc_Path[i] = 0;
						break;
					}
				}
			}
			else
			{
				snprintf(ruc_FilePath, ri_MaxLen, "%s/%s",
						uc_Path,
						pstru_DirInfo[num].uc_FileLongName);
				strcpy((char*)uc_Path, (char*)ruc_FilePath);	
			}
			CH_RTOS_FreeMemory(gh_APP_HEAP, pstru_DirInfo);
			pstru_DirInfo = NULL;
		}
	}
	return NULL;
}

#if 0 	/*sqzou20160608*/
CASE_F(TEST_USB_READDIR,
	"test usb ReadDir") {
	EXPECT_EQ(CH_TRUE, test_usb_ReadDir(".bin", "c:/", 20));
}
#endif


