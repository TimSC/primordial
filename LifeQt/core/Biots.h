
#ifndef biots_h
#define biots_h

#include <string>
#include <QDataStream>
#include <QGraphicsItemGroup>
#include "Etools.h"
#include "vector.h"
#include "Genotype.h"
#include "Brain.h"
#include "Environ.h"
#include "rapidjson/document.h"

// Sensor Definitions
//
// bit
// 1	
const int MAX_ENERGY_HISTORY = 21;

#define DSTERASE  (uint32_t)0x00220326

 //////////////////////////////////////////////////////////////////////
//
// Line Equations
//  m = (y1 - y2) / (x1 - x2) 
//
//  b = (y2x1 - y1x2)/ (x1 - x2)
//
//  x = (b2 - b1) / (m1 - m2)
//

//  y = (b2m1 - b1m2) / (m1 - m2)
//
//  y = mx + b

class Motion
{
  public:
    uint8_t period;
    uint8_t nPeno;
    uint8_t frequency;

    double dx;
    double dy;
    double dr;
};

////////////////////////////////////////////////////////////////////
// Collision
//
//
const int MAX_COLLISIONS = 5;

class Collision
{
  public:
    void setId(int ID) { id = ID; hits = 0; }
    void setSeen(uint32_t age) { seenToggle = (short) age; }
    bool wasSeen(uint32_t age) { return (seenToggle == (short) age); }
    int  addHit(void) { return ++hits; }
    int   id;
    short hits;
    short seenToggle;
};



//////////////////////////////////////////////////
// CRedraw
//
// Biot processes determine if a segment needs
// to be redrawn.  If so, it marks it for redrawing
//
// We use a union to accelerate our check, checking
// 4 bytes at a time.
//
class CRedraw
{
public:
	CRedraw() { ClearRedraw(); }

    bool ShouldRedraw() { return m_bShouldRedraw; }

    void SetRedraw(bool bRedraw = true)
	{
		if (bRedraw)
            m_bShouldRedraw = true;
	}

    void ClearRedraw() { m_bShouldRedraw = false; }

private:
    bool m_bShouldRedraw;
};


//////////////////////////////////////////////////////////////////////
// Biot
//
// Biot State Variables
//
class Biot: public BRectItem
{
  public:
	Biot(Environment& environment);
    virtual ~Biot(void);
	
	int m_bitmapWidth;
	int m_bitmapHeight;
	void CheckBitmapSize(int width, int height);

	enum {
		IS_HUNGRY = 0,  // Histeresis
		IS_INJURED,     // Any injury
		IS_LOW_ENERGY,	// Has less than genetic energy
		IS_FERTILE,		// Is fertile
		IS_MALE,		// Is a male
		IS_ASEXUAL,		// Is asexual
		IS_OLD,			// upper genetic
		IS_YOUNG,		// lower genetic
		IS_ADULT,		// Full grown
	};

	enum {
		PERCENT_AGE,
		PERCENT_ENERGY,
	};

	// Biot Naming
    std::string GetName();
    std::string GetWorldName()           { return m_sWorldName;  }
    std::string GetFullName();
    std::string GetFatherName();
    void SetName(std::string sName);
    void SetWorldName(std::string sName) { m_sWorldName = sName; }


	void Mutate(int chance);

    void ClearSettings(void);
    void Draw(void);
    void UpdateGraphics();
    void FormMask(void);
    void FreeBitmaps();
	enum {
		NORMAL,
		REDRAW,
		RECALCULATE,
		GROW
	};
    void Prepare(int operation);
    void WallBounce(int x, int y);
	
	void Motion(const double deltaX, const double deltaY, double Vx, double Vy, const double radius);

    void MoveBiot(int x, int y);

	void SetScreenRect() 
	{
        Set(leftX + origin.x(), topY + origin.y(), rightX + origin.x() + 1, bottomY + origin.y() + 1);
	}

#if defined(_DEBUG)
    void Trace(const std::string & s) { if (m_Id == 22) TRACE(s); }
    void Trace(const std::string & s, int value) { if (m_Id == 22) TRACE(s, value); }
#else
    void Trace(const std::string &) {};
    void Trace(const std::string &, int) {};
#endif

    bool Move(void);

	void  Reject(int side);
    int   RandomCreate(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);
    int   Initialize(bool bRandom = false);
    int   Contacter(Biot* enemy, int dx, int dy, int& x, int& y);
    virtual void  SerializeJson(rapidjson::Document &d, rapidjson::Value &v);
    void SerializeJsonLoad(const rapidjson::Value& v);

