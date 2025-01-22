#include "pch.h"
#include "CTaskScript.h"
#include "CDCMEngine.h"
#include "obj.h"

extern CDCMEngine* g_pEngine;

CTaskScript::CTaskScript(tagWndInfo* pWnd) : CTaskThread(pWnd)
{
	��ǰ���ͼ = cv����ͼ��(CW2CA(g_pEngine->m_strWorkPath + _T("/res/�����ƻ���.png")).GetString(), cv::IMREAD_UNCHANGED, 1);
	��ֵ���ͼ = cv����ͼ��(CW2CA(g_pEngine->m_strWorkPath + _T("/res/�����ƻ���2.png")).GetString(), cv::IMREAD_UNCHANGED, 1);
	
	��ͼ���� = CRect(1294 - 60, 127 - 60, 1294 + 60, 127 + 60);

	A����.ReadMap(g_pEngine->m_strWorkPath + _T("/res/�����ƻ���2.png"));

	�����ֲ���λ = false;
	��λ������� = 0;
	�ֲ���λ���� = CPoint(0, 0);

	�״����ű��� = 1.5;
}

CTaskScript::~CTaskScript()
{
	A����.DestoryMap();
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
	//�ű���һ��ִ��
	if (m_pWnd->pTaskInfo->pStatus->dwRunTick == 0)
	{
		m_pStatus = m_pWnd->pTaskInfo->pStatus;
		g_pEngine->GetAccount(m_pWnd->id, m_pStatus->strAccount, m_pStatus->strPw);
	}

	//double dTest = �������ű���();
	//��ӡ����();

	//¼�����겢�ƶ�(10);

	A���ƶ�(CPoint(1035, 929));

	return false;

	SleepEx(1);
	return true;
}

void CTaskScript::�쳣���()
{
	������Ϸ();
}

void CTaskScript::������Ϸ()
{

}

double CTaskScript::�������ű���()
{
	double ���� = 0.0;
	cv::Mat matCapture = cv��ͼת��ͼ��(��ͼ����.left, ��ͼ����.top, ��ͼ����.right, ��ͼ����.bottom, cv::COLOR_BGRA2BGR);

	return cv��̬���Ų��Ա���(��ǰ���ͼ, matCapture, false, 2);
}

cv::Mat CTaskScript::��ͼ����(double ����)
{
	cv::Mat ��_��ͼ;
	cv::Mat ���ź�ͼ��;
	cv::Size �ߴ� = cv::Size(0, 0);
	��_��ͼ = cv��ͼת��ͼ��(��ͼ����.left, ��ͼ����.top, ��ͼ����.right, ��ͼ����.bottom, cv::COLOR_BGRA2BGR);
	cv::resize(��_��ͼ, ���ź�ͼ��, �ߴ�, ����, ����);

	return ���ź�ͼ��;
}

CPoint CTaskScript::��ȡ����()
{
	cv::Mat �״�ͼ = ��ͼ����(�״����ű���);

	//ShowMat(�״�ͼ);

	CPoint ���� = CPoint(0, 0);

	if (�����ֲ���λ)
	{
		cv::Rect roi(�ֲ���λ����.x - 200, �ֲ���λ����.y - 200, 200 * 2, 200 * 2);
		cv::Mat �ֲ���λ���� = ��ǰ���ͼ(roi);

		if (cvģ��ƥ��(�ֲ���λ����, �״�ͼ, ����, 0.4))
		{
			//���ݷ��ص��������ü���Χ
			//��һ�βü����� �������ڵ�λ��
			�ֲ���λ����.x = �ֲ���λ����.x - 200 + ����.x;
			�ֲ���λ����.y = �ֲ���λ����.y - 200 + ����.y;

			//����ƥ�䲻�� ������ͼ������,  ��Ҫ���ϴβü���������� ����������ͼ�ϵ�����
			���� = �ֲ���λ����;
		}
		else
		{
			��λ�������++;
			if (��λ������� >= 3)
			{
				�����ֲ���λ = false;
				����.x = 0;
				����.y = 0;
				��λ������� = 0;
			}
		}
	}
	else
	{
		if (cvģ��ƥ��(��ǰ���ͼ, �״�ͼ, ����, 0.4))
		{
			�ֲ���λ����.x = ����.x;
			�ֲ���λ����.y = ����.y;
			�����ֲ���λ = true;
		}
		else
		{
			�����ֲ���λ = false;
		}
	}

	return ����;
}

