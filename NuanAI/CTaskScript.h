#pragma once
#include "CTaskThread.h"
#include "CAstar.h"

struct tagGameStatus;
class CTaskScript : public CTaskThread
{
public:
	CTaskScript(tagWndInfo* pWnd);
	~CTaskScript();

protected:
	virtual bool OnLoop();

protected:
	void RegTaskFun(CString strName, FunType fun);
	CString RunTaskFun(CString strName, CString strParam);

private:
	tagGameStatus*	m_pStatus;
	CMapFunc		m_mapFunc;

private:
	void 异常检查();
	void 启动游戏();

private:
	cv::Mat 当前大地图;
	cv::Mat 二值大地图;
	CRect 截图区域;//中心点 + 半径
	double 雷达缩放比例;

	bool 开启局部定位;
	CPoint 局部定位坐标;
	int 定位错误次数;
	CPoint 获取坐标();

	void 打印坐标();

	void 录制坐标并移动(int 记录时间);
	void A星移动(CPoint 目的坐标);

	double 测试缩放比例();
	cv::Mat 截图缩放(double 比例);

	CAstar	A星类;

};

