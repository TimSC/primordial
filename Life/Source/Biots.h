
#ifndef biots_h
#define biots_h

#include "etools.h"
#include "vector.h"
#include "genotype.h"
#include "Brain.h"


// Sensor Definitions
//
// bit
// 1	


#define DSTERASE  (DWORD)0x00220326

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
    BYTE period;
    BYTE nPeno;
    BYTE frequency;

    double dx;
    double dy;
    double dr;
};

////////////////////////////////////////////////////////////////////
// Collision
//
//
const int MAX_COLLISIONS = 5;


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


//
// Describes half of a collision between two objects
// Multiple collisions with the same biot are averaged
//
class CCollisionPoint
{
public:
	// The last time this collision was seen
	DWORD m_dwEnvTime;

	int m_nLine;
	int m_eLine;

	// Consecutive Hits
	int m_hits;

	double m_dx;
	double m_dy;
	double m_dr;

	int  m_deltaLength;
	int  m_deltaEnergy;
	bool m_bInteract;
	bool m_bCollide;

	CCollisionPoint& operator=(CCollisionPoint& to);
};


//
// Contains a list of collision points with the same object
//
class CCollisionObject : public CArray<CCollisionPoint, CCollisionPoint&> 
{
public:
	CCollisionObject() : m_pBiot(NULL) { };

	CCollisionObject& CCollisionObject::operator=(CCollisionObject& to);

	CCollisionPoint& Record(int nLine, int eLine);
	void PurgeOld(DWORD dwEnvTime);
	void Sum(VectorSum& sum);

	Biot* m_pBiot;
	DWORD m_dwEnvTime;

	// Consecutive Hits
	int m_hits;
};


//
// Contains a list of object collisions
//
class CCollisionSummary : public CArray<CCollisionObject, CCollisionObject&>
{
public:
	CCollisionObject* Record(DWORD dwEnvTime, Biot* pBiot);
	void PurgeOld(DWORD dwEnvTime);
};

class CBiotState
{
public:
	CBiotState() : m_nState(0) {};
	void SetField(int bitField, bool bValue)
	{
		if (bValue)
			m_nState |= bitField;
		else
			m_nState &= ~bitField;
	}

	void SetInternalState(int nInternalState)
	{
		m_nState = (m_nState & 0x000000FF) | nInternalState;
	}

	int GetState() { return m_nState; }

public:
	int m_nState;

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
	~Biot(void);

    DWORD  m_id;
	DWORD  m_fatherId;          // The Id of the father (if any)
    DWORD  m_motherId;
	DWORD  m_mateId;
	DWORD  m_generation;		// Indicates how far down the line a child is
	DWORD  m_fatherGeneration;

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
		STATE_BIOT_STARVING   = 0x00000100,
		STATE_BIOT_HUNGRY     = 0x00000200,
		STATE_BIOT_SICK       = 0x00000400,
		STATE_BIOT_DAMAGED    = 0x00000800,
		STATE_BIOT_MALE       = 0x00001000,
		STATE_BIOT_TOUCH      = 0x00002000, //??
		STATE_BIOT_ADULT      = 0x00004000,
		STATE_BIOT_OLD        = 0x00008000,
		STATE_LIMB_DAMAGED    = 0x00010000,
		STATE_LIMB_SEVERED    = 0x00020000,
		STATE_LIMB_HAVE_EATEN = 0x00040000,
		STATE_LIMB_BEEN_EATEN = 0x00080000,
		STATE_LIMB_ATTACKED   = 0x00100000,
		STATE_LIMB_SEE_FOOD   = 0x00200000,
		STATE_LIMB_SEE_MATE   = 0x00400000,
		STATE_LIMB_SEE_ATTACKER = 0x00800000,
		STATE_LIMB_SEE_DEFENDER = 0x02000000,
		STATE_LIMB_SEE_LIGHT    = 0x04000000,
		STATE_LIMB_SEE_FRIEND   = 0x08000000,
		STATE_LIMB_SEE_MALE     = 0x10000000,
		STATE_LIMB_SEE_PARENT   = 0x20000000,
		STATE_LIMB_SEE_WALL     = 0x40000000,
		STATE_LIMB_SEE_CHILD    = 0x80000000
	};


	void SetSensor(int dwBitField, bool bValue);

	enum {
		PERCENT_AGE,
		PERCENT_ENERGY,
	};

	// Biot Naming
	CString GetName();
	CString GetWorldName()           { return m_sWorldName;  }
	CString GetFullName();
	CString GetFatherName();
	void SetName(CString sName);
	void SetWorldName(CString sName) { m_sWorldName = sName; }


	void Mutate(int chance);

    void ClearSettings(void);
    void Draw(void);
    void SetErasePosition(void);
    void Erase(void);
    void FormBitmap(int pen = -1);
    void FormMask(void);
    void FreeBitmaps();
	enum {
		NORMAL,
		REDRAW,
		REFORM,
		RECALCULATE,
		GROW
	};
	void PrepareErase(int operation);
	void PrepareDraw(int operation);
	void EraseAndDraw(int operation);

	// Determine what biots this biot collides with
	void CollisionDetection(DWORD dwEnvTime);

	// Determine the motion effects of collisions
	void Collide();

	// Perform a real collision
	void CollisionVector(Biot& enemy, BPoint& pt, CCollisionPoint& point);
	void CollisionWall(BPoint& pt, CCollisionPoint& point);

	bool CheckInteraction(CCollisionPoint& point);

	// Purge collisions no longer happening!
	void PurgeOldCollisions(DWORD dwEnvTime);

	// Looks at biot centers and move them apart
	void ArtificialCollision(Biot& enemy, CCollisionPoint& point);
	void ArtificialWall(CCollisionPoint& point);

	// Calculate the relative limb motion effect on collisions
	void LimbRotatedDelta(int ptX, int ptY, int nLine, double& dx, double& dy);
	
    void MoveBiot(int x, int y);

	// Takes the local bounding rectangle and sets the bounding environment rectangle coordinates
	void SetScreenRect();

	BPoint& MassCenter(BPoint& massCenter);
	int     MassCenterY();
	int     MassCenterX();


