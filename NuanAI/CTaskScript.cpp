#include "pch.h"
#include "CTaskScript.h"
#include "CDCMEngine.h"
#include "obj.h"

extern CDCMEngine* g_pEngine;

CTaskScript::CTaskScript(tagWndInfo* pWnd) : CTaskThread(pWnd)
{
	当前大地图 = cv读入图像(CW2CA(g_pEngine->m_strWorkPath + _T("/res/暗黑破坏神.png")).GetString(), cv::IMREAD_UNCHANGED, 1);
	二值大地图 = cv读入图像(CW2CA(g_pEngine->m_strWorkPath + _T("/res/暗黑破坏神2.png")).GetString(), cv::IMREAD_UNCHANGED, 1);
	
	截图区域 = CRect(1294 - 60, 127 - 60, 1294 + 60, 127 + 60);

	A星类.ReadMap(g_pEngine->m_strWorkPath + _T("/res/暗黑破坏神2.png"));

	开启局部定位 = false;
	定位错误次数 = 0;
	局部定位坐标 = CPoint(0, 0);

	雷达缩放比例 = 1.5;
}

CTaskScript::~CTaskScript()
{
	A星类.DestoryMap();
}

void CTaskScript::RegTaskFun(CString strName, FunType fun)
{
	m_mapFunc[strName] = fun;
}

CString CTaskScript::RunTaskFun(CString strName, CString strParam)
{
	if (m_mapFunc[strName] == NULL)
	{
		ASSERT(FALSE);
		return _T("");
	}

	m_pStatus->dwRunTick = GetTickCount();
	m_pStatus->strTime = GetTimeStr();

	return (this->*m_mapFunc[strName])(strParam);
}

bool CTaskScript::OnLoop()
{
	//脚本第一次执行
	if (m_pWnd->pTaskInfo->pStatus->dwRunTick == 0)
	{
		m_pStatus = m_pWnd->pTaskInfo->pStatus;
		g_pEngine->GetAccount(m_pWnd->id, m_pStatus->strAccount, m_pStatus->strPw);
	}

	//double dTest = 测试缩放比例();
	//打印坐标();

	//录制坐标并移动(10);

	A星移动(CPoint(1035, 929));

	return false;

	SleepEx(1);
	return true;
}

void CTaskScript::异常检查()
{
	启动游戏();
}

void CTaskScript::启动游戏()
{

}

double CTaskScript::测试缩放比例()
{
	double 比例 = 0.0;
	cv::Mat matCapture = cv截图转换图像(截图区域.left, 截图区域.top, 截图区域.right, 截图区域.bottom, cv::COLOR_BGRA2BGR);

	return cv动态缩放测试比例(当前大地图, matCapture, false, 2);
}

cv::Mat CTaskScript::截图缩放(double 比例)
{
	cv::Mat 局_截图;
	cv::Mat 缩放后图像;
	cv::Size 尺寸 = cv::Size(0, 0);
	局_截图 = cv截图转换图像(截图区域.left, 截图区域.top, 截图区域.right, 截图区域.bottom, cv::COLOR_BGRA2BGR);
	cv::resize(局_截图, 缩放后图像, 尺寸, 比例, 比例);

	return 缩放后图像;
}

CPoint CTaskScript::获取坐标()
{
	cv::Mat 雷达图 = 截图缩放(雷达缩放比例);

	//ShowMat(雷达图);

	CPoint 坐标 = CPoint(0, 0);

	if (开启局部定位)
	{
		cv::Rect roi(局部定位坐标.x - 200, 局部定位坐标.y - 200, 200 * 2, 200 * 2);
		cv::Mat 局部定位区域 = 当前大地图(roi);

		if (cv模板匹配(局部定位区域, 雷达图, 坐标, 0.4))
		{
			//根据返回的坐标来裁剪范围
			//上一次裁剪坐标 加上现在的位置
			局部定位坐标.x = 局部定位坐标.x - 200 + 坐标.x;
			局部定位坐标.y = 局部定位坐标.y - 200 + 坐标.y;

			//区域匹配不是 整个大图的坐标,  需要将上次裁剪的坐标加上 才是整个大图上的坐标
			坐标 = 局部定位坐标;
		}
		else
		{
			定位错误次数++;
			if (定位错误次数 >= 3)
			{
				开启局部定位 = false;
				坐标.x = 0;
				坐标.y = 0;
				定位错误次数 = 0;
			}
		}
	}
	else
	{
		if (cv模板匹配(当前大地图, 雷达图, 坐标, 0.4))
		{
			局部定位坐标.x = 坐标.x;
			局部定位坐标.y = 坐标.y;
			开启局部定位 = true;
		}
		else
		{
			开启局部定位 = false;
		}
	}

	return 坐标;
}

