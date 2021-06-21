 ///////////////////////////////////////////////////////////////////////////////
//	      FOX WORKED ON THIS ENTIRE DOC(ANYTHING WITH 'options')
// Environment
//
// This class defines the environment of the biots
//
//
//#include "Primordial Life.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <QDateTime>
#include <QThread>
//#include "LoadBitmap.h"
//#include "MainFrm.h"
//#include "evolve.h"
#include "Environ.h"
#include "Biots.h"

// from evolve.cpp
extern char szFile[];

char szChance[]         = "chance";
char szSound[]          = "sound";
char szPopulation[]     = "population";
char szRegenCost[]      = "regenerationCost";
char szRegenTime[]      = "regenerationTime";
char szGreenValue[]     = "green";
char szStartNew[]       = "startNew";
char szMouse[]          = "mouse";
char szLifeSpan[]       = "lifeSpan";
char szOneColor[]       = "oneColor";
char szTwoColor[]       = "twoColor";
char szThreeColor[]     = "threeColor";
char szParentAttack[]   = "parentAttack";
char szSiblingsAttack[] = "siblingsAttack";
char szAsexual[]        = "asexual";
char szNoFlicker[]      = "noFlicker";
char szFriction[]       = "friction";

//CSock&  sock = g_sock;


//////////////////////////////////////////////////////////////////////
// CBiotList
//

CBiotList::CBiotList() : m_nBiot(-1), m_bLooped(false)
{
//    Biot* nullBiot = nullptr;
//    for(size_t i=0; i<200; i++)
 //       this->append(nullBiot);
}

//////////////////////////////////////////////////////////////////////
// FindBiotByID
//
//
Biot* CBiotList::FindBiotByID(uint32_t id)
{
    for (int i = 0; i < size(); i++)
        if (this->at(i)->m_Id == id)
            return this->at(i);
	return NULL;
}


/////////////////////////////////////////////////////////////////////
// FindBiotByPoint
//
//
int CBiotList::FindBiotByPoint(int x, int y)
{
int minDist = 30000;
int minBiot = -1;
int distance;

    for (int i = 0; i < size(); i++)
	{
        distance = this->at(i)->Contains(x, y);
		if (distance >= 0 &&
			distance < minDist)
		{
			minDist = distance;
			minBiot = i;
		}
	}
	return minBiot;
}


/////////////////////////////////////////////////////////////////////
// FreeAll
//
void CBiotList::FreeAll()
{
    for (int i = 0; i < size(); i++)
        delete this->at(i);

    clear();
}


/////////////////////////////////////////////////////////////////////
// NextBiot
//
//
Biot* CBiotList::NextBiot()
{
    if (++m_nBiot >= size())
	{
		m_nBiot = 0;

        if (m_nBiot == size())
		{
			m_nBiot = -1;
			return NULL;
		}
		else
		{
            m_bLooped = true;
		}
	}
	else
	{
        m_bLooped = false;
	}
    return this->at(m_nBiot);
}


