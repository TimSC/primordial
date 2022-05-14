//#include "primordial life.h"
#include "Settings.h"
#include <QDateTime>
#include <QSettings>
#include <QJsonArray>
#include <QJsonDocument>

using namespace rapidjson;

CSettings::CSettings() {
    Reset(600, 340);

    pens.clear();
    for (int i = 0; i <= MAX_LEAF; i++)
        pens.append(QPen());

    pens[GREEN_LEAF]      = QPen(QColor(0,255,0));
    pens[BLUE_LEAF]       = QPen(QColor(0,0,255));
    pens[RED_LEAF]        = QPen(QColor(255,0,0));
    pens[LBLUE_LEAF]      = QPen(QColor(0,255,255));
    pens[WHITE_LEAF]      = QPen(QColor(255,255,255));
    pens[DARK_GREEN_LEAF] = QPen(QColor(0, 128, 0));
    pens[DARK_BLUE_LEAF]  = QPen(QColor(0, 0, 128));
    pens[DARK_RED_LEAF]   = QPen(QColor(128, 0, 0));
    pens[DARK_LBLUE_LEAF] = QPen(QColor(0, 128, 128));
    pens[GREY_LEAF]       = QPen(QColor(128,128,128));
    pens[YELLOW_LEAF]     = QPen(QColor(255,255,0));
    pens[BLACK_LEAF]      = QPen(QColor(0,0,0));
    pens[PURPLE_LEAF]     = QPen(QColor(255,0,255));
}

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

    settings.setValue("network/m_enableNetworking", m_enableNetworking);
    settings.setValue("network/m_networkPort", m_networkPort);
    QJsonArray clj = QJsonArray::fromStringList(m_connectList);
    QJsonDocument cljdoc(clj);
    settings.setValue("network/m_connectList", cljdoc.toJson());

}

void CSettings::Load()
{
    QSettings settings("Kinatomic", "Primordial Life");

    QVariant val = settings.value("habitat/bSoundOn");
    if(val.isValid()) bSoundOn = val.toBool();
    val = settings.value("habitat/bMouse");
    if(val.isValid()) bMouse = val.toBool();
    val = settings.value("habitat/m_initialPopulation");
    if(val.isValid()) m_initialPopulation = val.toInt();
    val = settings.value("habitat/friction");
    if(val.isValid()) friction = val.toFloat();
    val = settings.value("habitat/m_leafEnergy");
    if(val.isValid()) m_leafEnergy = val.toInt();

    val = settings.value("biot/chance");
    if(val.isValid()) chance = val.toInt();
    val = settings.value("biot/regenCost");
    if(val.isValid()) regenCost = val.toInt();
    val = settings.value("biot/regenTime");
    if(val.isValid()) regenTime = val.toInt();
    val = settings.value("biot/nSexual");
    if(val.isValid()) nSexual = val.toInt();
    val = settings.value("biot/bParentAttack");
    if(val.isValid()) bParentAttack = val.toBool();
    val = settings.value("biot/bSiblingsAttack");
    if(val.isValid()) bSiblingsAttack = val.toBool();

    val = settings.value("network/m_enableNetworking");
    if(val.isValid()) m_enableNetworking = val.toBool();
    val = settings.value("network/m_networkPort");
    if(val.isValid()) m_networkPort = val.toUInt();
    val = settings.value("network/m_connectList");
    if(val.isValid())
    {
        QJsonDocument cljdoc = QJsonDocument::fromJson(val.toByteArray());
        if(cljdoc.isArray())
        {
            QJsonArray clja = cljdoc.array();
            m_connectList.clear();
            for(auto it = clja.begin(); it != clja.end(); it++)
            {
                if(it->isString())
                    m_connectList.append(it->toString());
            }
        }
    }

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

    m_enableNetworking = s.m_enableNetworking;
    m_networkPort = s.m_networkPort;
    m_connectList = s.m_connectList;

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

    chance              = 12;
    bSoundOn            = true;
    nSexual             = 3;
    startNew            = 1;
    m_initialPopulation = 20;
    bParentAttack       = true;
    bSiblingsAttack     = true;
    bSaveOnQuit     = true;

    friction            = 0.005f;
    bBarrier            = true;
    bMouse              = true;

    m_sound.SetScheme("PL", "Primordial Life");

    m_nSick = 200;

    m_enableNetworking = true;
    m_networkPort = 54275;
    m_connectList.clear();
}

void CSettings::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{

    const uint8_t archiveVersion = 13;
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("archiveVersion", archiveVersion, allocator);
    v.AddMember("m_generation", m_generation, allocator);
    v.AddMember("bSoundOn", bSoundOn, allocator);
    v.AddMember("bMouse", bMouse, allocator);
    v.AddMember("nSexual", nSexual, allocator);
    v.AddMember("bSiblingsAttack", bSiblingsAttack, allocator);
    v.AddMember("bSaveOnQuit", bSaveOnQuit, allocator);

    v.AddMember("bParentAttack", bParentAttack, allocator);
    v.AddMember("chance", chance, allocator);
    v.AddMember("regenCost", regenCost, allocator);
    v.AddMember("regenTime", regenTime, allocator);
    v.AddMember("m_leafEnergy", m_leafEnergy, allocator);
    v.AddMember("friction", friction, allocator);

    // The starting parameters
    v.AddMember("m_initialPopulation", m_initialPopulation, allocator);

}

void CSettings::SerializeJsonLoad(const rapidjson::Value& v)
{
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    const uint8_t archiveVersion = 13;

    int32_t version = v["archiveVersion"].GetInt();
    // Check version
    if (version != archiveVersion)
        throw std::runtime_error("eror parsing json");

    m_generation = v["m_generation"].GetUint();

    bSoundOn = v["bSoundOn"].GetBool();
    if (bSoundOn != false)
        bSoundOn = true;

    bMouse = v["bMouse"].GetBool();

    //if (AfxIsNT() && AfxGetPLife().GetView() == CPrimCmdLine::SHOW_SAVER_WINDOW)
    //    options.bMouse = false;

    nSexual = v["nSexual"].GetInt();
    if (nSexual < 1 ||
        nSexual > 3)
        nSexual = 3;

    bSiblingsAttack = v["bSiblingsAttack"].GetBool();
    if (bSiblingsAttack != false)
        bSiblingsAttack = true;

    bSaveOnQuit = v["bSaveOnQuit"].GetBool();
    if (bSaveOnQuit != false)
        bSaveOnQuit = true;

    bParentAttack = v["bParentAttack"].GetBool();
    if (bParentAttack != false)
        bParentAttack = true;

    chance = v["chance"].GetInt();
    if (chance < 0)
        chance = 41;

    regenCost = v["regenCost"].GetInt64();
    regenTime = v["regenTime"].GetUint();
    m_leafEnergy = v["m_leafEnergy"].GetInt();
    friction = v["friction"].GetFloat();

    // The starting parameters
    m_initialPopulation = v["m_initialPopulation"].GetInt();

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
