#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//数值定义
const TCHAR szProductKey[] = TEXT("AutoBoy");					//产品主键

//////////////////////////////////////////////////////////////////////////////////
//数据长度

//资料数据
#define LEN_MD5						33									//加密密码
#define LEN_USERNOTE				32									//备注长度
#define LEN_ACCOUNTS				32									//帐号长度
#define LEN_NICKNAME				32									//昵称长度
#define LEN_PASSWORD				33									//密码长度

//数据长度
#define LEN_QQ						16									//Q Q 号码
#define LEN_EMAIL					33									//电子邮件
#define LEN_USER_NOTE				256									//用户备注

//机器标识
#define LEN_NETWORK_ID				13									//网卡长度
#define LEN_MACHINE_ID				33									//序列长度


//数据库定义
#define DB_ERROR 					-1  								//处理失败
#define DB_SUCCESS 					0  									//处理成功
#define DB_NEEDMB 					18 									//处理失

////////////////////////////////////////////////////////////////////////////////
#endif