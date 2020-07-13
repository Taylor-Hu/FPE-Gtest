#ifndef __DEBUG_TUENR_H__
#define __DEBUG_TUENR_H__


extern "C" {

CHDRV_TUNER_RESULT_e  __real_CHDRV_TUNER_COFDM_SetIF(
				S32 ri_Device,
				S32 ri_InterFreqKHz,
				CHDRV_TUNER_COFDM_BANDWIDTH_e renm_Bandwidth,
				CHDRV_TUNER_IQ_TYPE_e *rpenm_IQSpectrum,
				CH_BOOL *rpb_IfLock);


CHDRV_TUNER_RESULT_e  __real_CHDRV_TUNER_QAM_SetIF(
				S32 ri_Device,
				S32 ri_InterFreqKHz,
				S32 ri_SymbolRateKbps,
				CHDRV_TUNER_QAM_MODE_e renm_QAMMode,
				CHDRV_TUNER_IQ_TYPE_e *rpenm_IQSpectrum,
				CH_BOOL *rpb_IfLock);


CHDRV_TUNER_RESULT_e __real_CHDRV_TUNER_DMBTH_SetIF(
				S32 ri_Device,
				S32 ri_InterFreqKHz,
				CHDRV_TUNER_DMBTH_SetTunerParams_t rstru_SetTunerParams,
				CH_BOOL *rpb_IfLock);

CHDRV_TUNER_RESULT_e __real_CHDRV_TUNER_QPSK_SetIF(
				S32 ri_Device,
				S32 ri_InterFreqKHz,
				S32 ri_SymbolRateKbps,
				CHDRV_TUNER_IQ_TYPE_e *rpenm_IQSpectrum,
				CH_BOOL *rpb_IfLock);

CHDRV_TUNER_RESULT_e __real_CHDRV_TUNER_InstallCallBack(
    S32 ri_Device,
    void (*Callback)(S32 ri_Device,
                     CHDRV_TUNER_EVENT_TYPE_e renm_EventType,
                     void *rpv_ReturnData));

CHDRV_TUNER_RESULT_e  __real_CHDRV_TUNER_UninstallCallBack(
    S32 ri_Device,
    void (*Callback)(S32 ri_Device,
                     CHDRV_TUNER_EVENT_TYPE_e renm_EventType,
                     void *rpv_ReturnData));

}
#endif




