#pragma once
#pragma pack(1)

struct tagGameObj {
	int obj_id; //����ID
	int type;
	float prob; //����
	int l;
	int t;
	int r;
	int b;
};

#define GAME_SCENE_OBJ_MAX 120
#define GAME_SCENE_BMP_SIZE 1920 * 1080 * 4 

struct tagYoloMem {
	int id; //����ID

	int iCfgIndex; //��ͬ�ļ�������ļ��±�

	int iImgSize;
	int iImgWidth;
	int iImgHeight;
	byte byImgData[GAME_SCENE_BMP_SIZE];

	//����ʶ����
	int iObjCount;
	tagGameObj  obj[GAME_SCENE_OBJ_MAX]; //��ǰ��Ļ�еĶ������ ���� ���� �ȵ�

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
	int id; //�����ID
	int iChannels; //ͼƬͨ��
	int iOcrCount; //����ocr����

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

//ocrʶ���������ͨ�Ĺ����ڴ���
#define DCM_GAME_OCR_MEM _T("DCM_GAME_OCR_MEM_QQ6685742")
#define DCM_OCR_START_EVENT _T("AB_OCR_START_EVENT_QQ6685742")
#define DCM_OCR_DONE_EVENT _T("AB_OCR_DONE_EVENT_QQ6685742")

//yoloʶ���������ͨ�Ĺ����ڴ���
#define DCM_GAME_YOLO_MEM _T("DCM_GAME_YOLO_QQ6685742")
#define DCM_GAME_YOLO_START_EVENT _T("AB_GAME_YOLO_START_EVENT_QQ6685742")
#define DCM_GAME_YOLO_DONE_EVENT _T("AB_GAME_YOLO_DONE_EVENT_QQ6685742")

#pragma pack()