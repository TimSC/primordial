// Resources.cpp: implementation of the CResources class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "primordial life.h"
#include "rand.h"
#include "etools.h"
#include "Resources.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CResources::m_nDensityGreen = -1;
int CResources::m_nDensityBlue = 0;
int CResources::m_nDensityRed = 1;

CResources::CResources()
	{
	m_nBlue = 0;
	m_nRed = 0;
	m_nGreen = 0;
	}

CResources::~CResources()
	{
	
	}

void CResources::Randomize()
	{
	m_nBlue = Integer(255);
	m_nRed = Integer(255);
	m_nGreen = Integer(255);
	}

CString CResources::Trace()
	{
	CString strOut;
	strOut.Format (_T("Red = %ld; Blue = %ld; Green = %ld "), m_nRed,  m_nBlue, m_nGreen); 
	return strOut;
	}