/*#if defined(_DEBUG)
	void Trace(LPCSTR s) { if (m_Id == 22) TRACE(s); }
	void Trace(LPCSTR s, int value) { if (m_Id == 22) TRACE(s, value); }
#else
	void Trace(LPCSTR) {};
	void Trace(LPCSTR, int) {};
#endif
*/
    void Move(void);

	void  Reject(int side);

	// Creates a biot from scratch
    void  RandomCreate(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);

    int   Initialize(bool bRandom = false);
    int   Contacter(Biot* enemy, int dx, int dy, int& x, int& y);
	virtual void  Serialize(CArchive& ar);

    int LengthLoss(int nPeno, int delta);
    
    long Distance(BPoint& start, BPoint& stop);
    LONG Symmetric(int aRatio);
//    LONG LineContact(int line, int eline, Biot* enemy, int* delta);
    bool AdjustState(int index, int delta);
	bool OnOpen();
  
	int Translate(double radius, double& newX, double& newY, int degrees, int aRatio);
    
    Biot& operator=(Biot& copyMe);

    int      newType;
    DWORD    m_age;
	DWORD    m_maxAge;

    bool     bDie;
    LONG     energy;
    LONG     childBaseEnergy;

	// Contains the results of collisions with other objects
    CCollisionSummary m_collision;

	// A line varies 0 to MAX_LINES
	int Line(int nLimb, int nSegment);

    Vector   vector;


	// Angle offset for each segment
	int    m_angle[MAX_LINES];
	int    m_angleDrawn[MAX_LINES];

	int    MoveLimbTypeSegment(int nSegment, int nLimbType, int nRate);
	int    MoveLimbTypeSegments(int nLimbType, int nRate);
	int    MoveLimbSegments(int nLimbType, int nRate);
	int    MoveLimbSegment(int nSegment, int nLimb, int nRate);

	// The retraction actually drawn
	// The retraction requested
	// The segment being retracted
	int   m_retractDrawn[MAX_LIMBS];
	int   m_retractRadius[MAX_LIMBS];
	int   m_retractSegment[MAX_LIMBS];

	// Retract or extend the tip of a limb
	BYTE    RetractSegment(int nSegment, int nLimb, int maxRadius);
	BYTE    ExtendSegment(int nSegment, int nLimb);
	BYTE    RetractLimbType(int nSegment, int nLimbType, int maxRadius);
	BYTE    ExtendLimbType(int nSegment, int nLimbType);


	CommandLimbStore m_store[MAX_LIMBS];

	CommandArray m_commandArray;
	CommandArray m_commandArray2;
	int          m_internalState;

	bool     bTerminateEvent;


	int      m_nSick;
	bool     m_bDrawn;
	bool     m_bSelected;

	CRedraw redraw;

    BPoint	startPt[MAX_LINES];
    BPoint	stopPt[MAX_LINES];
    int     distance[MAX_LINES];
    GeneTrait trait;

	bool IsSegmentShown(int nPeno)   { return (state[nPeno] > 0); }
	bool IsSegmentDamaged(int nPeno) { return (state[nPeno] != distance[nPeno]); }
	bool IsSegmentMissing(int nPeno) { return (state[nPeno] <= 0); }
	int GetSegmentLength(int nPeno) { return distance[nPeno]; }

	void CheckReproduction();

	// Bonus is related to your total size
	void SetBonus() { m_dBonusRatio = ((double)Area()) / 40000.0; }

	bool IsDead() { return (energy <= 0 || totalDistance <= 0 || genes <= 0); }

private:
    Environment& env;

	// What is the maximum allowed angle rate change
	void MaxAllowedRate(int& nMaxRate, int nDelta, int nRate);

	// Construct a line bounding rectangle
	void GetLineRect(int nLine, BRect& rect);

	// Construct an actual line
	void GetLine(int nLine, CLine& line);

	// Private statics
	// Angle limit rate for line movement
	static const int s_nAngleRateLimit;
	static bool s_bStaticsInitialized;

