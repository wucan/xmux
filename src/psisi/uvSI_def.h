#ifndef __UV_SIDEF_H_
#define __UV_SIDEF_H_

typedef enum {
 EUV_CHA0 = 0,
 EUV_CHA1,
 EUV_CHA2,
 EUV_CHA3,
 EUV_CHA4,
 EUV_CHA5,
 EUV_CHA6,
 EUV_CHA7,
 EUV_CHA8,
 EUV_CHA_NUM
} EUV_CHAS;

//-----PAT------
#define UV_SI_PAT_LEN_PER_CHA (1024)

//-----PMT------
#define UV_SI_PROG_MAX_PER_CHA (32)
#define UV_SI_PROG_DATA_PER_CHA (1024)

//-----CAT------
#define UV_SI_CAT_DATA_PER_CHA (1024)

//-----NIT------ 分段最大长度为4096个字节
#define UV_SI_NIT_DATA_PER_CHA (1024)

//-----SDT/EIT------
#define UV_SI_SDT_SECTION_NUM (5)
#define UV_SI_SDT_DATA_PER_CHA (1024)

#define UV_SI_EIT_SECTION_NUM (8)
#define UV_SI_EIT_DATA_PER_CHA (1024)

uint8_t sg_mib_pat[EUV_CHA_NUM][UV_SI_PAT_LEN_PER_CHA];
uint8_t sg_mib_pmt[EUV_CHA_NUM][UV_SI_PROG_MAX_PER_CHA][UV_SI_PROG_DATA_PER_CHA];
uint8_t sg_mib_cat[EUV_CHA_NUM][UV_SI_CAT_DATA_PER_CHA];
uint8_t sg_mib_nit[EUV_CHA_NUM][UV_SI_NIT_DATA_PER_CHA];
uint8_t sg_mib_sdt[EUV_CHA_NUM][UV_SI_SDT_SECTION_NUM][UV_SI_SDT_DATA_PER_CHA];
uint8_t sg_mib_eit[EUV_CHA_NUM][UV_SI_EIT_SECTION_NUM][UV_SI_EIT_DATA_PER_CHA];

//uint8_t sg_mib_pmt_cnt[EUV_CHA_NUM]; 

// 转发表+输出码率和码流格式
#define UV_PID_TRANS_SIZE (1048)
uint8_t sg_mib_trans[UV_PID_TRANS_SIZE ]; 

//登陆数据报
#define UV_LOAD_INFO_SIZE (34)
uint8_t sg_mib_loadinfo[UV_LOAD_INFO_SIZE ];

//心跳数据报
#define UV_HEART_DEVICE_SIZE (4)
uint8_t sg_mib_heartDevice[UV_HEART_DEVICE_SIZE ];

//输入/转发信息列表
#define UV_INPUT_TRANS_INFO_NUM (24)
#define UV_INPUT_TRANS_INFO_SIZE (1082)
uint8_t sg_mib_Inptu_Trans_info[UV_INPUT_TRANS_INFO_NUM][UV_INPUT_TRANS_INFO_SIZE ];

//设置IP(服务器IP，IP，SUB，GW，MAC)
#define UV_IP_INFO_SIZE (22)
uint8_t sg_mib_IP_info[UV_IP_INFO_SIZE ];
//用户名，密码
#define UV_USER_INFO_SIZE (32)
uint8_t sg_mib_User_info[UV_USER_INFO_SIZE ];

#endif // __UV_SI_H_
