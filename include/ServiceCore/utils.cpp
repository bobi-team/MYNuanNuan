#include "pch.h"
#include "utils.h"
#include <windows.h>
#include <tchar.h>
#include "stdio.h"
#include <TlHelp32.h> 
#include "fstream"
#include "TraceService.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

TCHAR printfilename[MAX_PATH];

BOOL FileExists(TCHAR *sFilePath)
{
	WIN32_FIND_DATA w32fd;
	HANDLE hFile = FindFirstFile(sFilePath, &w32fd);
	if (hFile != INVALID_HANDLE_VALUE) {
		FindClose(hFile);
		return TRUE;
	}

	return FALSE; 
}

DWORD GetProcessPid(TCHAR* strProcName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hProcess, &pe32) == TRUE)
	{
		do
		{
			if (_tcscmp(strProcName, pe32.szExeFile) == 0)
			{
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcess, &pe32));
	}

	return 0;
}
//
//DWORD GetProcessPid(TCHAR* strProcName, TCHAR* strWndTitle)
//{
//	GetProcessWnd();
//	CArray<tagEnumExeWndParam> arrEnumWndParam;
//	int iWndCount = GetProcessWnd(strProcName, strWndTitle, _T(""), arrEnumWndParam);
//
//
//}

void Split(CString strSrc, CStringArray& dstArray, CString strDiv)
{
	int iPos = 0;
	CString strTmp;
	strTmp = strSrc.Tokenize(strDiv, iPos);
	while (strTmp.Trim() != _T(""))
	{
		dstArray.Add(strTmp);
		strTmp = strSrc.Tokenize(strDiv, iPos);
	}
}

CString RepStr(CString strSrc, CString strOld, CString strNew)
{
	strSrc.Replace(strOld, strNew);
	return strSrc;
}

CString  MidStr(CString strSrc, CString str1, CString str2)
{
	if (str1 != _T("") && str2 != _T(""))
	{
		return strSrc.Mid(strSrc.Find(str1) + str1.GetLength(), strSrc.Find(str2) - strSrc.Find(str1) - str1.GetLength());
	}

	if (str1 != _T("") && str2 == _T(""))
	{
		if (strSrc.Find(str1) == -1)
		{
			return strSrc;
		}
		else {
			return strSrc.Mid(strSrc.Find(str1) + str1.GetLength());
		}
	}

	if (str1 == _T("") && str2 != _T(""))
	{
		if (strSrc.Find(str2) == -1)
		{
			return strSrc;
		}
		else {
			return strSrc.Mid(0, strSrc.Find(str2));
		}
	}

	return strSrc;
}

CStringA CW2CA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete[] str;
	return cstrDestA;
}

CStringW CA2CW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;
	return cstrDestW;
}

BOOL CALLBACK EnumChildWindowCallBack(_In_ HWND hWnd, _In_ LPARAM lParam)
{
	tagEnumExeWndParam* pWndParam = (tagEnumExeWndParam*)lParam;

	TCHAR szTitle[MAX_PATH] = { 0 };
	TCHAR szClz[MAX_PATH] = { 0 };
	int nMaxCount = MAX_PATH;

	GetWindowText(hWnd, szTitle, nMaxCount);
	GetClassName(hWnd, szClz, nMaxCount);

	//LogN(_T("child %s - %s"), szTitle, szClz);

	bool bTitleFind = false;
	bool bClzFind = false;
	if (_tcscmp(pWndParam->szTitleWord, _T("")) != 0)
	{
		if (_tcsstr(szTitle, pWndParam->szTitleWord) != NULL) {
			bTitleFind = true;
		}
	}
	else {
		bTitleFind = true;
	}

	if (_tcscmp(pWndParam->szClzWord, _T("")) != 0)
	{
		if (_tcsstr(szClz, pWndParam->szClzWord) != NULL)
		{
			bClzFind = true;
		}
	}
	else {
		bClzFind = true;
	}

	if (bTitleFind && bClzFind)
	{
		for (int i = 0; i < 100; i++)
		{
			if (pWndParam->hWnds[i] == 0)
			{
				pWndParam->hWnds[i] = hWnd;
				break;
			}
		}
	}
	
	//DWORD dwPid = 0;
	//GetWindowThreadProcessId(hWnd, &dwPid);
	//if (dwPid == pWndParam->dwPid)
	{
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	tagEnumExeWndParam* pWndParam = (tagEnumExeWndParam*)lParam;

	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);
	if (dwPid == pWndParam->dwPid)
	{
		TCHAR szTitle[MAX_PATH] = { 0 };
		TCHAR szClz[MAX_PATH] = { 0 };

		GetWindowText(hWnd, szTitle, MAX_PATH);
		GetClassName(hWnd, szClz, MAX_PATH);

		//LogN(_T("top %s - %s"), szTitle, szClz);

		bool bTitleFind = false;
		bool bClzFind = false;
		if (_tcscmp(pWndParam->szTitleWord, _T("")) != 0)
		{
			if (_tcsstr(szTitle, pWndParam->szTitleWord) != NULL) {
				bTitleFind = true;
			}
		}
		else {
			bTitleFind = true;
		}

		if (_tcscmp(pWndParam->szClzWord, _T("")) != 0)
		{
			if (_tcsstr(szClz, pWndParam->szClzWord) != NULL)
			{
				bClzFind = true;
			}
		}
		else {
			bClzFind = true;
		}

		if (bTitleFind && bClzFind)
		{
			for (int i = 0; i < 100; i++)
			{
				if (pWndParam->hWnds[i] == 0)
				{
					pWndParam->hWnds[i] = hWnd;
					break;
				}
			}
		}

		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}

	return TRUE;
}

