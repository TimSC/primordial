
#ifndef biots_h
#define biots_h

#include "Resources.h"	// Added by ClassView
#include "etools.h"
#include "vector.h"
#include "genotype.h"
#include "Brain.h"
#ifdef _METABOLISM
#include "Resources.h"
#include "Metabolism.h"
#endif
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

class Collision
{
  public:
    void setId(int ID) { id = ID; hits = 0; }
    void setSeen(DWORD age) { seenToggle = (short) age; }
    BOOL wasSeen(DWORD age) { return (seenToggle == (short) age); }
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

	BOOL ShouldRedraw() { return m_bShouldRedraw; }

	void SetRedraw(BOOL bRedraw = TRUE)
	{
		if (bRedraw)
			m_bShouldRedraw = TRUE;
	}

	void ClearRedraw() { m_bShouldRedraw = FALSE; }

private:
	BOOL m_bShouldRedraw;
};


//////////////////////////////////////////////////////////////////////
// Biot
//
// Biot State Variables
//
class Biot: public BRectItem
{
  public:
//////////////////////////////////////////////////////////////////
//Enumerations
//////////////////////////////////////////////////////////////////
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

	enum {
		NORMAL,
		REDRAW,
		REFORM,
		RECALCULATE,
		GROW
	};


/////////////////////////////////////////////////////////////////
//Public Variables
/////////////////////////////////////////////////////////////////
    BOOL     bTerminateEvent;
	BOOL     m_nSick;
	BOOL     m_bDrawn;
	BOOL     m_bSelected;	
	BOOL     bInjured;
    BOOL     bDie;
#pragma message ("Remove flap")
	short    flap[MAX_GENES]; 
	short    m_angle[MAX_GENES];
	short    m_angleDrawn[MAX_GENES];
	short    m_angleLimbType[MAX_LIMB_TYPES];
	short    m_angleLimbTypeDrawn[MAX_LIMB_TYPES];
	short    m_angleLimb[MAX_SYMMETRY];
	short    m_angleLimbDrawn[MAX_SYMMETRY];
	short    m_angleLimbTypeSegment[MAX_LIMB_TYPES][MAX_SEGMENTS];
	short    m_angleLimbTypeSegmentDrawn[MAX_LIMB_TYPES][MAX_SEGMENTS];
	short   m_retractDrawn[MAX_SYMMETRY];	// The retraction actually drawn
	short   m_retractRadius[MAX_SYMMETRY];	// The retraction requested
	short   m_retractSegment[MAX_SYMMETRY];	// The segment being retracted	
    short     distance[MAX_GENES];
#pragma message ("Remove reserved")
	BYTE     reservedLineType[MAX_LIMB_TYPES];
	BYTE     reservedSegType[MAX_LIMB_TYPES][MAX_SEGMENTS];
	BYTE     reservedLine[MAX_SYMMETRY];
	BYTE     reserved[MAX_GENES]; 
	BYTE     geneNo[MAX_GENES];
	BYTE     lineNo[MAX_GENES];
    
	int      newType;
	int		m_bitmapWidth;
	int		m_bitmapHeight;
	int     m_internalState;
	int      m_currentEvent;   // What event are we running
	int      m_nextEvent;	
	int      m_currentGroup;	// What commandGroup is running	
	int      m_currentLine;		// What line sensed the event?
	int      m_nextLine;	
	int      m_currentWait;		// How much time so far on a command group
    DWORD    m_age;
	DWORD    m_maxAge;

	//Metabolism - 
    //LONG    energy;
	//LONG	redIn;
	//LONG	blueIn;
	//LONG	greenIn;
	//LONG	redStore;
	//LONG	greenStore;	
	//LONG	blueStore;
	//LONG	e1store;
	//LONG	e2store;
	//LONG	e3store;
	//BYTE	m_inspiration[3];
	//BYTE	m_expiration[3];
	//BYTE	metabolism1[9]; //High Nibble of each Byte gives Energy Cost/Gain of Step
	//BYTE	metabolism2[9];	//Low Nibble of Each Byte gives the amount of corresponding 
							//Metabolite that participates in reaction

	//BYTE m_matesExpiration[3];
	//BYTE m_matesInspiration[3];
	//BYTE m_matesMetabolism2[9];
	//BYTE m_matesMetabolism1[9];

