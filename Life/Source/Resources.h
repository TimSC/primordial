// Resources.h: interface for the CResources class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCES_H__BD418F30_5327_486B_959C_B4BF06E9DB4B__INCLUDED_)
#define AFX_RESOURCES_H__BD418F30_5327_486B_959C_B4BF06E9DB4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CResources  : public BRect, public Randomizer
{
public:
	CString Trace();
	static int m_nDensityGreen;
	static int m_nDensityBlue;
	static int m_nDensityRed;
	void Randomize();
	DWORD m_nGreen;
	DWORD m_nBlue;
	DWORD m_nRed;
	CPoint m_position;
	CResources();
	virtual ~CResources();
	CResources & operator += (const CResources & rhs)
		{
		m_nGreen += rhs.m_nGreen;
		m_nRed += rhs.m_nRed;
		m_nBlue += rhs.m_nBlue;
		return *this;
		}


};

#endif // !defined(AFX_RESOURCES_H__BD418F30_5327_486B_959C_B4BF06E9DB4B__INCLUDED_)
