#include "stdafx.h"
#include "primordial life.h"
#include "settings.h"


void CSettings::Serialize(CArchive& a)
{
	const int nVersion = 2;

	if (a.IsStoring())
	{
		a << nVersion;
		a << m_nSeed;
		a << m_nStartingPopulation;
		a << m_nArmTypesPerBiot;
		a << m_nSegmentsPerArm;
		a << m_nArmsPerBiot;
		a << m_bGenerateOnExtinction;

		for (int i = 0; i < SIDES; i++)
		{
			a << m_sSideAddress[i];
			a << m_bSideEnable[i];
		}

		a << m_nHeight;
		a << m_nWidth;
		a << m_nSizeChoice;
	}
	else
	{
		int nThisVersion;
		a >> nThisVersion;

		if (nThisVersion != nVersion)
		{
			throw new CArchiveException(CArchiveException::badIndex);
		}

		a >> m_nSeed;
		a >> m_nStartingPopulation;
		a >> m_nArmTypesPerBiot;
		a >> m_nSegmentsPerArm;
		a >> m_nArmsPerBiot;
		a >> m_bGenerateOnExtinction;

		for (int i = 0; i < SIDES; i++)
		{
			a >> m_sSideAddress[i];
			a >> m_bSideEnable[i];
		}

		a >> m_nHeight;
		a >> m_nWidth;
		a >> m_nSizeChoice;
	}

	
	for (int n = 0; n < LINES_BASIC; n++)
		for (int m = 0; m < LINES_BASIC; m++)
			m_contact[n][m].Serialize(a);
}


void CSettings::SanityCheck()
{
	if (m_nStartingPopulation > 50)
		m_nStartingPopulation = 50;

	for (int i = 0; i < SIDES; i++)
	{
		if (m_sSideAddress[i].IsEmpty())
			m_bSideEnable[i]  = FALSE;
	}

	if (m_nHeight > 4000)
		m_nHeight = 4000;

	if (m_nWidth > 4000)
		m_nWidth = 4000;

	if (m_nSizeChoice  > 3 ||
		m_nSizeChoice < 0)
		m_nSizeChoice = 2;
}


CSettings& CSettings::operator=(CSettings& s)
{
	m_nSick               = s.m_nSick;
	m_initialPopulation   = s.m_initialPopulation;
	m_leafEnergy          = s.m_leafEnergy;
	m_sound               = s.m_sound;
    //hPen                  = s.hPen;
    startNew              = s.startNew;    
    chance                = s.chance;
    nSexual               = s.nSexual;
    //newType               = s.newType;
    //effectiveLength       = s.effectiveLength;
    m_generation          = s.m_generation;
    //leafContact           = s.leafContact;
    //leafMass              = s.leafMass;
    startEnergy           = s.startEnergy;
    bSoundOn              = s.bSoundOn;
    bBarrier              = s.bBarrier;
    bMouse                = s.bMouse;    
    bParentAttack         = s.bParentAttack;
    bSiblingsAttack       = s.bSiblingsAttack;    

    bNoFlickerSet         = s.bNoFlickerSet;
    friction              = s.friction;    
    maxLineSegments       = s.maxLineSegments;
    regenCost             = s.regenCost;
    regenTime             = s.regenTime;

  	m_nSeed                 = s.m_nSeed;                
	m_nStartingPopulation   = s.m_nStartingPopulation;  
	m_nArmTypesPerBiot      = s.m_nArmTypesPerBiot;     
	m_nSegmentsPerArm       = s.m_nSegmentsPerArm;      
	m_nArmsPerBiot          = s.m_nArmsPerBiot;         
	m_bGenerateOnExtinction = s.m_bGenerateOnExtinction;

	for (int i = 0; i < SIDES; i++)
	{
		m_sSideAddress[i] = s.m_sSideAddress[i];
		m_bSideEnable[i]  = s.m_bSideEnable[i];
	}

	m_nHeight     = s.m_nHeight;
	m_nWidth      = s.m_nWidth;
	m_nSizeChoice = s.m_nSizeChoice;

	for (int n = 0; n < LINES_BASIC; n++)
		for(int m = 0; m < LINES_BASIC; m++)
			m_contact[n][m] = s.m_contact[n][m];

	return *this;
}

