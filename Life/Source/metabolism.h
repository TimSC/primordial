#if !defined(AFX_METABOLISM_H__47DD68CC_9767_4DEC_B23A_A2DE65A7E4F3__INCLUDED_)
#define AFX_METABOLISM_H__47DD68CC_9767_4DEC_B23A_A2DE65A7E4F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// metabolism.h : header file
//


#include "resources.h"
/////////////////////////////////////////////////////////////////////////////
// CMetabolism command target
#define  MAX_ENERGY_HISTORY 21

class CMetabolism : public CObject
	{
	DECLARE_DYNCREATE(CMetabolism)
	class CMetStat
		{
	public:
		float energy;
		float red;
		float green;
		float blue;
		float e1;
		float e2;
		float e3;

		CMetStat & operator=(CMetStat & rhs)
			{
			energy = rhs.energy; red = rhs.red; green = rhs.green; e1 = rhs.e1; e2 = rhs.e2; e3 = rhs.e3;
			return *this;
			}
		};
public:
	CMetabolism();           
	virtual ~CMetabolism();
// Attributes Metabolism - 
    DWORD    energy;
	DWORD	redIn;
	DWORD	blueIn;
	DWORD	greenIn;
	DWORD	redStore;
	DWORD	greenStore;	
	DWORD	blueStore;
	DWORD	e1store;
	DWORD	e2store;
	DWORD	e3store;
	BYTE	m_inspiration[3];
	BYTE	m_expiration[3];
	BYTE	metabolism1[9]; //High Nibble of each Byte gives Energy Cost/Gain of Step
	BYTE	metabolism2[9];	//Low Nibble of Each Byte gives the amount of corresponding 
							//Metabolite that participates in reaction
	DWORD	m_arMetabolicLimits[9];		

	BYTE	m_matesExpiration[3];
	BYTE	m_matesInspiration[3];
	BYTE	m_matesMetabolism2[9];
	BYTE	m_matesMetabolism1[9];
	DWORD	m_arMatesMetabolicLimits[9];

	LONG    childBaseEnergy;
	LONG    adultBaseEnergy;
    LONG    stepEnergy;

	double m_dBonusRatio;
	CMetStat m_statEnergy[MAX_ENERGY_HISTORY];
	int  m_statIndex;
// Operations
public:
	
	void	TransferMaterial(DWORD & from, DWORD & to, int nChange);
	void	TransferMaterial(DWORD &from, DWORD& to, float ratio);
	void	DoStats(Biot * pBiot);
	void	Randomize();
	void	ClearSettings();
	void	CrossMetabolism(CMetabolism & pBiot);
	int		MetabolicPath(long metabolite, BYTE coefficient);
	void	StepMetabolism(CResources * cell);
	float	PercentEnergy() const;
	void	Serialize(CArchive& ar);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METABOLISM_H__47DD68CC_9767_4DEC_B23A_A2DE65A7E4F3__INCLUDED_)
