#ifndef __DEBUG_FSYS_H__
#define __DEBUG_FSYS_H__



extern "C" {
/**
@brief 函数功能:  打开目录

@param [in] rpc_PathName 目录完整名称

@retval CH_FSYS_DirHandle 打开目录的句柄

@attention \n
	需要调用CH_FSYS_Init 进行模块初始化。
*/
CH_FSYS_DirHandle __real_CH_FSYS_OpenDir(const S8 *rpc_PathName);



/**
@brief 函数功能:  关闭目录

@param [in] rh_DirHandle 目录句柄

@return 请参见@link CH_FSYS_RESULT_e @endlink 定义

@attention \n
	需要调用CH_FSYS_Init 进行模块初始化。
*/
CH_FSYS_RESULT_e __real_CH_FSYS_CloseDir(CH_FSYS_DirHandle rh_DirHandle);

/**
@brief 函数功能: 打开或者创建文件

@param [in] rpc_FilePathName 文件完整路径
@param [in] rpc_FileMode 打开模式
@arg "r"  只读          文件必须存在
@arg "r+" 读写			文件必须存在
@arg "w"  只写			文件不必存在	文件存在则清空，长度置0，不存在则创建
@arg "w+" 读写			文件不必存在	文件存在则清空，长度置0，不存在则创建
@arg "a"  附加只写		文件不必存在	文件存在则内容附加到文件尾，不存在则创建
@arg "a+" 附加可读写	文件不必存在	文件存在则内容附加到文件尾，不存在则创建

@retval CH_FSYS_FileHandle 文件打开句柄

@attention \n
	需要调用CH_FSYS_Init 进行模块初始化。
*/
CH_FSYS_FileHandle __real_CH_FSYS_OpenFile(const S8 *rpc_FilePathName,const S8 *rpc_FileMode);


/**
@brief 函数功能: 关闭文件

@param [in] rh_File 文件句柄

@return 请参见@link CH_FSYS_RESULT_e @endlink 定义

@attention \n
	需要调用CH_FSYS_Init 进行模块初始化。
*/
CH_FSYS_RESULT_e __real_CH_FSYS_CloseFile(CH_FSYS_FileHandle rh_File);
}

#endif