int GetProcessWnd(CString strProcess, CString strWndTitle, CString strWndClz, CArray<tagEnumExeWndParam>& arrWndInfo)
{
	DWORD dwPid = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	if (!Process32First(hSnapshot, &pe))
		return 0;

	CArray<int> arrPid;
	do
	{
		if (_tcscmp(pe.szExeFile, strProcess) == 0)
		{
			arrPid.Add(pe.th32ProcessID);
		}
	} while (Process32Next(hSnapshot, &pe));

	for (int i = 0; i < arrPid.GetCount(); i++)
	{
		tagEnumExeWndParam param;
		param.dwPid = arrPid[i];
		_tcscpy(param.szTitleWord, strWndTitle.GetString());
		_tcscpy(param.szClzWord, strWndClz.GetString());

		EnumWindows(EnumWindowCallBack, (LPARAM)&param);
		arrWndInfo.Add(param);
	}

	return arrWndInfo.GetCount();
}

bool EnableDebugPrivilege()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return  false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}

	return true;
}

void CloseProc(CString strProcName)
{
	EnableDebugPrivilege();

	int ret = 0;
	HANDLE  hSysSnapshot = NULL;
	PROCESSENTRY32 proc;
	hSysSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSysSnapshot == (HANDLE)-1)   return;
	proc.dwSize = sizeof(proc);
	if (Process32First(hSysSnapshot, &proc))
	{
		do 
		{
			if (_tcscmp(proc.szExeFile, strProcName) == 0)
			{
				HANDLE Proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc.th32ProcessID);
				if (Proc_handle == NULL || !TerminateProcess(Proc_handle, 0)) 
					ret = 1;
				else 
					ret = 0;
			}
		} while (Process32Next(hSysSnapshot, &proc));
	}

	CloseHandle(hSysSnapshot);

	return;
}

bool StrIsDigit(CString strSrc)
{
	if (strSrc.TrimLeft(_T("0123456789")) == _T(""))
		return true;

	return false;
}

void LogF(CString strTxtPath, CString strLog)
{
	if (strLog.GetLength() > 2000)
		return;
	
	TCHAR szInfo[2048] = { 0 };
	_tcscpy(szInfo, strLog.GetString());

	if (_taccess(strTxtPath, 0) == -1)
	{
		FILE* fp;
		_tfopen_s(&fp, strTxtPath, _T("wb"));
		if (fp != NULL)
		{
			uint16_t wSignature = 0xFEFF;
			fwrite(&wSignature, 2, 1, fp);
			SYSTEMTIME st;
			GetLocalTime(&st);
			wchar_t buf[128] = { 0 };
			swprintf_s(buf, 128, L"【%04d-%02d-%02d %02d:%02d:%02d:%03d】", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			fwrite(buf, sizeof(TCHAR), _tcslen(buf), fp);
			fwrite(szInfo, sizeof(TCHAR), _tcslen(szInfo), fp);
			fwrite(_T("\r\n"), sizeof(wchar_t), 2, fp);
			fclose(fp);
		}
	}
	else
	{
		FILE* fp;
		_wfopen_s(&fp, strTxtPath, _T("ab"));
		if (fp != NULL)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			wchar_t buf[128] = { 0 };
			swprintf_s(buf, 128, L"【%04d-%02d-%02d %02d:%02d:%02d:%03d】", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			fwrite(buf, sizeof(TCHAR), _tcslen(buf), fp);
			fwrite(szInfo, sizeof(TCHAR), _tcslen(szInfo), fp);
			fwrite(L"\r\n", sizeof(TCHAR), 2, fp);
			fclose(fp);
		}
	}
}

void LogN(TCHAR* szFormat, ...)
{
	TCHAR szInfo[2048] = { 0 };

	va_list args;
	va_start(args, szFormat);
	_vstprintf(szInfo, szFormat, args);
	va_end(args);

	CTraceService::TraceString(szInfo, TraceLevel_Normal);
}

void LogD(TCHAR* szFormat, ...)
{
	TCHAR szInfo[2048] = { 0 };

	va_list args;
	va_start(args, szFormat);
	_vstprintf(szInfo, szFormat, args);
	va_end(args);

	CTraceService::TraceString(szInfo, TraceLevel_Debug);
}

void LogE(TCHAR* szFormat, ...)
{
	TCHAR szInfo[2048] = { 0 };

	va_list args;
	va_start(args, szFormat);
	_vstprintf(szInfo, szFormat, args);
	va_end(args);

	CTraceService::TraceString(szInfo, TraceLevel_Exception);
}

CString GetTimeStr()
{
	CTime time = CTime::GetCurrentTime();
	CString curTime;
	curTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d",
		time.GetYear(),
		time.GetMonth(),
		time.GetDay(),
		time.GetHour(),
		time.GetMinute(),
		time.GetSecond());

	return curTime;
}

