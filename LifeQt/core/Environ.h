 ///////////////////////////////////////////////////////////////////////////////
//
// Environment
//
// This class defines the environment of the biots
//
//

#ifndef Environment_h
#define Environment_h

#include <string>
#include <QDataStream>
#include <QOpenGLWidget>
#include <QMediaPlayer>
#include "Etools.h"
#include "Connect.h"
#include "Genotype.h"
#include "Settings.h"

// Predefine Biot to the Environment
class Biot;
class Environment;
class CSettings;

// /////////////////////////////////////////////////////////////////////
// CEnvironmentStats
//
// Holds statistics related to the environment.  I just broke this off
// from the Environment class to cut the complexity down.
//
class CEnvStats
{
public:
	CEnvStats() { Clear(); }

	void Clear();
	void Sample(Environment& env);
	void NewSample();
	
    std::string GetDaysStr() const;
    std::string GetPopulationStr() const;
    std::string GetExtinctionsStr() const;
    static std::string ToDays(uint32_t dwAge);
    static uint32_t   ToGenerations(const std::string &szDays);

    void SerializeJson(rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(const rapidjson::Value& v);

	enum {
		SAMPLES     = 100,
		INTERVALS   = 20,
		GENERATIONS = 512,
		SAMPLE_TIME = 0x000001FF,
		ENERGY_LEVELS = 20
	};

	float PercentUncoveredByBiots();


public:
    int64_t m_births;
    int64_t m_deaths;
    int64_t m_arrivals;
    int64_t m_departures;
    int64_t m_peakPopulation;
    int64_t m_population;
    int64_t m_extinctions;
    int64_t m_collisionCount;

    int64_t m_freeEnvArea;
    int64_t m_totalEnvArea;

    uint32_t m_ageIntervals;
    uint32_t m_ageRange;
    int64_t  m_ages[INTERVALS];
    int64_t  m_energy[ENERGY_LEVELS];

	float m_perWhite;
	float m_perGreen;
	float m_perRed;
	float m_perBlue;
	float m_perLtBlue;

	double m_days;
};


// /////////////////////////////////////////////////////////////////////
// CEnvironmentStatsList
//
// Holds statistics related to the environment.  I just broke this off
// from the Environment class to cut the complexity down.
//
class CEnvStatsList : public QList<CEnvStats>
{
public:
	CEnvStatsList(){};
    virtual ~CEnvStatsList();

    void SerializeJson(rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(const rapidjson::Value& v);

};


// ////////////////////////////////////////////////////////////////////
// CBiotList
//
// Holds a list of biots!  Initial size is for 250 biots
//
class CBiotList : public QList<Biot*>
{
public:
    CBiotList();
    Biot* FindBiotByID(uint32_t id);
    Biot* FindBiotByPoint(int x, int y);

	Biot* HitCheck(Biot *me, int* pStart);
	Biot* NextBiot();
	void RemoveBiot();
    bool Looped() { return m_bLooped; }

	void FreeAll();

    void SerializeJson(class Environment &env, rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(class Environment &env, const rapidjson::Value& v);

private:
	int  m_nBiot;
    bool m_bLooped;
};

// ////////////////////////////////////////////////////////////////////
//
// Environment
//
//
class EnvironmentListener
{
public:
    EnvironmentListener() {};
    virtual ~EnvironmentListener() {};

    virtual void BiotUpdated(Biot* pBiot) {};
};

// ////////////////////////////////////////////////////////////////////
//
// Environment
//
//
class Environment : public BRect, Randomizer
{
public:                  
	Environment();
	~Environment(void);