    short LengthLoss(int nPeno, short delta);
    
    int64_t Distance(QPoint& start, QPoint& stop);
    int64_t UpdateShape(int aRatio);
    void UpdateShapeRotation();
    int64_t LineContact(int line, int eline, Biot* enemy, short* delta);
    bool AdjustState(int index, short delta);
    bool OnOpen();
  
    short Translate(double radius, double& newX, double& newY, int degrees, int aRatio);
    inline QPointF RotatePoint(const QPointF &pt, double rotCos, double rotSin);
    
    Biot& operator=(Biot& copyMe);

    int      newType;
    uint32_t    m_age;
    uint32_t    m_maxAge;

    bool     bDie;
    int64_t     energy;
    int64_t     childBaseEnergy;

    Collision collider[MAX_COLLISIONS];
    void RemoveCollisions(uint32_t age);
    int  FindCollision(int enemyId);
    int  AddCollision(int enemyId);
    void ClearCollisions(void){for(int i = 0; i < MAX_COLLISIONS; i++) collider[i].id = -1; }

    Vector   vector;


	short    m_angle[MAX_GENES];
	short    m_angleDrawn[MAX_GENES];

	short    m_angleLimbType[MAX_LIMB_TYPES];
	short    m_angleLimbTypeDrawn[MAX_LIMB_TYPES];

	short    m_angleLimb[MAX_SYMMETRY];
	short    m_angleLimbDrawn[MAX_SYMMETRY];

	short    m_angleLimbTypeSegment[MAX_LIMB_TYPES][MAX_SEGMENTS];
	short    m_angleLimbTypeSegmentDrawn[MAX_LIMB_TYPES][MAX_SEGMENTS];

	short    MoveLimbTypeSegment(int nSegment, int nLimbType, int nRate);
	short    MoveLimbTypeSegments(int nLimbType, int nRate);
	short    MoveLimbSegments(int nLimbType, int nRate);
	short    MoveLimbSegment(int nSegment, int nLimb, int nRate);

	// The retraction actually drawn
	// The retraction requested
	// The segment being retracted
	short   m_retractDrawn[MAX_SYMMETRY];
	short   m_retractRadius[MAX_SYMMETRY];
	short   m_retractSegment[MAX_SYMMETRY];

	// Retract or extend the tip of a limb
    uint8_t    RetractLine(int nSegment, int nLimb, int maxRadius);
    uint8_t    ExtendLine(int nSegment, int nLimb);
    uint8_t    RetractLimbType(int nSegment, int nLimbType, int maxRadius);
    uint8_t    ExtendLimbType(int nSegment, int nLimbType);


	CommandLimbStore m_store[MAX_SYMMETRY];

	CommandArray m_commandArray;
	CommandArray m_commandArray2;
	int          m_internalState;

    uint8_t     geneNo[MAX_GENES];
    uint8_t     lineNo[MAX_GENES];

    bool     bTerminateEvent;


    bool     m_nSick;
    bool     m_bDrawn;
    bool     m_bSelected;

	void MoveArm(int nPeno, short degree);
	void MoveSegment(int nPeno, short degree);
	void SeekSegment(int nPeno, short degree, short offset);
	void SeekArm(int nPeno, short rate, short offset);
	void IncreaseAngle(int nPeno, short rate);
    bool MoveLineType(int nLineType, short rate, short offset);
    bool MoveSegmentType(int nLineType, int nSegment, short rate, short offset);
    bool MoveLine(int nLine, short rate, short offset);

	CRedraw redraw;

    QPointF     startPtLocal[MAX_GENES];
    QPointF     stopPtLocal[MAX_GENES];
    QPointF     startPt[MAX_GENES];
    QPointF     stopPt[MAX_GENES];
    bool        bShapeChanged;

    short     distance[MAX_GENES];
    GeneTrait trait;

    bool IsSegmentDamaged(int nPeno) { return (state[nPeno] != distance[nPeno]); }
    bool IsSegmentMissing(int nPeno) { return (state[nPeno] <= 0); }
	short GetSegmentLength(int nPeno) { return distance[nPeno]; }

	void CheckReproduction();

	// Bonus is related to your total size
	void SetBonus() { m_dBonusRatio = ((double)Area()) / 40000.0; }

private:
    Environment& env;


public:
	// Do not save these variables
	float m_statEnergy[MAX_ENERGY_HISTORY];
	int  m_statIndex;

    //HBITMAP  m_hBitmap;

