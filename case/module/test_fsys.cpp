/**
@file test_fsys.cpp
@def group fsys测试集

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
#define CASE_NAME TestFsys


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
	//excute before each test suit
	//std::cout << "init SetUp rtos TestCase " << std::endl;
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

/*
2016年12月12日 需完善的点
1、错误参数，每次只错一个，其余参数正确
2、PROMPT只能用于需要输入前的提示
*/

#ifdef BLOCK_SIZE
#undef BLOCK_SIZE
#endif

#define BLOCK_SIZE 1024 * 256
#define BLOCK_NUMS 400
static int gh_fsysEvent[CH_FSYS_STATUS_MAX][3];
static vector<CH_FSYS_FileHandle> gh_fileHandle;
static CH_FSYS_DirHandle gh_dirHandle;
CH_BOOL usb_test_over = CH_FALSE;

CH_BOOL b_formateOver = CH_FALSE;
char usb_fsysName[50][100];
extern int fsys_info(int argc, CMD_PARAM argv[]);
//匹配存储的u盘文件及文件夹名

int matchUSBFileDirName(char *dir)
{
	char pathName[100];
	int fileNum = 0,j=0;
	CH_FSYS_DirFileInfo_t *stru_dirFile;
	
	memset(pathName,0,100);
	strcpy(pathName,dir);
	CH_FSYS_GetFileAndDirNumber(reinterpret_cast<S8 *>(pathName), reinterpret_cast<U32 *>(&fileNum));
	if(fileNum == 0)
	{
		return 0;
	}
	stru_dirFile = (CH_FSYS_DirFileInfo_t *)malloc(sizeof(CH_FSYS_DirFileInfo_t)*(fileNum+1));
	memset(stru_dirFile, 0, sizeof(CH_FSYS_DirFileInfo_t)*(fileNum+1));
	CH_FSYS_GetDirInfo(reinterpret_cast<S8 *>(pathName),0, &stru_dirFile[0], reinterpret_cast<U32 *>(&fileNum));
	
	int i;
	for(i= 0; i<fileNum;i++)
	{
		switch(stru_dirFile[i].enm_FileType)
		{
		case CH_FSYS_DIR:
			strcat(pathName,reinterpret_cast<char *>(stru_dirFile[i].uc_FileLongName));
			strcat(pathName,"/");
			strcpy(usb_fsysName[j], pathName);
			
			j++;
			
			//matchUSBFileDirName(pathName);
			
			break;
		case CH_FSYS_FILE:
			strcat(pathName,reinterpret_cast<char *>(stru_dirFile[i].uc_FileLongName));
			strcpy(usb_fsysName[j], pathName);
			strcat(pathName,"/");
			j++;
			break;
		default:
			break;
		}
		memset(pathName,0,100);
		strcpy(pathName,dir);
	}
	free(stru_dirFile);
	return j+1;
}

//读取指定的文件并将内容读到file_buf，file_buf的内存由本函数分配，返回已读取的文件长度
int fsys_read_file(const char *file_path, char *& file_buf)
{
	int len;
	CH_FSYS_FileHandle hfile = 0;
	S64 l_offset;
	int ret;
	
	
	hfile = CH_FSYS_OpenFile((const S8 *)file_path,(const S8 *)"r+");
	if(hfile == 0)
	{
		LOG("ERROR: CH_FSYS_OpenFile %s fail\n", file_path);
		return -1;
	}
	CHS64_SetValue32(0, 0, &l_offset);
	ret = CH_FSYS_SeekFile(hfile, l_offset, CH_FSYS_SEEK_END);
	if(ret != CH_FSYS_OK)
	{
		LOG("ERROR: CH_FSYS_SeekFile return %d\n", ret);
		CH_FSYS_CloseFile(hfile);
		return 0;
	}
	ret = CH_FSYS_TellFile(hfile, &l_offset);
	if(ret != CH_FSYS_OK)
	{
		LOG("ERROR: CH_FSYS_TellFile return %d\n", ret);
		CH_FSYS_CloseFile(hfile);
		return 0;
	}
	len = CHS64_GetLowValue(l_offset);
	CHS64_SetValue32(0, 0, &l_offset);
	ret = CH_FSYS_SeekFile(hfile, l_offset, CH_FSYS_SEEK_SET);
	if(ret != CH_FSYS_OK)
	{
		LOG("ERROR: CH_FSYS_SeekFile return %d\n", ret);
		CH_FSYS_CloseFile(hfile);
		return 0;
	}
	file_buf = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, len + 1);
	if(file_buf == NULL)
	{
		LOG("ERROR: CH_RTOS_AllocateMemory %d fail\n", len + 1);
		CH_FSYS_CloseFile(hfile);
		return -len ;
	}
	LOG("len=%d,file=%s\n", len, file_path);
	ret = CH_FSYS_ReadFile(hfile, file_buf, 1, len);
	CH_FSYS_CloseFile(hfile);
	if(ret <= 0)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, file_buf);
		file_buf = NULL;
	}
	else
	{
		file_buf[len] = 0;
	}
	return ret;
}

//读取指定的文件并将内容读到file_buf，file_buf的内存由本函数分配，返回已读取的文件长度
int fsys_write_file(const char *file_path, char * file_buf, int len)
{
	CH_FSYS_FileHandle hfile = 0;
	int ret;
	S64 ul_Size;
	
	hfile = CH_FSYS_OpenFile((const S8 *)file_path,(const S8 *)"w+");
	if(hfile == 0)
	{
		LOG("ERROR: CH_FSYS_OpenFile %s fail\n", file_path);
		return 0;
	}
	CHS64_SetValue32(0, 0, &ul_Size);
	CH_FSYS_ResizeFile((CH_FSYS_FileHandle)hfile, ul_Size);
	ret = CH_FSYS_WriteFile(hfile, file_buf, 1, len);
	CH_FSYS_CloseFile(hfile);
	return ret;
}



int findFileDirName(char *rp_srcPathName, char *&rp_fileDirName, int *pos)
{
	int i;
	*pos = 0;
	if(!rp_srcPathName || (i = strlen(rp_srcPathName)) == 0)
	{
		return -1;
	}
	
	while(i-- > 0)
	{
		if(rp_srcPathName[i] == '/')
		{
			*pos = i + 1;
			break;
		}
	}
	if(*pos == 0)
	{
		rp_fileDirName = NULL;
		return -1;
	}
	
	rp_fileDirName = (char *)malloc(sizeof(char)*(strlen(&rp_srcPathName[*pos])+ 2));
	
	strcpy(rp_fileDirName, &rp_srcPathName[*pos]);
	return 0;
}

static int cpFile(S8 *rpc_srcPath, S8 *rpc_destPath, U8 *longname)
{
	CH_FSYS_FileOpHandle fileOpt = NULL;
	S8 srcPath[256], destPath[256];
	
	memset(srcPath, 0, 256*sizeof(S8));
	memset(destPath, 0, 256*sizeof(S8));
	
	strcpy(reinterpret_cast<char *>(srcPath), reinterpret_cast<char *>(rpc_srcPath));
	strcpy(reinterpret_cast<char *>(destPath), reinterpret_cast<char *>(rpc_destPath));
	
	strcat(reinterpret_cast<char *>(destPath),"/");
	strcat(reinterpret_cast<char *>(destPath), (char *)longname);
	strcat(reinterpret_cast<char *>(srcPath),"/");
	strcat(reinterpret_cast<char *>(srcPath),(char *)longname);
	CH_FSYS_CopyFile(srcPath, destPath, NULL, &fileOpt);
	
	return 0;
}

