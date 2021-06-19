 ///////////////////////////////////////////////////////////////////////////////
//
// Environment
//						---FOX MODIFIED MUCH OF THIS DOC---
// This class defines the environment of the biots
//
//

#ifndef Environment_h
#define Environment_h

#include <string>
#include <QDataStream>
#include <QGraphicsScene>
#include "Etools.h"
#include "Connect.h"
#include "Genotype.h"
//#include "sock.h"
//#include "KeyRegistry.h"
//#include "MagnifyWnd.h"
//#include "EnvStatsWnd.h"
//#include "BiotEditor.h"
#include "Settings.h"


// Predefine Biot to the Environment
class Biot;
class Environment;
class CSettings;

///////////////////////////////////////////////////////////////////////
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
	
    std::string GetDaysStr();
    std::string GetPopulationStr();
    std::string GetExtinctionsStr();
    static std::string ToDays(uint32_t dwAge);
    static uint32_t   ToGenerations(const std::string &szDays);

    //virtual void Serialize(QDataStream& ar);

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


///////////////////////////////////////////////////////////////////////
// CEnvironmentStatsList
//
// Holds statistics related to the environment.  I just broke this off
// from the Environment class to cut the complexity down.
//
class CEnvStatsList : public QList<CEnvStats *>
{
public:
	CEnvStatsList(){};
    //virtual void Serialize(QDataStream& ar);

};




//////////////////////////////////////////////////////////////////////
// CBiotList
//
// Holds a list of biots!  Initial size is for 250 biots
//
class CBiotList : public QList<Biot*>
{
public:
    CBiotList();
    Biot* FindBiotByID(uint32_t id);
	int  FindBiotByPoint(int x, int y);

	Biot* HitCheck(Biot *me, int* pStart);
	Biot* NextBiot();
	void RemoveBiot();
    bool Looped() { return m_bLooped; }

	void FreeAll();

    //virtual void  Serialize(QDataStream& ar, Environment& env);

private:
	int  m_nBiot;
    bool m_bLooped;
};

//////////////////////////////////////////////////////////////////////
//
// Environment
//
//
class Environment : public BRect, Randomizer
{
public:                  
	Environment();
	~Environment(void);

	Biot* HitCheck(Biot *me, BRectSortPos& pos);
//	Biot* HitCheck(Biot *me, int* pStart = NULL); { return m_biotList.HitCheck(me, pStart); }
    Biot* FindBiotByID(uint32_t id) { return m_biotList.FindBiotByID(id); }

	void  AddBiot(Biot* newBiot);

	void Clear();

    void PlayResource(const std::string & sound, bool bSync = false);
//	void Paint(HDC hDC, RECT* pRect);
//	void Paint(CDC* pDC, CRect& rect);

    void lights(bool bOn);
	void NoRoomToGiveBirth(){/*m_stats.m_collisionCount += 4;*/};
	void FreeBitPadDC();

//	HDC  GetBitPadDC(int width, int height);
    uint32_t	GetID(void){return ++m_uniqueID; }

	int  FindBiot(int x, int y);
	void MoveBiot(Biot* pBiot) { m_sort.Move((BRectItem*) pBiot); }
	void MoveBiot(Biot* pBiot, BRect* pOrigRect) { m_sort.Move((BRectItem*) pBiot, pOrigRect); }
		
    int  GetPopulation() { return m_biotList.size(); }
    //void Serialize(QDataStream& ar);
	void DeleteContents();
    void Skip();
//    void BiotOperation(CScrollView* pView, int x, int y, int operation);

    bool  BiotShouldBox(uint32_t biotId) { return (biotId == m_selectedId && m_bIsSelected); }
	Biot* GetSelectedBiot()           { return m_biotList.FindBiotByID(m_selectedId); }

//	void MagnifyBiot(CDC& dc, Biot* pBiot, CRect& rect);
//	void OpenEnvironmentStatistics(CWnd* pWnd);

    bool IsIntersect(CLine& bLine, int& x, int& y)
    {
      for (int i = 0; i < 4; i++)
        if (side[i]->IsIntersect(bLine, x, y))
          return true;
      return false;
    }

    bool WithinBorders(BRect& rect);

//	void GetDefaultSettings();
//	void SetDefaultSettings();
    void OnNew(QGraphicsScene &scene, QRect worldRect, int population, int seed,
               int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);
	void OnRestart();
//	void OnOpen(CScrollView* pView);
	void OnStop();

//	void SaveBiot(Biot* pBiot);
//	void LoadBiot(int x, int y);

public:
/*	CBitmap           m_topBitmap;
	BITMAP            m_topBm;

	CBitmap           m_bottomBitmap;
	BITMAP            m_bottomBm;
*/
    QGraphicsScene *  m_scene;
	BRectSort         m_sort;

	CEnvStats         m_stats;
	CEnvStatsList     m_statsList;

    bool m_bBlocked;
	CSettings options; //The optins: Fox
/*
	CEnvStatsWnd* m_pEnvStatsWnd;
	CMagnifyWnd*  m_pMagnifyWnd;

	CBiotEditor m_editor;
*/
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
//    HDC     m_hMemoryDC;
//    HDC     m_hScreenDC;
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
//    HDC     m_hMemPadDC;
//    HBITMAP m_hBitPad;
//    HBITMAP m_hOldPad;

	// Should we sleep between loops to slow things down?
    uint64_t   m_dwTicks;
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
