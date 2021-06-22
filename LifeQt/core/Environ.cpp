 ///////////////////////////////////////////////////////////////////////////////
//
// Environment
//
// This class defines the environment of the biots
//
// FOX WORKED ON THIS ENTIRE DOC(ANYTHING WITH 'options')

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <QDateTime>
#include <QThread>
//#include "evolve.h"
#include "Environ.h"
#include "Biots.h"

using namespace rapidjson;

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

void CBiotList::SerializeJson(class Environment &env, rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_bLooped", m_bLooped, allocator);

    Value biotsJson(kArrayType);
    for(int i=0; i<env.m_biotList.size(); i++)
    {
        Value biotJson(kObjectType);
        Biot *biot = env.m_biotList[i];
        biot->SerializeJson(d, biotJson);
        biotsJson.PushBack(biotJson, d.GetAllocator());
    }
    v.AddMember("biots", biotsJson, d.GetAllocator());
}

void CBiotList::SerializeJsonLoad(class Environment &env, const rapidjson::Value& v)
{
    FreeAll();

    Biot* pBiot = nullptr;

    const Value &biotsJson = v["biots"];
    m_nBiot = biotsJson.Size();

    m_bLooped = v["m_bLooped"].GetBool();

    for (int i = 0; i < m_nBiot; i++)
    {
        if ((pBiot = new Biot(env)) != NULL)
        {
            pBiot->SerializeJsonLoad(biotsJson[i]);
            env.AddBiot(pBiot);
        }
        else
            break;
    }
}

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

void CEnvStats::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    const long archVersion = 2;
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("archVersion", archVersion, allocator);
    v.AddMember("m_collisionCount", m_collisionCount, allocator);
    v.AddMember("m_births", m_births, allocator);
    v.AddMember("m_deaths", m_deaths, allocator);
    v.AddMember("m_arrivals", m_arrivals, allocator);
    v.AddMember("m_departures", m_departures, allocator);
    v.AddMember("m_days", m_days, allocator);
    v.AddMember("m_peakPopulation", m_peakPopulation, allocator);
    v.AddMember("m_population", m_population, allocator);
    v.AddMember("m_extinctions", m_extinctions, allocator);
    v.AddMember("m_perWhite", m_perWhite, allocator);
    v.AddMember("m_perGreen", m_perGreen, allocator);
    v.AddMember("m_perRed", m_perRed, allocator);
    v.AddMember("m_perBlue", m_perBlue, allocator);
    v.AddMember("m_perLtBlue", m_perLtBlue, allocator);
    v.AddMember("m_ageRange", m_ageRange, allocator);
    v.AddMember("m_ageIntervals", m_ageIntervals, allocator);

    Value agesArray(kArrayType);
    for(int i=0; i<INTERVALS; i++)
        agesArray.PushBack(Value(m_ages[i]), allocator);
    v.AddMember("m_ages", agesArray, allocator);

    Value energyArray(kArrayType);
    for(int i=0; i<ENERGY_LEVELS; i++)
        energyArray.PushBack(Value(m_energy[i]), allocator);
    v.AddMember("m_energy", energyArray, allocator);

}

void CEnvStats::SerializeJsonLoad(const rapidjson::Value& v)
{
    long version = v["archVersion"].GetInt();

    m_births = v["m_births"].GetInt64();
    m_deaths = v["m_deaths"].GetInt64();
    m_arrivals = v["m_arrivals"].GetInt64();
    m_departures = v["m_departures"].GetInt64();
    m_peakPopulation = v["m_peakPopulation"].GetInt64();
    m_population = v["m_population"].GetInt64();
    m_extinctions = v["m_extinctions"].GetInt64();
    m_collisionCount = v["m_collisionCount"].GetInt64();

    //m_freeEnvArea = v["m_freeEnvArea"].GetInt64();
    //m_totalEnvArea = v["m_totalEnvArea"].GetInt64();

    m_ageIntervals = v["m_ageIntervals"].GetUint64();
    m_ageRange = v["m_ageRange"].GetUint64();

    m_perWhite = v["m_perWhite"].GetFloat();
    m_perGreen = v["m_perGreen"].GetFloat();
    m_perRed = v["m_perRed"].GetFloat();
    m_perBlue = v["m_perBlue"].GetFloat();
    m_perLtBlue = v["m_perLtBlue"].GetFloat();

    m_days = v["m_days"].GetDouble();

    const Value &ageArray = v["m_ages"];
    for(int i=0; i<INTERVALS; i++)
        m_ages[i] = ageArray[i].GetInt64();

    const Value &energyArray = v["m_energy"];
    for(int i=0; i<ENERGY_LEVELS; i++)
        m_energy[i] = energyArray[i].GetInt64();

}

//////////////////////////////////////////////////////////
// Serialize
// 
//

CEnvStatsList::~CEnvStatsList()
{

}

void CEnvStatsList::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value arr(kArrayType);
    for(auto it=this->begin(); it!=this->end(); it++)
    {
        Value stats(kObjectType);
        CEnvStats &statPtr = *it;
        statPtr.SerializeJson(d, stats);
        arr.PushBack(stats, allocator);
    }
    v.AddMember("stats", arr, allocator);
}