int isDirOrFile(S8 *rpc_srcPath)
{
	int i_fileNum = 0, i, name_position;
	CH_FSYS_DirFileInfo_t *stru_DirFile = NULL;
	
	S8 srcPath[256], tempPath[256];
	char *short_name = NULL;
	
	memset(srcPath, 0, 256*sizeof(S8));	
	memset(tempPath, 0, 256*sizeof(S8));
	
	strcpy(reinterpret_cast<char *>(srcPath), reinterpret_cast<char *>(rpc_srcPath));
	findFileDirName(reinterpret_cast<char *>(srcPath), short_name, &name_position);
	if(short_name== NULL)
	{
		LOG("can not find file or path %s\n", rpc_srcPath);
		return -1;
	}
	strncpy(reinterpret_cast<char *>(tempPath), reinterpret_cast<char *>(srcPath), name_position);
	CH_FSYS_GetFileAndDirNumber(tempPath, reinterpret_cast<U32 *>(&i_fileNum));
	if(i_fileNum == 0)
	{
		LOG("%s file num = 0\n", tempPath);
		return -1;
	}
	stru_DirFile = (CH_FSYS_DirFileInfo_t *)malloc(sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
	memset(stru_DirFile, 0, sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
	CH_FSYS_GetDirInfo(tempPath, 0, &stru_DirFile[0], reinterpret_cast<U32 *>(&i_fileNum));
	
	for(i= 0; i<i_fileNum;i++)
	{
		
		if(stru_DirFile[i].enm_FileType == CH_FSYS_DIR)
		{
			if(strcmp((char *)stru_DirFile[i].uc_FileLongName, short_name) == 0)
			{
				free(stru_DirFile);
				free(short_name);
				return 0;
			}
		}
		if(stru_DirFile[i].enm_FileType == CH_FSYS_FILE)
		{
			if(strcmp((char *)stru_DirFile[i].uc_FileLongName, short_name) == 0)
			{
				free(stru_DirFile);
				free(short_name);
				return 1;
			}
		}
	}
	free(stru_DirFile);
	free(short_name);
	return -1;
}

static int cp(S8 *rpc_srcPath, S8 *rpc_destPath)
{
	int i_fileNum = 0, i, name_position;
	CH_FSYS_FileOpHandle h_fileOp = NULL;
	CH_FSYS_DirFileInfo_t *stru_DirFile;
	
	S8 srcPath[256], destPath[256];	
	memset(srcPath, 0, 256*sizeof(S8));	
	memset(destPath, 0, 256*sizeof(S8));
	
	char *short_name = NULL;
	
	strcpy(reinterpret_cast<char *>(srcPath), reinterpret_cast<char *>(rpc_srcPath));
	strcpy(reinterpret_cast<char *>(destPath), reinterpret_cast<char *>(rpc_destPath));
	if(isDirOrFile(srcPath) == 0)
	{
		strcat(reinterpret_cast<char *>(destPath),"/");
		findFileDirName(reinterpret_cast<char *>(srcPath), short_name, &name_position);
		strcat(reinterpret_cast<char *>(destPath), short_name);
		
		CH_FSYS_GetFileAndDirNumber(srcPath, reinterpret_cast<U32 *>(&i_fileNum));
		if(i_fileNum == 0)
		{
			return -1;
		}
		
		stru_DirFile = (CH_FSYS_DirFileInfo_t *)malloc(sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
		memset(stru_DirFile, 0, sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
		CH_FSYS_GetDirInfo(srcPath, 0, &stru_DirFile[0], reinterpret_cast<U32 *>(&i_fileNum));
		
		CH_FSYS_CreateDir(destPath);
		
		for(i= 0; i<i_fileNum;i++)
		{
			switch(stru_DirFile[i].enm_FileType)
			{
			case CH_FSYS_DIR:
				
				strcat(reinterpret_cast<char *>(destPath),"/");
				strcat(reinterpret_cast<char *>(destPath), (char *)stru_DirFile[i].uc_FileLongName);
				CH_FSYS_CreateDir(destPath);
				strcat(reinterpret_cast<char *>(srcPath),"/");
				strcat(reinterpret_cast<char *>(srcPath),(char *)stru_DirFile[i].uc_FileLongName);
				cp(srcPath, destPath); 
				break;
			case CH_FSYS_FILE:
				cpFile(srcPath, destPath, stru_DirFile[i].uc_FileLongName);
				break;
			default:
				break;
			}
		}
		free(stru_DirFile);
		free(short_name);
	}
	else if(isDirOrFile(srcPath) == 1)
	{
		strcat(reinterpret_cast<char *>(destPath),"/");
		findFileDirName(reinterpret_cast<char *>(srcPath), short_name, &name_position);
		strcat(reinterpret_cast<char *>(destPath), short_name);
		
		CH_FSYS_CopyFile(srcPath, destPath, NULL, &h_fileOp);
		free(short_name);
		
	}
	else
	{
		LOG("COPY dir or file Wrong!\n");
		
		return -1;
	}
	return 0;
	
}

static const char *guc_FsysEventName[CH_FSYS_STATUS_MAX] = 
{
	"CH_FSYS_MOUNTED",		///< 文件系统已挂载
		"CH_FSYS_UNMOUNTED",		///< 文件系统已卸载
		"CH_FSYS_UNSUPPORTED"	///< 文件系统不支持
};

void fsys_call1( S32	ri_Port, S8	*rpc_PartitonList, CH_FSYS_STATUS_e	renm_Status)
{
	if(renm_Status < CH_FSYS_STATUS_MAX)
	{
		gh_fsysEvent[renm_Status][0]++;
		LOG("%s %c, times=%d\n",guc_FsysEventName[renm_Status],rpc_PartitonList ? rpc_PartitonList[0] :'?' ,
			gh_fsysEvent[renm_Status][0]);
	}
}

void fsys_call2( S32	ri_Port, S8	*rpc_PartitonList, CH_FSYS_STATUS_e	renm_Status)
{
	if(renm_Status < CH_FSYS_STATUS_MAX)
	{
		gh_fsysEvent[renm_Status][1]++;
		LOG("%s %c, times=%d\n",guc_FsysEventName[renm_Status],rpc_PartitonList ? rpc_PartitonList[0] :'?' ,
			gh_fsysEvent[renm_Status][1]);
		
	}
}

void fsys_call3( S32	ri_Port, S8	*rpc_PartitonList, CH_FSYS_STATUS_e	renm_Status)
{
	if(renm_Status < CH_FSYS_STATUS_MAX)
	{
		gh_fsysEvent[renm_Status][2]++;
		LOG("%s %c, times=%d\n",guc_FsysEventName[renm_Status],rpc_PartitonList ? rpc_PartitonList[0] :'?' ,
			gh_fsysEvent[renm_Status][2]);
		
	}
}

void formate_call( CH_FSYS_FORMATSTAGE_e renm_Stage,U8 ruc_StageProcess,U8 ruc_TotalProcess)
{
	char *puc_info[CH_FSYS_FORMATSTAGE_MAX] = {
		"CH_FSYS_FORMAT_START" ,			///< 格式化开始
			"CH_FSYS_FORMAT_FATDONE ",		///< FAT域初始化完成
			"CH_FSYS_FORMAT_ROOTDIRDONE" ,	///< 根目录格式化完成
			"CH_FSYS_FORMAT_INPROGRESS" ,		///< 格式化进行中
			"CH_FSYS_FORMAT_FATINPROGRESS" ,	///< FAT域初始化中
			"CH_FSYS_FORMAT_ALLDONE" ,		///< 格式化完成
	};
	if(renm_Stage < CH_FSYS_FORMATSTAGE_MAX)
	{
		LOG("%s ruc_StageProcess=%d/%d\n", puc_info[renm_Stage], ruc_StageProcess, ruc_TotalProcess);
	}
	else
	{
		LOG("unkown %d ruc_StageProcess=%d/%d\n", renm_Stage, ruc_StageProcess, ruc_TotalProcess);
	}
	if(renm_Stage == CH_FSYS_FORMAT_ALLDONE
		|| ruc_StageProcess == ruc_TotalProcess)
	{
		b_formateOver = CH_TRUE;
	}
}

#if ENABLE_TESTCASE 	/*sqzou20171009*/
CASE_F(TEST_FSYS_INIT1,
	   "测试多次初始化时是否返回OK") {
	EXPECT_EQ(CH_FSYS_OK, CH_FSYS_Init());
	EXPECT_EQ(CH_FSYS_OK, CH_FSYS_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_FSYS_INIT2,
	   "测试模块是否可被销毁") {
	//EXPECT_EQ(CH_FSYS_OK, CH_FSYS_Term());
	EXPECT_EQ(CH_FSYS_OK, CH_FSYS_Init());
}


CASE_F(TEST_FSYS_INIT3,
	   "测试CH_FSYS_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_FSYS_OK, CH_FSYS_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_FSYS_PARAMS1,
	   "CH_FSYS_OpenFile错误参数测试"){
	
	EXPECT_EQ(CH_FSYS_OpenFile(NULL, reinterpret_cast<const S8 *>("r")), NULL);	//测试文件路径为空，测试是否通过
	EXPECT_EQ(CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/test.txt"), NULL), NULL);		//测试文件模式为空，测试是否通过
}

CASE_F(TEST_FSYS_PARAMS2,
	   "CH_FSYS_CloseFile错误参数测试"){
	
	CH_FSYS_FileHandle h_fileHandle; 
	h_fileHandle = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>(fw_getenv("IFRAME_FILE")), reinterpret_cast<const S8 *>("r+"));
	gh_fileHandle.push_back(h_fileHandle);
	
	EXPECT_NE(CH_FSYS_CloseFile(0), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS3,
	   "CH_FSYS_DeleteFile错误参数测试"){
	
	CH_FSYS_FileOpHandle fileOpt;
	//EXPECT_NE(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>(fw_getenv("IFRAME_FILE")), NULL, NULL), CH_FSYS_OK);
	EXPECT_NE(CH_FSYS_DeleteFile(NULL, NULL, &fileOpt), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS4,
	   "CH_FSYS_MoveFile错误参数测试"){
	
	CH_FSYS_FileOpHandle h_fileOpt;
	//EXPECT_NE(CH_FSYS_MoveFile(reinterpret_cast<const S8 *>(fw_getenv("IFRAME_FILE")), NULL, NULL, &h_fileOpt), CH_FSYS_OK); //目标文件名错误测试
	EXPECT_NE(CH_FSYS_MoveFile(NULL, reinterpret_cast<const S8 *>("C:/test.txt"), NULL, &h_fileOpt), CH_FSYS_OK); //源文件错误测试
}


CASE_F(TEST_FSYS_PARAMS7,
	   "CH_FSYS_ReadFile错误参数测试"){
	
	U8 *readBuf = new U8[1024];
	memset(readBuf, 0, sizeof(U8)*1024);
	
	EXPECT_LE(CH_FSYS_ReadFile(0, (void *)readBuf, 1, 1024), 0);	//handle为空测试
	EXPECT_LE(CH_FSYS_ReadFile(gh_fileHandle[0], (void *)NULL, 1, 1024), 0);	//buffer为空测试
	EXPECT_LE(CH_FSYS_ReadFile(gh_fileHandle[0], (void *)readBuf, 0, 1024), 0);	//传入block大小为零测试
	EXPECT_LE(CH_FSYS_ReadFile(gh_fileHandle[0], (void *)readBuf, 1, 0), 0);	//每块的字节数大小为零测试
	delete[] readBuf;
}

CASE_F(TEST_FSYS_PARAMS8,
	   "CH_FSYS_WriteFile错误参数测试"){
	
	U8 *writeBuf = new U8[1024];
	memset(writeBuf, 0, sizeof(U8)*1024);
	
	EXPECT_LE(CH_FSYS_WriteFile(0, (void *)writeBuf, 1, 1024), 0);	//handle为空测试
	EXPECT_LE(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)NULL, 1, 1024), 0);	//buffer为空测试
	EXPECT_LE(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)writeBuf, 0, 1024), 0);	//传入block大小为零测试
	EXPECT_LE(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)writeBuf, 1024, 0), 0);	//每块的字节数大小为零测试
	delete []writeBuf;
}

CASE_F(TEST_FSYS_PARAMS9,
	   "CH_FSYS_RenameFile错误参数测试"){
	
	EXPECT_NE(CH_FSYS_RenameFile(reinterpret_cast<const S8 *>("C:/test.txt"), NULL), CH_FSYS_OK);	//目标文件名错误测试
	EXPECT_NE(CH_FSYS_RenameFile(0, reinterpret_cast<const S8 *>("C:/rename.txt")), CH_FSYS_OK);	//源文件错误测试
}

CASE_F(TEST_FSYS_PARAMS10,
	   "CH_FSYS_SeekFile错误参数测试"){
	
	S64 l_offset;
	
	CHS64_SetValue32(0,0,&l_offset);
	EXPECT_NE(CH_FSYS_SeekFile(0, l_offset, CH_FSYS_SEEK_SET), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS11,
	   "CH_FSYS_TellFile错误参数测试"){
	
	S64 l_offset;
	
	CHS64_SetValue32(0,0,&l_offset);
	EXPECT_NE(CH_FSYS_TellFile(0, &l_offset), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS12,
	   "CH_FSYS_FileIsEOF错误参数测试"){
	
	CH_BOOL b_isEof = CH_FALSE;
	EXPECT_NE(CH_FSYS_FileIsEOF(0, &b_isEof), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS13,
	   "CH_FSYS_ResizeFile错误参数测试"){
	
	S64 l_size;
	
	CHS64_SetValue32(0,0,&l_size);
	EXPECT_NE(CH_FSYS_ResizeFile(0, l_size), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS14,
	   "CH_FSYS_GetFileInfo错误参数测试"){
	
	CH_FSYS_DirFileInfo_t stru_DirInfo;
	memset(&stru_DirInfo, 0, sizeof(stru_DirInfo));
	EXPECT_NE(CH_FSYS_GetFileInfo(0, &stru_DirInfo), CH_FSYS_OK);	//文件名错误测试
	EXPECT_NE(CH_FSYS_GetFileInfo(reinterpret_cast<const S8 *>("C:/test.txt"), NULL), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS15,
	   "CH_FSYS_GetFileInfoEx错误参数测试"){
	
	CH_FSYS_DirFileInfo_t stru_DirInfo;
	memset(&stru_DirInfo, 0, sizeof(stru_DirInfo));
	EXPECT_NE(CH_FSYS_GetFileInfoEx(0, &stru_DirInfo), CH_FSYS_OK);
	EXPECT_NE(CH_FSYS_GetFileInfoEx(gh_fileHandle[0], NULL), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS16,
	   "CH_FSYS_GetFileAndDirNumber错误参数测试"){
	
	U32 i_fileNum = 0;
	EXPECT_NE(CH_FSYS_GetFileAndDirNumber(NULL, &i_fileNum), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS17,
	   "CH_FSYS_OpenDir错误参数测试"){
	
	EXPECT_EQ(CH_FSYS_OpenDir(NULL), NULL);
}

CASE_F(TEST_FSYS_PARAMS18,
	   "CH_FSYS_CloseDir错误参数测试"){
	
	gh_dirHandle = CH_FSYS_OpenDir(reinterpret_cast<const S8 *>("C:/"));
	EXPECT_NE(CH_FSYS_CloseDir(0), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS19,
	   "CH_FSYS_ReadDir错误参数测试"){
	
	CH_FSYS_Dirent_t stru_dirent;
	memset(&stru_dirent, 0, sizeof(stru_dirent));
	
	EXPECT_NE(CH_FSYS_ReadDir(0, &stru_dirent), CH_FSYS_OK);
	EXPECT_NE(CH_FSYS_ReadDir(gh_dirHandle, NULL), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS20,
	   "CH_FSYS_RewindDir错误参数测试"){
	
	EXPECT_NE(CH_FSYS_RewindDir(0), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS21,
	   "CH_FSYS_CreateDir错误参数测试"){
	
	EXPECT_NE(CH_FSYS_CreateDir(NULL), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS22,
	   "CH_FSYS_DeleteDir错误参数测试"){
	
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_DeleteDir(NULL), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
}

CASE_F(TEST_FSYS_PARAMS23,
	   "CH_FSYS_RenameDir错误参数测试"){
	
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_RenameDir(NULL, NULL), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
}

CASE_F(TEST_FSYS_PARAMS24,
	   "CH_FSYS_GetDirInfo错误参数测试"){
	
	U32 i_fileNum = 0;
	CH_FSYS_DirFileInfo_t stru_dirInfo;
	memset(&stru_dirInfo, 0, sizeof(CH_FSYS_DirFileInfo_t));
	
	EXPECT_NE(CH_FSYS_GetDirInfo(NULL, 0, &stru_dirInfo, &i_fileNum), CH_FSYS_OK);
	EXPECT_NE(CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>("C:/"), 0, NULL, &i_fileNum), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_PARAMS25,
	   "CH_FSYS_Format错误参数测试"){
	
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_Format(NULL, CH_FSYS_FAT32, formate_call), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
	
	EXPECT_NE(CH_FSYS_Format(reinterpret_cast<const S8 *>("C"), CH_FSYS_TYPE_MAX, formate_call), CH_FSYS_OK);
	//EXPECT_NE(CH_FSYS_Format(reinterpret_cast<const S8 *>("C"), CH_FSYS_FAT32, NULL), CH_FSYS_OK);
}

/**/
CASE_F(TEST_FSYS_PARAMS26,
	   "CH_FSYS_GetPartitionList错误参数测试"){
	
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_GetPartitionList(NULL, NULL), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
	
}


CASE_F(TEST_FSYS_PARAMS27,
	   "CH_FSYS_InstallCallback错误参数测试"){
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_InstallCallback((CH_FSYS_OPERATION)NULL), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
}

CASE_F(TEST_FSYS_PARAMS28,
	   "CH_FSYS_UninstallCallback错误参数测试"){
	if(fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_NE(CH_FSYS_UninstallCallback((CH_FSYS_OPERATION)NULL), CH_FSYS_OK);
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
}

/*
CASE_F(TEST_FSYS_PARAMS29,
"CH_FSYS_ConvertLogicToMountpoint错误参数测试"){

  EXPECT_EQ(CH_FSYS_ConvertLogicToMountpoint(NULL,NULL), CH_FSYS_OK);
  }
*/
#if FOR_AUTO_RUN==0

CASE_F(TEST_FSYS_CALL1,
	   "注册3个回调函数，均应返回成功"){
	
	
	memset(gh_fsysEvent, 0, sizeof(gh_fsysEvent));
	EXPECT_EQ(CH_FSYS_InstallCallback(fsys_call1), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_InstallCallback(fsys_call2), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_InstallCallback(fsys_call3), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_CALL2,
	   "提示用户插入/拔除U盘，3个回调函数均应收到正确的通知"){
	
	int retry = 30;
	PROMPT("请插入U盘,完成后按回车(如已插入，直接回车):\n");
	test_input_none();
	while(retry -- > 0 && gh_fsysEvent[CH_FSYS_MOUNTED][2] == 0)
	{
		CH_RTOS_TaskDelay(100);
	}
	EXPECT_GT(gh_fsysEvent[CH_FSYS_MOUNTED][0],0);
	EXPECT_GT(gh_fsysEvent[CH_FSYS_MOUNTED][1],0);
	EXPECT_GT(gh_fsysEvent[CH_FSYS_MOUNTED][2],0);
	
	
	PROMPT("请拔出U盘,完成后按回车:\n");
	test_input_none();
	
	retry = 100;
	while(retry -- > 0 && gh_fsysEvent[CH_FSYS_UNMOUNTED][2] == 0)
	{
		CH_RTOS_TaskDelay(100);
	}
	EXPECT_GT(gh_fsysEvent[CH_FSYS_UNMOUNTED][0],0);
	EXPECT_GT(gh_fsysEvent[CH_FSYS_UNMOUNTED][1],0);
	EXPECT_GT(gh_fsysEvent[CH_FSYS_UNMOUNTED][2],0);
	
	if(gh_fileHandle.size() > 0)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(gh_fileHandle[0]), CH_FSYS_OK);
		gh_fileHandle.clear();
	}
	if(gh_dirHandle)
	{
		EXPECT_EQ(CH_FSYS_CloseDir(gh_dirHandle), CH_FSYS_OK);
		gh_dirHandle = 0;
	}
	
	
}



CASE_F(TEST_FSYS_DIR1,
	   "提示用户插入U盘，然后调用create dir在C:/根目录创建一个文件夹，应返回OK"){
	
	memset(gh_fsysEvent, 0, sizeof(gh_fsysEvent));
	
	PROMPT("请插入U盘,完成后按回车:\n");
	test_input_none();
	
	int retry = 100;
	while(retry -- > 0 && gh_fsysEvent[CH_FSYS_MOUNTED][0] == 0)
	{
		CH_RTOS_TaskDelay(100);
	}
	
	char *rest_name = "C:/yh_dir";
	
	ASSERT_EQ(CH_FSYS_CreateDir(reinterpret_cast<const S8 *>(rest_name)), CH_FSYS_OK);
	//再用open dir去检查一下目录有没有创建成功
	CH_FSYS_DirHandle h_dir;
	
	
	EXPECT_NE(h_dir = CH_FSYS_OpenDir(reinterpret_cast<const S8 *>(rest_name)), 0) << "目录"<< rest_name << "没有真正被创建";
	if(h_dir)
	{
		CH_FSYS_CloseDir(h_dir);
	}
}

CASE_F(TEST_FSYS_DIR2,
	   "在刚创建的文件夹内递归创建文件夹,能否创建30个"){
	
	int i_levelNum = 1, ret, i_dirNum, len = 0, i = 0;
	char *puc_dir_name = NULL ;
	int path_len;
	
	i_dirNum = fw_getenv_i("FSYS_DIR_DEPTH");
	char *rest_name = "yh_dir";
	path_len = (strlen(rest_name) + 1) * i_dirNum  + 32; //数组长度应可变，避免修改脚本后死机隐患
	puc_dir_name = new char[path_len];
	
	memset(puc_dir_name, 0, path_len*sizeof(char));
	strcpy(puc_dir_name,"C:/yh_dir");
	
	while(i_dirNum--)
	{
		strcat(puc_dir_name,"/");
		strcat(puc_dir_name, rest_name);
		ret = CH_FSYS_CreateDir(reinterpret_cast<const S8 *>(puc_dir_name));
		if(ret != CH_FSYS_OK)
		{
			break;
		}
		i_levelNum++;
	}
	
	EXPECT_GT(i_levelNum,i_dirNum);
	len = strlen(puc_dir_name);
	while(i_levelNum-- > 1)
	{
		LOG("puc_dir_name = %s.\r\n", puc_dir_name);
		ret = CH_FSYS_DeleteDir(reinterpret_cast<const S8 *>(puc_dir_name));
		if(ret != CH_FSYS_OK)
		{
			break;
		}
		
		i++;
		puc_dir_name[len-i*(strlen(rest_name)+1)] = 0;
	}
	
	delete []puc_dir_name;
}

CASE_F(TEST_FSYS_DIR3,
	   "调用delete dir删除TEST_FSYS_DIR1中创建的文件夹"){
	
	EXPECT_EQ(CH_FSYS_DeleteDir(reinterpret_cast<const S8 *>("C:/yh_dir")), CH_FSYS_OK);
	//再opendir是否成功删除
}
CASE_F(TEST_FSYS_FILE1,
	   "调用create dir创建一个文件夹，并在该文件夹中open file创建一个文件"){
	
	CH_FSYS_FileHandle fp = 0;
	CH_FSYS_DirHandle h_dir = 0;
	ASSERT_EQ(CH_FSYS_CreateDir(reinterpret_cast<const S8 *>("C:/test_dir")), CH_FSYS_OK) << "创建文件夹返回失败";
	ASSERT_NE(h_dir = CH_FSYS_OpenDir(reinterpret_cast<const S8 *>("C:/test_dir")), 0) << "创建文件夹不成功";
	ASSERT_EQ(CH_FSYS_CloseDir(h_dir), CH_FSYS_OK);
	ASSERT_NE(fp = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/test_dir/test_file.txt"), reinterpret_cast<const S8 *>("w+")), 0);
	ASSERT_EQ(CH_FSYS_CloseFile(fp), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_FILE2,
	   "rename file重命名该文件,get fileInfo能获取到重命名后的文件信息"){
	
	CH_FSYS_DirFileInfo_t stru_fileInfo;
	memset(&stru_fileInfo,0,sizeof(stru_fileInfo));
	
	EXPECT_EQ(CH_FSYS_RenameFile(reinterpret_cast<const S8 *>("C:/test_dir/test_file.txt"), reinterpret_cast<const S8 *>("hello_world.txt")), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_GetFileInfo(reinterpret_cast<const S8 *>("C:/test_dir/hello_world.txt"), &stru_fileInfo), CH_FSYS_OK) << "更名未成功"; 
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/test_dir/hello_world.txt"), NULL, NULL), CH_FSYS_OK)<< "更名未成功"; 
}


CASE_F(TEST_FSYS_FILE3,
	   "在刚创建的文件夹内递归创建文件200个,能够成功"){
	
	char part_name[50];
	int i = 0; 
	CH_FSYS_FileHandle *ph_fileHandle = NULL;
	int fsys_file_count = fw_getenv_i("FSYS_FILE_COUNT");
	
	
	ph_fileHandle = new CH_FSYS_FileHandle[fsys_file_count + 1];
	memset(ph_fileHandle, 0, sizeof(CH_FSYS_FileHandle) * (fsys_file_count + 1));
	while(fsys_file_count-- > 0)
	{
		memset(part_name,0,sizeof(char)*50);
		sprintf(part_name,"C:/test_dir/test_file%d.txt", i);
		
		ph_fileHandle[i] = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>(part_name), reinterpret_cast<const S8 *>("w"));
		if(!ph_fileHandle[i])
		{
			break;
		}
		
		i++;
	}
	
	EXPECT_GE(i, fw_getenv_i("FSYS_FILE_COUNT")) << "最大应能同时打开"<< fw_getenv_i("FSYS_FILE_COUNT")<<"个文件";
	
	for(i = 0; ph_fileHandle[i]; i++)
	{
		CH_FSYS_CloseFile(ph_fileHandle[i]);
	}
	delete []ph_fileHandle;
}

CASE_F(TEST_FSYS_FILE4,
	   "调用get DirInfo获取刚创建的所有文件信息"){
	
	CH_FSYS_DirFileInfo_t *pstru_DirFile;
	int fsys_file_count = fw_getenv_i("FSYS_FILE_COUNT");
	U32 ui_count = 0, i = 0;
	char part_name[50];
	CH_FSYS_FileOpHandle fileOpHandle = NULL;
	
	ui_count = fsys_file_count + 5;
	pstru_DirFile = new CH_FSYS_DirFileInfo_t[ui_count]; 
	for(i = 0;  i < (fsys_file_count + 2) / 10; i++)
	{
		ui_count = 10;
		EXPECT_EQ(CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>("C:/test_dir"), i * 10, &pstru_DirFile[0], reinterpret_cast<U32 *>(&ui_count)), CH_FSYS_OK);
		EXPECT_EQ(ui_count, 10) << "CH_FSYS_GetDirInfo未正确实现 i=" << i;
	}
	
	if(((fsys_file_count + 2) % 10) != 0)
	{
		ui_count = 10;
		EXPECT_EQ(CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>("C:/test_dir"), i * 10, &pstru_DirFile[0], reinterpret_cast<U32 *>(&ui_count)), CH_FSYS_OK);
		EXPECT_EQ(ui_count, (fsys_file_count + 2) % 10) << "CH_FSYS_GetDirInfo未正确实现 i =" << i;
	}
	
	ui_count = fsys_file_count + 5;
	EXPECT_EQ(CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>("C:/test_dir"), 0, &pstru_DirFile[0], reinterpret_cast<U32 *>(&ui_count)), CH_FSYS_OK);
	for (i = 0; i < ui_count; i++)
	{
		if(pstru_DirFile[i].enm_FileType == CH_FSYS_FILE)
		{
			snprintf(part_name, sizeof(part_name), "C:/test_dir/%s", pstru_DirFile[i].uc_FileLongName);
			CH_FSYS_DeleteFile((const S8*)part_name, NULL, &fileOpHandle);
		}
	}
	delete[] pstru_DirFile;
	EXPECT_GE(ui_count, fsys_file_count) << "目录下的文件个数应不低于" << fsys_file_count << "个";
}

CASE_F(TEST_FSYS_FILE5,
	   "调用delete dir删除TEST_FSYS_FILE1中创建的文件夹"){
	
	EXPECT_EQ(CH_FSYS_DeleteDir(reinterpret_cast<const S8 *>("C:/test_dir")), CH_FSYS_OK);
	//再检查一下open dir是否成功
	EXPECT_EQ(CH_FSYS_OpenDir(reinterpret_cast<const S8 *>("C:/test_dir")), 0) << "目录没有被真正删除";
}

CASE_F(TEST_FSYS_FILE6,
	   "调用open file创建一个文件，通过write file往文件中写入100M数据"){
	
	int i;
	CH_FSYS_FileHandle h_fileHandle;
	char *file_buf = NULL;
	int ret = 1;
	
	h_fileHandle = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/normal.txt"), reinterpret_cast<const S8 *>("w+"));
	ASSERT_NE(h_fileHandle, 0) << "创建的文件句柄不应为NULL";
	
	file_buf = new char[BLOCK_SIZE];
	memset(file_buf, 'c', sizeof(char) * BLOCK_SIZE);
	
	gh_fileHandle.push_back(h_fileHandle);
	
	ret = 1;
	for(i = 0; ret == 1 &&  i < BLOCK_NUMS; i++)
	{
		EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle, (void *)file_buf, BLOCK_SIZE, 1), 1);
	}
	delete []file_buf;
}



CASE_F(TEST_FSYS_FILE8,
	   "seek file移动指针到文件末尾,tell file判断当前文件指针位置是否在末尾"){
	
	char *file_buf = NULL;
	S64 l_offset, l_zero;
	
	CHS64_SetValue32(0, 0, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * BLOCK_NUMS);
	//file size=BLOCK_SIZE*400
	
	//写一段数据后再测试是否成功，seek到为0和非0都要测试
	file_buf = new char[BLOCK_SIZE];
	memset(file_buf, 'c', sizeof(char) * BLOCK_SIZE);
	CHS64_SetValue32(0, 0, &l_offset);
	
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * BLOCK_NUMS+BLOCK_SIZE);
	
	//file size=BLOCK_SIZE*401
	CHS64_SetValue32(0, 0, &l_zero);
	CHS64_SetValue32(0, BLOCK_SIZE, &l_offset);
	CHS64_Sub(l_zero, l_offset, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * BLOCK_NUMS) << "CH_FSYS_SeekFile(fp, -0x40000, SEEK_END)没有成功";
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	CHS64_SetValue32(0,0, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE*BLOCK_NUMS+BLOCK_SIZE) << "CH_FSYS_SeekFile(fp, -0x40000, SEEK_END)没有成功";;
	
	//file size=BLOCK_SIZE*401
	CHS64_SetValue32(0, 0, &l_zero);
	CHS64_SetValue32(0, BLOCK_SIZE, &l_offset);
	CHS64_Sub(l_zero, l_offset, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE*BLOCK_NUMS) << "CH_FSYS_SeekFile(fp, -0x40000, SEEK_END)没有成功";
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	CHS64_SetValue32(0,0, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE*BLOCK_NUMS+BLOCK_SIZE * 2);
	
	//file size=BLOCK_SIZE*402
	delete []file_buf;
}

CASE_F(TEST_FSYS_FILE9,
	   "seek file移动指针到文件开头,tell file判断当前文件指针位置是否在文件头"){
	
	char *file_buf = NULL;
	S64 l_offset;
	
	CHS64_SetValue32(0, 0, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_SET), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset),0);
	
	/*写一段数据,再测试是否成功,seek到0与非0都要测试*/
	file_buf = new char[BLOCK_SIZE];
	memset(file_buf, 'c', sizeof(char) * BLOCK_SIZE);
	
	CHS64_SetValue32(0, 0, &l_offset);
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_SET), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), 0);
	
	CHS64_SetValue32(0, BLOCK_SIZE, &l_offset);
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_SET), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE);
	//BLOCK_SIZE
	
	
	CHS64_SetValue32(0, BLOCK_SIZE * 2, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_SET), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * 2) << "CH_FSYS_SeekFile(fp, -0x80000, SEEK_SET)没有成功";
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * 3);
	//BLOCK_SIZE * 3
	
	delete []file_buf;
}

