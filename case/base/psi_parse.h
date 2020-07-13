#ifndef __PSI_PARSE_H__
#define __PSI_PARSE_H__

typedef struct
{
	unsigned char len;
	unsigned char tag;
}des_header_t;

class DescriptData
{
public:
	
	des_header_t header;
	unsigned char *data;

	
	DescriptData(){data = NULL;}
	~DescriptData(){if(data)delete[] data;}
};

typedef struct
{
	unsigned int invalid_bytes2 : 16;
	unsigned int descriptors_loop_length : 12;
	unsigned int free_CA_mode : 1;
	unsigned int running_status : 3;

	unsigned int invalid_bytes1 : 8;
	unsigned int EIT_present_following_flag : 1;
	unsigned int EIT_schedule_flag : 1;
	unsigned int reserved_futrue_use : 6;
	unsigned int service_id : 16;	
}sdt_service_header_t;

typedef struct
{
	unsigned int invalid_bytes3 : 8;
	unsigned int reserved_futrue_use2 : 8;
	unsigned int original_network_id : 16;

	unsigned int invalid_bytes2 : 8;
	unsigned int last_section_number : 8;
	unsigned int section_number : 8;
	unsigned int current_next_indicator : 1;
	unsigned int version_number : 5;
	unsigned int reserved2 : 2;
	
	unsigned int transport_stream_id : 16;
	unsigned int section_length : 12;
	unsigned int reserved1 : 2;
	unsigned int reserved_futrue_use1 : 1;
	unsigned int section_sytax_indicator : 1;

	unsigned int invalid_bytes1 : 24;
	unsigned int table_id : 8;
}sdt_des_header_t;

typedef struct
{
	unsigned int ES_info_length : 12;
	unsigned int reserved2 : 4;
	unsigned int elementary_PID : 13;
	unsigned int reserved1 : 3;

	unsigned int invalid_bytes1 : 24;
	unsigned int stream_type : 8;	
}pmt_stream_header_t;

typedef struct
{
	unsigned int program_info_length : 12;
	unsigned int reserved4 : 4;
	unsigned int PCR_PID : 13;
	unsigned int reserved3 : 3;

	unsigned int invalid_bytes2 : 8;
	unsigned int last_section_number : 8;
	unsigned int section_number : 8;
	unsigned int current_next_indicator : 1;
	unsigned int version_number : 5;
	unsigned int reserved2 : 2;
	
	unsigned int program_number : 16;
	unsigned int section_length : 12;
	unsigned int reserved1 : 3;
	unsigned int section_sytax_indicator : 1;

	unsigned int invalid_bytes1 : 24;
	unsigned int table_id : 8;	
}pmt_des_header_t;

typedef struct
{
	unsigned int invalid_bytes1 : 16;
	unsigned int transport_descriport_length : 12;
	unsigned int reserved_future_use : 4;
	unsigned int original_network_id : 16;
	unsigned int transport_stream_id : 16;	
}nit_ts_header_t;

typedef struct
{
	unsigned int invalid_bytes3 : 16;
	unsigned int network_descriptor_length : 12;
	unsigned int reserved_future_use2 : 4;

	unsigned int invalid_bytes2 : 8;
	unsigned int last_section_number : 8;
	unsigned int section_number : 8;
	unsigned int current_next_indicator : 1;
	unsigned int version_number : 5;
	unsigned int reserved2 : 2;
	
	unsigned int network_id : 16;
	unsigned int section_length : 12;
	unsigned int reserved1 : 2;
	unsigned int reserved_future_use : 1;
	unsigned int section_sytax_indicator : 1;

	unsigned int invalid_bytes1 : 24;
	unsigned int table_id : 8;	
}nit_des_header_t;

typedef struct
{
	unsigned int invalid_bytes3 : 16;
	unsigned int boquet_descriptor_length : 12;
	unsigned int reserved_future_use2 : 4;

	unsigned int invalid_bytes2 : 8;
	unsigned int last_section_number : 8;
	unsigned int section_number : 8;
	unsigned int current_next_indicator : 1;
	unsigned int version_number : 5;
	unsigned int reserved2 : 2;
	
	unsigned int boquet_id : 16;
	unsigned int section_length : 12;
	unsigned int reserved1 : 2;
	unsigned int reserved_future_use1 : 1;
	unsigned int section_sytax_indicator : 1;

	unsigned int invalid_bytes1 : 24;
	unsigned int table_id : 8;	
}bat_des_header_t;