CString GetTimeStr(SYSTEMTIME tm)
{
	CString curTime;
	curTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d",
		tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour,
		tm.wMinute,
		tm.wSecond);

	return curTime;
}

bool TimeExceed(SYSTEMTIME tmOld, int iSec)
{
	CTimeSpan timeSpan = CTime::GetCurrentTime() - CTime(tmOld);
	return timeSpan.GetTotalSeconds() > iSec;
}

std::string GbkToUtf8(const char* src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	ZeroMemory(wstr, (len + 1) * 2);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	ZeroMemory(str, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

std::string GbkToUtf8(const wchar_t* src_wstr)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, src_wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	ZeroMemory(str, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, src_wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;
	if (str) delete[] str;
	return strTemp;
}

std::string Utf8ToGbk(const char* src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	ZeroMemory(wszGBK, (len + 1) * 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	ZeroMemory(szGBK, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

std::string Utf8ToGbk(const wchar_t* wszGBK)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	ZeroMemory(szGBK, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (szGBK) delete[] szGBK;
	return strTemp;
}

bool IsAscii(std::string strIn)
{
	for (int i = 0; i < (int)strIn.size(); i++)
	{
		if (!isascii(strIn.at(i)))
		{
			return false;
		}
	}

	return true;
}

bool IsGbk(std::string strIn)
{
	unsigned int nBytes = 0;//GBK可用1-2个字节编码,中文两个 ,英文一个 
	unsigned char chr = strIn.at(0);
	bool bAllAscii = true; //如果全部都是ASCII,  

	for (unsigned int i = 0; strIn[i] != '\0'; ++i) {
		chr = strIn.at(i);
		if ((chr & 0x80) != 0 && nBytes == 0) {// 判断是否ASCII编码,如果不是,说明有可能是GBK
			bAllAscii = false;
		}

		if (nBytes == 0) {
			if (chr >= 0x80) {
				if (chr >= 0x81 && chr <= 0xFE) {
					nBytes = +2;
				}
				else {
					return false;
				}

				nBytes--;
			}
		}
		else {
			if (chr < 0x40 || chr>0xFE) {
				return false;
			}
			nBytes--;
		}//else end
	}

	if (nBytes != 0) {		//违返规则 
		return false;
	}

	if (bAllAscii) { //如果全部都是ASCII, 也是GBK
		return true;
	}

	return true;
}

void ListDir(CString strPath, CString strKey, CStringArray& arrDir)
{
	if (strPath.GetAt(strPath.GetLength() - 1) == _T('\\'))
		strPath.Left(strPath.GetLength() - 1);
	
	CFileFind fFind;
	CString strDir = strPath + "/*.*";
	BOOL res = fFind.FindFile(strDir);
	while (res)
	{
		res = fFind.FindNextFile();
		if (fFind.IsDirectory() && (!fFind.IsDots()))
		{
			CString strFile = fFind.GetFileName();
			if (strKey != _T(""))
			{
				if (strFile.Find(strKey) != -1)
					arrDir.Add(strFile);
			}
			else {
				arrDir.Add(strFile);
			}
		}
	}

	fFind.Close();
}

void ListFile(CString strPath, CString strKey, CStringArray& arrFile)
{
	if (strPath.GetAt(strPath.GetLength() - 1) == _T('\\'))
		strPath.Left(strPath.GetLength() - 1);

	CFileFind fFind;
	CString strDir = strPath + "/*.*";
	BOOL res = fFind.FindFile(strDir);
	while (res)
	{
		res = fFind.FindNextFile();
		if (!fFind.IsDirectory() && (!fFind.IsDots()))
		{
			CString strFile = fFind.GetFileName();
			if (strKey != _T(""))
			{
				if (strFile.Find(strKey) != -1)
					arrFile.Add(strFile);
			}
			else {
				arrFile.Add(strFile);
			}
		}
	}

	fFind.Close();
}

std::string Base64_Encode(const unsigned char* Data, int DataByte)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	std::string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

std::string Base64_Decode(const char* Data, int DataByte, int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	std::string strDecode;
	int nValue;
	int i = 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data != '\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode += (nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode += nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}

CString I2S(int iValue)
{
	CString strValue;
	strValue.Format(_T("%d"), iValue);
	return strValue;
}

///end