CASE_F(TEST_FSYS_FILE10,
	   "CH_FSYS_SeekFile移动指针到一指定位置，tell file判断当前文件指针是否在该位置"){
	
	char *file_buf = NULL;
	S64 l_offset;
	
	CHS64_SetValue32(0, 20, &l_offset);
	EXPECT_EQ(CH_FSYS_SeekFile(gh_fileHandle[0], l_offset, CH_FSYS_SEEK_CUR), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * 3 + 20);
	
	/*写一段数据,再测试是否成功*/
	file_buf = new char[BLOCK_SIZE];
	memset(file_buf, 'c', sizeof(char) * BLOCK_SIZE);
	CHS64_SetValue32(0, 0, &l_offset);
	
	ASSERT_EQ(CH_FSYS_WriteFile(gh_fileHandle[0], (void *)file_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_TellFile(gh_fileHandle[0], &l_offset), CH_FSYS_OK);
	EXPECT_EQ(CHS64_GetLowValue(l_offset), BLOCK_SIZE * 4+20);
	delete []file_buf;
}

CASE_F(TEST_FSYS_FILE7,
	   "get fileInfo获取到到该文件的信息"){
	
	CH_FSYS_DirFileInfo_t stru_fileInfo;
	U32 ui_length;
	
	if(gh_fileHandle.size() > 0)
	{
		CH_FSYS_CloseFile(gh_fileHandle[0]);
		gh_fileHandle.clear();
	}
	
	EXPECT_EQ(CH_FSYS_GetFileInfo(reinterpret_cast<const S8 *>("C:/normal.txt"), &stru_fileInfo), CH_FSYS_OK);
	ui_length = CHS64_GetLowValue(stru_fileInfo.ul_FileSize);
	EXPECT_EQ( ui_length, BLOCK_SIZE * 402);
}

CASE_F(TEST_FSYS_FILE11,
	   "delete file删除该文件"){
	
	CH_FSYS_FileOpHandle h_fileOpHdl;
	if(gh_fileHandle.size() > 0)
	{
		CH_FSYS_CloseFile(gh_fileHandle[0]);
		gh_fileHandle.clear();
	}
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/normal.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_FILE_RW1,
	   "调用open file创建一个文件，创建方式分别为w,wb,w+，通过write file往文件中写入100M数据"){
	
	CH_FSYS_FileHandle h_fileHandle1,h_fileHandle2,h_fileHandle3;
	char *pfile_buf = NULL;
	int i, ret;
	
	pfile_buf = new char[BLOCK_SIZE];
	
	memset(pfile_buf,'c',sizeof(char)*BLOCK_SIZE);
	h_fileHandle1 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example1.txt"), reinterpret_cast<const S8 *>("w"));
	h_fileHandle2 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example2.txt"), reinterpret_cast<const S8 *>("wb"));
	h_fileHandle3 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example3.txt"), reinterpret_cast<const S8 *>("w+"));
	
	
	EXPECT_NE(h_fileHandle1, 0) << "w方式打开文件失败";
	if(h_fileHandle1)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle1, (void *)pfile_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
		}
	}
	EXPECT_NE(h_fileHandle2, 0) << "wb方式打开文件失败";
	if(h_fileHandle2)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle2, (void *)pfile_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
		}
	}
	EXPECT_NE(h_fileHandle3, 0) << "w+方式打开文件失败";
	if(h_fileHandle3)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle3, (void *)pfile_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
		}
	}
	if(h_fileHandle1)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle1), CH_FSYS_OK);
	}
	if(h_fileHandle2)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle2), CH_FSYS_OK);
	}
	if(h_fileHandle3)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle3), CH_FSYS_OK);
	}
	delete[] pfile_buf;
}