	// Normal Variables
//	GeneEvent	 event[GeneEvent::EVENT_MAX];
    std::string      m_sName;
    std::string      m_sWorldName;
    std::string      m_sFatherName;
    std::string      m_sFatherWorldName;

    // Genes stored from mating
//	GeneResponse        response2[GeneEvent::RESPONSE_GROUPS];
//	GeneCommandGroup    group2[GeneCommandGroup::COMMAND_GROUPS];
//	GeneEvent           event2[GeneEvent::EVENT_MAX];
    GeneTrait           trait2;

	double m_dBonusRatio;		// Storage for the bonus ratio
	int    m_livingChildren;	// The number of living children this biot has
	int    m_totalChildren;		// The number of childen this biot has contributed to
    uint32_t  m_fatherId;          // The Id of the father (if any)
    uint32_t  m_motherId;
    uint32_t  m_mateId;
    uint32_t  m_Id;
    uint32_t  m_generation;		// Indicates how far down the line a child is
    uint32_t  m_fatherGeneration;

    QPoint    origin;
    short    nType[MAX_GENES];
    short    state[MAX_GENES];
//    uint8_t     typeState[MAX_GENES];
    uint8_t     color[MAX_GENES];

    bool     bInjured;
    int      genes2;
    int      topY;
    int      bottomY;
    int      leftX;
    int      rightX;
    int      ratio;
    int      lastType;
    int      max_genes;
    int      genes;
    int64_t     turnBenefit;
    int64_t     totalDistance;
    int64_t     colorDistance[WHITE_LEAF + 1];
    int64_t     adultBaseEnergy;
    int64_t     stepEnergy;
    int  PlaceRandom(void);
    bool PlaceNear(Biot& parent);
    bool ChangeDirection(Biot* enemy, int dx, int dy);
    void SetRatio(void);
    void SetSymmetry(void);
    void CopyGenes(Biot& enemy);
    void InjectGenes(int type, Biot& enemy);

	float PercentEnergy();
    void paintGL(QPainter &painter);
 
	short CheckWhite(int type)
	{
		// If IsMale(), 
		return (short) ((type == WHITE_LEAF && (env.options.nSexual == 2 || (env.options.nSexual == 3 && trait.IsAsexual())))?GREEN_LEAF:type);
//		return (short) ((type == WHITE_LEAF && !trait.IsMale() && trait.IsAsexual())?GREEN_LEAF:type);
	}

    //Get world coordinates of gene line
    int x1(int gene)
    {
      return startPt[gene].x() + origin.x();
    }

    int x2(int gene)
    {
      return stopPt[gene].x() + origin.x();
    }

    int y1(int gene)
    {
      return startPt[gene].y() + origin.y();
    }

    int y2(int gene)
    {
      return stopPt[gene].y() + origin.y();
    }

    bool AreSiblings(Biot& enemy){ return (m_motherId == enemy.m_motherId && m_motherId != 0); }

    bool OneIsChild(Biot& enemy){ return (m_Id == enemy.m_motherId || enemy.m_Id == m_motherId); }

    bool SiblingsAttack(Biot& enemy) { return (trait.GetAttackSiblings() || enemy.trait.GetAttackSiblings()); }

    bool ContactLine(Biot& enemy, int nEnemyPeno, int nPeno, short& delta, int64_t& deltaEnergy);

    bool AttackChildren(Biot& enemy) { return (trait.GetAttackChildren() ||
                                              enemy.trait.GetAttackChildren()); }
    // Species match if there id is within one of each other.  There are
    // 16 identifiers possible.  This allows species to drift apart.  Otherwise
    // a mutation of id would case a single creature to be alone.
    bool SpeciesMatch(int enemySpecies) {
      enemySpecies = abs (enemySpecies - trait.GetSpecies());
      return (enemySpecies <= 1 || enemySpecies >= 15);
    }

	double PercentColor(int color) { return ((double) colorDistance[color]) / ((double) totalDistance); }

    int BaseRatio(void) { return ratio - (trait.GetAdultRatio() - 1); }

    void  ValidateBorderMovement(double& dx, double& dy);

	short minShort(int a, short b)
	{
		return (((short)a)<b?((short)a):b);
	}
};


//
// Move the bounding rectangle and the origin
//
inline void Biot::MoveBiot(int x, int y)
{
	Offset(x, y);
    origin.setX(origin.x() + x);
    origin.setY(origin.y() + y);
}

// ////////////////////////////////////////////////////////////////////
/// \brief deg2rad
/// \param degrees
/// \return
///
inline double deg2rad (double deg) {
    return deg * M_PI / 180.0;
}

#endif
