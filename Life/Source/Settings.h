// CSettings
//
// Contains all the possible settings for an Environment
// and its biots.
#pragma once

#include "genotype.h"
#include "etools.h"

class BRect;


///////////////////////////////////////////////////////////
// ContactLine
// 
// Stores information concerning how line to line interaction
// should be interpreted.
//
class ContactLine
{
public:
	ContactLine() : m_nContact(Ignore), m_fStrength(1.0) {};

	// Contact Types
	enum {
		Ignore = 0,
		Eat,		// A biot eats another biots line and gains energy
		Eaten,		// A biot is eaten and loses energy and mass
		Damage,		// A biot damages another biot but does not gain energy
		Damaged,	// A biot is damaged by another biot and loses energy and mass
		Defend,
		Attack		// A biot attacks another biot, both lose
	};

	// Type of Contact
	int    m_nContact;

	// 
	double m_fStrength;

	void Set(int nContact, double fStrength)
	{
		m_nContact  = nContact;
		m_fStrength = fStrength;

		// Sanity Check
		ASSERT(m_nContact <= Attack && m_nContact >= Ignore);
		ASSERT(m_fStrength >= 0.0 && m_fStrength <= 10.0);
	}

	ContactLine& operator=(ContactLine& to)
	{
		m_nContact  = to.m_nContact;
		m_fStrength = to.m_fStrength;

		// Sanity Check;
		ASSERT(m_nContact <= Attack && m_nContact >= Ignore);
		ASSERT(m_fStrength >= 0.0 && m_fStrength <= 10.0);
		return *this;
	}

	void Serialize(CArchive& a)
	{
		if (a.IsStoring())
		{
			a << m_nContact;
			a << m_fStrength;
		}
		else
		{
			a >> m_nContact;
			a >> m_fStrength;
		}
	}

	int Strengthen(int myState)
	{
		return int((m_fStrength * myState) + 0.5);
	}

	int Weaken(int yourState)
	{
		return int((yourState / m_fStrength) + 0.5);
	}
};


///////////////////////////////////////////////////////////
// CSettings
// 
// Settings impacting biots and their environment
//
class CSettings 
{
public:
	CSettings() { Reset(600, 340); }
	CSettings(CSettings &s){*this = s;}

	int  GetFriction() { return (int) ((1000.0 * friction) + 0.5F); }	
	void SetFriction(int iFriction) {friction = ((float)iFriction) / 1000.0F; }

	void Serialize(CArchive& a);
	void Reset(int nWidth, int nHeight);
	CSettings& operator=(CSettings& s);
	
	void SanityCheck();

	void SetDefaultContact();

	enum {
		BOTTOM = 0,
		TOP    = 1,
		RIGHT  = 2,
		LEFT   = 3,
		SIDES  = 4
	};

// Public Variables
public: 
    int m_nSick;
	int m_initialPopulation;		//Population used on restart/new
	int m_leafEnergy;
	CSoundRegistry m_sound;
	HPEN hPen[LINE_MAX];
    int     startNew;    // Should we use the old population, or the new?
    int     chance;
    int     nSexual;
	int     newType[LINE_MAX];
    DWORD   m_generation;
    ContactLine m_contact[LINE_MAX][LINE_MAX];
    LONG    leafMass[LINE_MAX];
    LONG    startEnergy;
    BOOL    bSoundOn;
    BOOL    bBarrier;
    BOOL    bMouse;    
    BOOL    bParentAttack;
    BOOL    bSiblingsAttack;    

    BOOL    bNoFlickerSet;
    float   friction;
    BRect   barrier;
    int     maxLineSegments;
    LONG    regenCost;
    DWORD   regenTime;
	int  	m_nSeed;
	int		m_nStartingPopulation;
	int		m_nArmTypesPerBiot;
	int		m_nSegmentsPerArm;
	int		m_nArmsPerBiot;
	BOOL	m_bGenerateOnExtinction;

	CString m_sSideAddress[SIDES];
	BOOL    m_bSideEnable[SIDES];

	int m_nHeight;
	int m_nWidth;
	int m_nSizeChoice;
};