CASE_F(TEST_FSYS_FILE_RW2,
	   "调用open file创建另一个文件，创建方式分别为w,wb,w+，通过read file（打开方式分别为r,rb,r+）将上一个文件中的内容读出到该文件"){
	
	CH_FSYS_FileHandle h_fileHandle1,h_fileHandle2,h_fileHandle3;
	CH_FSYS_FileHandle h_fileHandle4,h_fileHandle5,h_fileHandle6;
	char *pdata_buf = NULL;
	int i, ret;
	
	pdata_buf = new char[BLOCK_SIZE];
	memset(pdata_buf,0,sizeof(char)*BLOCK_SIZE);
	
	h_fileHandle4 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example4.txt"), reinterpret_cast<const S8 *>("w"));
	h_fileHandle5 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example5.txt"), reinterpret_cast<const S8 *>("wb"));
	h_fileHandle6 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example6.txt"), reinterpret_cast<const S8 *>("w+"));
	
	EXPECT_NE(h_fileHandle4, 0) << "w模式打开文件失败";
	EXPECT_NE(h_fileHandle5, 0) << "wb模式打开文件失败";;
	EXPECT_NE(h_fileHandle6, 0) << "w+模式打开文件失败";;
	
	h_fileHandle1 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example1.txt"), reinterpret_cast<const S8 *>("r"));
	h_fileHandle2 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example2.txt"), reinterpret_cast<const S8 *>("rb"));
	h_fileHandle3 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example3.txt"), reinterpret_cast<const S8 *>("r+"));
	EXPECT_NE(h_fileHandle1, 0) << "r模式打开文件失败";
	EXPECT_NE(h_fileHandle2, 0) << "rb模式打开文件失败";
	EXPECT_NE(h_fileHandle3, 0) << "r+模式打开文件失败";
	
	if(h_fileHandle1 && h_fileHandle4)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle1, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle4, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			memset(pdata_buf,0,sizeof(U8)*BLOCK_SIZE);
		}
	}
	
	if(h_fileHandle2 && h_fileHandle5)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle2, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle5, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			memset(pdata_buf,0,sizeof(U8)*BLOCK_SIZE);
		}
	}
	
	if(h_fileHandle3 && h_fileHandle6)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle3, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_WriteFile(h_fileHandle6, pdata_buf, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;;
			if(ret != 1)
			{
				break;
			}
			memset(pdata_buf,0,sizeof(U8)*BLOCK_SIZE);
		}
	}
	
	if(h_fileHandle1)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle1), CH_FSYS_OK);
	}
	if(h_fileHandle2)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle2), CH_FSYS_OK);
	}
	if(h_fileHandle3)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle3), CH_FSYS_OK);
	}
	if(h_fileHandle4)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle4), CH_FSYS_OK);
	}
	if(h_fileHandle5)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle5), CH_FSYS_OK);
	}
	if(h_fileHandle6)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle6), CH_FSYS_OK);
	}
	delete[] pdata_buf;
}

