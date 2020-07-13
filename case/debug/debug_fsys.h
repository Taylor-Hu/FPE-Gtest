#ifndef __DEBUG_FSYS_H__
#define __DEBUG_FSYS_H__



extern "C" {
/**
@brief ��������:  ��Ŀ¼

@param [in] rpc_PathName Ŀ¼��������

@retval CH_FSYS_DirHandle ��Ŀ¼�ľ��

@attention \n
	��Ҫ����CH_FSYS_Init ����ģ���ʼ����
*/
CH_FSYS_DirHandle __real_CH_FSYS_OpenDir(const S8 *rpc_PathName);



/**
@brief ��������:  �ر�Ŀ¼

@param [in] rh_DirHandle Ŀ¼���

@return ��μ�@link CH_FSYS_RESULT_e @endlink ����

@attention \n
	��Ҫ����CH_FSYS_Init ����ģ���ʼ����
*/
CH_FSYS_RESULT_e __real_CH_FSYS_CloseDir(CH_FSYS_DirHandle rh_DirHandle);

/**
@brief ��������: �򿪻��ߴ����ļ�

@param [in] rpc_FilePathName �ļ�����·��
@param [in] rpc_FileMode ��ģʽ
@arg "r"  ֻ��          �ļ��������
@arg "r+" ��д			�ļ��������
@arg "w"  ֻд			�ļ����ش���	�ļ���������գ�������0���������򴴽�
@arg "w+" ��д			�ļ����ش���	�ļ���������գ�������0���������򴴽�
@arg "a"  ����ֻд		�ļ����ش���	�ļ����������ݸ��ӵ��ļ�β���������򴴽�
@arg "a+" ���ӿɶ�д	�ļ����ش���	�ļ����������ݸ��ӵ��ļ�β���������򴴽�

@retval CH_FSYS_FileHandle �ļ��򿪾��

@attention \n
	��Ҫ����CH_FSYS_Init ����ģ���ʼ����
*/
CH_FSYS_FileHandle __real_CH_FSYS_OpenFile(const S8 *rpc_FilePathName,const S8 *rpc_FileMode);


/**
@brief ��������: �ر��ļ�

@param [in] rh_File �ļ����

@return ��μ�@link CH_FSYS_RESULT_e @endlink ����

@attention \n
	��Ҫ����CH_FSYS_Init ����ģ���ʼ����
*/
CH_FSYS_RESULT_e __real_CH_FSYS_CloseFile(CH_FSYS_FileHandle rh_File);
}

#endif

