#include "pch.h"
#include "ShareMemory.h"

CShareMemory::~CShareMemory()
{
	Destory();
}

CShareMemory::CShareMemory(TCHAR *szMapName, DWORD dwSize)
{
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_dwSize = 0;

	_tcscpy(m_pMapName, szMapName);
	m_dwSize = dwSize;
}

bool CShareMemory::Create()
{
	//SECURITY_ATTRIBUTES sa;
	//SECURITY_DESCRIPTOR sd;

	//InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	//SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	//sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	//sa.lpSecurityDescriptor = &sd,
	//	sa.bInheritHandle = FALSE;

	m_hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		m_dwSize,
		m_pMapName);

	if (m_hFileMap)
	{
		m_lpFileMapBuffer = MapViewOfFile(
			m_hFileMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			m_dwSize);

		if (m_lpFileMapBuffer)
		{
			ZeroMemory(m_lpFileMapBuffer, m_dwSize);
			return true;
		}
	}

	return false;
}

bool CShareMemory::Open()
{
	m_hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_pMapName);
	if (m_hFileMap)
	{
		m_lpFileMapBuffer = MapViewOfFile(
			m_hFileMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			m_dwSize);

		if (m_lpFileMapBuffer)
		{
			return true;
		}
	}

	return false;
}

void CShareMemory::Destory()
{
	if (m_lpFileMapBuffer != NULL)
	{
		UnmapViewOfFile(m_lpFileMapBuffer);
		m_lpFileMapBuffer = NULL;
	}
	if (m_hFileMap != NULL)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
}

LPVOID CShareMemory::GetBuffer()
{
	return (m_lpFileMapBuffer) ? (m_lpFileMapBuffer) : (NULL);
}

DWORD CShareMemory::GetSize()
{
	return m_dwSize;
}

BOOL CShareMemory::ReadData(LPVOID pOutBuf, DWORD dwDataSize)
{
	ASSERT(pOutBuf != NULL);
	ASSERT(m_dwSize > dwDataSize);
	ASSERT(m_lpFileMapBuffer != NULL);

	if (NULL == m_lpFileMapBuffer)
	{
		return FALSE;
	}

	if (m_dwSize < dwDataSize)
	{
		return FALSE;
	}

	ZeroMemory(pOutBuf, dwDataSize);
	memcpy(pOutBuf, m_lpFileMapBuffer, dwDataSize);
	return TRUE;
}

BOOL CShareMemory::WriteData(const LPVOID pBuf, DWORD dwDataSize)
{
	ASSERT(pBuf != NULL);
	ASSERT(m_dwSize > dwDataSize);
	ASSERT(m_lpFileMapBuffer != NULL);

	if (NULL == m_lpFileMapBuffer)
	{
		return FALSE;
	}

	if (m_dwSize < dwDataSize)
	{
		return FALSE;
	}

	ZeroMemory(m_lpFileMapBuffer, m_dwSize);
	memcpy(m_lpFileMapBuffer, pBuf, dwDataSize);
	FlushViewOfFile(m_lpFileMapBuffer, dwDataSize);
	return TRUE;
}