CASE_F(TEST_FSYS_FILE_RW3,
	   "比较2个文件中的内容是否一致，判断读写是否成功"){
	
	char *pdata_buf1 = NULL;
	char *pdata_buf2 = NULL;
	int i, ret;
	
	CH_FSYS_FileHandle h_fileHandle1,h_fileHandle2,h_fileHandle3;
	CH_FSYS_FileHandle h_fileHandle4,h_fileHandle5,h_fileHandle6;
	
	pdata_buf1 = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, BLOCK_SIZE+1);
	pdata_buf2 = (char*)CH_RTOS_AllocateMemory(gh_APP_HEAP, BLOCK_SIZE+1);
	
	ASSERT_NE(pdata_buf1, (void*)NULL);
	ASSERT_NE(pdata_buf2, (void*)NULL);
	h_fileHandle1 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example1.txt"), reinterpret_cast<const S8 *>("r"));
	h_fileHandle2 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example2.txt"), reinterpret_cast<const S8 *>("r"));
	h_fileHandle3 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example3.txt"),reinterpret_cast<const S8 *>("r"));
	h_fileHandle4 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example4.txt"), reinterpret_cast<const S8 *>("r"));
	h_fileHandle5 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example5.txt"), reinterpret_cast<const S8 *>("r"));
	h_fileHandle6 = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/example6.txt"), reinterpret_cast<const S8 *>("r"));
	
	
	if(h_fileHandle1 && h_fileHandle4)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle1, pdata_buf1, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle4, pdata_buf2, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = memcmp(pdata_buf1,pdata_buf2, BLOCK_SIZE), 0) << "ERROR i=" << i;
			if(ret != 0)
			{
				break;
			}
		}
	}
	if(h_fileHandle2 && h_fileHandle5)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle2, pdata_buf1, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle5, pdata_buf2, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = memcmp(pdata_buf1,pdata_buf2, BLOCK_SIZE), 0) << "ERROR i=" << i;
			if(ret != 0)
			{
				break;
			}
		}
	}
	if(h_fileHandle3 && h_fileHandle6)
	{
		for(i = 0; i < BLOCK_NUMS; i++)
		{
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle3, pdata_buf1, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = CH_FSYS_ReadFile(h_fileHandle6, pdata_buf2, BLOCK_SIZE, 1), 1) << "ERROR i=" << i;
			if(ret != 1)
			{
				break;
			}
			EXPECT_EQ(ret = memcmp(pdata_buf1,pdata_buf2, BLOCK_SIZE), 0) << "ERROR i=" << i;
			if(ret != 0)
			{
				break;
			}
		}
	}
	RUN_CMD("fsys.info");
	
	if(h_fileHandle1)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle1), CH_FSYS_OK);
	}
	if(h_fileHandle2)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle2), CH_FSYS_OK);
	}
	if(h_fileHandle3)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle3), CH_FSYS_OK);
	}
	if(h_fileHandle4)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle4), CH_FSYS_OK);
	}
	if(h_fileHandle5)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle5), CH_FSYS_OK);
	}
	if(h_fileHandle6)
	{
		EXPECT_EQ(CH_FSYS_CloseFile(h_fileHandle6), CH_FSYS_OK);
	}
	
	CH_RTOS_FreeMemory(gh_APP_HEAP, pdata_buf1);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pdata_buf2);
}

