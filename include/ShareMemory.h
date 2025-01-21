#pragma once
#include <aclapi.h>

//////////////////////////////////////////////////////////////////////
#define DEFAULT_FILENAME NULL    
#define DEFAULT_MAPSIZE  (10 * 1024 * 1024) //10MB
//////////////////////////////////////////////////////////////////////
class CShareMemory
{
public:
	CShareMemory(TCHAR *szMapName, DWORD dwSize);
	virtual ~CShareMemory();

protected:
	HANDLE m_hFileMap;
	LPVOID m_lpFileMapBuffer;
	TCHAR m_pMapName[64];
	DWORD m_dwSize;

public:
	LPVOID GetBuffer();   
	DWORD GetSize();

	bool Create();
	bool Open();

	void Destory();
	BOOL WriteData(const LPVOID pBuf, DWORD dwDataSize);
	BOOL ReadData(LPVOID pOutBuf, DWORD dwDataSize);
};