void CEnvStatsList::SerializeJsonLoad(const rapidjson::Value& v)
{
    clear();
    const Value &arr = v["stats"];

    for(int i=0; i!=arr.Size(); i++)
    {
        CEnvStats stats;
        stats.SerializeJsonLoad(arr[i]);
        this->push_back(stats);
    }
}

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

}
//Fox END

//////////////////////////////////////////////////////////////////////
// Environment Destructor
//
//  
//
Environment::~Environment(void)
{

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

void Environment::OnOpen()
{ 
	// Establish our boundaries
	leftSide.SetSide(this);
	rightSide.SetSide(this);
	topSide.SetSide(this);
	bottomSide.SetSide(this);

	options.maxLineSegments      = (MAX_GENES / MAX_SYMMETRY);

//	sock.StartSession(pView->GetSafeHwnd(), this);
//	sock.Listen();

	// Create our biots
    for (int i = 0; i < m_biotList.size(); i++)
		m_biotList[i]->OnOpen();
}


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
*/
	// Create our biots
	CreateBiots(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);

	options.m_generation = 0;

	m_stats.Sample(*this);
    m_statsList.append(m_stats);
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
                m_statsList.append(m_stats);
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

*/

    for(int i=0; i<m_biotList.size(); i++)
        m_biotList[i]->UpdateGraphics();

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

/*


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


// /////////////////////////////////////////////////////////////
// Serialize
//
//Fox BEGIN

void Environment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    const uint8_t archiveVersion = 13;
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("archiveVersion", archiveVersion, allocator);
    v.AddMember("m_generation", options.m_generation, allocator);
    v.AddMember("m_uniqueID", m_uniqueID, allocator);
    v.AddMember("bSoundOn", options.bSoundOn, allocator);
    v.AddMember("bMouse", options.bMouse, allocator);
    v.AddMember("nSexual", options.nSexual, allocator);
    v.AddMember("bSiblingsAttack", options.bSiblingsAttack, allocator);
    v.AddMember("bParentAttack", options.bParentAttack, allocator);
    v.AddMember("chance", options.chance, allocator);
    v.AddMember("regenCost", options.regenCost, allocator);
    v.AddMember("regenTime", options.regenTime, allocator);
    v.AddMember("m_leafEnergy", options.m_leafEnergy, allocator);
    v.AddMember("friction", options.friction, allocator);

    // The starting parameters
    v.AddMember("m_orginalSeed", m_orginalSeed, allocator);
    v.AddMember("m_initialPopulation", options.m_initialPopulation, allocator);

    Value br(kObjectType);
    BRect::SerializeJson(d, br);
    v.AddMember("brect", br, allocator);

    // Random state
    Value ran(kObjectType);
    Randomizer::SerializeJson(d, ran);
    v.AddMember("randomizer", ran, allocator);

    // Stats
    Value stats(kObjectType);
    m_stats.SerializeJson(d, stats);
    v.AddMember("m_stats", stats, allocator);

    Value statsLi(kObjectType);
    m_statsList.SerializeJson(d, statsLi);
    v.AddMember("m_statsList", statsLi, allocator);

//		sock.Serialize(ar);
    Value biotLi(kObjectType);
    m_biotList.SerializeJson(*this, d, biotLi);
    v.AddMember("biots", biotLi, allocator);
}

void Environment::SerializeJsonLoad(const rapidjson::Value& v)
{
    DeleteContents();
    const uint8_t archiveVersion = 13;

    int32_t version = v["archiveVersion"].GetInt();
    // Check version
    assert (version == archiveVersion);

    options.m_generation = v["m_generation"].GetUint();

    m_uniqueID = v["m_uniqueID"].GetUint();

    options.bSoundOn = v["bSoundOn"].GetBool();
    if (options.bSoundOn != false)
        options.bSoundOn = true;

    options.bMouse = v["bMouse"].GetBool();

    //if (AfxIsNT() && AfxGetPLife().GetView() == CPrimCmdLine::SHOW_SAVER_WINDOW)
    //    options.bMouse = false;

    options.nSexual = v["nSexual"].GetInt();
    if (options.nSexual < 1 ||
        options.nSexual > 3)
        options.nSexual = 3;

    options.bSiblingsAttack = v["bSiblingsAttack"].GetBool();
    if (options.bSiblingsAttack != false)
        options.bSiblingsAttack = true;

    options.bParentAttack = v["bParentAttack"].GetBool();
    if (options.bParentAttack != false)
        options.bParentAttack = true;

    options.chance = v["chance"].GetInt();
    if (options.chance < 0)
        options.chance = 41;

    options.regenCost = v["regenCost"].GetInt64();
    options.regenTime = v["regenTime"].GetUint();
    options.m_leafEnergy = v["m_leafEnergy"].GetInt();
    options.friction = v["friction"].GetFloat();

    // The starting parameters
    m_orginalSeed = v["m_orginalSeed"].GetInt();
    options.m_initialPopulation = v["m_initialPopulation"].GetInt();

    BRect::SerializeJsonLoad(v["brect"]);

    // Random state
    Randomizer::SerializeJsonLoad(v["randomizer"]);

    // Stats
    m_stats.SerializeJsonLoad(v["m_stats"]);
    m_statsList.SerializeJsonLoad(v["m_statsList"]);

//		sock.Serialize(ar);
    m_biotList.SerializeJsonLoad(*this, v["biots"]);

}