CASE_F(TEST_FSYS_FILE_RW4,
	   "delete file删除文件"){
	CH_FSYS_FileOpHandle h_fileOpHdl;
	
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example1.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example2.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example3.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example4.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example5.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>("C:/example6.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
}

CASE_F(TEST_FSYS_FILE_RW5,
	   "测试所有的读写方式是否符合预期"){
	
	CH_FSYS_FileHandle h_file;
	char *puc_buf = NULL;
	char *puc_mod[] = {"r", "rb", "r+", "rb+", "w", "wb", "w+", "wb+", /*"wa",*/ "a", "a+", "ab", NULL};
	char *puc_mod_readonly[] = {"r", "rb", "r+", /*"rb+",*/ NULL};
	int i;
	S64 l_Offset, l_size;
	CH_FSYS_FileOpHandle h_fileOpHdl;
	
	puc_buf = new char[BLOCK_SIZE];
	
	//先创建一部分文件
	EXPECT_NE(h_file = CH_FSYS_OpenFile((const S8 *)("C:/example1.txt"), (const S8 *)("w")), 0);
	EXPECT_EQ(CH_FSYS_WriteFile(h_file, puc_buf, BLOCK_SIZE, 1), 1);
	EXPECT_EQ(CH_FSYS_CloseFile(h_file), CH_FSYS_OK);
	
	for(i = 0; puc_mod[i]; i++)
	{
		ASSERT_NE(h_file = CH_FSYS_OpenFile((const S8 *)("C:/example1.txt"), (const S8 *)("r")), 0);
		CHS64_SetValue32(0, 0, &l_Offset);
		ASSERT_EQ(CH_FSYS_SeekFile(h_file, l_Offset, CH_FSYS_SEEK_END), CH_FSYS_OK);
		ASSERT_EQ(CH_FSYS_TellFile(h_file, &l_size),  CH_FSYS_OK);
		ASSERT_EQ(CH_FSYS_CloseFile(h_file), CH_FSYS_OK);
		
		EXPECT_NE(h_file = CH_FSYS_OpenFile((S8*)("C:/example1.txt"), (S8*)(puc_mod[i])), 0) 
			<< "以模式" << puc_mod[i] << "打开文件失败" << endl;
		if(!h_file)
		{
			continue;
		}
		
		
		if(strstr(puc_mod[i], "+") || strstr(puc_mod[i], "w"))
		{
			EXPECT_EQ(CH_FSYS_WriteFile(h_file, puc_buf, BLOCK_SIZE, 1), 1) << puc_mod[i]  << "模式下应可写";
			ASSERT_EQ(CH_FSYS_TellFile(h_file, &l_Offset),  CH_FSYS_OK);
			if(strstr(puc_mod[i], "a"))
			{
				//EXPECT_GT(CHS64_GetLowValue(l_Offset), CHS64_GetLowValue(l_size)) << puc_mod[i] << "模式打开，应以追加方式写文件";
				EXPECT_EQ(CHS64_Comp(l_Offset,l_size),1) << puc_mod[i] << "模式打开，应以追加方式写文件";
			}
			else
			{
				//EXPECT_LE(CHS64_GetLowValue(l_Offset), CHS64_GetLowValue(l_size))<< puc_mod[i]  << "模式打开，应以覆盖方式写文件";
				EXPECT_EQ(CHS64_Comp(l_Offset,l_size),-1)<< puc_mod[i]  << "模式打开，应以覆盖方式写文件";
			}
		}
		
		if(strstr(puc_mod[i], "+") || strstr(puc_mod[i], "r"))
		{
			CHS64_SetValue32(0, 0, &l_Offset);
			CH_FSYS_SeekFile(h_file, l_Offset, CH_FSYS_SEEK_SET);
			EXPECT_EQ(CH_FSYS_ReadFile(h_file, puc_buf, BLOCK_SIZE, 1), 1) << puc_mod[i]  << "模式下应可读";
		}
		EXPECT_EQ(CH_FSYS_CloseFile(h_file), CH_FSYS_OK);
	}
	
	EXPECT_EQ(CH_FSYS_DeleteFile((const S8 *)("C:/example1.txt"), NULL, &h_fileOpHdl), CH_FSYS_OK);
	
	for(i = 0; puc_mod_readonly[i]; i++)
	{
		EXPECT_EQ(h_file = CH_FSYS_OpenFile((const S8 *)("C:/example1.txt"), 
			(const S8 *)puc_mod_readonly[i]), 0) << "文件不存在不应成功";
		if(h_file)
		{
			CH_FSYS_CloseFile(h_file);
		}
	}
	delete[] puc_buf;
}


void remindPerson(void *data)
{
	CH_RTOS_TaskEnter(NULL);
	PROMPT("请拔出U盘等待1S后再插上，然后回车:\n");
	test_input_none();
	usb_test_over = CH_TRUE;
	CH_RTOS_TaskExit();
	LOG("exit ....usb_test_over=%d\n", usb_test_over);
}

CASE_F(TEST_FSYS_FILE_RW6,
	   "测试在写入文件的过程中拔掉U盘，不应死机"){
	
	CH_RTOS_TaskHandle h_Task;
	CH_FSYS_FileHandle h_fileHandle;
	char *puc_buf = NULL;
	int i, ret;
	S64 offset;
	
	puc_buf = new char[BLOCK_SIZE];
	memset(gh_fsysEvent, 0, sizeof(gh_fsysEvent));
	usb_test_over = CH_FALSE;
	memset(puc_buf, 'c', BLOCK_SIZE*sizeof(char));
	h_fileHandle = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/usbTest.txt"), reinterpret_cast<const S8 *>("w"));
	CHS64_SetValue32(0, 0, &offset);
	CH_RTOS_CreateTask(remindPerson, NULL,	NULL, 8 * 1024, 200, reinterpret_cast<const S8 *>("test main"), &h_Task);
	for(i = 0; ;i++)
	{
		ret = CH_FSYS_WriteFile(h_fileHandle, puc_buf, BLOCK_SIZE, 1); //拔掉后出错是正常的，不应该判断返回值
		if(i > 10)
		{
			i = 0;
			CH_FSYS_SeekFile(h_fileHandle, offset, CH_FSYS_SEEK_SET);//避免操作员长时间不按enter导致写入文件无限变大
		}
		if(usb_test_over == CH_TRUE || ret != 1)
		{
			break;
		}
		
		CH_RTOS_TaskDelay(10);
	}
	CH_FSYS_CloseFile(h_fileHandle);
	while(!usb_test_over) //一定要等到用户确认
	{
		LOG("等待按键..\n");
		CH_RTOS_TaskDelay(1000);
	}
	CH_RTOS_WaitTaskTerm(h_Task, 5000);
	CH_RTOS_DeleteTask(h_Task);
	delete[] puc_buf;
}

CASE_F(TEST_FSYS_FILE_RW7,
	   "测试在读取过程中拔掉U盘，不应死机"){
	
	CH_RTOS_TaskHandle h_Task;
	CH_FSYS_FileHandle h_fileHandle;
	int i, ret, retry = 20;
	S64 offset;
	char *puc_buf = NULL;
	
	//	PROMPT("请插入U盘,完成后按回车:\n");
	//	test_input_none();
	puc_buf = new char[BLOCK_SIZE];
	while(retry -- > 0 && gh_fsysEvent[CH_FSYS_MOUNTED][0] == 0)
	{
		CH_RTOS_TaskDelay(100);
	}
	usb_test_over = CH_FALSE;
	
	CH_RTOS_TaskDelay(1000);
	memset(puc_buf, 0, BLOCK_SIZE*sizeof(char));
	CHS64_SetValue32(0, 0, &offset);
	h_fileHandle = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>("C:/usbTest.txt"), reinterpret_cast<const S8 *>("w+"));
	
	CH_RTOS_CreateTask(remindPerson, NULL,	NULL, 8 * 1024, 200, reinterpret_cast<const S8 *>("test main"), &h_Task);
	
	for(i = 0; ;i++)
	{
		ret = CH_FSYS_ReadFile(h_fileHandle, puc_buf, BLOCK_SIZE, 1);
		if(ret != 1)
		{
			if(i > 2)
			{
				i = 0;
				CH_FSYS_SeekFile(h_fileHandle, offset, CH_FSYS_SEEK_SET);//避免操作员长时间不按enter导致文件读到末尾
			}
			else
			{
				LOG("read error!\n");
				break;
			}
		}
		if(usb_test_over == CH_TRUE)
		{
			LOG("usb_test_over\n");
			break;
		}
	}
	
	CH_FSYS_CloseFile(h_fileHandle);
	while(!usb_test_over) //一定要等到用户确认
	{
		CH_RTOS_TaskDelay(1000);
		LOG("等待按键..\n");
	}
	CH_RTOS_WaitTaskTerm(h_Task, 5000);
	CH_RTOS_DeleteTask(h_Task);
	delete[]puc_buf;
	
}