//////////////////////////////////////////////////////////////////////
// HitCheck
//
//
Biot* CBiotList::HitCheck(Biot *me, int* pStart)
{
	int i;

	if (pStart)
		i = *pStart;
	else
		i = 0;

    for (; i < size(); i++)
	{
        if (this->at(i) != me &&
            this->at(i)->Touches(*me))
		{
			if (pStart)
				*pStart = i + 1;
            return this->at(i);
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// Serialize
//
//
/*
void  CBiotList::Serialize(QDataStream& ar, Environment& env)
{
	if (ar.IsLoading())
	{
		FreeAll();

		int size;
		Biot* pBiot;

		ar >> size;
		ar >> m_bLooped;
		ar >> m_nBiot;

		for (int i = 0; i < size; i++)
		{
			if ((pBiot = new Biot(env)) != NULL)
			{
				pBiot->Serialize(ar);
				Add(pBiot);
			}
			else
				break;
		}
	}
	else
	{
		ar << GetSize();
		ar << m_bLooped;
		ar << m_nBiot;
		for (int i = 0; i < GetSize(); i++)
			GetAt(i)->Serialize(ar);
	}
}	

*/
//////////////////////////////////////////////////////////////////////
// OnStop
//
//
void CBiotList::RemoveBiot()
{
    delete this->at(m_nBiot);
    this->removeAt(m_nBiot);
	m_nBiot--;
}


//////////////////////////////////////////////////////////////////////
// CEnvironmentStats
//
//
void CEnvStats::Clear()
{
	m_collisionCount = 0;
	m_births         = 0;
	m_deaths         = 0;
	m_arrivals       = 0;
	m_departures     = 0;
	m_days           = 0;
	m_peakPopulation = 0;
	m_population     = 0;
	m_extinctions    = 0;
	m_perWhite       = 0.0;
	m_perGreen       = 0.0;
	m_perRed         = 0.0;
	m_perBlue        = 0.0;
	m_perLtBlue      = 0.0;
	m_ageRange       = 0;
	m_ageIntervals   = INTERVALS;
}


std::string CEnvStats::ToDays(uint32_t dwAge)
{
	double days = dwAge;

	days = (days / GENERATIONS) * 0.05;

    QString sFormat = QString::asprintf("%6.2f", days);
    return sFormat.toStdString();
}

uint32_t CEnvStats::ToGenerations(const std::string &szDays)
{
    return (uint32_t)((((atof(szDays.c_str()) * GENERATIONS)) / 0.05));
}

//////////////////////////////////////////////////////////
// Sample
// 
//
void CEnvStats::Sample(Environment& env)
{
	m_days += .05;

    m_population = env.m_biotList.size();

	if (m_population > m_peakPopulation)
		m_peakPopulation = m_population;

	// Determine color percentages...
    long     colorDistance[WHITE_LEAF + 2];
    for (int i = 0; i < WHITE_LEAF + 2; i++)
        colorDistance[i] = 0;
    uint32_t maxAge = 0;

	for (int j = 0; j < m_population; j++)
	{
		Biot* pBiot = env.m_biotList[j];

		if (pBiot->m_age > maxAge)
			maxAge = pBiot->m_age;

		for (int i = 0; i < WHITE_LEAF + 1; i++)
		{
			colorDistance[i] += pBiot->colorDistance[i];
			colorDistance[WHITE_LEAF + 1] += pBiot->colorDistance[i];
		}
	}

    m_ageRange = std::max(maxAge + 1, (uint32_t)(GENERATIONS * INTERVALS));
	m_ageIntervals = INTERVALS;

    for(int i = 0; i < INTERVALS; i++)
        m_ages[i] = 0;
    for(int i = 0; i < ENERGY_LEVELS; i++)
        m_energy[i] = 0;

	m_totalEnvArea = m_freeEnvArea = env.Area();

	for (int j = 0; j < m_population; j++)
	{
		Biot* pBiot = env.m_biotList[j];

		m_ages[(pBiot->m_age * m_ageIntervals) / m_ageRange]++;
        int level = pBiot->PercentEnergy() / (100.0 / ENERGY_LEVELS);
        if (level >= 0 and level < ENERGY_LEVELS)
            m_energy[level]++;

		m_freeEnvArea -= pBiot->Area();
	}

	m_perWhite  = (float) (100 * ((double) colorDistance[WHITE_LEAF] / (double) colorDistance[WHITE_LEAF + 1]));
	m_perGreen  = (float) (100 * ((double) colorDistance[GREEN_LEAF] / (double) colorDistance[WHITE_LEAF + 1]));
	m_perRed    = (float) (100 * ((double) colorDistance[RED_LEAF]   / (double) colorDistance[WHITE_LEAF + 1]));
	m_perBlue   = (float) (100 * ((double) colorDistance[BLUE_LEAF]  / (double) colorDistance[WHITE_LEAF + 1]));
	m_perLtBlue = (float) (100 * ((double) colorDistance[LBLUE_LEAF] / (double) colorDistance[WHITE_LEAF + 1]));


}


float CEnvStats::PercentUncoveredByBiots()
{
    //TRACE("Area free %f\n", (float) m_freeEnvArea / (float) m_totalEnvArea);
	return (float) m_freeEnvArea / (float) m_totalEnvArea;
}


void CEnvStats::NewSample()
{
	m_births         = 0;
	m_deaths         = 0;
	m_arrivals       = 0;
	m_departures     = 0;
	m_collisionCount = 0;
}


//////////////////////////////////////////////////////////
// GetDays
// 
//
std::string CEnvStats::GetDaysStr()
{
    QString sString = QString::asprintf("%6.2f", m_days);
    return sString.toStdString();
}


//////////////////////////////////////////////////////////
// GetPopulationStr
// 
//
std::string CEnvStats::GetPopulationStr()
{
    QString sString = QString::asprintf("%lu/%lu", m_population, m_peakPopulation);
    return sString.toStdString();
}


//////////////////////////////////////////////////////////
// GetExtinctionsStr
// 
//
std::string CEnvStats::GetExtinctionsStr()
{
    QString sString = QString::asprintf("%lu", m_extinctions);
    return sString.toStdString();
}


//////////////////////////////////////////////////////////
// Serialize
// 
//
/*
void CEnvStats::Serialize(QDataStream& ar)
{
	const long archVersion = 2;

	if (ar.IsLoading())
	{
		long version;
		ar >> version;
		ar >> m_collisionCount;
		ar >> m_births;
		ar >> m_deaths;
		ar >> m_arrivals;
		ar >> m_departures;
		ar >> m_days;
		ar >> m_peakPopulation;
		ar >> m_population;
		ar >> m_extinctions;
		ar >> m_perWhite;
		ar >> m_perGreen;
		ar >> m_perRed;
		ar >> m_perBlue;
		ar >> m_perLtBlue;
		ar >> m_ageRange;
		ar >> m_ageIntervals;
		ar.Read(m_ages, sizeof(m_ages));
		ar.Read(m_energy, sizeof(m_energy));
	}
	else
	{
		ar << archVersion;
		ar << m_collisionCount;
		ar << m_births;
		ar << m_deaths;
		ar << m_arrivals;
		ar << m_departures;
		ar << m_days;
		ar << m_peakPopulation;
		ar << m_population;
		ar << m_extinctions;
		ar << m_perWhite;
		ar << m_perGreen;
		ar << m_perRed;
		ar << m_perBlue;
		ar << m_perLtBlue;
		ar << m_ageRange;
		ar << m_ageIntervals;
		ar.Write(m_ages, sizeof(m_ages));
		ar.Write(m_energy, sizeof(m_energy));
	}
}

*/
//////////////////////////////////////////////////////////
// Serialize
// 
//
/*
void CEnvStatsList::Serialize(QDataStream& ar)
{
	if (ar.IsLoading())
	{
		RemoveAll();

		CEnvStats stats;
		int count;

		ar >> count;
		while (count > 0)
		{
			stats.Serialize(ar);
			AddTail(stats);
			count--;
		}
	}
	else
	{
		POSITION pos = GetHeadPosition();
		ar << GetCount();

		while (pos != NULL)
			GetNext(pos).Serialize(ar);
	}


}
*/
//////////////////////////////////////////////////////////////////////
// Environment Class
//
//Fox BEGIN
Environment::Environment()
{
    m_scene = nullptr;

    options.pens.clear();
	for (int i = 0; i <= MAX_LEAF; i++)
        options.pens.append(QPen());

    options.pens[GREEN_LEAF]      = QPen(QColor(0,255,0));
    options.pens[BLUE_LEAF]       = QPen(QColor(0,0,255));
    options.pens[RED_LEAF]        = QPen(QColor(255,0,0));
    options.pens[LBLUE_LEAF]      = QPen(QColor(0,255,255));
    options.pens[WHITE_LEAF]      = QPen(QColor(255,255,255));
    options.pens[DARK_GREEN_LEAF] = QPen(QColor(0, 128, 0));
    options.pens[DARK_BLUE_LEAF]  = QPen(QColor(0, 0, 128));
    options.pens[DARK_RED_LEAF]   = QPen(QColor(128, 0, 0));
    options.pens[DARK_LBLUE_LEAF] = QPen(QColor(0, 128, 128));
    options.pens[GREY_LEAF]       = QPen(QColor(128,128,128));
    options.pens[YELLOW_LEAF]     = QPen(QColor(255,255,0));
    options.pens[BLACK_LEAF]      = QPen(QColor(0,0,0));
    options.pens[PURPLE_LEAF]     = QPen(QColor(255,0,255));

	Clear();

//	m_pMagnifyWnd  = NULL;
//	m_pEnvStatsWnd = NULL;
}
//Fox END

//////////////////////////////////////////////////////////////////////
// Environment Destructor
//
//  
//
Environment::~Environment(void)
{
/*	for (int i = 0; i <= MAX_LEAF; i++)
		if (options.hPen[i] != NULL)
			DeleteObject(options.hPen[i]);
*/
}


//////////////////////////////////////////////////////////////////////
// Clear
//
//Fox BEGIN
void Environment::Clear()
{
	options.effectiveLength[GREEN_LEAF] = 1;
	options.effectiveLength[BLUE_LEAF]  = 2;
	options.effectiveLength[RED_LEAF]   = 1;
	options.effectiveLength[WHITE_LEAF] = 1;
	options.effectiveLength[LBLUE_LEAF] = 1;
	
	// You have / he has
	options.leafContact[GREEN_LEAF][GREEN_LEAF ] = CONTACT_IGNORE;
	options.leafContact[GREEN_LEAF][BLUE_LEAF  ] = CONTACT_IGNORE;
	options.leafContact[GREEN_LEAF][WHITE_LEAF ] = CONTACT_IGNORE;
	options.leafContact[GREEN_LEAF][RED_LEAF   ] = CONTACT_EATEN;
	options.leafContact[GREEN_LEAF][LBLUE_LEAF ] = CONTACT_IGNORE; 

	options.leafContact[BLUE_LEAF][BLUE_LEAF  ] = CONTACT_IGNORE;
	options.leafContact[BLUE_LEAF][GREEN_LEAF ] = CONTACT_IGNORE;
	options.leafContact[BLUE_LEAF][RED_LEAF   ] = CONTACT_DEFEND;
	options.leafContact[BLUE_LEAF][WHITE_LEAF ] = CONTACT_IGNORE;
	options.leafContact[BLUE_LEAF][LBLUE_LEAF ] = CONTACT_IGNORE;

	options.leafContact[RED_LEAF][RED_LEAF   ]   = CONTACT_ATTACK;
	options.leafContact[RED_LEAF][GREEN_LEAF ]   = CONTACT_EAT;
	options.leafContact[RED_LEAF][BLUE_LEAF  ]   = CONTACT_DEFENDED;
	options.leafContact[RED_LEAF][WHITE_LEAF ]   = CONTACT_DESTROY;
	options.leafContact[RED_LEAF][LBLUE_LEAF ]   = CONTACT_DESTROY;

	options.leafContact[WHITE_LEAF][RED_LEAF  ]  = CONTACT_DESTROYED;
	options.leafContact[WHITE_LEAF][GREEN_LEAF]  = CONTACT_IGNORE;
	options.leafContact[WHITE_LEAF][BLUE_LEAF ]  = CONTACT_IGNORE;
	options.leafContact[WHITE_LEAF][WHITE_LEAF]  = CONTACT_IGNORE;
	options.leafContact[WHITE_LEAF][LBLUE_LEAF]  = CONTACT_IGNORE; 

	options.leafContact[LBLUE_LEAF][RED_LEAF  ]  = CONTACT_DESTROYED;
	options.leafContact[LBLUE_LEAF][GREEN_LEAF]  = CONTACT_IGNORE;
	options.leafContact[LBLUE_LEAF][BLUE_LEAF ]  = CONTACT_IGNORE;
	options.leafContact[LBLUE_LEAF][WHITE_LEAF]  = CONTACT_IGNORE;
	options.leafContact[LBLUE_LEAF][LBLUE_LEAF]  = CONTACT_IGNORE; 

	options.leafMass[RED_LEAF]    = 1;
	options.leafMass[BLUE_LEAF]   = 2;
	options.leafMass[WHITE_LEAF]  = 1;
	options.leafMass[GREEN_LEAF]  = 4;
	options.leafMass[LBLUE_LEAF]  = 1;

	options.newType[RED_LEAF]    = RED_LEAF;
	options.newType[BLUE_LEAF]   = BLUE_LEAF;
	options.newType[WHITE_LEAF]  = WHITE_LEAF;
	options.newType[GREEN_LEAF]  = YELLOW_LEAF;
	options.newType[LBLUE_LEAF]  = LBLUE_LEAF;

	options.m_leafEnergy = 2;
	options.regenCost  = 200;
	options.regenTime  = 0x00000007;

	options.startEnergy = 400 * 8;  

	options.friction = (float) 0.005;

	// Set up sides
	side[0] = (Side*) &leftSide;
	side[1] = (Side*) &rightSide;
	side[2] = (Side*) &topSide;
	side[3] = (Side*) &bottomSide;

	// Save the state of these
	m_uniqueID  = 0;

	// Ini File parameters
	options.chance              = 12;
    options.bSoundOn            = true;
	options.nSexual             = 3;
	options.startNew            = 1;
	options.m_initialPopulation = 20;
    options.bParentAttack       = true;
    options.bSiblingsAttack     = true;
    options.bBarrier            = true;
    options.bMouse              = true;
    bNetworkSettingsChange = false;

	// Don't save the state of these
	// They maintain a global scratch pad for drawing
/*	m_hScreenDC       = NULL;
	m_hMemPadDC       = NULL;
	m_hMemoryDC       = NULL;
	m_hBitPad         = NULL;
	m_hOldPad         = NULL;
*/
    m_maxBitPadWidth  = 0;
	m_maxBitPadHeight = 0;


	// We are not blocked to start with
    m_bBlocked = false;

	options.m_sound.SetScheme("PL", "Primordial Life");

	options.m_nSick = 200;

    m_bIsSelected = false;

	m_stats.Clear();
    m_statsList.clear();

    m_dwTicks      = QDateTime::currentMSecsSinceEpoch();

}
//Fox END

//////////////////////////////////////////////////////////////////////
// GetBitPad - allocates a bitmap, compatible with hScreenDC.
// Note: Ensures the scratch pad area is no larger than needed.
//
//
/*
HDC Environment::GetBitPadDC(int width, int height)
{
	if (width  > m_maxBitPadWidth ||
		height > m_maxBitPadHeight ||
		m_hBitPad == NULL)
	{
		if (width > m_maxBitPadWidth)
			m_maxBitPadWidth = NextSize(width);

	    if (height > m_maxBitPadHeight)
			m_maxBitPadHeight = NextSize(height);

		m_hBitPad = CreateCompatibleBitmap(m_hScreenDC, m_maxBitPadWidth, m_maxBitPadHeight);

		ASSERT(m_hBitPad);

		if (m_hMemPadDC == NULL)
		{
			if (m_hBitPad)
			{
				m_hMemPadDC = CreateCompatibleDC(m_hScreenDC);
				ASSERT(m_hMemPadDC);
			}
		}
		else
		{
			// Disassociate and free old bitmap
			// SelectObject will return the previous m_hBitPad
			VERIFY(DeleteObject(SelectObject(m_hMemPadDC, (HGDIOBJ) m_hOldPad)));
	    }

		// Select new one
		m_hOldPad = (HBITMAP) SelectObject(m_hMemPadDC, m_hBitPad);
	}
	return m_hMemPadDC;
}
*/

//////////////////////////////////////////////////////////////////////
// FreeBitPadDC
//
//
/*
void Environment::FreeBitPadDC()
{
	if (m_hMemPadDC)
	{
		SelectObject(m_hMemPadDC, m_hOldPad);
		VERIFY(DeleteObject(m_hBitPad));

		VERIFY(DeleteDC(m_hMemPadDC));

		m_hBitPad         = NULL;
		m_hMemPadDC       = NULL;
		m_hOldPad         = NULL;

		// We preserve the size of m_maxBitPadWidth and m_maxBitPadHeight
		// to help reduce the number of re-allocations
    }
}

*/
//////////////////////////////////////////////////////////////////////
// PlayResource
//
//
void Environment::PlayResource(const std::string &szSound, bool bSync)
{
/*	if (options.bSoundOn && !AfxGetPLife().IsSmall())
	{
        std::string sSound = options.m_sound.GetPath(szSound);

		if (!sSound.IsEmpty())
			sndPlaySound(sSound, SND_FILENAME | SND_NODEFAULT | ((bSync)?SND_SYNC:SND_ASYNC));
    }*/
}


//////////////////////////////////////////////////////////////////////
// CreateBiots
//
//
void Environment::CreateBiots(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
    Biot* pNew = nullptr;

//	m_sort.SetIncrementalSort(false);
	for (long lIndex = 0; lIndex < options.m_initialPopulation; lIndex++)
	{
		if ((pNew = new Biot(*this)) != NULL)
		{
            m_biotList.append(pNew);
			pNew->RandomCreate(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);
			m_sort.Add(pNew);
//			m_sort.TraceDebug();
		}
	}
//	m_sort.SortAll();
	m_sort.TraceDebug();
//	m_sort.SetIncrementalSort(true);

//	BRectSortPos pos;

//	pos.FindRectsInPoint(200, 200);
//	pos.FindRectsInPoint(100, 100);

//	while (m_sort.IterateRects(pos) != NULL);
//	m_sort.
//	pNew = new Biot(*this);
//	pNew = m_biotList[0];
//	m_sort.Add(pNew);

}
     

//////////////////////////////////////////////////
// OnOpen
//
// Loads up biots, or creates them
//
//
/*
void Environment::OnOpen(CScrollView* pView)
{ 
	// Establish our boundaries
	leftSide.SetSide(this);
	rightSide.SetSide(this);
	topSide.SetSide(this);
	bottomSide.SetSide(this);

	options.maxLineSegments      = (MAX_GENES / MAX_SYMMETRY);

	sock.StartSession(pView->GetSafeHwnd(), this);
	sock.Listen();

	// Create our display context
	CDC* pScreenDC = pView->GetDC();

	m_hScreenDC = pScreenDC->GetSafeHdc();
	m_hMemoryDC = ::CreateCompatibleDC(m_hScreenDC);

	// Create our biots
	for (int i = 0; i < m_biotList.GetSize(); i++)
		m_biotList[i]->OnOpen();

	VERIFY(::DeleteDC(m_hMemoryDC));
	VERIFY(pView->ReleaseDC(pScreenDC));
	m_hScreenDC = NULL;
	
	pView->Invalidate();
}
*/

//////////////////////////////////////////////////
// OnNew
//
// Loads up biots, or creates them
//
//


void Environment::OnNew(QGraphicsScene &scene,
                        QRect worldRect, int population, int seed,
						int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
    m_scene = &scene;
    m_scene->setBackgroundBrush(QBrush(QColor(0,0,0)));
	Clear();

	options.startNew = 1;
	Set(&worldRect);

	options.m_initialPopulation  = population;
	options.maxLineSegments      = (MAX_GENES / MAX_SYMMETRY);

	m_orginalSeed = seed;

	RandSeed(m_orginalSeed);

	// Establish our boundaries
	leftSide.SetSide(this);
	rightSide.SetSide(this);
	topSide.SetSide(this);
	bottomSide.SetSide(this);
/*
	// Are we being displayed in the small window?
	if (AfxGetPLife().IsSmall())
	{
		options.startNew            = 1;
		options.m_initialPopulation = 4;
		options.maxLineSegments     = 4;
	}
	else
	{
		sock.StartSession(pView->GetSafeHwnd(), this);
		sock.Listen();
    }

	// Create our display context
	CDC* pScreenDC = pView->GetDC();

	if (m_topBitmap == (HBITMAP) NULL)
	{
		HBITMAP bitmap_handle = ::LoadBmpResource(pScreenDC, MAKEINTRESOURCE(IDB_TOP));

		// Attach this bitmap to our CBitmap object.
		// This call fails if bitmap_handle is NULL.
		// Note that the CBitmap destructor will destroy the bitmap.
		m_topBitmap.Attach(bitmap_handle);
		m_topBitmap.GetBitmap(&m_topBm);
	}

	if (m_bottomBitmap == (HBITMAP) NULL)
	{
		HBITMAP bitmap_handle = ::LoadBmpResource(pScreenDC, MAKEINTRESOURCE(IDB_BOTTOM));

		// Attach this bitmap to our CBitmap object.
		// This call fails if bitmap_handle is NULL.
		// Note that the CBitmap destructor will destroy the bitmap.
		m_bottomBitmap.Attach(bitmap_handle);
		m_bottomBitmap.GetBitmap(&m_bottomBm);
	}

	m_hScreenDC = pScreenDC->GetSafeHdc();
	m_hMemoryDC = ::CreateCompatibleDC(m_hScreenDC);
*/
	// Create our biots
	CreateBiots(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);
/*
	FreeBitPadDC();
	VERIFY(::DeleteDC(m_hMemoryDC));
	VERIFY(pView->ReleaseDC(pScreenDC));
	m_hScreenDC = NULL;	
*/
	options.m_generation = 0;

	m_stats.Sample(*this);
    m_statsList.append(&m_stats);
	m_stats.NewSample();
}


//////////////////////////////////////////////////////////////////////
// Skip
//
//

void Environment::Skip()
{
//	ASSERT(0);
    int i = 0;
    Biot* pBiot = nullptr;

	if (m_bBlocked)
		return;

//	bool bSample = ((options.m_generation & 0x000001FF) == 0x00000000);

    m_bBlocked = true;


/*	if ((generation & 0x000001FF) == 0x00000000)
	{
		sock.Listen();
		sock.ConnectAll();

        bSoundOn = true;
	}
	else
	{
		if (bNetworkSettingsChange)
		{
			sock.Listen();
			sock.ConnectAll();
            bNetworkSettingsChange = false;
		}
	}
*/
	// Create our display context
/*	CDC* pScreenDC = pView->GetDC();
	pView->OnPrepareDC(pScreenDC);

	m_hScreenDC = pScreenDC->GetSafeHdc();
	m_hMemoryDC = ::CreateCompatibleDC(m_hScreenDC);
*/

	// Write a biot out if required
/*	if (sock.WriteAll())
	{
		// Import biots
        QDataStream* pPost;
		sock.LockAll();
		for (i = 0; i < 4; i++)
		{
			pPost = side[i]->Import();
			while (pPost)
			{
				pBiot = new Biot(*this);
				if (pBiot)
				{
					if (!pBiot->Serialize(*pPost))
					{
						delete pBiot;
					}
					else
					{
						AddBiot(pBiot);
						side[i]->AdjustBiot(*pBiot);
					}

				}
				delete pPost;
				pPost = side[i]->Import();
			}
		}
		sock.UnlockAll();
	}
*/

    int64_t dwTicks = QDateTime::currentMSecsSinceEpoch();
	// Process all the biots now
    if ((QDateTime::currentMSecsSinceEpoch() - dwTicks) < 200)
	{
		pBiot = m_biotList.NextBiot();

		if (m_biotList.Looped())
		{
			// Another approach would store the delta time between each loop
			// and if it was under a certain threshold, it would add a sleep
			// statement.
            if ((QDateTime::currentMSecsSinceEpoch() - m_dwTicks) < 25)
                QThread::msleep(8);

            m_dwTicks = QDateTime::currentMSecsSinceEpoch();

			if ((options.m_generation & CEnvStats::SAMPLE_TIME) == CEnvStats::SAMPLE_TIME)
			{
				m_stats.Sample(*this);
                m_statsList.append(&m_stats);
				m_stats.NewSample();
                if (m_statsList.size() > 100)
                    m_statsList.removeFirst();
/*
                AfxMainFrame().UpdateStatusBar();

				if (m_pEnvStatsWnd)
					m_pEnvStatsWnd->PaintNow();
*/
				if (m_stats.PercentUncoveredByBiots() < 0.50)
				{
                    m_biotList[Integer(m_biotList.size())]->m_nSick = options.m_nSick;
				}

			}

			options.m_generation++;

			if (m_bIsSelected)
			{
                /*Biot* pBiot = GetSelectedBiot();
				if (m_pMagnifyWnd)
					m_pMagnifyWnd->PaintNow(pBiot);
				m_editor.UpdateBiot(pBiot);
*/
                m_bIsSelected = (pBiot != NULL);
			}
		}

		// Is the biot present?
		if (pBiot)
		{
			BRect origPos(pBiot);

			if (!pBiot->Move())
			{
				m_sort.Remove(pBiot);
				m_biotList.RemoveBiot();
				m_stats.m_deaths++;
			}
			else
			{
				m_sort.Move(pBiot, &origPos);
				for (i = 0; i < 4; i++)
				{
                    /*if (pBiot->IsContainedBy(*side[i]))
					{
                        sock.LockAll();
						if (side[i]->Export(pBiot))
						{
							pBiot->Erase();
							m_sort.Remove(pBiot);
							m_biotList.RemoveBiot();
						}
						else
						{	
							pBiot->Reject(i);
						}
						sock.UnlockAll();
						i = 4;
                    }*/
				}
			}
		}
	}
/*
	if (m_biotList.GetSize() < 4)
	{
		if (m_biotList.GetSize() == 0 &&
			!sock.ConnectionActive())
		{  
			m_stats.m_extinctions++;
            PlayResource("PL.Extinction", true);
			Sleep(1000);
			PlayResource("PL.Start");
			CreateBiots(0, 0, 0);
		}
    }

	// Let the window paint now
	FreeBitPadDC();

	VERIFY(::DeleteDC(m_hMemoryDC));
	VERIFY(pView->ReleaseDC(pScreenDC));
	m_hScreenDC = NULL;
*/
    m_bBlocked = false;
}

 
//////////////////////////////////////////////////////////////////////
// OnStop
//
//
void Environment::OnStop()
{
    //sock.Disconnect();
}

//////////////////////////////////////////////////////////////////////
// DeleteContents
//
//
void Environment::DeleteContents()
{
	m_sort.FreeAll();
	m_biotList.FreeAll();
}


//////////////////////////////////////////////////////////////////////
// AddBiot
//
//
void Environment::AddBiot(Biot* pNewBiot)
{
	if (pNewBiot)
	{
		m_sort.Add(pNewBiot);
        m_biotList.append(pNewBiot);
		m_sort.TraceDebug();
	}
}


//////////////////////////////////////////////////////////////////////
// Paint
//
// How do we know if the biot has an bitmap filled in?
//
/*void Environment::Paint(HDC hDC, RECT* pRect)
{
	BRect c(pRect);

	m_hScreenDC = hDC;
	m_hMemoryDC = CreateCompatibleDC(m_hScreenDC);
	for (int i = 0; i < m_biotList.GetSize(); i++)
		if (c.Touches(*m_biotList[i]) && 
			m_biotList[i]->m_bitmapWidth > 0 && 
			m_biotList[i]->m_bitmapHeight > 0)
			m_biotList[i]->Draw();

	FreeBitPadDC();
	VERIFY(DeleteDC(m_hMemoryDC));
}*/
//////////////////////////////////////////////////////////////////////
// Paint
//
// How do we know if the biot has an bitmap filled in?
//
/*
void Environment::Paint(CDC* pDC, CRect& rect)
{
	BRect c((RECT*)&rect);

	m_hScreenDC = pDC->m_hDC;
	m_hMemoryDC = CreateCompatibleDC(m_hScreenDC);

	CDC dcImage;

	dcImage.Attach(m_hMemoryDC);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_topBitmap);
	int width = 0;
	while (width < rect.Width())
	{
		pDC->BitBlt(width, 0, m_topBm.bmWidth, m_topBm.bmHeight, &dcImage, 0, 0, SRCCOPY);
		width += m_topBm.bmWidth;
	}
	dcImage.SelectObject(pOldBitmap);

	int height = m_topBm.bmHeight;
	float scale =  (100.0f * 10.0f) / (float)(rect.Height() - (m_topBm.bmHeight + m_bottomBm.bmHeight));
	float d = 0;
	while(height < m_bottom)
	{
		CRect rect(0, height, rect.Width(), height + 20);
//		TRACE("COLOR = %d at height %d\n", 100 - d, height);
		pDC->FillSolidRect(rect, RGB(10, 10, 100 - (int) d));
		height += 10;
		d += scale;
	}

	pOldBitmap = dcImage.SelectObject(&m_bottomBitmap);
	width = 0;
	int bottom = rect.Height() - m_bottomBm.bmHeight;
	while (width < rect.Width())
	{
		pDC->BitBlt(width, bottom, m_bottomBm.bmWidth, m_bottomBm.bmHeight, &dcImage, 0, 0, SRCCOPY);
		width += m_bottomBm.bmWidth;
	}
	dcImage.SelectObject(pOldBitmap);


	dcImage.Detach();

	for (int i = 0; i < m_biotList.GetSize(); i++)
		if (c.Touches(*m_biotList[i]) && 
			m_biotList[i]->m_bitmapWidth > 0 && 
			m_biotList[i]->m_bitmapHeight > 0)
			m_biotList[i]->Draw();

	FreeBitPadDC();
	VERIFY(DeleteDC(m_hMemoryDC));
}


//////////////////////////////////////////////////////////////////////
// OpenEnvironmentStatistics
//
//
void Environment::OpenEnvironmentStatistics(CWnd* pWnd)
{
	if (!m_pEnvStatsWnd)
		CEnvStatsWnd::CreateWnd(this, pWnd, &m_pEnvStatsWnd);

	if (m_pEnvStatsWnd)
		m_pEnvStatsWnd->PaintNow();
}


/////////////////////////////////////////////////////////////////////
// MagnifyBiot
//
//
void Environment::MagnifyBiot(CDC& dc, Biot* pBiot, CRect& rect)
{
	float fWidth  = (float) rect.Width() / (float) pBiot->Width();
	float fHeight = (float) rect.Height() / (float) pBiot->Height();
	int width;
	int height;

	if (fWidth > fHeight)
	{
		width  = (int)(fHeight * pBiot->Width());
		height = (int)(fHeight * pBiot->Height());
	}
	else
	{
		width  = (int)(fWidth * pBiot->Width());
		height = (int)(fWidth * pBiot->Height());
	}

		
	HDC hMemoryDC = ::CreateCompatibleDC(dc);

	CDC mem;

	mem.Attach(hMemoryDC);
	HBITMAP hOld = (HBITMAP) SelectObject(hMemoryDC, pBiot->m_hBitmap);

	CRect biot;
	biot.left   = (rect.Width() - width) / 2;
	biot.top    = (rect.Height() - height) / 2;
	biot.right  = biot.left + width;
	biot.bottom = biot.top + height;

	dc.StretchBlt(biot.left, biot.top, biot.Width(), biot.Height(),
		&mem, 0, 0, pBiot->Width(), pBiot->Height(), SRCCOPY);

	CRect black(0, 0, biot.left, rect.bottom);
		dc.FillSolidRect(black, RGB(0, 0, 0));

	black.SetRect(biot.right, 0, rect.right, rect.bottom);
	dc.FillSolidRect(black, RGB(0, 0, 0));
	
	black.SetRect(biot.left, 0, biot.right, biot.top);
	dc.FillSolidRect(black, RGB(0, 0, 0));

	black.SetRect(biot.left, biot.bottom, biot.right, rect.bottom);
	dc.FillSolidRect(black, RGB(0, 0, 0));

	SelectObject(hMemoryDC, hOld);
}

*/
Biot* Environment::HitCheck(Biot *me, BRectSortPos& pos)
{
	Biot* pBiot;
	do
	{
		pBiot = dynamic_cast<Biot*>(m_sort.IterateRects(pos));
	}
	while (me == pBiot && me != NULL);
	return pBiot;
}

/////////////////////////////////////////////////////////////////////
// BiotOperation
//
//
/*
void Environment::BiotOperation(CScrollView* pView, int x, int y, int operation)
{
	Biot* pBiot = NULL;

    m_bBlocked = true;

	// Create our display context
	CDC* pScreenDC = pView->GetDC();
	pView->OnPrepareDC(pScreenDC);

	m_hScreenDC = pScreenDC->GetSafeHdc();
	m_hMemoryDC = ::CreateCompatibleDC(m_hScreenDC);


	if (m_bIsSelected)
	{
		pBiot = m_biotList.FindBiotByID(m_selectedId);

        m_bIsSelected = false;
		m_selectedId  = 0;

		if (pBiot)
		{
			pBiot->Erase();
			pBiot->PrepareDraw(Biot::REDRAW);
			pBiot->Draw();
		}
	}

	BRectSortPos pos;
	pos.FindRectsInPoint(x, y);
	Biot* pNewBiot = dynamic_cast<Biot*>(m_sort.IterateRects(pos));

	if (pNewBiot == NULL)
	{
		if (operation == IDC_RELOCATE && pBiot)
		{
			pBiot->Erase();
			pBiot->origin.x = x;
			pBiot->origin.y = y;
			pBiot->SetScreenRect();

			pBiot->vector.setX(x);
			pBiot->vector.setY(y);

			pBiot->SetErasePosition();
			pBiot->Draw();
		}

		if (operation == IDC_LOAD)
		{
			LoadBiot(x, y);
		}

		VERIFY(::DeleteDC(m_hMemoryDC));
		VERIFY(pView->ReleaseDC(pScreenDC));
		m_hScreenDC = NULL;

		if (m_pMagnifyWnd)
			m_pMagnifyWnd->PostMessage(WM_CLOSE);

        m_bBlocked = false;
		return;
	}

	pBiot         = pNewBiot;//m_biotList[nBiot];
	m_operation   = operation;
	m_selectedId  = pBiot->m_Id;
    m_bIsSelected = true;

	if (pBiot)
	{
		pBiot->Erase();
		pBiot->PrepareDraw(Biot::REDRAW);
		pBiot->Draw();
	}

	switch(operation)
	{
		case IDC_TERMINATE:
		{
			PlayResource("PL.Terminate");
			pBiot->newType = YELLOW_LEAF;
			pBiot->m_age = pBiot->m_maxAge;
		}
		break;

		case IDC_EDIT:
			PlayResource("PL.Edit");

			pBiot->Mutate(100);
			pBiot->newType = WHITE_LEAF;
			m_sort.Move(pBiot);

//			if (!m_editor.GetSafeHwnd())
//				m_editor.Create(IDD_EDITOR, pView);
//			m_editor.UpdateBiot(pBiot);
//			m_editor.ShowWindow(SW_SHOWNORMAL);
//			m_editor.SetActiveWindow();
			break;

		case IDC_FEED:
			PlayResource("PL.Feed");
			pBiot->energy += pBiot->childBaseEnergy;
			pBiot->newType = GREEN_LEAF;
			break;

		case IDC_MAGNIFY:
			if (!m_pMagnifyWnd)
				CMagnifyWnd::CreateWnd(this, pView, &m_pMagnifyWnd);

			if (m_pMagnifyWnd)
				m_pMagnifyWnd->PaintNow(pBiot);

			break;

		case IDC_SAVE:
			SaveBiot(pBiot);
			break;

		case IDC_LOAD:
			break;

		case IDC_CURE:
			if (pBiot->m_nSick == 0)
			{
				PlayResource("PL.TooOld");
				pBiot->newType = PURPLE_LEAF;
				pBiot->m_nSick = 200;
			}
			else
			{
				pBiot->newType = -1;
				pBiot->m_nSick = 0;
			}
			break;
	}

	if (operation == IDC_TERMINATE ||
		operation == IDC_FEED ||
		operation == IDC_EDIT ||
		operation == IDC_CURE)
	{
		pBiot->Erase();
		if (operation == IDC_EDIT)
			pBiot->SetScreenRect();

//		pBiot->newType = WHITE_LEAF;
		pBiot->PrepareDraw(Biot::REDRAW);

		for (int i = 0; i < 2; i++)
		{
			pBiot->Draw();
			Sleep(150);
			pBiot->Erase();
			Sleep(150);
		}
		pBiot->newType = -1;
		pBiot->PrepareDraw(Biot::REDRAW);
		pBiot->Draw();
	}

	VERIFY(::DeleteDC(m_hMemoryDC));
	VERIFY(pView->ReleaseDC(pScreenDC));
	m_hScreenDC = NULL;

    m_bBlocked = false;
}
*/
/*
///////////////////////////////////////////////////////////////
// GetDefaultSettings
//
// Reads the registry to retrieve default settings
//
void Environment::GetDefaultSettings()
{
	CKeyRegistry& registry = AfxUserRegistry();

	options.bSoundOn = registry.GetValue(szSound, options.bSoundOn);
    if (options.bSoundOn != false)
        options.bSoundOn = true;

	if (!AfxIsNT() || AfxGetPLife().GetView() != CPrimCmdLine::SHOW_SAVER_WINDOW)
	{
		options.bMouse = registry.GetValue(szMouse, options.bMouse);
        if (options.bMouse != false)
            options.bMouse = true;
	}
	else
        options.bMouse = false;

	options.nSexual = registry.GetValue(szAsexual, options.nSexual);
	if (options.nSexual < 1 ||
		options.nSexual > 3)
		options.nSexual = 3;	

	options.bSiblingsAttack = registry.GetValue(szSiblingsAttack, options.bSiblingsAttack);
    if (options.bSiblingsAttack != false)
        options.bSiblingsAttack = true;

	options.bParentAttack = registry.GetValue(szParentAttack, options.bParentAttack);
    if (options.bParentAttack != false)
        options.bParentAttack = true;
		

	options.chance = registry.GetValue(szChance, options.chance);
	if (options.chance < 0)
		options.chance = 41;

	options.m_initialPopulation    = registry.GetValue(szPopulation, options.m_initialPopulation);
	if (options.m_initialPopulation <= 0)
		options.m_initialPopulation = 1;
	options.regenCost            = registry.GetValue(szRegenCost,  options.regenCost);
	options.regenTime            = registry.GetValue(szRegenTime,  options.regenTime);
	options.m_leafEnergy         = registry.GetValue(szGreenValue, options.m_leafEnergy);

	options.SetFriction(registry.GetValue(szFriction, options.GetFriction()));

	options.startNew             = registry.GetValue(szStartNew, options.startNew);
	if (options.startNew < 0 || 
		options.startNew > 1)
		options.startNew = 1;

	sock.GetDefaultSettings();
}     


///////////////////////////////////////////////////////////////
// SetDefaultSettings
//
// Writes the registry to retrieve default settings
//

void Environment::SetDefaultSettings()
{
	CRegistry& registry = AfxUserRegistry();
	
	registry.SetValue(szSound, options.bSoundOn);
	registry.SetValue(szAsexual, options.nSexual);
	registry.SetValue(szMouse, options.bMouse);	

	registry.SetValue(szSiblingsAttack, options.bSiblingsAttack);
	registry.SetValue(szParentAttack, options.bParentAttack);
  
	registry.SetValue(szChance, options.chance);
	registry.SetValue(szPopulation, options.m_initialPopulation);

	registry.SetValue(szRegenCost, options.regenCost);
	registry.SetValue(szRegenTime, options.regenTime);

	registry.SetValue(szGreenValue, options.m_leafEnergy);
  
	registry.SetValue(szStartNew, options.startNew);

	registry.SetValue(szFriction, options.GetFriction());

	sock.SetDefaultSettings();
}

*/

///////////////////////////////////////////////////////////////
// Serialize
//
//Fox BEGIN
/*
void Environment::Serialize(QDataStream& ar)
{
	const BYTE archiveVersion = 13;
	CPostData data;

	if (ar.IsStoring())
	{
		ar << archiveVersion;
		ar << options.m_generation;
		ar << m_uniqueID;
		ar << options.bSoundOn;
		ar << options.bMouse;
		ar << options.nSexual;		
		ar << options.bSiblingsAttack;
		ar << options.bParentAttack;		
		ar << options.chance;
		ar << options.regenCost;
		ar << options.regenTime;
		ar << options.m_leafEnergy;
		ar << options.friction;

		// The starting parameters
		ar << m_orginalSeed;
		ar << options.m_initialPopulation;
		BRect::Serialize(ar);

		// Random state
		Randomizer::Serialize(ar);

		// Stats
		m_stats.Serialize(ar);
		m_statsList.Serialize(ar);

//		sock.Serialize(ar);
		m_biotList.Serialize(ar, *this);
	}
	else
	{
		BYTE version;
		// Check version
		ar >> version;
		if (version != archiveVersion)
            AfxThrowArchiveException(QDataStreamException::badSchema, _T("Environment"));

		ar >> options.m_generation;

		ar >> m_uniqueID;

		ar >> options.bSoundOn;
        if (options.bSoundOn != false)
            options.bSoundOn = true;

		ar >> options.bMouse;

		if (AfxIsNT() && AfxGetPLife().GetView() == CPrimCmdLine::SHOW_SAVER_WINDOW)
            options.bMouse = false;

		ar >> options.nSexual;
		if (options.nSexual < 1 ||
			options.nSexual > 3)
			options.nSexual = 3;		

		ar >> options.bSiblingsAttack;
        if (options.bSiblingsAttack != false)
            options.bSiblingsAttack = true;

		ar >> options.bParentAttack;
        if (options.bParentAttack != false)
            options.bParentAttack = true;

		ar >> options.chance;
		if (options.chance < 0)
			options.chance = 41;

		ar >> options.regenCost;
		ar >> options.regenTime;
		ar >> options.m_leafEnergy;
		ar >> options.friction;

		// The starting parameters
		ar >> m_orginalSeed;
		ar >> options.m_initialPopulation;
		BRect::Serialize(ar);

		// Random state
		Randomizer::Serialize(ar);

		// Stats
		m_stats.Serialize(ar);
		m_statsList.Serialize(ar);

//		sock.Serialize(ar);
		m_biotList.Serialize(ar, *this);
	}
}     
//Fox END
*/
//////////////////////////////////////////////////////
// SaveBiot
//
//
/*
void Environment::SaveBiot(Biot* pBiot)
{
	static char BASED_CODE szFilter[] = _T("Biot Files (*.bot)|*.bot|");

    CFileDialog dlg(false, _T("bot"), pBiot->GetFullName(),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		szFilter);

    std::string sDir;
	AfxCommonRegistry().GetString(_T("biot.path.save"), sDir);

	if (sDir.IsEmpty())
	{
		AfxCommonRegistry().GetString(_T("path"), sDir);
		sDir += _T("\\Biots");
	}

	dlg.m_ofn.lpstrInitialDir = sDir;
	dlg.m_ofn.lpstrTitle = _T("Capture biot to a file:");

	if (dlg.DoModal() == IDOK)
	{
		try {
			CFile file(dlg.GetPathName(), CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite);
            QDataStream ar(&file, QDataStream::store);
		    pBiot->Serialize(ar);
			ar.Close();
			file.Flush();
			file.Close();

            std::string sPath = dlg.GetPathName();
			int nPosition = sPath.ReverseFind('\\');
			if (nPosition != -1)
				AfxCommonRegistry().SetString(_T("biot.path.save"), sPath.Left(nPosition));
		}
		catch (CException* pEx)
		{
	        TCHAR    szCause[255];
	        pEx->GetErrorMessage(szCause, 255);
		
            std::string sError = _T("Unable to save file:\n");
			sError += szCause;
			AfxMessageBox(sError);
			pEx->Delete();
		}
	}
}
*/

//////////////////////////////////////////////////////
// LoadBiot
//
//
/*
void Environment::LoadBiot(int x, int y)
{
	static char BASED_CODE szFilter[] = _T("Biot Files (*.bot)|*.bot|");

    CFileDialog dlg(true, _T("bot"), NULL,
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		szFilter);

    std::string sDir;
	AfxCommonRegistry().GetString(_T("biot.path.load"), sDir);

	if (sDir.IsEmpty())
	{
		AfxCommonRegistry().GetString(_T("path"), sDir);
		sDir += _T("\\Biots");
	}

	dlg.m_ofn.lpstrInitialDir = sDir;
	dlg.m_ofn.lpstrTitle = _T("Release biot from a file:");

	if(dlg.DoModal() == IDOK)
	{
		try {
			CFile file(dlg.GetPathName(), CFile::modeRead | CFile::shareDenyWrite);

            QDataStream ar(&file, QDataStream::load);

			Biot* pBiot = new Biot(*this);

			if (!pBiot)
				return;

			pBiot->Serialize(ar);

			AddBiot(pBiot);

			pBiot->origin.x = x;
			pBiot->origin.y = y;
			pBiot->SetScreenRect();

			pBiot->vector.setX(x);
			pBiot->vector.setY(y);

			pBiot->SetErasePosition();
			pBiot->OnOpen();

			pBiot->Draw();

			m_stats.m_arrivals++;

			ar.Close();
			file.Close();

            std::string sPath = dlg.GetPathName();
			int nPosition = sPath.ReverseFind('\\');
			if (nPosition != -1)
				AfxCommonRegistry().SetString(_T("biot.path.load"), sPath.Left(nPosition));
		}
		catch (CException* pEx)
		{
	        TCHAR    szCause[255];
	        pEx->GetErrorMessage(szCause, 255);
		
            std::string sError = _T("Unable to load file:\n");
			sError += szCause;
			AfxMessageBox(sError);
			pEx->Delete();
		}
	}
}
*/