public:
	// Do not save these variables
	float m_statEnergy[MAX_ENERGY_HISTORY];
	int  m_statIndex;

    HBITMAP  m_hBitmap;

	// Normal Variables
//	GeneEvent	 event[GeneEvent::EVENT_MAX];
	CString      m_sName;
	CString      m_sWorldName;
	CString      m_sFatherName;
	CString      m_sFatherWorldName;

    // Genes stored from mating
//	GeneResponse        response2[GeneEvent::RESPONSE_GROUPS];
//	GeneCommandGroup    group2[GeneCommandGroup::COMMAND_GROUPS];
//	GeneEvent           event2[GeneEvent::EVENT_MAX];
    GeneTrait           trait2;

	double m_dBonusRatio;		// Storage for the bonus ratio
	int    m_livingChildren;	// The number of living children this biot has
	int    m_totalChildren;		// The number of childen this biot has contributed to

    BPoint  origin;
    int     m_nType[MAX_LINES];
    int     state[MAX_LINES];
//    BYTE     typeState[MAX_LINES];
	BYTE     color[MAX_LINES];

    bool     bInjured;
    int      genes2;
    int      topY;
    int      bottomY;
    int      leftX;
    int      rightX;
    int      ratio;
    int      lastType;
    int      lastLeft;
    int      lastTop;
    int      max_genes;
    int      genes;
    long     turnBenefit;
    long     totalDistance;
    long     colorDistance[LINES_BASIC];
    long     adultBaseEnergy;
    long     stepEnergy;

	// The center of mass computed in symmetric
	BPoint   m_massCenter;

	// Places a biot in a random location in the environment
    void PlaceRandom(void);

    bool PlaceNear(Biot& parent);
    bool ChangeDirection(Biot* enemy, int dx, int dy);
    void SetRatio(void);
    void SetSymmetry(void);
    void CopyGenes(Biot& enemy);
    void InjectGenes(int type, Biot& enemy);

	float PercentEnergy();

    int x1(int nLine)
    {
      return startPt[nLine].x + origin.x;
    }

    int x2(int nLine)
    {
      return stopPt[nLine].x + origin.x;
    }

    int y1(int nLine)
    {
      return startPt[nLine].y + origin.y;
    }

    int y2(int nLine)
    {
      return stopPt[nLine].y + origin.y;
    }

    bool AreSiblings(Biot& enemy){ return (m_motherId == enemy.m_motherId && m_motherId != 0); }

    bool OneIsChild(Biot& enemy){ return (m_id == enemy.m_motherId || enemy.m_id == m_motherId); }

    bool SiblingsAttack(Biot& enemy) { return (trait.GetAttackSiblings() || enemy.trait.GetAttackSiblings()); }

	bool LineContact(Biot& enemy, CCollisionPoint& point);

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

	// Public statics
	// Precalculated conversion between line numbers and segments/limbs
	static int s_line2segment[MAX_LINES];
	static int s_line2limb[MAX_LINES];
	
/*	int minShort(int a, int b)
	{
		return (((int)a)<b?((int)a):b);
	}
*/};


//
// Adjust bounding rectangle to a specific environment screen coordinate.
//
inline void Biot::SetScreenRect() 
{
	Set(leftX + origin.x, topY + origin.y, rightX + origin.x + 1, bottomY + origin.y + 1);
}

//
// Move the bounding rectangle and the origin
//
inline void Biot::MoveBiot(int x, int y)
{
	Offset(x, y);
	origin.x += x;
	origin.y += y;
}


/////////////////////////////////////////////////////////////////////
// Line
//
// There are MAX_SEGMENTS * MAX_LIMBS lines possible. A line is
// a segment defined by a certain limb type on a specific limb.
//
inline int Biot::Line(int nLimb, int nSegment)
{
	return nLimb + nSegment * MAX_LIMBS;
}


//////////////////////////////////////////////////////////////////////
// PrepareDraw
//
// For a new bitmap if required.
//
inline void Biot::PrepareDraw(int operation)
{
	if (operation != NORMAL)
		FormBitmap(newType);
}


/////////////////////////////////////////////////////////////////////
// GetLineRect
// 
// Fills in a lines bounding rectangle
//
inline void Biot::GetLineRect(int nLine, BRect& rect)
{
	rect.Set(x1(nLine), y1(nLine), x2(nLine), y2(nLine));
}


/////////////////////////////////////////////////////////////////////
// GetLine
// 
// Fills in a line
//
inline void Biot::GetLine(int nLine, CLine& line)
{
	line.Set(x1(nLine), y1(nLine), x2(nLine), y2(nLine));
}


/////////////////////////////////////////////////////////////////////
inline BPoint& Biot::MassCenter(BPoint& massCenter)
{
	massCenter.x = m_massCenter.x + origin.x;
	massCenter.y = m_massCenter.y + origin.y;
	return massCenter;
}

inline int Biot::MassCenterX()
{
	return m_massCenter.x + origin.x;
}

inline int Biot::MassCenterY()
{
	return m_massCenter.y + origin.y;
}

#endif