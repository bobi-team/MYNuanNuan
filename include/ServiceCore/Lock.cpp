#include "pch.h"
#include "Lock.h"

CLock::CLock()
{
	InitializeCriticalSection(&m_Section);
}

CLock::~CLock()
{
	::DeleteCriticalSection(&m_Section);
}

BOOL CLock::Lock()
{
	::EnterCriticalSection(&m_Section);

	return TRUE;
}

BOOL CLock::UnLock()
{
	::LeaveCriticalSection(&m_Section);

	return TRUE;
}