void CTaskScript::打印坐标()
{
	CPoint 坐标 = 获取坐标();
	LogD(_T("当前坐标:%d,%d"), 坐标.x, 坐标.y);
}

void CTaskScript::A星移动(CPoint 目的坐标)
{
	LogD(_T("开始A星寻路!"));

	//A星寻路
	CPoint ptMe = 获取坐标();
	std::vector<CPoint> arrPts;
	A星类.FindPath(ptMe, 目的坐标, arrPts);

	if (arrPts.size() == 0)
	{
		LogE(_T("没有发现可行走路径"));
		return;
	}

	//显示调试信息
	cv::Mat matPath = A星类.DrawPath(二值大地图, arrPts);
	ShowMat(matPath);

	MoveTo(718, 424);
	LeftClick();
	Sleep(200);
	LeftDown();

	for (int i = 0; i < arrPts.size(); i += 4)
	{
		CPoint 寻路坐标 = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 200)
		{
			CPoint 当前坐标 = 获取坐标();

			if (GetPtDis(寻路坐标, 当前坐标) < 10)
			{
				LogD(_T("移动完成"));
				break;
			}
			else
			{
				CPoint 移动鼠标 = 根据两点斜率计算平行线终点(当前坐标, 寻路坐标, CPoint(718, 424), 40);
				MoveTo(移动鼠标.x, 移动鼠标.y);
			}

			LogD(_T("当前坐标:%d,%d 寻路坐标:%d,%d 尝试次数:%d"), 当前坐标.x, 当前坐标.y, 寻路坐标.x, 寻路坐标.y, tryConut);
			Sleep(1);
		}

		if (tryConut >= 200)
		{
			LogE(_T("寻路失败"));
			break;
		}
	}

	LeftUp();

	LogD(_T("全部移动完成"));
}

void  CTaskScript::录制坐标并移动(int 记录时间)
{
	LogD(_T("5秒后开始录制!"));
	Sleep(5000);

	int 录制时间 = 0;
	std::vector<CPoint> arrPts;
	while (录制时间 < 记录时间 * 1000)
	{
		CPoint 坐标 = 获取坐标();
		if (坐标.x != 0)
		{
			LogD(_T("录制坐标:%d,%d"), 坐标.x, 坐标.y);
			arrPts.push_back(坐标);
		}

		Sleep(200);

		录制时间 += 200;
	}

	LogD(_T("5秒后开始寻路!"));

	Sleep(5000);

	MoveTo(718, 424);
	LeftClick();
	Sleep(200);

	LeftDown();

	for (int i = arrPts.size() - 1; i >= 0; i--)
	{
		CPoint 寻路坐标 = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 200)
		{
			CPoint 当前坐标 = 获取坐标();

			if (GetPtDis(寻路坐标, 当前坐标) < 6)
			{
				LogD(_T("移动完成"));
				break;
			}
			else
			{
				CPoint 移动鼠标 = 根据两点斜率计算平行线终点(当前坐标, 寻路坐标, CPoint(718, 424), 80);
				MoveTo(移动鼠标.x, 移动鼠标.y);
			}

			LogD(_T("当前坐标:%d,%d 寻路坐标:%d,%d 尝试次数:%d"), 当前坐标.x, 当前坐标.y, 寻路坐标.x, 寻路坐标.y, tryConut);
			Sleep(1);
		}

		if (tryConut >= 200)
		{
			LogE(_T("寻路失败"));
			break;
		}
	}

	LeftUp();

	LogD(_T("全部移动完成"));
}

