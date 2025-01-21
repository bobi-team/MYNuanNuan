#ifndef UTILS_H
#define UTILS_H

#pragma warning(disable:4996)

#include "string"

inline void LogPrintA(LPSTR szFormat, ...)
{
	CHAR szInfo[1024] = {0};

	va_list args;
	va_start(args, szFormat);
	vsprintf_s(szInfo, szFormat, args);
	va_end(args);

	OutputDebugStringA(szInfo);
}

inline void LogPrintW(LPWSTR szFormat, ...)
{
	WCHAR szInfo[1024] = {0};

	va_list args;
	va_start(args, szFormat);
	vswprintf(szInfo, szFormat, args);
	va_end(args);

	OutputDebugStringW(szInfo);
}

inline CString GetCurrentPath()
{
	CString strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();

	int pos = strPath.ReverseFind('\\');
	strPath = strPath.Left(pos);

	return strPath;
}

#ifdef UNICODE
#define LogPrint LogPrintW
#else
#define LogPrint LogPrintA
#endif


BOOL FileExists(TCHAR *sFilePath);

DWORD GetProcessPid(TCHAR* strProcName);
//DWORD GetProcessPid(TCHAR* strProcName, TCHAR* strWndTitle);

void Split(CString strSrc, CStringArray& dstArray, CString strDiv);
CString  MidStr(CString strSrc, CString str1, CString str2);
CString  RepStr(CString strSrc, CString strOld, CString strNew);

CStringA CW2CA(const CStringW &cstrSrcW);
CStringW CA2CW(const CStringA &cstrSrcA);

//获取指定进程的窗口

struct tagEnumExeWndItem
{
	DWORD dwPid;
	HWND hWnd;

	TCHAR szTitle[MAX_PATH];
	TCHAR szClz[MAX_PATH];

	tagEnumExeWndItem()
	{
		dwPid = 0;
		hWnd = 0;
		ZeroMemory(szTitle, sizeof(szTitle));
		ZeroMemory(szClz, sizeof(szClz));
	}
};

struct tagEnumExeWndParam
{
	DWORD dwPid;
	HWND hWnds[100];

	TCHAR szTitleWord[MAX_PATH];
	TCHAR szClzWord[MAX_PATH];

	tagEnumExeWndParam()
	{
		dwPid = 0;

		ZeroMemory(hWnds, sizeof(hWnds));
		ZeroMemory(szTitleWord, sizeof(szTitleWord));
		ZeroMemory(szClzWord, sizeof(szClzWord));
	}
};

int GetProcessWnd(CString strProcess, CString strWndTitle, CString strWndClz, CArray<tagEnumExeWndParam>& arrWndInfo);

bool EnableDebugPrivilege();
void CloseProc(CString strProcName);

//判断str是否是数字
bool StrIsDigit(CString strSrc);

//log到文件
void LogF(CString strTxtPath, CString strLog);

//输出到界面控件 界面需要绑定一个edit控件
void LogN(TCHAR* szFormat, ...);
void LogD(TCHAR* szFormat, ...);
void LogE(TCHAR* szFormat, ...);

//获取当前时间字符串2020-01-01 12:30:30
CString GetTimeStr();
CString GetTimeStr(SYSTEMTIME tm);

//判断当前时间是否超过tm 多少秒
bool TimeExceed(SYSTEMTIME tmOld, int iSec);

std::string GbkToUtf8(const char* src_str);
std::string GbkToUtf8(const wchar_t* src_wstr);
std::string Utf8ToGbk(const char* src_str);
std::string Utf8ToGbk(const wchar_t* wszGBK);

bool IsAscii(std::string strIn);
bool IsGbk(std::string strIn);

void ListDir(CString strPath, CString strKey, CStringArray& arrDir);
void ListFile(CString strPath, CString strKey, CStringArray& arrFile);

std::string Base64_Encode(const unsigned char* Data, int DataByte);
std::string Base64_Decode(const char* Data, int DataByte, int& OutByte);

CString I2S(int iValue);

#endif /* UTILS_H */