	//LONG     childBaseEnergy;
    POINT     startPt[MAX_GENES];
    POINT     stopPt[MAX_GENES];
    Collision collider[MAX_COLLISIONS];
	Vector   vector;
	CommandLimbStore m_store[MAX_SYMMETRY];
	CommandArray m_commandArray;
	CommandArray m_commandArray2;
	// What is the state of each command in the command group
    //CSTATE   cState[GeneCommandGroup::COMMANDS_PER_GROUP];
	//GeneResponse        response[GeneEvent::RESPONSE_GROUPS];
	//GeneCommandGroup    group[GeneCommandGroup::COMMAND_GROUPS];
	CRedraw redraw;
    GeneTrait trait;
public:
	// Do not save these variables
	BYTE     color[MAX_GENES];
    short    nType[MAX_GENES];
    short    state[MAX_GENES];
	//int  m_statIndex;
	int    m_livingChildren;	// The number of living children this biot has
	int    m_totalChildren;		// The number of childen this biot has contributed to
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
    long     colorDistance[WHITE_LEAF + 1];
#ifdef _METABOLISM
	CMetabolism metabolism;
#else
	LONG		energy;
    long		adultBaseEnergy;
    long		stepEnergy;
	LONG		childBaseEnergy;
	int			m_statIndex;
	float		m_statEnergy[MAX_ENERGY_HISTORY];
	double		m_dBonusRatio;		// Storage for the bonus ratio
#endif
	DWORD  m_fatherId;          // The Id of the father (if any)
    DWORD  m_motherId;
	DWORD  m_mateId;
    DWORD  m_Id;
	DWORD  m_generation;		// Indicates how far down the line a child is
	DWORD  m_fatherGeneration;
	//float m_statEnergy[MAX_ENERGY_HISTORY];
	//double m_dBonusRatio;		// Storage for the bonus ratio
    POINT    origin;
    HBITMAP  m_hBitmap;

	// Normal Variables
	//GeneEvent	 event[GeneEvent::EVENT_MAX];
	CString      m_sName;
	CString      m_sWorldName;
	CString      m_sFatherName;
	CString      m_sFatherWorldName;

    // Genes stored from mating
	//GeneResponse        response2[GeneEvent::RESPONSE_GROUPS];
	//GeneCommandGroup    group2[GeneCommandGroup::COMMAND_GROUPS];
	//GeneEvent           event2[GeneEvent::EVENT_MAX];
    GeneTrait           trait2;

//    BYTE     typeState[MAX_GENES];
/////////////////////////////////////////////////////////////////
//Private  Member Variables
/////////////////////////////////////////////////////////////////
private:
    Environment& env;

///////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////
public:
//	void CrossMetabolism(Biot * pBiot);
//	int MetabolicPath(long metabolite, BYTE coefficient);
	
//	void StepMetabolism();
	Biot(Environment& environment);
	~Biot(void);
	Biot& operator=(Biot& copyMe);
	int  RandomCreate(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm);
    int  Initialize(BOOL bRandom = FALSE);
    void ClearSettings(void);
/////////////////////////////////////////////////////////////////////////
//Constant Functions - These functions do not change 
//the internal state of the Biot
/////////////////////////////////////////////////////////////////////////
#ifndef _METABOLISM
	float PercentEnergy() const;
#endif
	CString GetFatherName() const;	
/////////////////////////////////////////////////////////////////////////
//Mutater Functions - These functions do change 
//the internal state of the Biot
/////////////////////////////////////////////////////////////////////////
    int		AddCollision(int enemyId);
    BOOL	AdjustState(int nPeno, short delta);
	void	ChangeType(int nPeno, int aType);
    void	ChangeColorEvent(int nPeno, int aType);
    BOOL	ChangeDirection(Biot* enemy, int dx, int dy);
    void	CheckBitmapSize(int width, int height);		
	void	CheckReproduction();
    void	ClearCollisions(void){for(int i = 0; i < MAX_COLLISIONS; i++) collider[i].id = -1; }
	void	ClearReservations();
    int		Contacter(Biot* enemy, int dx, int dy, int& x, int& y);
	BOOL	ContactLine(Biot& enemy, int nEnemyPeno, int nPeno, short& delta, long& deltaEnergy);
    void	CopyGenes(Biot& enemy);
	long	Distance(POINT& start, POINT& stop);
    void	Draw(void);
    void	Erase(void);
	void	EraseAndDraw(int operation);
	BYTE	ExtendLimbType(int nSegment, int nLimbType);
	BYTE	ExtendLine(int nSegment, int nLimb);
    int		FindCollision(int enemyId);
    void	FormBitmap(int pen = -1);
    void	FormMask(void);
    void	FreeBitmaps();
#ifdef _METABOLISM	
	CResources * GetCell();
#endif
	CString GetName();
	CString GetFullName();
	short	GetSegmentLength(int nPeno) { return distance[nPeno]; }
	void	IncreaseAngle(int nPeno, short rate);
    void	InjectGenes(int type, Biot& enemy);
	BOOL	IsReserved(int nPeno);
	BOOL	IsSegmentDamaged(int nPeno) { return (state[nPeno] != distance[nPeno]); }
	BOOL	IsSegmentMissing(int nPeno) { return (state[nPeno] <= 0); }
    short	LengthLoss(int nPeno, short delta);
    LONG	LineContact(int line, int eline, Biot* enemy, short* delta);
	void	Motion(const double deltaX, const double deltaY, double Vx, double Vy, const double radius);
    BOOL	Move(void);
	void	MoveArm(int nPeno, short degree);
	short	MoveLimbSegment(int nSegment, int nLimb, int nRate);	
	short	MoveLimbSegments(int nLimbType, int nRate);
	short	MoveLimbTypeSegment(int nSegment, int nLimbType, int nRate);
	short	MoveLimbTypeSegments(int nLimbType, int nRate);
	BOOL	MoveLine(int nLine, short rate, short offset);
	BOOL	MoveLineType(int nLineType, short rate, short offset);
	void	MoveSegment(int nPeno, short degree);
	BOOL	MoveSegmentType(int nLineType, int nSegment, short rate, short offset);
	void	Mutate(int chance);    
	BOOL	OnOpen();
    BOOL	PlaceNear(Biot& parent);
    int		PlaceRandom(void);
	void	PrepareDraw(int operation);
	void	PrepareErase(int operation);
	void	PrepareEvent(int nextDesiredEvent, int nextDesiredLine);
	void	Reject(int side);
	void	ReleaseFullLine(int nLine);
	void	ReleaseLine(int nPeno);
	void	ReleaseLineType(int nLineType);
	void	ReleaseSegment(int nPeno);
	void	ReleaseSegType(int nLineType, int segment);
    void	RemoveCollisions(DWORD age);
//	void	RequestNextEvent(int nextDesiredEvent, int nextDesiredLine);
	BOOL	ReserveFullLine(int nLine);
	BOOL	ReserveLine(int nPeno);
	BOOL	ReserveLineType(int nLineType);
	BOOL	ReserveSegment(int nPeno);
	BOOL	ReserveSegType(int nLineType, int segment);
	BYTE	RetractLimbType(int nSegment, int nLimbType, int maxRadius);
	BYTE	RetractLine(int nSegment, int nLimb, int maxRadius);// Retract or extend the tip of a limb
	void	SeekArm(int nPeno, short rate, short offset);
	void	SeekSegment(int nPeno, short degree, short offset);
	virtual void  Serialize(CArchive& ar);
    void	SetErasePosition(void);
	void	SetName(CString sName);	
    void	SetRatio(void);
    void	SetSymmetry(void);
    LONG	Symmetric(int aRatio);
	short	Translate(double radius, double& newX, double& newY, int degrees, int aRatio);
    void	ValidateBorderMovement(double& dx, double& dy);
    void	WallBounce(int x, int y);	

