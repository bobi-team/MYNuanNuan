#pragma once
#pragma pack(1)

struct tagGameObj {
	int obj_id; //种类ID
	int type;
	float prob; //概率
	int l;
	int t;
	int r;
	int b;
};

#define GAME_SCENE_OBJ_MAX 120
#define GAME_SCENE_BMP_SIZE 1920 * 1080 * 4 

struct tagYoloMem {
	int id; //窗口ID

	int iCfgIndex; //不同的检测配置文件下标

	int iImgSize;
	int iImgWidth;
	int iImgHeight;
	byte byImgData[GAME_SCENE_BMP_SIZE];

	//接受识别结果
	int iObjCount;
	tagGameObj  obj[GAME_SCENE_OBJ_MAX]; //当前屏幕中的对象比如 建筑 人物 等等

	tagYoloMem()
	{
		iCfgIndex = 0;
		id = -1;

		iImgSize = 0;
		ZeroMemory(byImgData, sizeof(byImgData));

		iObjCount = 0;
		ZeroMemory(obj, sizeof(obj));
	}
};

struct tagOcrMem {
	int id; //虚拟机ID
	int iChannels; //图片通道
	int iOcrCount; //发起ocr数量

	int iImgSize;
	int iImgWidth;
	int iImgHeight;
	byte byImgData[GAME_SCENE_BMP_SIZE];

	char szOcr[2048];
	tagOcrMem()
	{
		id = -1;
		iChannels = 1;
		iOcrCount = 0;
		iImgSize = 0;
		iImgWidth = 0;
		iImgHeight = 0;
		ZeroMemory(byImgData, sizeof(byImgData));
		ZeroMemory(szOcr, sizeof(szOcr));
	}
};

//ocr识别和主程序沟通的共享内存名
#define DCM_GAME_OCR_MEM _T("DCM_GAME_OCR_MEM_QQ6685742")
#define DCM_OCR_START_EVENT _T("AB_OCR_START_EVENT_QQ6685742")
#define DCM_OCR_DONE_EVENT _T("AB_OCR_DONE_EVENT_QQ6685742")

//yolo识别和主程序沟通的共享内存名
#define DCM_GAME_YOLO_MEM _T("DCM_GAME_YOLO_QQ6685742")
#define DCM_GAME_YOLO_START_EVENT _T("AB_GAME_YOLO_START_EVENT_QQ6685742")
#define DCM_GAME_YOLO_DONE_EVENT _T("AB_GAME_YOLO_DONE_EVENT_QQ6685742")

#pragma pack()