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
	void �쳣���();
	void ������Ϸ();

private:
	cv::Mat ��ǰ���ͼ;
	cv::Mat ��ֵ���ͼ;
	CRect ��ͼ����;//���ĵ� + �뾶
	double �״����ű���;

	bool �����ֲ���λ;
	CPoint �ֲ���λ����;
	int ��λ�������;
	CPoint ��ȡ����();

	void ��ӡ����();

	void ¼�����겢�ƶ�(int ��¼ʱ��);
	void A���ƶ�(CPoint Ŀ������);

	double �������ű���();
	cv::Mat ��ͼ����(double ����);

	CAstar	A����;

};