	///////////////////////////////////////////////////////////////////////////////////////
//Inline Functions
///////////////////////////////////////////////////////////////////////////////////////
#ifdef _METABOLISM
	void SetBonus() { metabolism.m_dBonusRatio = ((double)Area()) / 40000.0; }
#else
	void SetBonus() { m_dBonusRatio = ((double)Area()) / 40000.0; }
#endif

#if defined(_DEBUG)
	void Trace(LPCSTR s) { if (m_Id == 22) TRACE(s); }
	void Trace(LPCSTR s, int value) { if (m_Id == 22) TRACE(s, value); }
#else
	void Trace(LPCSTR) {};
	void Trace(LPCSTR, int) {};
#endif

	CString GetWorldName() const      { return m_sWorldName; }
	void SetWorldName(CString sName) { m_sWorldName = sName; }

	void SetScreenRect() 
	{
		Set(leftX + origin.x, topY + origin.y, rightX + origin.x + 1, bottomY + origin.y + 1);
	}

	void MoveBiot(int x, int y)
    {
      Offset(x, y);
      origin.x += x;
      origin.y += y;
    }

	short CheckWhite(int type)
	{
		// If IsMale(), 
		return (short) ((type == WHITE_LEAF && (env.options.nSexual == 2 || (env.options.nSexual == 3 && trait.IsAsexual())))?GREEN_LEAF:type);
//		return (short) ((type == WHITE_LEAF && !trait.IsMale() && trait.IsAsexual())?GREEN_LEAF:type);
	}

    int x1(int gene)
    {
      return startPt[gene].x + origin.x;
    }

    int x2(int gene)
    {
      return stopPt[gene].x + origin.x;
    }

    int y1(int gene)
    {
      return startPt[gene].y + origin.y;
    }

    int y2(int gene)
    {
      return stopPt[gene].y + origin.y;
    }

    BOOL AreSiblings(Biot& enemy){ return (m_motherId == enemy.m_motherId && m_motherId != 0); }