void CTaskScript::��ӡ����()
{
	CPoint ���� = ��ȡ����();
	LogD(_T("��ǰ����:%d,%d"), ����.x, ����.y);
}

void CTaskScript::A���ƶ�(CPoint Ŀ������)
{
	LogD(_T("��ʼA��Ѱ·!"));

	//A��Ѱ·
	CPoint ptMe = ��ȡ����();
	std::vector<CPoint> arrPts;
	A����.FindPath(ptMe, Ŀ������, arrPts);

	if (arrPts.size() == 0)
	{
		LogE(_T("û�з��ֿ�����·��"));
		return;
	}

	//��ʾ������Ϣ
	cv::Mat matPath = A����.DrawPath(��ֵ���ͼ, arrPts);
	ShowMat(matPath);

	MoveTo(718, 424);
	LeftClick();
	Sleep(200);
	LeftDown();

	for (int i = 0; i < arrPts.size(); i += 4)
	{
		CPoint Ѱ·���� = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 200)
		{
			CPoint ��ǰ���� = ��ȡ����();

			if (GetPtDis(Ѱ·����, ��ǰ����) < 10)
			{
				LogD(_T("�ƶ����"));
				break;
			}
			else
			{
				CPoint �ƶ���� = ��������б�ʼ���ƽ�����յ�(��ǰ����, Ѱ·����, CPoint(718, 424), 40);
				MoveTo(�ƶ����.x, �ƶ����.y);
			}

			LogD(_T("��ǰ����:%d,%d Ѱ·����:%d,%d ���Դ���:%d"), ��ǰ����.x, ��ǰ����.y, Ѱ·����.x, Ѱ·����.y, tryConut);
			Sleep(1);
		}

		if (tryConut >= 200)
		{
			LogE(_T("Ѱ·ʧ��"));
			break;
		}
	}

	LeftUp();

	LogD(_T("ȫ���ƶ����"));
}

void  CTaskScript::¼�����겢�ƶ�(int ��¼ʱ��)
{
	LogD(_T("5���ʼ¼��!"));
	Sleep(5000);

	int ¼��ʱ�� = 0;
	std::vector<CPoint> arrPts;
	while (¼��ʱ�� < ��¼ʱ�� * 1000)
	{
		CPoint ���� = ��ȡ����();
		if (����.x != 0)
		{
			LogD(_T("¼������:%d,%d"), ����.x, ����.y);
			arrPts.push_back(����);
		}

		Sleep(200);

		¼��ʱ�� += 200;
	}

	LogD(_T("5���ʼѰ·!"));

	Sleep(5000);

	MoveTo(718, 424);
	LeftClick();
	Sleep(200);

	LeftDown();

	for (int i = arrPts.size() - 1; i >= 0; i--)
	{
		CPoint Ѱ·���� = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 200)
		{
			CPoint ��ǰ���� = ��ȡ����();

			if (GetPtDis(Ѱ·����, ��ǰ����) < 6)
			{
				LogD(_T("�ƶ����"));
				break;
			}
			else
			{
				CPoint �ƶ���� = ��������б�ʼ���ƽ�����յ�(��ǰ����, Ѱ·����, CPoint(718, 424), 80);
				MoveTo(�ƶ����.x, �ƶ����.y);
			}

			LogD(_T("��ǰ����:%d,%d Ѱ·����:%d,%d ���Դ���:%d"), ��ǰ����.x, ��ǰ����.y, Ѱ·����.x, Ѱ·����.y, tryConut);
			Sleep(1);
		}

		if (tryConut >= 200)
		{
			LogE(_T("Ѱ·ʧ��"));
			break;
		}
	}

	LeftUp();

	LogD(_T("ȫ���ƶ����"));
}

