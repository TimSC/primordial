#ifndef SETTINGS_H
#define SETTINGS_H
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
#include <QPair>
#include <QString>

typedef QPair<QString, int> ListEntry;

class BRect;

class CSettings 
{
    // Public Methods
public:
    CSettings();

    // Identity constructor
    CSettings(CSettings &s){*this = s;}

    int  GetFriction() { return (int) ((1000.0 * friction) + 0.5F); }    
    void SetFriction(int iFriction) {friction = ((float)iFriction) / 1000.0F; }

    void Load();
    void Save();
    void SetToDefaults();

    void Reset(int nWidth, int nHeight);
    CSettings& operator=(CSettings& s);
    
    void SanityCheck();

    void SerializeJson(rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(const rapidjson::Value& v);

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
    int m_initialPopulation;        //Population used on restart/new
    int     m_leafEnergy; //Solar energy collected per time
    CSoundRegistry m_sound;
    QList<QPen> pens;
    int     startNew;    // Should we use the old population, or the new?
    int     chance; //mutation rate
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
    bool    bSaveOnQuit;

    float   friction;
    BRect   barrier;
    int     maxLineSegments;
    int64_t    regenCost;
    uint32_t   regenTime;
    int      m_nSeed;
    int        m_nArmTypesPerBiot;
    int        m_nSegmentsPerArm;
    int        m_nArmsPerBiot;
    bool    m_bGenerateOnExtinction;

    std::string m_sSideAddress[SIDES];
    bool    m_bSideEnable[SIDES];

    int m_nHeight;
    int m_nWidth;
    int m_nSizeChoice;

    bool m_enableNetworking;
    uint16_t m_networkPort;
    QList<QString> m_connectList;



};//Fox END;

extern const int REGENCOST_OPTIONS;
extern const ListEntry regenCostList[];
extern const int REGENTIME_OPTIONS;
extern const ListEntry regenTimeList[];
extern const int BENEFIT_OPTIONS;
extern const ListEntry benefitList[];
extern const int LIFESPAN_OPTIONS;
extern const ListEntry lifeSpanList[];
extern const int POPULATION_OPTIONS;
extern const ListEntry populationList[];
extern const int MUTATION_OPTIONS;
extern const ListEntry mutationList[];
extern const int FRICTION_OPTIONS;
extern const ListEntry frictionList[];
extern const int SETTINGS_SEXUAL_OPTIONS;
extern const ListEntry settingsSexualList[];

QString SettingFindClosestTextByValue(const ListEntry *list, int listSize, int value);
int SettingFindValueByText(const ListEntry *list, int listSize, const QString &text);

#endif //SETTINGS_H