typedef struct
{
	unsigned int invalid_bytes1 : 16;
	unsigned int transport_descriport_length : 12;
	unsigned int reserved_future_use : 4;
	unsigned int original_network_id : 16;
	unsigned int transport_stream_id : 16;	
}bat_ts_header_t;

typedef struct
{
	unsigned int program_map_PID : 13;
	unsigned int reserved1 : 3;
	unsigned int  program_number : 16;	
}pat_program_header_t;

typedef struct
{
	unsigned int invalid_bytes2 : 8;
	unsigned int last_section_number : 8;
	unsigned int section_number : 8;
	unsigned int current_next_indicator : 1;
	unsigned int version_number : 5;
	unsigned int reserved2 : 2;

	unsigned int transport_stream_id : 16;
	unsigned int section_length : 12;
	unsigned int reserved1 : 3;
	unsigned int section_sytax_indicator : 1;
	
	unsigned int invalid_bytes1 : 24;
	unsigned int table_id : 8;	
}pat_des_header_t;

class SdtServiceDes
{
public:
	
	sdt_service_header_t header;
	unsigned int des_nums : 8;//activity
	DescriptData *pDes;


	
	SdtServiceDes();
	~SdtServiceDes();
};



class SdtDes
{
public:
	
	sdt_des_header_t header;
	unsigned int service_nums : 8;//activity;
	SdtServiceDes *pService;
	unsigned int CRC_32;
	
	SdtDes();
	~SdtDes();
};

class PmtStreamDes
{
public:
	
	pmt_stream_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;
	
	PmtStreamDes();
	~PmtStreamDes();
};



class PmtDes
{
public:
	
	pmt_des_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;
	unsigned int stream_nums : 8;
	PmtStreamDes *pStream;
	unsigned int CRC_32;

	

	PmtDes();
	~PmtDes();
};


class NitTsDes
{
public:
	
	nit_ts_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;

	NitTsDes();
	~NitTsDes();
};


typedef struct
{
	unsigned short transport_stream_loop_length : 12;
	unsigned short reserved_future_use : 4;
}des_length_12bit_t;

class NitDes
{
public:
	
	nit_des_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;
	des_length_12bit_t len;
	unsigned int ts_nums : 8;
	NitTsDes *pTs;
	unsigned int CRC_32;

	NitDes();
	~NitDes();
};



class BatTsDes
{
public:
	
	bat_ts_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;


	
	BatTsDes();
	~BatTsDes();
};


class BatDes
{
public:
	
	bat_des_header_t header;
	unsigned int des_nums : 8;
	DescriptData *pDes;
	des_length_12bit_t len;
	unsigned int ts_nums : 8;
	BatTsDes *pTs;
	unsigned int CRC_32;


	BatDes();
	~BatDes();
};

class PatDes
{
public:
	
	pat_des_header_t header;
	unsigned int des_nums : 8;
	pat_program_header_t*pDes;
	unsigned int CRC_32;

	

	PatDes(){
		des_nums = 0;
		pDes = NULL;

		}
	~PatDes(){
		if(pDes)delete[] pDes;
		}
};


typedef enum
{
	E_CH_PSI_OTHER,
	E_CH_PSI_PAT,
	E_CH_PSI_PMT,
	E_CH_PSI_SDT,
	E_CH_PSI_NIT,
	E_CH_PSI_BAT,
	E_CH_PSI_TYPE_MAX,
}CH_PSI_TYPE_e;

typedef union
{
	PatDes *pat;
	BatDes *bat;
	PmtDes *pmt;
	SdtDes *sdt;
	NitDes *nit;
}PsiDes_u;

class PsiDescript
{
public:
	
