#pragma once
#include <aclapi.h>
#include "ShareMemData.h"
#include "ShareMemory.h"



class CAiTool
{
public:
	~CAiTool();
	CAiTool(const CAiTool&) = delete;
	CAiTool& operator=(const CAiTool&) = delete;

	static CAiTool& get_instance() {
		static CAiTool instance;
		return instance;
	}

	//创建全局的内存信息 必须提前调用
	void Create();

	CString Ocr(int id, byte* pbData, CSize szImg);

	//0成功 1未找到共享内存信息 2超时
	int Yolo(int id, byte* pbImgData, CSize szImg, int iCfgIndex, tagGameObj* pObj);

private:
	CAiTool();

	CStringA CW2CA(const CStringW& cstrSrcW);
	CStringW CA2CW(const CStringA& cstrSrcA);

private:
	CShareMemory* m_pYoloMem;
	HANDLE m_hYoloDetectStartEvent; //通知yolo进程开始分析游戏场景的事件
	HANDLE m_hYoloDetectDoneEvent; //yolo进程反通知分析完成

	CShareMemory* m_pOcrMem;
	HANDLE m_hOcrDetectStartEvent; //通知ocr进程开始分析游戏场景的事件
	HANDLE m_hOcrDetectDoneEvent; //ocr进程反通知分析完成

};

