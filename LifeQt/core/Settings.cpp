//#include "primordial life.h"
#include "Settings.h"
#include <QDateTime>
#include <QSettings>

void CSettings::Save()
{
    QSettings settings("Kinatomic", "Primordial Life");

    settings.setValue("habitat/bSoundOn", bSoundOn);
    settings.setValue("habitat/bMouse", bMouse);
    settings.setValue("habitat/m_initialPopulation", m_initialPopulation);
    settings.setValue("habitat/friction", friction);
    settings.setValue("habitat/m_leafEnergy", m_leafEnergy);

    settings.setValue("biot/chance", chance);
    settings.setValue("biot/regenCost", (qlonglong)regenCost);
    settings.setValue("biot/regenTime", regenTime);
    settings.setValue("biot/nSexual", nSexual);
    settings.setValue("biot/bParentAttack", bParentAttack);
    settings.setValue("biot/bSiblingsAttack", bSiblingsAttack);
}

void CSettings::Load()
{
    QSettings settings("Kinatomic", "Primordial Life");

    QVariant val = settings.value("habitat/bSoundOn");
    if(val.isValid()) bSoundOn = val.toBool();
    val = settings.value("habitat/bMouse");
    bMouse = val.toBool();
    val = settings.value("habitat/m_initialPopulation");
    m_initialPopulation = val.toInt();
    val = settings.value("habitat/friction");
    friction = val.toFloat();
    val = settings.value("habitat/m_leafEnergy");
    m_leafEnergy = val.toInt();

    val = settings.value("biot/chance");
    chance = val.toInt();
    val = settings.value("biot/regenCost");
    regenCost = val.toInt();
    val = settings.value("biot/regenTime");
    regenTime = val.toInt();
    val = settings.value("biot/nSexual");
    nSexual = val.toInt();
    val = settings.value("biot/bParentAttack");
    bParentAttack = val.toBool();
    val = settings.value("biot/bSiblingsAttack");
    bSiblingsAttack = val.toBool();
}

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

void CSettings::SetToDefaults()
{
    effectiveLength[GREEN_LEAF] = 1;
    effectiveLength[BLUE_LEAF]  = 2;
    effectiveLength[RED_LEAF]   = 1;
    effectiveLength[WHITE_LEAF] = 1;
    effectiveLength[LBLUE_LEAF] = 1;

    // You have / he has
    leafContact[GREEN_LEAF][GREEN_LEAF ] = CONTACT_IGNORE;
    leafContact[GREEN_LEAF][BLUE_LEAF  ] = CONTACT_IGNORE;
    leafContact[GREEN_LEAF][WHITE_LEAF ] = CONTACT_IGNORE;
    leafContact[GREEN_LEAF][RED_LEAF   ] = CONTACT_EATEN;
    leafContact[GREEN_LEAF][LBLUE_LEAF ] = CONTACT_IGNORE;

    leafContact[BLUE_LEAF][BLUE_LEAF  ] = CONTACT_IGNORE;
    leafContact[BLUE_LEAF][GREEN_LEAF ] = CONTACT_IGNORE;
    leafContact[BLUE_LEAF][RED_LEAF   ] = CONTACT_DEFEND;
    leafContact[BLUE_LEAF][WHITE_LEAF ] = CONTACT_IGNORE;
    leafContact[BLUE_LEAF][LBLUE_LEAF ] = CONTACT_IGNORE;

    leafContact[RED_LEAF][RED_LEAF   ]   = CONTACT_ATTACK;
    leafContact[RED_LEAF][GREEN_LEAF ]   = CONTACT_EAT;
    leafContact[RED_LEAF][BLUE_LEAF  ]   = CONTACT_DEFENDED;
    leafContact[RED_LEAF][WHITE_LEAF ]   = CONTACT_DESTROY;
    leafContact[RED_LEAF][LBLUE_LEAF ]   = CONTACT_DESTROY;

    leafContact[WHITE_LEAF][RED_LEAF  ]  = CONTACT_DESTROYED;
    leafContact[WHITE_LEAF][GREEN_LEAF]  = CONTACT_IGNORE;
    leafContact[WHITE_LEAF][BLUE_LEAF ]  = CONTACT_IGNORE;
    leafContact[WHITE_LEAF][WHITE_LEAF]  = CONTACT_IGNORE;
    leafContact[WHITE_LEAF][LBLUE_LEAF]  = CONTACT_IGNORE;

    leafContact[LBLUE_LEAF][RED_LEAF  ]  = CONTACT_DESTROYED;
    leafContact[LBLUE_LEAF][GREEN_LEAF]  = CONTACT_IGNORE;
    leafContact[LBLUE_LEAF][BLUE_LEAF ]  = CONTACT_IGNORE;
    leafContact[LBLUE_LEAF][WHITE_LEAF]  = CONTACT_IGNORE;
    leafContact[LBLUE_LEAF][LBLUE_LEAF]  = CONTACT_IGNORE;

    leafMass[RED_LEAF]    = 1;
    leafMass[BLUE_LEAF]   = 2;
    leafMass[WHITE_LEAF]  = 1;
    leafMass[GREEN_LEAF]  = 4;
    leafMass[LBLUE_LEAF]  = 1;

    newType[RED_LEAF]    = RED_LEAF;
    newType[BLUE_LEAF]   = BLUE_LEAF;
    newType[WHITE_LEAF]  = WHITE_LEAF;
    newType[GREEN_LEAF]  = YELLOW_LEAF;
    newType[LBLUE_LEAF]  = LBLUE_LEAF;

    m_leafEnergy = 2;
    regenCost  = 200;
    regenTime  = 0x00000007;

    startEnergy = 400 * 8;

    friction = (float) 0.005;

    chance              = 12;
    bSoundOn            = true;
    nSexual             = 3;
    startNew            = 1;
    m_initialPopulation = 20;
    bParentAttack       = true;
    bSiblingsAttack     = true;
    bBarrier            = true;
    bMouse              = true;

    m_sound.SetScheme("PL", "Primordial Life");

    m_nSick = 200;
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
