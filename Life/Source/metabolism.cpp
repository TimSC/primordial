// metabolism.cpp : implementation file
//

#include "stdafx.h"
#include "primordial life.h"
#include "rand.h"
#include "etools.h"
#include "metabolism.h"
#include "environ.h"
#include "biots.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMetabolism

IMPLEMENT_DYNCREATE(CMetabolism, CObject)

CMetabolism::CMetabolism()
	{
	ClearSettings();	
	}

CMetabolism::~CMetabolism()
{
}

int CMetabolism::MetabolicPath(long metabolite, BYTE coefficient)
	{
	int nChange = (int)(metabolite * ((float)(coefficient & 0xf)/ 32.0));

	if (coefficient > 128)
		{
		if (nChange != 0 )
			energy -= ((coefficient >> 4));
		}
	else
		{
		if (nChange != 0)
			energy += ((coefficient >> 4));
		}

	return nChange;
	}

void CMetabolism::CrossMetabolism(CMetabolism & pBiot)
	{
	Randomizer rand;
	int Crossover1 = rand.Integer(9);
	int Crossover2 = rand.Integer(9);
	int Crossover3 = rand.Integer(3);
	int Crossover4 = rand.Integer(9);
	
	if (m_matesMetabolism1[0] > 0)
		{	
		for (int i = 0; i < 9; i++)
			{
			if (i < Crossover1)
				pBiot.metabolism1[i] = metabolism1[i];
			else
				pBiot.metabolism1[i] = m_matesMetabolism1[i];

			if (i < Crossover2)
				{
				pBiot.metabolism2[i] = metabolism2[i];
				}
			else
				{
				pBiot.metabolism2[i] = m_matesMetabolism2[i];
				}
			}
		}
	else
		{
		for (int i = 0; i < 9; i++)
			{
			if (i != Crossover1)
				{
				pBiot.metabolism1[i] = metabolism1[i];
				}
			else
				{
				pBiot.metabolism1[i] = rand.twoNibbles();
				}

			if (i != Crossover2)
				{
				pBiot.metabolism2[i] = metabolism2[i];
				}
			else
				{
				pBiot.metabolism2[i] = rand.twoNibbles();
				}
			}
		}
				
	for (int i = 0; i < 3; i++)
		{
		if (m_matesInspiration[0] > 0)
			{
			if (i < Crossover3)
				{
				pBiot.m_inspiration[i] = m_inspiration[i];
				}
			else
				{
				pBiot.m_inspiration[i] = m_matesInspiration[i];
				}
			}
		else
			{
			if (i != Crossover3)
				{
				pBiot.m_inspiration[i] = m_inspiration[i];
				}
			else
				{
				pBiot.m_inspiration[i] = rand.Byte();
				}
			}
		if (m_matesExpiration[0] > 0)
			{
			if (i < Crossover3)
				{
				pBiot.m_expiration[i] = m_expiration[i];
				}
			else
				{
				pBiot.m_expiration[i] = m_matesExpiration[i];
				}
			}
		else
			{
			if (i != Crossover3)
				{
				pBiot.m_expiration[i] = m_expiration[i];
				}
			else
				{
				pBiot.m_expiration[i] = rand.Byte();
				}
			}		
		}

	for (i = 0; i < 9; i++)
		{
		if (m_arMatesMetabolicLimits[0] > 0)
			{
			if (i < Crossover4)
				{
				pBiot.m_arMetabolicLimits[i] = m_arMetabolicLimits[i];	
				}
			else
				{
				pBiot.m_arMetabolicLimits[i] = m_arMatesMetabolicLimits[0];
				}
			}
		else
			{
			if (i != Crossover4)
				{
				pBiot.m_arMetabolicLimits[i] = m_arMetabolicLimits[i];
				}
			else
				{
				pBiot.m_arMetabolicLimits[i] = 265 + rand.Integer(1920);
				}
			}
		
		}

	//Lose amount of metabolic resources equivalent to childs body structure
	TransferMaterial(redStore, pBiot.redStore, (float)0.25);
	TransferMaterial(greenStore, pBiot.greenStore, (float)0.25);
	TransferMaterial(blueStore, pBiot.blueStore, (float)0.25);
	TransferMaterial(e1store, pBiot.e1store, (float)0.25);
	TransferMaterial(e2store, pBiot.e2store, (float)0.25);
	TransferMaterial(e3store, pBiot.e3store, (float)0.25);
	}


