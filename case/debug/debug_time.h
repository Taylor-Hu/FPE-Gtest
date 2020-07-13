//-----------��̨ʱ�亯������----------->
#ifndef __DEBUG_TIME_H__
#define __DEBUG_TIME_H__
#define MAX_CHANNEL_TEST_TIMES  200 //���ͳ��200��


typedef enum
{
	TEST_KEY_START,/*0*/
	TEST_KEY_SEND,/*1*/
	TEST_SET_FREQ,/*2*/
	TEST_TUNER_LOCK,/*3*/
	TEST_SET_AVFILTER,/*4*/
	TEST_AUDIO_START,
	TEST_VIDEO_START,
	TEST_AUDIO_STOP,
	TEST_VIDEO_STOP,
	TEST_SET_ECM_FILTER,/*5*/
	TEST_OPEN_DECODER,/*6*/
	TEST_PCR_COME,/*7*/
	TEST_ECM_COME,/*8*/
	TEST_SET_CW,/*9*/
	TEST_IFRAME_COME,/*10*/
	TEST_VID_SYNC,/*11*/
	TEST_VIDEO_SHOW,/*12*/
	TEST_PMT_COME,/*13*/
	TEST_REQUEST_PAT,/*14*/
	TEST_REQUEST_PMT,/*15*/
	TEST_PAT_COME,/*16*/
	TEST_DEMOD_LOCK,/*17*/
	TEST_SET_FREQ2,/*18*/
	TEST_SET_FREQ3,/*19*/
	TEST_AUD_NEWFRAME,
	TEST_STEP_MAX
}TEST_STEP_e;

/**FuncName:Test_ChannelTestTime
@brief ... 
@param [in] step �μ�TEST_STEP_e, -1��ӡ���ܵĲ��Խ��
@param [out] none 
@return success  
@return fail     
@attention TEST_KEY_START��ʾһ�ֻ�̨���ԵĿ�ʼ�����е�ʱ���Դ�Ϊ��׼���У�����Ҫ���ʼ���á�*/
void Test_ChannelRecordTime(int step);

/**FuncName:get_channel_switch_time
@brief �õ���̨ʱĳ������ĺ�ʱƽ��ֵ
@param [in] step �μ�TEST_STEP_e
@param [out] none 
@return success  
@return fail     
@attention ʧ�ܷ���-1*/
int get_channel_switch_time(int step);

#endif