    BOOL OneIsChild(Biot& enemy){ return (m_Id == enemy.m_motherId || enemy.m_Id == m_motherId); }

    BOOL SiblingsAttack(Biot& enemy) { return (trait.GetAttackSiblings() || enemy.trait.GetAttackSiblings()); }

	

    BOOL AttackChildren(Biot& enemy) { return (trait.GetAttackChildren() ||
                                              enemy.trait.GetAttackChildren()); }
    // Species match if their id is within one of each other.  There are
    // 16 identifiers possible.  This allows species to drift apart.  Otherwise
    // a mutation of id would case a single creature to be alone.
    BOOL SpeciesMatch(int enemySpecies) {
      enemySpecies = abs (enemySpecies - trait.GetSpecies());
      return (enemySpecies <= 1 || enemySpecies >= 31);
    }

	double PercentColor(int color) { return ((double) colorDistance[color]) / ((double) totalDistance); }

    int BaseRatio(void) { return ratio - (trait.GetAdultRatio() - 1); }


	short minShort(int a, short b)
	{
		return (((short)a)<b?((short)a):b);
	}
};


/////////////////////////////////////////////////////////
// IsReserved
//
// Returns TRUE if the target was reserved.
//
inline BOOL Biot::IsReserved(int nPeno)
{
	return reserved[nPeno];
}


/////////////////////////////////////////////////////////
// ReserveSegment
//
// Returns TRUE if the target was reserved. Pass in the
// segment to reserve.
//
inline BOOL Biot::ReserveSegment(int nPeno)
{
	if (!reserved[nPeno])
	{
		reserved[nPeno]++;
		return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////
// ReleaseSegment
//
//
inline void Biot::ReleaseSegment(int nPeno)
{
	reserved[nPeno] = 0;
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


///////////////////////////////////////////////////////////////////////////
//Reference Counted Auto Pointer Class
template <class T> 
class ObjVar;

template <class T>
class Counted
	{
    friend class ObjVar<T>;
private:
    Counted(T* pT) : Count(0), my_pT(pT) { ASSERT(pT != 0); }
    ~Counted()   { ASSERT(Count == 0); delete my_pT; }

    unsigned GetRef()  { return ++Count; }
    unsigned FreeRef() { ASSERT(Count!=0); return --Count; }

    T* const my_pT;
    unsigned Count;
	};

template <class T>
class ObjVar
	{
public:
     ObjVar();
     ObjVar(T* pT);
     ~ObjVar();
     ObjVar(const ObjVar<T>& rVar);

     ObjVar<T>& operator=(const ObjVar<T>& rVar);
        
     T* operator->();
     const T* operator->() const;
     friend bool operator==(const ObjVar<T>& lhs, const ObjVar<T>& rhs);

     bool Null() const {return m_pCounted == 0};
     void SetNull() { UnBind(); }

private:
     void UnBind();
     Counted<T>* m_pCounted;
	};


template<class T>
ObjVar<T>::ObjVar(): m_pCounted(0) {}

template<class T>
ObjVar<T>::ObjVar(T* pT)
	{
    m_pCounted = new Counted<T>(pT);
    m_pCounted->GetRef();
	}

template<class T>
ObjVar<T>::~ObjVar()
	{
    UnBind();
	}

template<class T>
void ObjVar<T>::UnBind()
	{
    if (!Null() && m_pCounted->FreeRef() == 0)       
		delete m_pCounted;
    m_pCounted = 0;
	}

template<class T>
T* ObjVar<T>::operator->()
	{
    if (Null())
        throw NulRefException();
    return m_pCounted->my_pT;
	}

template<class T>
const T* ObjVar<T>::operator->() const
	{
    if (Null())
        throw NulRefException();
    return m_pCounted->my_pT;
	}

template<class T>
ObjVar<T>::ObjVar(const ObjVar<T>& rVar)
	{
    m_pCounted = rVar.m_pCounted;
    if (!Null())
        m_pCounted->GetRef();
	}



template<class T>
ObjVar<T>&  ObjVar<T>::operator=(const ObjVar<T>& rVar)
	{
    if (!rVar.Null())
        rVar.m_pCounted->GetRef();
    UnBind();
    m_pCounted = rVar.m_pCounted;
    return *this;
	}

template<class T>
bool operator==(const ObjVar<T>& lhs, const ObjVar<T>& rhs)
	{
    return lhs.m_pCounted->my_pT == rhs.m_pCounted->my_pT;
        // or *(lhs.m_pCounted->my_pT) == *(rhs.....)
	}

template<class T>
bool operator!=(const ObjVar<T>& lhs, const ObjVar<T>& rhs)
	{
    return !(lhs == rhs);
	}

typedef ObjVar< Biot > BiotPtr;

#endif