void CMetabolism::StepMetabolism(CResources * cell)
	{
	//Get Input from Environment	
	TransferMaterial(cell->m_nRed, redIn, (float)m_inspiration[0]/(float)1550);
	if (redIn > m_arMetabolicLimits[0])
		{
		cell->m_nRed += redIn - m_arMetabolicLimits[0];
		redIn = m_arMetabolicLimits[0];
		}
	TransferMaterial(cell->m_nBlue, blueIn, (float)m_inspiration[1]/(float)1550);
	if (blueIn > m_arMetabolicLimits[1])
		{
		cell->m_nBlue += blueIn - m_arMetabolicLimits[1];
		blueIn = m_arMetabolicLimits[1];
		}
	TransferMaterial(cell->m_nGreen, greenIn, (float)m_inspiration[2]/(float)1550);
	if (greenIn > m_arMetabolicLimits[2])
		{
		cell->m_nGreen += greenIn - m_arMetabolicLimits[2];
		greenIn = m_arMetabolicLimits[2];
		}
	

	//Carry out reactions to produce intermediates
	bool bE1 = false;
	bool bE2 = false;
	bool bE3 = false;
	Randomizer rand;
	while (!(bE3 && bE2 && bE1))
		{
		switch (rand.Integer(3))
			{
			case 0:
				if (!bE1)
					{
					TransferMaterial(redIn, e1store, MetabolicPath(redIn,metabolism1[0]));
					TransferMaterial(greenIn, e1store, MetabolicPath(greenIn, metabolism1[1]));					
					TransferMaterial(blueIn, e1store, MetabolicPath(blueIn, metabolism1[2]));
					bE1 = true;
					}
				break;
			case 1:
				if (!bE2)
					{
					TransferMaterial(redIn, e2store, MetabolicPath(redIn,metabolism1[3]));					
					TransferMaterial(greenIn, e2store, MetabolicPath(greenIn, metabolism1[4])); 
					TransferMaterial(blueIn, e2store, MetabolicPath(blueIn, metabolism1[5]));
					bE2 = true;
					}
				break;
			case 2:
				if (!bE3)
					{
					TransferMaterial(redIn, e3store, MetabolicPath(redIn,metabolism1[6]));					
					TransferMaterial(greenIn, e3store, MetabolicPath(greenIn, metabolism1[7])); 					
					TransferMaterial(blueIn, e3store, MetabolicPath(blueIn, metabolism1[8]));
					bE3 = true;
					}
				break;
			}
		}


	if (e1store > m_arMetabolicLimits[3])
		{
		e1store = m_arMetabolicLimits[3];
		energy *= 0.9;
		}
	if (e2store > m_arMetabolicLimits[4])
		{
		e2store = m_arMetabolicLimits[4];
		energy *= 0.9;
		}
	if (e3store > m_arMetabolicLimits[5])
		{
		e3store = m_arMetabolicLimits[5];
		energy *= 0.9;
		}

	//Carry out reactions to produce building blocks
	bE1 = false;
	bE2 = false;
	bE3 = false;
	while (!(bE1 && bE2 && bE3))
		{
		switch (rand.Integer(3))
			{
			case 0:
				if (!bE1)
					{
					TransferMaterial(e1store, redStore, MetabolicPath(e1store,metabolism2[0]));					
					TransferMaterial(e2store, redStore, MetabolicPath(e2store, metabolism2[1]));
					TransferMaterial(e3store, redStore, MetabolicPath(e3store, metabolism2[2]));
					bE1 = true;
					}
				break;
			case 1:
				if (!bE2)
					{
					TransferMaterial(e1store, blueStore, MetabolicPath(e1store,metabolism2[3]));					
					TransferMaterial(e2store, blueStore, MetabolicPath(e2store, metabolism2[4]));					
					TransferMaterial(e3store, blueStore, MetabolicPath(e3store, metabolism2[5]));					
					bE2 = true;
					}
				break;
			case 2:
				if (!bE3)
					{
					TransferMaterial(e1store, greenStore, MetabolicPath(e1store,metabolism2[6]));
					TransferMaterial(e2store, greenStore, MetabolicPath(e2store, metabolism2[7]));
					TransferMaterial(e3store, greenStore, MetabolicPath(e3store, metabolism2[8]));
					bE3 = true;
					}
				break;
			}
		}

	//Throw Away Waste products
	TransferMaterial(redStore, cell->m_nRed, ((float)m_expiration[0]/(float)1550));
	if (redStore > m_arMetabolicLimits[6])
		{
		cell->m_nRed += redStore - m_arMetabolicLimits[6];
		redStore = m_arMetabolicLimits[6];
		energy *= 0.8;		//Feel the effects of Metabolic poisioning
		}
	

	TransferMaterial(blueStore, cell->m_nBlue, ((float)m_expiration[1]/(float)1550));
	if (blueStore > m_arMetabolicLimits[7])
		{
		cell->m_nBlue += blueStore - m_arMetabolicLimits[7];
		blueStore = m_arMetabolicLimits[7];
		energy *= 0.8;		//Feel the effects of Metabolic poisioning
		}
	
	
	TransferMaterial(greenStore, cell->m_nGreen, ((float)m_expiration[2]/(float)1550));
	if (greenStore > m_arMetabolicLimits[8])
		{
		cell->m_nGreen += greenStore - m_arMetabolicLimits[8];
		greenStore = m_arMetabolicLimits[8];
		energy *= 0.8;		//Feel the effects of Metabolic poisioning
		}
	
	}


