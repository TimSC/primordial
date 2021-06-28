//#include "primordial life.h"
#include "Settings.h"
#include <QDateTime>
/*
void CSettings::Serialize(QDataStream& a)
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
}
*/
void CSettings::SanityCheck()
{
    if (this->m_initialPopulation > 50)
        this->m_initialPopulation = 50;
    if (this->m_initialPopulation < 1)
        this->m_initialPopulation = 1;

	for (int i = 0; i < SIDES; i++)
	{
        if (m_sSideAddress[i].empty())
            m_bSideEnable[i]  = false;
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

    friction              = s.friction;    
    maxLineSegments       = s.maxLineSegments;
    regenCost             = s.regenCost;
    regenTime             = s.regenTime;

  	m_nSeed                 = s.m_nSeed;                
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

	return *this;
}

void CSettings::Reset(int nWidth, int nHeight)
{
    m_nSeed                 = QDateTime::currentMSecsSinceEpoch();
	m_nArmTypesPerBiot      = 1;
	m_nSegmentsPerArm       = 10;
	m_nArmsPerBiot          = 0;
    m_bGenerateOnExtinction = true;

	for (int i = 0; i < SIDES; i++)
	{
        m_sSideAddress[i] = "";
        m_bSideEnable[i]  = false;
	}

	m_nHeight     = nWidth;
	m_nWidth      = nHeight;
	m_nSizeChoice = 1;

}

// ************************************

const int REGENCOST_OPTIONS=5;
const ListEntry regenCostList[REGENCOST_OPTIONS] = {
  {"Nothing",  0},
  {"Cheap", 100},
  {"Normal", 200},
  {"Expensive", 300},
  {"Too expensive", 400}
};

const int REGENTIME_OPTIONS=7;
const ListEntry regenTimeList[REGENTIME_OPTIONS] = {
  {"Immediate",    0x0000},
  {"Very quick", 0x0001},
  {"Quickly",      0x0003},
  {"Normal",       0x0007},
  {"Slowly",       0x000F},
  {"Very slow",    0x001F},
  {"Too slow",    0x003F},
};

const int BENEFIT_OPTIONS=5;
const ListEntry benefitList[BENEFIT_OPTIONS] = {
  {"Too low", 2},
  {"Low", 3},
  {"Normal", 4},
  {"High", 5},
  {"High noon", 6}
};

const int LIFESPAN_OPTIONS=6;
const ListEntry lifeSpanList[LIFESPAN_OPTIONS] = {
  {"Ephemeral", 3500},
  {"Transitory", 7000},
  {"Moderate", 9000},
  {"Long", 12000},
  {"Very Long", 15000},
  {"Immortal", 0}
};

const int MUTATION_OPTIONS=8;
const ListEntry mutationList[MUTATION_OPTIONS] = {
  {"None",         0},
  {"Nearly none",  1},
  {"Very slight",  3},
  {"Slight",       8},
  {"Moderate",    12},
  {"High",        24},
  {"Very high",   48},
  {"Too high",    80}
};


const int FRICTION_OPTIONS=5;
const ListEntry frictionList[FRICTION_OPTIONS] = {
  { "None",      0},
  { "Slight",    2},
  { "Moderate",  5},
  { "High",      20},
  { "Rigid",     50}
};

const int SETTINGS_SEXUAL_OPTIONS=3;
const ListEntry settingsSexualList[SETTINGS_SEXUAL_OPTIONS] = {
  { "Asexual",                  0},
  { "Sexual",                   1},
  { "Both asexual and sexual",  2}
};

QString SettingFindClosestTextByValue(const ListEntry *list, int listSize, int value)
{
    int bestIndex = -1;
    int bestDist = 0;
    for(int i=0; i<listSize; i++)
    {
        int dist = abs(list[i].second - value);
        if(bestIndex < 0 or dist < bestDist)
        {
            bestDist = dist;
            bestIndex = i;
        }
    }
    return list[bestIndex].first;
}

int SettingFindValueByText(const ListEntry *list, int listSize, const QString &text)
{
    for(int i=0; i<listSize; i++)
    {
        if(list[i].first == text)
        {
            return list[i].second;
        }
    }
    assert(0); //Setting text not known
    return list[0].second;
}