    void SerializeJson(rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(const rapidjson::Value& v);

	Biot* HitCheck(Biot *me, BRectSortPos& pos);
    Biot* FindBiotByID(uint32_t id) { return m_biotList.FindBiotByID(id); }
    Biot* FindBiotByPoint(int x, int y) { return m_biotList.FindBiotByPoint(x, y); };

	void  AddBiot(Biot* newBiot);

	void Clear();

    void PlayResource(const std::string & sound);

    void lights(bool bOn);
	void NoRoomToGiveBirth(){/*m_stats.m_collisionCount += 4;*/};
	void FreeBitPadDC();

    uint32_t	GetID(void){return ++m_uniqueID; }

	void MoveBiot(Biot* pBiot) { m_sort.Move((BRectItem*) pBiot); }
	void MoveBiot(Biot* pBiot, BRect* pOrigRect) { m_sort.Move((BRectItem*) pBiot, pOrigRect); }
		
    int  GetPopulation() { return m_biotList.size(); }
    int  GetNumConnectedSides();
	void DeleteContents();
    void Update();

    bool  BiotShouldBox(uint32_t biotId) { return (biotId == m_selectedId && m_bIsSelected); }
	Biot* GetSelectedBiot()           { return m_biotList.FindBiotByID(m_selectedId); }
    void SetSelectedBiot(uint32_t biotId);

    bool IsIntersect(CLine& bLine, int& x, int& y)
    {
      for (int i = 0; i < 4; i++)
        if (side[i]->IsIntersect(bLine, x, y))
          return true;
      return false;
    }

    bool WithinBorders(BRect& rect);

    void OnNew(QOpenGLWidget &scene, QRect worldRect, int population, int seed,
               int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);
	void OnRestart();
    void OnOpen();
	void OnStop();

    void paintGL(QPainter &painter);
    void AddListener(class EnvironmentListener *listener);

public:

    QOpenGLWidget *  m_scene;
	BRectSort         m_sort;

	CEnvStats         m_stats;
	CEnvStatsList     m_statsList;

	CSettings options; //The optins: Fox

    LeftSide   leftSide;
    RightSide  rightSide;
    TopSide    topSide;
    BottomSide bottomSide;

	int m_orginalSeed;				//Seed used on restart    

	// What biot is selected and for what operation
    uint32_t m_selectedId;
    bool  m_bIsSelected;

	int   m_operation;

    Side*   side[4];
    bool    bNetworkSettingsChange;

	// Save the state of these variables	

    CBiotList   m_biotList;

  private:
	void CreateBiots(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);

  private:
	// For generating unique ids for the environment biots
    uint32_t   m_uniqueID;

	// Global scratch pad for flicker free drawing
    int     m_maxBitPadWidth;
    int     m_maxBitPadHeight;

    uint64_t tickStart, tickCount;
    double ticksPerSec;

    QMediaPlayer *mediaPlayer;
    QList<class EnvironmentListener *> listeners;
};

inline bool Environment::WithinBorders(BRect& rect)
{
	if (!(Inside(rect.m_left, rect.m_top) ||
			rightSide.Inside(rect.m_left, rect.m_top) ||
			leftSide.Inside(rect.m_left, rect.m_top)  ||
			topSide.Inside(rect.m_left, rect.m_top)  ||
			bottomSide.Inside(rect.m_left, rect.m_top)))
        return false;

	if (!(Inside(rect.m_left, rect.m_bottom) ||
			rightSide.Inside(rect.m_left, rect.m_bottom) ||
			leftSide.Inside(rect.m_left, rect.m_bottom)  ||
			topSide.Inside(rect.m_left, rect.m_bottom)  ||
			bottomSide.Inside(rect.m_left, rect.m_bottom)))
        return false;

	if (!(Inside(rect.m_right, rect.m_bottom) ||
            rightSide.Inside(rect.m_right, rect.m_bottom) ||
            leftSide.Inside(rect.m_right, rect.m_bottom)  ||
            topSide.Inside(rect.m_right, rect.m_bottom)  ||
            bottomSide.Inside(rect.m_right, rect.m_bottom)))
        return false;

	if (!(Inside(rect.m_right, rect.m_top) ||
            rightSide.Inside(rect.m_right, rect.m_top) ||
            leftSide.Inside(rect.m_right, rect.m_top)  ||
            topSide.Inside(rect.m_right, rect.m_top)  ||
            bottomSide.Inside(rect.m_right, rect.m_top)))
        return false;

    return true;
}

#endif