//////////////////////////////////////////////////////////////////////
// PercentEnergy
//
float CMetabolism::PercentEnergy() const
	{
	float f = ((float) 100 * energy) / ((float) (adultBaseEnergy << 1));
	if (f > 100.0)
		return 100.0;
	else
		return f;
	}


void CMetabolism::ClearSettings()
	{
	ZeroMemory(m_matesMetabolism1,			sizeof(m_matesMetabolism1));
	ZeroMemory(m_matesMetabolism2,			sizeof(m_matesMetabolism2));
	ZeroMemory(metabolism1,					sizeof(metabolism1));
	ZeroMemory(metabolism2,					sizeof(metabolism2));
	ZeroMemory(m_statEnergy,				sizeof(m_statEnergy));
	ZeroMemory(m_arMetabolicLimits,			sizeof(m_arMetabolicLimits));
	ZeroMemory(m_arMatesMetabolicLimits,	sizeof(m_arMatesMetabolicLimits));
	ZeroMemory(m_matesInspiration,			sizeof(m_matesInspiration));
	ZeroMemory(m_matesExpiration,			sizeof(m_matesExpiration));
	ZeroMemory(m_inspiration,				sizeof(m_inspiration));
	ZeroMemory(m_expiration,				sizeof(m_expiration));

	energy = 0;
	adultBaseEnergy = 0;
	childBaseEnergy = 0;
	stepEnergy = 0;
	m_dBonusRatio = 0.0;
	m_statIndex = 0;
	redIn = 0;
	greenIn = 0;
	blueIn = 0;
	redStore = 0;
	greenStore = 0;
	blueStore = 0;
	e1store = 0;
	e2store = 0;
	e3store = 0;
	}

void CMetabolism::Randomize()
	{
	//Create Metabolic paths
	Randomizer rand;
	m_inspiration[0] = rand.Byte();
	m_inspiration[1] = rand.Byte();
	m_inspiration[2] = rand.Byte();
	m_expiration[0] = rand.Byte();
	m_expiration[1] = rand.Byte();
	m_expiration[2] = rand.Byte();
	for (int i = 0; i < 9; i ++)
		{
		metabolism1[i] = rand.twoNibbles();
		metabolism2[i] = rand.twoNibbles();
		}

	redStore = 64 + rand.Integer(92);
	greenStore = 64 + rand.Integer(92);
	blueStore = 64 + rand.Integer(92);
	for (i = 0; i < 9; i++)
		m_arMetabolicLimits[i] = 265 + rand.Integer(1920);
	redIn = 0;
	blueIn = 0;
	greenIn = 0;
	e1store = 0;
	e2store = 0;
	e3store = 0;
	}

void CMetabolism::DoStats(Biot * pBiot)
	{//(redStore + greenStore + blueStore)
	m_statEnergy[m_statIndex].energy = PercentEnergy();
	m_statEnergy[m_statIndex].red = (float)(100 * redStore) / (float) m_arMetabolicLimits[6];
	m_statEnergy[m_statIndex].blue = (float)(100 * blueStore) / (float) m_arMetabolicLimits[7];
	m_statEnergy[m_statIndex].green = (float)(100 * greenStore) / (float)m_arMetabolicLimits[8];

	m_statEnergy[m_statIndex].e1 = (float)(100 * e1store) / (float)m_arMetabolicLimits[3] ;
	m_statEnergy[m_statIndex].e2 = (float)(100 * e2store) / (float)m_arMetabolicLimits[4] ;
	m_statEnergy[m_statIndex++].e3 = (float)(100 * e3store) / (float)m_arMetabolicLimits[5] ;


	if (m_statIndex >= MAX_ENERGY_HISTORY)
		m_statIndex = 0;
	}