CASE_F(TEST_FSYS_FILE_FORMAT1,
	   "调用get partition取得已挂载设备上的分区表，与测试员确认是否正确"){
	CH_FSYS_PartitionInfo_t pstru_partion[16];
	S32 i_PartitionNumb = 16, i, j = 0, retry = 20;
	char* puc_str = NULL;
	char *puc_info[] = {
		"CH_FSYS_FAT32",			///< FAT32
			"CH_FSYS_NTFS",			///< NTFS
			"CH_FSYS_EXT2",			///< EXT2
			"CH_FSYS_EXT3",			///< EXT3
			"CH_FSYS_JFFS2",			///< JFFS2
			"CH_FSYS_UBIFS",			///< UBIFS
			"CH_FSYS_YAFFS",			///< YSFFS
			"CH_FSYS_CRAMFS",			///< CRAMFS
			"CH_FSYS_UNKOWN"
	};
	
	puc_str = new char[1024];
	puc_str[1023] = 0;
	//	PROMPT("请插入U盘,完成后按回车:\n");
	//	test_input_none();
	
	while(retry -- > 0 && gh_fsysEvent[CH_FSYS_MOUNTED][0] == 0)
	{
		CH_RTOS_TaskDelay(100);
	}
	memset(pstru_partion, 0, sizeof(pstru_partion));
	//pstru_partion = new CH_FSYS_PartitionInfo_t[16];
	retry = 5;
	do{
		EXPECT_EQ(CH_FSYS_GetPartitionList(pstru_partion, &i_PartitionNumb), CH_FSYS_OK);
		CH_RTOS_TaskDelay(1000);
	}while(i_PartitionNumb == 0 && retry -- > 0);
	
	j = snprintf(&puc_str[j], 1024 - j, "这个U盘有 %d 个分区 %s\n", i_PartitionNumb, i_PartitionNumb > 0 ? ",详情如下:" : "");
	for(i = 0; i < i_PartitionNumb; i++)
	{
		if(pstru_partion[i].enm_FsType > CH_FSYS_TYPE_MAX)
		{
			pstru_partion[i].enm_FsType = CH_FSYS_TYPE_MAX;
		}
		j += snprintf(&puc_str[j], 1024 - j, "patition %d:\n"
			"enm_FsType = %s\n"
			"i_Port = %d\n"
			"uc_name = %c\n"
			"uc_Vol = %s\n"
			"ui_SizePerBlock = %d\n"
			"ul_FreeSizeKB = %dKB (%d.%dMB)\n"
			"ul_TotalSizeKB = %dKB (%d.%dMB)\n",
			i,
			puc_info[pstru_partion[i].enm_FsType], pstru_partion[i].i_Port, pstru_partion[i].uc_name, pstru_partion[i].uc_Vol,
			pstru_partion[i].ui_SizePerBlock, 
			pstru_partion[i].ul_FreeSizeKB, 
			pstru_partion[i].ul_FreeSizeKB / 1024,
			(pstru_partion[i].ul_FreeSizeKB % 1024) / 102,
			pstru_partion[i].ul_TotalSizeKB,
			pstru_partion[i].ul_TotalSizeKB / 1024,
			(pstru_partion[i].ul_TotalSizeKB % 1024) / 102);
	}
	LOG("%s", puc_str);
	PROMPT("上述信息是否正确?是请按Y，否则按N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE) << "测试员认为下述分区信息不正确:" << puc_str;
	//delete []pstru_partion;
	RUN_CMD("fsys.info");
	delete [] puc_str;
}


/**/
CASE_F(TEST_FSYS_FILE_FORMAT2,
	   "调用format分别对每个分区进行格式化，分区中的数据应全部被格式化"){
	
	S32 i_PartitionNumb = 16, i;
	CH_FSYS_PartitionInfo_t *pstru_partion = NULL;
	S8 puc_diskname[2] = {0, 0};
	
	pstru_partion = new CH_FSYS_PartitionInfo_t[16];
	
	EXPECT_EQ(CH_FSYS_GetPartitionList(pstru_partion, (&i_PartitionNumb)), CH_FSYS_OK);
	LOG("分区数为%d\n", i_PartitionNumb);
	ASSERT_GT(i_PartitionNumb, 0) << "未获取到分区信息，无法进行格式化";
	PROMPT("是否要对USB设备进行格式化?重要数据请先备份,确认请按Y，否则按N:");
	
	ASSERT_EQ(test_input_yes(), CH_TRUE) << "测试员未同意对分区进行格式化";
	
	for(i=0;i<i_PartitionNumb;i++)
	{	
		puc_diskname[0] = pstru_partion[i].uc_name;
		b_formateOver = CH_FALSE;//一定要先初始化
		EXPECT_EQ(CH_FSYS_Format(puc_diskname, pstru_partion[i].enm_FsType, formate_call), CH_FSYS_OK);
		for(;!b_formateOver;)
		{
			CH_RTOS_TaskDelay(1000);
		}
	}
	
	delete []pstru_partion; 
}

CASE_F(TEST_FSYS_FILE_FORMAT3,
	   "卸载刚刚注册的回调函数，应成功。插入拔除U盘，不应再收到通知"){
	
	EXPECT_EQ(CH_FSYS_UninstallCallback(fsys_call1), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_UninstallCallback(fsys_call2), CH_FSYS_OK);
	EXPECT_EQ(CH_FSYS_UninstallCallback(fsys_call3), CH_FSYS_OK);
	
	
	memset(gh_fsysEvent, 0, sizeof(gh_fsysEvent));
	
	PROMPT("请拔出U盘,完成按回车:\n");
	test_input_none();
	
	
	EXPECT_EQ(gh_fsysEvent[CH_FSYS_UNMOUNTED][0], 0);
	EXPECT_EQ(gh_fsysEvent[CH_FSYS_UNMOUNTED][1], 0);
	EXPECT_EQ(gh_fsysEvent[CH_FSYS_UNMOUNTED][2], 0);
	
	PROMPT("请插入U盘,完成按回车:\n");
	test_input_none();
}
#endif /* ONLY_FOR_TEST==0 */

#endif

static int fsys_init(int argc, CMD_PARAM argv[])
{
	int ret;
	
	ret = CH_FSYS_Init();
	LOG("CH_FSYS_Init return %d\n", ret);
	return ret;
}

static int fsys_term(int argc, CMD_PARAM argv[])
{
	int ret;
	
	ret = CH_FSYS_Term();
	LOG("CH_FSYS_Term return %d\n", ret);
	return ret;
}

static int fsys_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t stru_ver;
	
	ret = CH_FSYS_GetVersion(&stru_ver);
	LOG("CH_FSYS_GetVersion return 0x%x\n", stru_ver);
	return ret;
}