void CSettings::Reset(int nWidth, int nHeight)
{
  	m_nSeed                 = GetTickCount();
	m_nStartingPopulation   = 20;
	m_nArmTypesPerBiot      = 1;
	m_nSegmentsPerArm       = 10;
	m_nArmsPerBiot          = 0;
	m_bGenerateOnExtinction = TRUE;

	for (int i = 0; i < SIDES; i++)
	{
		m_sSideAddress[i] = _T("");
		m_bSideEnable[i]  = FALSE;
	}

	m_nHeight     = nWidth;
	m_nWidth      = nHeight;
	m_nSizeChoice = 1;

}


void CSettings::SetDefaultContact()
{
	// You have and the enemy has
	m_contact[LINE_LEAF][LINE_LEAF    ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_LEAF][LINE_SHIELD  ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_LEAF][LINE_INJECTOR].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_LEAF][LINE_MOUTH   ].Set(ContactLine::Eaten, 0.5);
	m_contact[LINE_LEAF][LINE_EYE     ].Set(ContactLine::Ignore, 1.0); 
	m_contact[LINE_LEAF][LINE_TOOTH   ].Set(ContactLine::Damaged, 0.5); 

	m_contact[LINE_SHIELD][LINE_SHIELD  ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_SHIELD][LINE_LEAF    ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_SHIELD][LINE_MOUTH   ].Set(ContactLine::Defend, 2.0);
	m_contact[LINE_SHIELD][LINE_INJECTOR].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_SHIELD][LINE_EYE     ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_SHIELD][LINE_TOOTH   ].Set(ContactLine::Defend, 0.5); 

	m_contact[LINE_MOUTH][LINE_MOUTH   ].Set(ContactLine::Attack, 1.0);
	m_contact[LINE_MOUTH][LINE_LEAF    ].Set(ContactLine::Eat, 2.0);
	m_contact[LINE_MOUTH][LINE_SHIELD  ].Set(ContactLine::Defend, 0.5);
	m_contact[LINE_MOUTH][LINE_INJECTOR].Set(ContactLine::Eat, 2.0);
	m_contact[LINE_MOUTH][LINE_EYE     ].Set(ContactLine::Eat, 2.0);
	m_contact[LINE_MOUTH][LINE_TOOTH   ].Set(ContactLine::Defend, 2.0); 

	m_contact[LINE_INJECTOR][LINE_MOUTH   ].Set(ContactLine::Eaten, 0.5);
	m_contact[LINE_INJECTOR][LINE_LEAF    ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_INJECTOR][LINE_SHIELD  ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_INJECTOR][LINE_INJECTOR].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_INJECTOR][LINE_EYE     ].Set(ContactLine::Ignore, 1.0); 
	m_contact[LINE_INJECTOR][LINE_TOOTH   ].Set(ContactLine::Damaged, 0.5); 

	m_contact[LINE_EYE][LINE_MOUTH   ].Set(ContactLine::Eaten, 0.5);
	m_contact[LINE_EYE][LINE_LEAF    ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_EYE][LINE_SHIELD  ].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_EYE][LINE_INJECTOR].Set(ContactLine::Ignore, 1.0);
	m_contact[LINE_EYE][LINE_EYE     ].Set(ContactLine::Ignore, 1.0); 
	m_contact[LINE_EYE][LINE_TOOTH   ].Set(ContactLine::Damaged, 0.5); 

	m_contact[LINE_TOOTH][LINE_MOUTH   ].Set(ContactLine::Defend, 0.5);
	m_contact[LINE_TOOTH][LINE_LEAF    ].Set(ContactLine::Damage, 2.0);
	m_contact[LINE_TOOTH][LINE_SHIELD  ].Set(ContactLine::Damage, 2.0);
	m_contact[LINE_TOOTH][LINE_INJECTOR].Set(ContactLine::Damage, 2.0);
	m_contact[LINE_TOOTH][LINE_EYE     ].Set(ContactLine::Damage, 2.0); 
	m_contact[LINE_TOOTH][LINE_TOOTH   ].Set(ContactLine::Defend, 1.0); 
}