	CH_PSI_TYPE_e type;
	PsiDes_u psi;
	PsiDescript();
	~PsiDescript();
};

class CaEcmPid_t
{
public:
	
	U16 us_CaSystemID;
	U16 us_EcmPid;
};

class StreamInfo_t
	
{
public:
	
	StreamInfo_t(){
		pui_language.clear();
		pus_EcmPid.clear();
	}
	~StreamInfo_t(){
		pui_language.clear();
		pus_EcmPid.clear();
	}
	StreamInfo_t(const StreamInfo_t & rhs){
		us_PrivateEsPid = rhs.us_PrivateEsPid;
		uc_PrivateEsStreamType = rhs.uc_PrivateEsStreamType;
		pui_language = rhs.pui_language;
		pus_EcmPid = rhs.pus_EcmPid;
		uc_Reserved = rhs.uc_Reserved;
		}
	U16 		us_PrivateEsPid;				/*私有流PID*/
	U8		uc_PrivateEsStreamType;	/*私有流类型，对应PMT中定义的值*/
	vector<U32>			pui_language;
	vector<CaEcmPid_t>	pus_EcmPid;
	U8		uc_Reserved;			/*保留，保证4字节对齐*/
};

class Channel_Info_t 
{
public:
	U16 							us_ChannelIndex; 											/*节目序号	，一般为该节目在节目列表中的序号*/
	U16 							us_CarrierIndex; 											/*频点/转发器索引号，一般为该节目所属频点/转发器在频点列表中的序号*/
	U16 							us_PmtPid; 													/*节目的PMT表格PID*/
	U8 							uc_ChannelType; 											/*节目的类型（电视、广播、数据广播等），该类型是从SDT中取得的类型*/
	U8 							uc_CaFlag; 													/*节目是否加密（1-加密，0-不加密）*/
	U16 							us_ServiceID; 												/*节目的ServiceID*/
	U16 							us_PcrPid; 													/*节目的PCR PID*/
	vector<StreamInfo_t>			pstru_Stream;
	vector<StreamInfo_t>			pstru_Audio;
	vector<StreamInfo_t>			pstru_Video;
	vector<CaEcmPid_t>			pus_EcmPid;
	U8 							auc_ChannelName[64]; 	/*节目名称*/
	U8 							auc_ProviderName[64]; /*节目提供商名称*/
	U32							ui_PMT_CRC32;												/*节目对应PMT表的CRC32值*/
	U16							us_AudioIndex;	//当前播放的音频序号
	U8							uc_Reserved;												/*保留，保证4字节对齐*/

	Channel_Info_t(){
	}
	~Channel_Info_t(){
		pstru_Stream.clear();
		pstru_Audio.clear();
		pstru_Video.clear();
		pus_EcmPid.clear();
	}
	Channel_Info_t(const Channel_Info_t & rhs){
		us_ChannelIndex = rhs.us_ChannelIndex;
		us_CarrierIndex = rhs.us_CarrierIndex;
		us_PmtPid = rhs.us_PmtPid;
		uc_ChannelType = rhs.uc_ChannelType;
		uc_CaFlag = rhs.uc_CaFlag;
		us_ServiceID = rhs.us_ServiceID;
		us_PcrPid = rhs.us_PcrPid;
		pstru_Stream = rhs.pstru_Stream;
		pstru_Audio = rhs.pstru_Audio;
		pstru_Video = rhs.pstru_Video;

		pus_EcmPid = rhs.pus_EcmPid;
		ui_PMT_CRC32 = rhs.ui_PMT_CRC32;
		us_AudioIndex = rhs.us_AudioIndex;
		uc_Reserved = rhs.uc_Reserved;
		memcpy(&auc_ChannelName[0] , &rhs.auc_ChannelName[0], sizeof(auc_ChannelName));
		memcpy(&auc_ProviderName[0] , &rhs.auc_ProviderName[0], sizeof(auc_ChannelName));
		}
};

//搜索节目
int prog_search(int device_id, vector <Channel_Info_t> &prog_list);



//将一个section解析出来
int ParseSection(U8* puc_sectioin, int i_len, PsiDescript *pDes);

#endif
