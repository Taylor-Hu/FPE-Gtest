#ifndef __DEBUG_AOUT_H__
#define __DEBUG_AOUT_H__

CHDRV_AOUT_DeviceHandle aout_GetHandle(int index);

int aout_GetHandleNums(void);



extern "C" {
CHDRV_AOUT_RESULT_e  __real_CHDRV_AOUT_Open(
				S32 ri_DeviceID,
				CHDRV_AOUT_DeviceHandle	*rph_SoundHandle);

CHDRV_AOUT_RESULT_e  __real_CHDRV_AOUT_Close(
				CHDRV_AOUT_DeviceHandle rph_SoundHandle);

CHDRV_AOUT_RESULT_e __real_CHDRV_AOUT_SetMute(
				CHDRV_AOUT_DeviceHandle rh_SoundHandle,
				CH_BOOL rb_MuteOnOff );


CHDRV_AOUT_RESULT_e __real_CHDRV_AOUT_SetVolume(
				CHDRV_AOUT_DeviceHandle rh_SoundHandle,
				S8 rc_LeftVolume,
				S8 rc_RightVolume);

CHDRV_AOUT_RESULT_e __real_CHDRV_AOUT_SetSoundChannelMode(
				CHDRV_AOUT_DeviceHandle rh_SoundHandle,
				CHDRV_AOUT_SOUND_CHANNEL_e renm_SoundChannel);

CHDRV_AOUT_RESULT_e __real_CHDRV_AOUT_SetSPDIFOutMode(
				CH_BOOL rb_CompressMode);

CHDRV_AOUT_RESULT_e __real_CHDRV_AOUT_SetHDMIOutMode(
				CH_BOOL rb_CompressMode);
}

#endif