void CMetabolism::Serialize(CArchive& ar)
	{
	const BYTE archiveVersion = 13;
	long i;
	BYTE version;

	if (ar.IsLoading())
		{
		// Check version
		ar >> version;
		if (version > archiveVersion)
			AfxThrowArchiveException(CArchiveException::badSchema, _T("Biot"));
		}
	else
		{
		ar << archiveVersion;
		}

	// Now handle biot level variables
	if (ar.IsLoading())
		{
		ar.Read((LPBYTE) metabolism1, sizeof(metabolism1));
		ar.Read((LPBYTE) metabolism2, sizeof(metabolism2));
		ar.Read((LPBYTE) m_matesMetabolism1, sizeof(m_matesMetabolism1));
		ar.Read((LPBYTE) m_matesMetabolism2, sizeof(m_matesMetabolism2));
		ar.Read((LPBYTE) m_inspiration, sizeof(m_inspiration));
		ar.Read((LPBYTE) m_expiration, sizeof(m_expiration));
		ar.Read((LPBYTE) m_matesInspiration, sizeof(m_matesInspiration));
		ar.Read((LPBYTE) m_matesExpiration, sizeof(m_matesExpiration));
		ar.Read((LPBYTE) m_statEnergy, sizeof(m_statEnergy));
		ar.Read((LPBYTE) m_arMetabolicLimits, sizeof(m_arMetabolicLimits));
		
		ar >> redIn;
		ar >> greenIn;
		ar >> blueIn;
		ar >> e1store;
		ar >> e2store;
		ar >> e3store;
		ar >> redStore;
		ar >> greenStore;
		ar >> blueStore;
		ar >> energy;
		ar >> adultBaseEnergy;
		ar >> childBaseEnergy;
		ar >> m_dBonusRatio;
		ar >> m_statIndex;
		ar >> stepEnergy;	
		}
	else
		{
		ar.Write((LPBYTE) metabolism1, sizeof(metabolism1));
		ar.Write((LPBYTE) metabolism2, sizeof(metabolism2));
		ar.Write((LPBYTE) m_matesMetabolism1, sizeof(m_matesMetabolism1));
		ar.Write((LPBYTE) m_matesMetabolism2, sizeof(m_matesMetabolism2));
		ar.Write((LPBYTE) m_inspiration, sizeof(m_inspiration));
		ar.Write((LPBYTE) m_expiration, sizeof(m_expiration));
		ar.Write((LPBYTE) m_matesInspiration, sizeof(m_matesInspiration));
		ar.Write((LPBYTE) m_matesExpiration, sizeof(m_matesExpiration));
		ar.Write((LPBYTE) m_statEnergy, sizeof(m_statEnergy));
		ar.Write((LPBYTE) m_arMetabolicLimits, sizeof(m_arMetabolicLimits));
		
		ar << redIn;
		ar << greenIn;
		ar << blueIn;
		ar << e1store;
		ar << e2store;
		ar << e3store;
		ar << redStore;
		ar << greenStore;
		ar << blueStore;
		ar << energy;
		ar << adultBaseEnergy;
		ar << childBaseEnergy;
		ar << m_dBonusRatio;
		ar << m_statIndex;
		ar << stepEnergy;	
		}
	}

void CMetabolism::TransferMaterial(DWORD &from, DWORD &to, float ratio)
	{
	int nChange = (int)from * ratio;
	TransferMaterial(from, to, nChange);
	}

void CMetabolism::TransferMaterial(DWORD &from, DWORD &to, int nChange)
	{
	if (nChange > 0)
		{
		if (nChange < from)
			{
			from -= nChange;
			to += nChange;
			}
		else 
			{
			to += from;
			from = 0;		
			}
		}
	else
		{
		if (-nChange < to)
			{
			from -= nChange;
			to += nChange;
			}
		else
			{
			from += to;
			to = 0;					
			}
		}
		
	ASSERT((to >= 0) && (from >= 0));
	}
