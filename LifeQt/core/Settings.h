// CSettings
//
// Contains all the possible settings for an Environment
// and its biots.

#include <string>
#include <QDataStream>
#include <QPen>
#include "Genotype.h"
#include "Etools.h"
#include "SoundRegistry.h"

class BRect;

#pragma once

class CSettings 
{
	// Public Methods
public:
	CSettings() { Reset(600, 340); }

	// Identity constructor
	CSettings(CSettings &s){*this = s;}

	int  GetFriction() { return (int) ((1000.0 * friction) + 0.5F); }	
	void SetFriction(int iFriction) {friction = ((float)iFriction) / 1000.0F; }

    void Serialize(QDataStream& a);
	void Reset(int nWidth, int nHeight);
	CSettings& operator=(CSettings& s);
	
	void SanityCheck();

	enum {
		BOTTOM = 0,
		TOP    = 1,
		RIGHT  = 2,
		LEFT   = 3,
		SIDES  = 4
	};

// Public Variables
public: //Fox BEGIN
    int m_nSick;
	int m_initialPopulation;		//Population used on restart/new
	int     m_leafEnergy;
	CSoundRegistry m_sound;
    QList<QPen> pens;
    int     startNew;    // Should we use the old population, or the new?
    int     chance;
    int     nSexual;
    int     newType[MAX_LEAF];
    short   effectiveLength[MAX_LEAF];
    uint32_t   m_generation;
    int64_t    leafContact[MAX_LEAF][MAX_LEAF];
    int64_t    leafMass[MAX_LEAF];
    int64_t    startEnergy;
    bool    bSoundOn;
    bool    bBarrier;
    bool    bMouse;
    bool    bParentAttack;
    bool    bSiblingsAttack;

    bool    bNoFlickerSet;
    float   friction;
    BRect   barrier;
    int     maxLineSegments;
    int64_t    regenCost;
    uint32_t   regenTime;
	int  	m_nSeed;
	int		m_nStartingPopulation;
	int		m_nArmTypesPerBiot;
	int		m_nSegmentsPerArm;
	int		m_nArmsPerBiot;
    bool	m_bGenerateOnExtinction;

    std::string m_sSideAddress[SIDES];
    bool    m_bSideEnable[SIDES];

	int m_nHeight;
	int m_nWidth;
	int m_nSizeChoice;
};//Fox END;