static int fsys_ls(int argc, CMD_PARAM argv[])
{
	int i_fileNum = 0, i;
	
	CH_FSYS_GetFileAndDirNumber(reinterpret_cast<const S8 *>(argv[1].get_str()), reinterpret_cast<U32 *>(&i_fileNum));
	if(i_fileNum == 0)
	{
		return -1;
	}
	
	CH_FSYS_DirFileInfo_t *stru_DirFile;
	stru_DirFile = (CH_FSYS_DirFileInfo_t *)malloc(sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
	memset(stru_DirFile, 0, sizeof(CH_FSYS_DirFileInfo_t)*(i_fileNum+1));
	CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>(argv[1].get_str()), 0, &stru_DirFile[0], reinterpret_cast<U32 *>(&i_fileNum));
	
	for(i= 0; i<i_fileNum;i++)
	{
		switch(stru_DirFile[i].enm_FileType)
		{
		case CH_FSYS_DIR:
			LOG("%-50s%-10s%d\n",stru_DirFile[i].uc_FileLongName,"Dir", CHS64_GetLowValue(stru_DirFile[i].ul_FileSize)); 
			break;
		case CH_FSYS_FILE:
			LOG("%-50s%-10s%d\n",stru_DirFile[i].uc_FileLongName,"File", CHS64_GetLowValue(stru_DirFile[i].ul_FileSize)); 
			break;
		default:
			break;
		}
	}
	return 0;
}

static int fsys_cat(int argc, CMD_PARAM argv[])
{	
	CH_FSYS_FileHandle h_fileHandle = 0;
	U8 data_buf[16];
	int i = 0, j, ret;
	
	memset(data_buf,0,sizeof(data_buf));
	h_fileHandle = CH_FSYS_OpenFile(reinterpret_cast<const S8 *>(argv[1].get_str()), reinterpret_cast<const S8 *>("r+"));
	if(!h_fileHandle)
	{
		LOG("Open File %s Failure!\n ", argv[1].get_str());
		return -1;
	}
	
	while((ret = CH_FSYS_ReadFile(h_fileHandle, data_buf, 1, 16)) > 0) //应该以16进制+字符的方式打印 --- 如何解决换行符的问题??
	{
		printf("%08x: ", i);
		for(j = 0; j < ret; j++)
		{
			printf("%02x ", data_buf[j]);
		}
		printf("%s", "; ");
		for(j = 0; j < ret; j++)
		{
			if(data_buf[j] == '\r' || data_buf[j] == '\n')
			{
				printf("%s", " ");
			}
			else
			{
				printf("%c", data_buf[j]);
			}
		}
		printf("\r\n");
		i += ret;
	}
	CH_FSYS_CloseFile(h_fileHandle);
	return 0;
}

static int fsys_mv(int argc, CMD_PARAM argv[])
{
	//要先判断是文件还是目录 不能直接尝试.
	int  ret;
	S8 * oldPath = reinterpret_cast<S8 *>(argv[1].get_str());
	S8 * newPath = reinterpret_cast<S8 *>(argv[2].get_str());
	
	if(isDirOrFile(oldPath) == 0)
	{
		ret = CH_FSYS_RenameDir(oldPath, newPath);
		
		if(ret != 0)
		{
			LOG("CH_FSYS_RenameDir Wrong!\n");
			return -1;
		}
	}
	if(isDirOrFile(oldPath) == 1)
	{
		ret = CH_FSYS_RenameFile(oldPath, newPath);
		
		if(ret != 0)
		{
			LOG("CH_FSYS_RenameFile Wrong!\n");
			return -1;
		}
	}
	/*
	if(CH_FSYS_RenameFile(reinterpret_cast<const S8 *>(argv[1].get_str()), reinterpret_cast<const S8 *>(argv[2].get_str())) != 0 );
	{
	if(CH_FSYS_RenameDir(reinterpret_cast<const S8 *>(argv[1].get_str()), reinterpret_cast<const S8 *>(argv[2].get_str())) != 0)
	{
	LOG("The pathname Wrong!\n");
	return -1;
	}
	return 0;
}*/
	return 0;
}

static int fsys_cp(int argc, CMD_PARAM argv[])
{
	int ret;
	char * srcPath = argv[1].get_str();
	char * destPath =  argv[2].get_str();
	
	ret = cp(reinterpret_cast<S8 *>(srcPath), reinterpret_cast<S8 *>(destPath));
	if(ret != 0)
	{
		LOG("The is Wrong!\n");
		return -1;
	}
	return 0;
}

static int fsys_mkdir(int argc, CMD_PARAM argv[])
{	
	if(CH_FSYS_CreateDir(reinterpret_cast<const S8 *>(argv[1].get_str())) != CH_FSYS_OK)
	{
		LOG("Open Dirent Wrong!\n");
		return -1;
	}
	return 0;
}

static int fsys_rm(int argc, CMD_PARAM argv[])
{	
	CH_FSYS_FileOpHandle h_fileOp;
	
	//要先判断是文件还是目录 不能直接这么尝试
	int  ret;
	S8 * oldPath = reinterpret_cast<S8 *>(argv[1].get_str());
	
	ret = isDirOrFile(oldPath);
	if(ret == 0)
	{
		ret = CH_FSYS_DeleteDir(oldPath);
		
		if(ret != 0)
			return -1;
	}
	else if(ret == 1)
	{
		ret = CH_FSYS_DeleteFile(oldPath, NULL, &h_fileOp);
		
		if(ret != 0)
			return -1;
	}
	else
	{
		return -1;
	}
	/*
	if(CH_FSYS_DeleteFile(reinterpret_cast<const S8 *>(argv[1].get_str()), NULL, &h_fileOp) != 0)
	{
	if(CH_FSYS_DeleteDir(reinterpret_cast<const S8 *>(argv[1].get_str())) != 0)
	{
	LOG("Delete Wrong!\n");
	return -1;
	}
	return 0;
	}
	*/
	return 0;
}

static int fsys_format(int argc, CMD_PARAM argv[])
{	
	b_formateOver = CH_FALSE;
	if(CH_FSYS_Format(reinterpret_cast<const S8 *>(argv[1].get_str()), (CH_FSYS_TYPE_e)(argv[2].get_i()), formate_call) != 0)
	{
		LOG("Format Wrong!\n");
		return -1;
	}
	
	for(;;)
	{
		if(b_formateOver == CH_TRUE)
		{
			break;
		}
	}
	LOG("Format Complete!\n");
	return 0;
}

static int fsys_stat(int argc, CMD_PARAM argv[])
{	
	CH_FSYS_DirFileInfo_t stru_Info;
	int i_fileNum = 1;
	memset(&stru_Info,0,sizeof(CH_FSYS_DirFileInfo_t));
	
	if(CH_FSYS_GetFileInfo(reinterpret_cast<const S8 *>(argv[1].get_str()), &stru_Info) != 0)
	{	
		memset(&stru_Info,0,sizeof(CH_FSYS_DirFileInfo_t));
		if(CH_FSYS_GetDirInfo(reinterpret_cast<const S8 *>(argv[1].get_str()), 0, &stru_Info, reinterpret_cast<U32 *>(&i_fileNum)) !=0)
		{
			PROMPT("Get File/Dirent Information Wrong!\n");
			return -1;
		}
		return 0;
	}
	LOG("%-50s%*s%d\n",stru_Info.uc_FileLongName,4,"", CHS64_GetLowValue(stru_Info.ul_FileSize));
	return 0;
}



void test_fsys_init()
{
	REGISTER_CMD(fsys_init, "初始化fsys模块", "",  "");
	REGISTER_CMD(fsys_term, "销毁fsys模块", "",  "");
	REGISTER_CMD(fsys_ver, "获取fsys版本号", "",  "");
	REGISTER_CMD(fsys_ls, "列表显示目录下的所有文件及文件夹信息、大小", "dir_path",  "c:/");
	REGISTER_CMD(fsys_cat, "显示文件的内容", "file_path",  "c:/test.txt");
	REGISTER_CMD(fsys_mv, "文件或文件夹更名", "path1 path2",  "c:/test.txt c:/test1.txt");
	REGISTER_CMD(fsys_cp, "文件或文件夹拷贝", "path1 path2",  "c:/test.txt c:/new/");
	REGISTER_CMD(fsys_mkdir, "创建新文件夹", "path",  "c:/new");
	REGISTER_CMD(fsys_rm, "删除文件或文件夹", "path",  "c:/new");
	REGISTER_CMD(fsys_format, "格式化磁盘为指定文件系统格式", "disk_name sys_type",  "c: 0");
	REGISTER_CMD(fsys_stat, "显示文件或文件夹的信息", "path",  "c:/test.txt");
	REGISTER_CMD(fsys_info, "已挂载的磁盘信息及已打开的文件目录", "",  "");
}














