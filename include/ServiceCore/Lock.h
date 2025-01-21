#pragma once
class CLock
{
public:
	CLock();
	virtual ~CLock();

protected:
	CRITICAL_SECTION m_Section;

public:
	BOOL UnLock();
	BOOL Lock();
};

