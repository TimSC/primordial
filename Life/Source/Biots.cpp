//////////////////////////////////////////////////////////////////////
// Biot Class
//
//
//
#include "stdafx.h"
#include "etools.h"
#include "genotype.h"
#include "environ.h"
#include "biots.h"


CCollisionObject* CCollisionSummary::Record(DWORD dwEnvTime, Biot* pBiot)
{
	int i;

	// First attempt to find this biot id
	for(i = 0; i < GetSize(); i++)
	{
		CCollisionObject& object = ElementAt(i);
		if (object.m_pBiot == pBiot)
		{
			if (object.m_dwEnvTime != dwEnvTime)
			{
				object.m_hits++;
				object.m_dwEnvTime = dwEnvTime;
//				TRACE("Biot Collision Updated %d Times\n", object.m_hits);
			}
			else
			{
//				TRACE("Biot Collision Already Updated %d Times\n", object.m_hits);
			}
			return &object;
		}

		if (DWORD(pBiot) > DWORD(object.m_pBiot))
			break;
	}

	
	CCollisionObject object;
	object.m_hits           = 1;
	object.m_dwEnvTime      = dwEnvTime;
	object.m_pBiot          = pBiot;
//	TRACE("New Biot Collision\n");
	InsertAt(i, object);
	return &ElementAt(i);
}

//
// Get rid of old collision objects.  If the biot was deleted,
// than no collision would have happened and the old pointer
// gets purged here.
//
void CCollisionSummary::PurgeOld(DWORD dwEnvTime)
{
	for(int i = 0; i < GetSize(); i++)
	{
		if (ElementAt(i).m_dwEnvTime != dwEnvTime)
		{
			// Remove it, but we have try this index again
			RemoveAt(i--);
		}
		else
		{
			ElementAt(i).PurgeOld(dwEnvTime);
		}
	}
}


CCollisionPoint& CCollisionObject::Record(int nLine, int eLine)
{
	int i = 0;
	// First attempt to find this biot id
	for(; i < GetSize(); i++)
	{
		CCollisionPoint& point = ElementAt(i);
		if (nLine == point.m_nLine &&
			eLine == point.m_eLine)
		{
			if (point.m_dwEnvTime != m_dwEnvTime)
			{
				point.m_hits++;
				point.m_dwEnvTime = m_dwEnvTime;
//				TRACE(" %d times\n", point.m_hits);
			}
			else
			{
//				TRACE(" %d times - already entered\n", point.m_hits);
			}
			return point;
		}

		if (nLine > point.m_nLine &&
			eLine > point.m_eLine)
			break;
	}
	
	CCollisionPoint point;
	point.m_hits         = 1;
	point.m_dwEnvTime    = m_dwEnvTime;
	point.m_eLine        = eLine;
	point.m_nLine        = nLine;
	point.m_deltaLength  = 0;
	point.m_deltaEnergy  = 0;
	point.m_bInteract    = false;
	point.m_bCollide     = false;

	//	TRACE(" %d times\n", point.m_hits);
	InsertAt(i, point);
	return ElementAt(i);

}

CCollisionObject& CCollisionObject::operator=(CCollisionObject& to)
{
	m_dwEnvTime = to.m_dwEnvTime;
	m_pBiot     = to.m_pBiot;
	m_hits      = to.m_hits;
	RemoveAll();
	for (int i = 0; i < to.GetSize(); i++)
		Add(to.ElementAt(i));
	return *this;
}


void CCollisionObject::Sum(VectorSum& sum)
{
	for(int i = 0; i < GetSize(); i++)
	{
		CCollisionPoint& point = ElementAt(i);
		sum.Add(point.m_dx, point.m_dy, point.m_dr);
	}
}


void CCollisionObject::PurgeOld(DWORD dwEnvTime)
{
	for(int i = 0; i < GetSize(); i++)
	{
		if (ElementAt(i).m_dwEnvTime != dwEnvTime)
		{
			// Remove it, but we have try this index again
			RemoveAt(i--);
		}
	}
}


CCollisionPoint& CCollisionPoint::operator=(CCollisionPoint& to)
{
	m_dwEnvTime    = to.m_dwEnvTime;
	m_eLine        = to.m_eLine;
	m_nLine        = to.m_nLine;
	m_hits         = to.m_hits;
	m_dx           = to.m_dx;
	m_dy           = to.m_dy;
	m_dr           = to.m_dr;
	m_deltaLength  = to.m_deltaLength;
	m_deltaEnergy  = to.m_deltaEnergy;
	m_bInteract    = to.m_bInteract;
	m_bCollide     = to.m_bCollide;
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Biot Class
//
//
//
//
//

// Have we initialized our statics?
bool Biot::s_bStaticsInitialized = false;

// What is the max angle rate for each line
const int Biot::s_nAngleRateLimit = 3;

// Precalculated conversion between line numbers and segments/limbs
int Biot::s_line2segment[MAX_LINES];
int Biot::s_line2limb[MAX_LINES];

Biot::Biot(Environment& environment) : env(environment)
{
	// Initialize statics (there is probably a better way to do this!)
	if (!s_bStaticsInitialized)
	{
		s_bStaticsInitialized = true;

		int nLine = 0;
		for (int nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
		{
			for (int nLimb = 0; nLimb < MAX_LIMBS; nLimb++)
			{
				s_line2segment[nLine]   = nSegment;
				s_line2limb[nLine++]    = nLimb;
			}
		}
	}

	max_genes = 1;
	ClearSettings();
}


Biot::~Biot(void)
{
	FreeBitmaps();
}


//////////////////////////////////////////////////////////////////////
// GetName
//
CString Biot::GetName()
{
static const CString vowels = _T("aeiouy");
static const CString cons   = _T("bcdfghjklmnpqrstvwx");

	Randomizer rand;
	if (m_sName.IsEmpty())
	{
		int max = 1 + rand.Integer(3);

		for (int i = 0; i < max; i++)
		{
			if (rand.Bool())
			{
				m_sName += vowels[rand.Integer(vowels.GetLength())];
				m_sName += cons[rand.Integer(vowels.GetLength())];
			}
			else
			{
				m_sName += cons[rand.Integer(vowels.GetLength())];
				m_sName += vowels[rand.Integer(vowels.GetLength())];
			}
		}

		if (rand.Bool())
			m_sName += vowels[rand.Integer(vowels.GetLength())];

		m_sName.SetAt(0, (char) toupper(m_sName[0]));
	}
	return m_sName;
}


//////////////////////////////////////////////////////////////////////
// SetFirstName
//
void Biot::SetName(CString sName)
{
	m_sName = sName;
}

//////////////////////////////////////////////////////////////////////
// GetFatherName
//
CString Biot::GetFatherName()
{
	CString sName;
	if (m_fatherId == 0)
	{
		sName = _T("No");
	}
	else
	{
		if (m_fatherGeneration > 0)
			sName.Format(_T(":%lu"), m_fatherGeneration);
		sName = m_sFatherName + sName;

		if (!m_sFatherWorldName.IsEmpty())
			sName += _T(" of ") + m_sFatherWorldName;
	}
	return sName;
}

//////////////////////////////////////////////////////////////////////
// GetFullName
//
CString Biot::GetFullName()
{
	CString sName;
	if (m_generation > 0)
	{
		sName.Format(_T(":%lu"), m_generation);
	}
	sName = GetName() + sName;

	if (!GetWorldName().IsEmpty())
		sName += _T(" of ") + GetWorldName();

	return sName;
}


//////////////////////////////////////////////////////////////////////
// PercentEnergy
//
float Biot::PercentEnergy()
{
	float f = ((float) 100 * energy) / ((float) (adultBaseEnergy << 1));
	if (f > 100.0)
		return 100.0;
	else
		return f;
}


//////////////////////////////////////////////////////////////////////
// FreeBitmaps
//
void Biot::FreeBitmaps()
{
	if (m_hBitmap)
	{
		VERIFY(DeleteObject(m_hBitmap));
		m_hBitmap = NULL;
	}

	m_bitmapWidth  = 0;
	m_bitmapHeight = 0;
}


//////////////////////////////////////////////////////////////////////
// Clear Settings
//
//
void Biot::ClearSettings(void)
{
	bDie               = false;
	genes              = MAX_LIMBS; // We start out showing a little
	genes2             = 0;
	m_fatherId         = 0;
	m_mateId           = 0;
	m_livingChildren   = 0;
	m_totalChildren    = 0;
	m_generation       = 0;
	m_fatherGeneration = 0;

	m_hBitmap        = NULL;
	m_bitmapWidth  = 0;
	m_bitmapHeight = 0;
	m_nSick        = 0;
	lastType       = LINE_BLANK;
	newType        = -2;
	lastLeft       = lastTop   = 0;
	ratio          = 1;
	m_age          = 0;
	m_internalState = 0;
	
	bInjured     = false;
	m_collision.RemoveAll();
	m_bDrawn     = false;
	m_bSelected  = false;

	ZeroMemory(m_angle,                      sizeof(m_angle));
	ZeroMemory(m_angleDrawn,                 sizeof(m_angleDrawn));

	ZeroMemory(m_retractDrawn,               sizeof(m_retractDrawn));
	ZeroMemory(m_retractRadius,              sizeof(m_retractRadius));

	for (int i = 0; i < MAX_LIMBS; i++)
		m_retractSegment[i] = -1;

	ZeroMemory(state,    sizeof(state));

	// Statistics
	ZeroMemory(m_statEnergy, sizeof(m_statEnergy));
	m_statIndex = 0;
}  


//////////////////////////////////////////////////////////////////////
// RandomCreate - creates a biot randomly
//
//
void Biot::RandomCreate(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
	max_genes = MAX_LINES;

	genes = MAX_LINES; // We show all of the biot on creation

	//TODO: Switch to Randomize
	trait.Randomize(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);
//	trait.Debug(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);

	m_commandArray.Randomize();

	// Initially we have no parent
	m_motherId = 0;

	vector.setDeltaY(0); 
	vector.setDeltaX(0);
	vector.setDeltaRotate(0);

	Initialize(true);

	PlaceRandom();
	SetBonus();
	GetName();
}


//////////////////////////////////////////////////////////////////////
// Mutate
//
//
void Biot::Mutate(int chance)
{
	int i;

	max_genes = MAX_LINES;

	genes = MAX_LINES; // We show all of the biot on creation

	trait.Mutate(chance);
	m_commandArray.Mutate(chance);

	adultBaseEnergy = Symmetric(trait.GetAdultRatio()) * env.options.startEnergy;
	if (energy < adultBaseEnergy)
		energy = adultBaseEnergy;

	SetRatio();
	totalDistance   = Symmetric(ratio);
	childBaseEnergy = totalDistance * env.options.startEnergy;
  
	// If not loaded, we need a new ID
	m_maxAge = trait.GetMaxAge();

	// Set a fully charged initial state
	for (i = 0; i < MAX_LINES; i++)
		state[i] = distance[i];

	SetBonus();
}


//////////////////////////////////////////////////////////////////////
// RunTimeVariables
//
//
/*void Biot::RunTimeVariables(void)
{



}*/


//////////////////////////////////////////////////////////////////////
// SetRatio
//
// Sets the growth ratio and the stepEnergy
// energy and adultBaseEnergy must be set first
//
void Biot::SetRatio(void)
{
  if (energy > 0)
  {
    ratio = ((2 * adultBaseEnergy) / energy) + trait.GetAdultRatio() - 1;
    if (ratio > MAX_RATIO)
      ratio = MAX_RATIO;

    if (ratio < trait.GetAdultRatio())
      ratio = trait.GetAdultRatio();
  }
  else
    ratio = MAX_RATIO;

  stepEnergy = ((2 * adultBaseEnergy) / BaseRatio());
}


/////////////////////////////////////////////////////////////////////
// Initialize - translates genes to phenotype and places biot
//
//
int Biot::Initialize(bool bRandom)
{
	int i;

	adultBaseEnergy = Symmetric(trait.GetAdultRatio()) * env.options.startEnergy;
	if (bRandom || energy <= 0)
		energy = adultBaseEnergy;

	SetRatio();
	totalDistance   = Symmetric(ratio);

	childBaseEnergy = totalDistance * env.options.startEnergy;
  
	// If not loaded, we need a new ID
	m_id     = env.GetId();
	m_maxAge = trait.GetMaxAge();

	// Set a fully charged initial state
	for (i = 0; i < MAX_LINES; i++)
		state[i] = distance[i];

	for (i = 0; i < MAX_LIMBS; i++)
		m_store[i].Initialize(trait.GetLineTypeIndex(i), i, *this);

	return 0;
}


//////////////////////////////////////////////////////////////////////
//
// Randomly looks for a place to put a new biot.
//
void Biot::PlaceRandom(void)
{
	int i;
	Randomizer rand;

	// Search for a spot to place the biot.
	// If we can't find one, go with the last one checked.
	for (i = 0; i < 24; i++)
	{
		//WARNING: Make sure we never have an environment width/height of zero!
		origin.x = rand.Integer((env.Width()) + leftX - rightX)  - leftX;
		origin.y = rand.Integer((env.Height()) + topY  - bottomY) - topY;

		SetScreenRect();

		BRectSortPos pos;
		pos.FindRectsIntersecting(*this);

		// If the first iteration returns no one, we found a spot
		if (env.HitCheck(this, pos) == NULL)
			break;
	}
    
	// The vector contains a fractional location
	vector.setX(origin.x);
	vector.setY(origin.y);

	FormBitmap();
	SetErasePosition();
}


//////////////////////////////////////////////////////////////////////
//
// PlaceNear - this function attempts to find a position near the 
// parent to place the child.
//
bool Biot::PlaceNear(Biot& parent)
{
int nSide, nPos;
static int side[8][2] = {
    {-1, -1},
    {-1,  0},
    {-1,  1},
    { 0, -1},
    { 0,  1},
    { 1, -1},
    { 1,  0},
    { 1,  1}};

  Randomizer rand;
  nPos = rand.Integer(8);
  for (nSide = 0; nSide < 8; nSide++)
  {
    nPos++;
    if (nPos > 7)
      nPos -= 8;

	  origin.x = parent.origin.x + parent.Width() * side[nPos][0];
	  origin.y = parent.origin.y + parent.Height() * side[nPos][1];
	  SetScreenRect();

    if (IsContainedBy(env))
    {
		BRectSortPos pos;
		pos.FindRectsIntersecting(*this);

      if (env.HitCheck(this, pos) == NULL)
      {
        if (parent.trait.GetDisperseChildren())
        {
          vector.setDeltaX(side[nPos][0] * (float) fabs(parent.vector.getDeltaX()));
          vector.setDeltaY(side[nPos][1] * (float) fabs(parent.vector.getDeltaY()));
        }
        else
        {
          vector.setDeltaX(parent.vector.getDeltaX());
          vector.setDeltaY(parent.vector.getDeltaY());
        }
        return true;
      }
    }
  }
  return false;
}


//////////////////////////////////////////////////////////////////////
// Symmetric
//
//
LONG Biot::Symmetric(int aRatio)
{
int  dist    = 0;
double X,Y;
int nSegment;
int i, nLine;
//int nSet = 0;
//bool bFirstLine = true;
int  nLastLine = -1;

	leftX   =
	topY    =
	rightX  =
	bottomY = 0;

	turnBenefit    = 0;
	redraw.ClearRedraw();

	ZeroMemory(colorDistance, sizeof(colorDistance));
	ZeroMemory(distance,      sizeof(distance));
	ZeroMemory(stopPt,        sizeof(stopPt));
	ZeroMemory(startPt,       sizeof(startPt));
	ZeroMemory(m_nType,       sizeof(m_nType));

	m_massCenter.Clear();

	for (int nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{	
		nLastLine = -1;

		X = 0.0;
		Y = 0.0;

		for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
		{
			GeneSegment& segment = trait.GetSegment(nLimb, nSegment);

			if (segment.IsVisible())
			{
				nLine = Line(nLimb, nSegment);

				m_angleDrawn[nLine] = m_angle[nLine];

				if (nLastLine < 0)
				{
					startPt[nLine].x = 0;
					startPt[nLine].y = 0;
				}
				else
				{
					//TODO: Branching involves setting new start points
//					startPt[nLine] = stopPt[nLine - ((nSegment - nLastLine) * MAX_LIMBS)];
					if (segment.GetStart() < nSegment &&
						trait.GetSegment(nLimb, segment.GetStart()).IsVisible())
						startPt[nLine] = stopPt[nLine - ((nSegment - segment.GetStart()) * MAX_LIMBS)];
					else
						startPt[nLine] = stopPt[nLine - ((nSegment - nLastLine) * MAX_LIMBS)];


				}
				nLastLine = nSegment;
				
				double radius = segment.GetRadius();

				if (nSegment == m_retractSegment[nLimb])
				{
					radius -= (double) m_retractRadius[nLimb];
					m_retractDrawn[nLimb] = m_retractRadius[nLimb];
				}

				distance[nLine] = Translate(radius, X, Y,
					trait.GetAngle(nLimb, nSegment) + 
					vector.getRotate()           + 
					m_angleDrawn[nLine],  
					aRatio);

				// Pass doubles in and out to keep track of the fractional portion
				// We are getting lots of jitter during rotation.  To smooth it out
				// we must pass on the fraction portion for each limb
				stopPt[nLine].x = startPt[nLine].x + int(X);
				stopPt[nLine].y = startPt[nLine].y + int(Y);

				X -= int(X);
				Y -= int(Y);

				if (stopPt[nLine].x < leftX)
					leftX = stopPt[nLine].x;

				if (stopPt[nLine].x > rightX)
					rightX = stopPt[nLine].x;

				if (stopPt[nLine].y < topY)
					topY = stopPt[nLine].y;

				if (stopPt[nLine].y > bottomY)
					bottomY = stopPt[nLine].y;

				dist += distance[nLine];

				m_massCenter.x += (startPt[nLine].x + int(X/2.0)) * distance[nLine];
				m_massCenter.y += (startPt[nLine].y + int(Y/2.0)) * distance[nLine];

				m_nType[nLine] = segment.GetColor(trait.IsMale());

				ASSERT(m_nType[nLine] != LINE_INJECTOR || trait.IsMale());
       
				if (m_nType[nLine] == LINE_LEAF)
					turnBenefit += (env.options.m_leafEnergy * distance[nLine]);

				colorDistance[m_nType[nLine]] += distance[nLine];
 			}
		}
	}

	vector.setMass(0.0f);

	for (i = LINE_LEAF; i < LINES_BASIC; i++)
		vector.addMass((float)(colorDistance[i] * env.options.leafMass[i]));

	// Set the relative center of mass
	m_massCenter.x /= dist;
	m_massCenter.y /= dist;
	//TODO: Random Create creates biots with no lines sometimes!!!

//	TRACE("CENTER(%d, %d)  MASSCENTER(%d, %d)\n", CenterX(), CenterY(), origin.x + m_massCenter.x, origin.y + m_massCenter.y);

	return dist;
}


///////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 
// Translate                                                           
//
// Takes a line and scales/rotates it. Returns the new radius and
// starting coordinates.
//
inline int Biot::Translate(double radius, double& newX, double& newY, int degrees, int aRatio)
{
static const double scale[MAX_RATIO] =	{0.70, 0.76, 0.84, 0.92,
										1.00, 1.10, 1.22, 1.34, 1.48, 1.70, 1.94, 2.21,
										2.47, 2.77, 3.11, 3.48, 3.90, 4.36, 4.89, 5.47};

    radius /= scale[aRatio - 1];

	// We would like at least one pixel
	if (radius < 1.42)
		radius = 1.42;

    // Now rotate degrees
	// Convert degrees to radians
    double theta  = double(degrees) * RADIANS;

	newX += radius * cos(theta);
	newY += radius * sin(theta);

	return (int) (radius + .5);
}


/////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 
// Distance                                                           
//
// d=square root of (x1 - x2) sqaured + (y1 - y2) squared
//
long Biot::Distance(BPoint& start, BPoint& stop)
{
double d = double(((start.x - stop.x) * (start.x - stop.x)) +
					((start.y - stop.y) * (start.y - stop.y)));
  return (long) sqrt(d);
}


//////////////////////////////////////////////////////////////////////
// Draw 
//
// Draws the biot during a WM_PAINT
//
//
void Biot::Draw(void)
{
	HBITMAP hOld;

	hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap); 
	ASSERT(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
	VERIFY(BitBlt(env.m_hScreenDC, m_left, m_top, Width(), Height(), env.m_hMemoryDC, 0, 0, SRCPAINT));
	SelectObject(env.m_hMemoryDC, hOld);

	SetErasePosition();
	m_bDrawn = true;
}


//////////////////////////////////////////////////////////////////////
// EraseAndDraw - draws the biot
//
//
void Biot::EraseAndDraw(int operation)
{
	HBITMAP hOld;
	int  lastWidth  = Width();
	int  lastHeight = Height();

	// Copy off current image
	if (env.options.bNoFlickerSet || 1)
	{
		RECT region;

		if (m_bDrawn)
		{
			region.left  = lastLeft;
			region.right = lastLeft + lastWidth;

			region.top    = lastTop;
			region.bottom = lastTop + lastHeight;

			PrepareErase(operation);

			if (region.left > m_left)
				region.left = m_left;

			if (region.right < m_left + Width())
				region.right = m_left + Width();

			if (region.top > m_top)
				region.top = m_top;

			if (region.bottom < m_top + Height())
				region.bottom = m_top + Height();
		}
		else
		{
			PrepareErase(operation);
			GetRECT(&region);
		}

		// Get background bitmap
		HDC hMemPadDC = env.GetBitPadDC(region.right - region.left, region.bottom - region.top);

		VERIFY(BitBlt(hMemPadDC, 0, 0, region.right - region.left,
			region.bottom - region.top, env.m_hScreenDC,
			region.left, region.top, SRCCOPY));
  
		if (m_bDrawn)
		{
			// Erase
			hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
			ASSERT(lastWidth <= m_bitmapWidth && lastHeight <= m_bitmapHeight);
			VERIFY(BitBlt(hMemPadDC, lastLeft - region.left, lastTop - region.top,
				lastWidth,
				lastHeight, env.m_hMemoryDC, 0, 0, DSTERASE));
			SelectObject(env.m_hMemoryDC, hOld);
		}


		PrepareDraw(operation);
 
		hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
		ASSERT(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
		VERIFY(BitBlt(hMemPadDC, m_left - region.left, m_top - region.top,
			Width(), Height(), env.m_hMemoryDC, 0, 0, SRCPAINT));
		SelectObject(env.m_hMemoryDC, hOld);

		VERIFY(BitBlt(env.m_hScreenDC, region.left, region.top,
			region.right - region.left, region.bottom - region.top, hMemPadDC, 0, 0, SRCCOPY));

		m_bDrawn = true;
	}
	else
	{
		// Flicker drawing routines
		PrepareErase(operation);

		if (m_bDrawn)
		{
			hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
			ASSERT(lastWidth <= m_bitmapWidth && lastHeight <= m_bitmapHeight);
 			VERIFY(BitBlt(env.m_hScreenDC, lastLeft, lastTop, lastWidth, lastHeight, env.m_hMemoryDC, 0, 0, DSTERASE));
			SelectObject(env.m_hMemoryDC, hOld);
		}

		PrepareDraw(operation);

		hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
		ASSERT(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
		VERIFY(BitBlt(env.m_hScreenDC, m_left, m_top, Width(), Height(), env.m_hMemoryDC, 0, 0, SRCPAINT));
		SelectObject(env.m_hMemoryDC, hOld);

		m_bDrawn = true;
	}

	SetErasePosition();
	newType = -1;
}


//////////////////////////////////////////////////////////////////////
// PrepareErase
//
//
void Biot::PrepareErase(int operation)
{
	switch (operation)
	{
		case GROW:
		{
			ratio--;
			stepEnergy = ((2 * adultBaseEnergy) / BaseRatio());
			totalDistance   = Symmetric(ratio);
			for (int i = 0; i < MAX_LINES; i++)
				state[i] = distance[i];
			childBaseEnergy = totalDistance * env.options.startEnergy;
			SetScreenRect();
			SetBonus();
			break;
		}

		case RECALCULATE:
		    Symmetric(ratio);
			SetScreenRect();
			SetBonus();
			break;

		default:
			break;
	}
}


//////////////////////////////////////////////////////////////////////
// CheckBitmapSize
//
// Cache bitmap unless size gets larger.  We only allocate
// new bitmaps when the size grows over the NextSize
//
void Biot::CheckBitmapSize(int width, int height)
{
	if (width > m_bitmapWidth || height > m_bitmapHeight)
	{
		FreeBitmaps();

		if (width > m_bitmapWidth)
			m_bitmapWidth = BRect::NextSize(width);

		if (height > m_bitmapHeight)
			m_bitmapHeight = BRect::NextSize(height);

		ASSERT(m_hBitmap == NULL);

		m_hBitmap = ::CreateCompatibleBitmap(env.m_hScreenDC, m_bitmapWidth, m_bitmapHeight);
	}
	ASSERT(m_hBitmap);
}


//////////////////////////////////////////////////////////////////////
// Erase - erases the biot if it was drawn
//
//
void Biot::Erase(void)
{
	if (m_bDrawn)
	{
		m_bDrawn = false;
		HBITMAP hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
		ASSERT(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
		VERIFY(BitBlt(env.m_hScreenDC, lastLeft, lastTop, Width(), Height(), env.m_hMemoryDC, 0, 0, DSTERASE));//SRCAND));
		SelectObject(env.m_hMemoryDC, hOld);
	}
}


//////////////////////////////////////////////////////////////////////
// SetErasePosition
//
// Remembers where we are so we can erase it.
//
void Biot::SetErasePosition(void)
{
	lastLeft = m_left;
	lastTop  = m_top;
}


//////////////////////////////////////////////////////////////////////
// FormBitmap
//
//
void Biot::FormBitmap(int pen)
{
	RECT    rc;
	int i;

	rc.left   = 0;
	rc.top    = 0;
	rc.right  = Width();
	rc.bottom = Height();

	CheckBitmapSize(rc.right, rc.bottom);

	lastType   = pen;

	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(env.m_hMemoryDC, m_hBitmap);
	::FillRect(env.m_hMemoryDC, &rc, (HBRUSH) GetStockObject( BLACK_BRUSH ));

	ASSERT(env.m_hMemoryDC);
	ASSERT(pen < LINE_MAX);
	HPEN hOldPen;

	if (env.BiotShouldBox(m_id))
	{
		hOldPen = (HPEN) ::SelectObject(env.m_hMemoryDC, env.options.hPen[LINE_HIGHLIGHT]);
		::MoveToEx(env.m_hMemoryDC, rc.left, rc.top, NULL);
		::LineTo(env.m_hMemoryDC, rc.right - 1, rc.top);
		::LineTo(env.m_hMemoryDC, rc.right - 1, rc.bottom - 1);
		::LineTo(env.m_hMemoryDC, rc.left, rc.bottom - 1);
		::LineTo(env.m_hMemoryDC, rc.left, rc.top);
		SelectObject(env.m_hMemoryDC, hOldPen);
	}

	if (pen > -1)
	{
		hOldPen = (HPEN) ::SelectObject(env.m_hMemoryDC, env.options.hPen[pen]);

		for (i = 0; i < genes; i++)
		{
			if (state[i] > 0)
			{
				//State should be zero if distance is
				ASSERT(distance[i] > 0);

				VERIFY(::MoveToEx(env.m_hMemoryDC, startPt[i].x - leftX, startPt[i].y - topY, NULL));
				VERIFY(::LineTo(env.m_hMemoryDC, stopPt[i].x - leftX, stopPt[i].y - topY));
			}
		}
		::SelectObject(env.m_hMemoryDC, hOldPen);
	}
	else
	{
		int aPen = -1;
		register int lastX = -999999;
		register int lastY = -999999;
		int startX;
		int startY;

		HPEN* hPen = &env.options.hPen[0];

		hOldPen = (HPEN) SelectObject(env.m_hMemoryDC, hPen[0]);
		for (i = 0; i < genes; i++)
		{
			if (state[i] > 0)
			{
				if (m_nSick)
				{
					aPen = m_nType[i] + LINES_SICK;
				}
				else
				{
					aPen = m_nType[i];

					if (state[i] != distance[i])
						aPen += LINES_DAMAGED;
				}

				if (aPen != pen)
				{
					pen = aPen;
					SelectObject(env.m_hMemoryDC, hPen[pen]);
				}

				startX = startPt[i].x - leftX;
				startY = startPt[i].y - topY;

				if (lastX != startX || lastY != startY)
					VERIFY(::MoveToEx(env.m_hMemoryDC, startX, startY, NULL));

				lastX = stopPt[i].x - leftX; lastY = stopPt[i].y - topY;
				VERIFY(::LineTo(env.m_hMemoryDC, lastX, lastY));
			}
		}
		::SelectObject(env.m_hMemoryDC, hOldPen);
	}
	::SelectObject(env.m_hMemoryDC, hOldBitmap);
}


/////////////////////////////////////////////////////////
// Reject
//
//
void Biot::Reject(int side)
{
	env.side[side]->RejectBiot(*this);
	vector.invertDeltaY();
	vector.invertDeltaX();
}


///////////////////////////////////////////////////////////////////////
// PurgeOldCollisions
//
// Since collisions may stick around turn to turn, those that are no
// longer in effect are removed during this step.
//
void Biot::PurgeOldCollisions(DWORD dwEnvTime)
{
	m_collision.PurgeOld(dwEnvTime);
}


///////////////////////////////////////////////////////////////////////
// CollisionDetection
//
// Collects information on every collision this biot has with other 
// biots and walls.
// 
void Biot::CollisionDetection(DWORD dwEnvTime)
{
	BRect rect;
	BRect erect;
	CLine line;
	CLine eline;
	Biot* enemy;
	BPoint pt;

	BRectSortPos pos;
	pos.FindRectsIntersecting(*this);

	// Enumerate biots with intersecting rectangles
	while ((enemy = env.HitCheck(this, pos)) != NULL)
    {
		CCollisionObject* pObject = NULL;
		bool bCollided = false;

		ASSERT(enemy != this);

		for (int nLine = 0; nLine < genes; nLine++)
		{
			if (state[nLine] > 0)
			{
				GetLineRect(nLine, rect);

				// If it isn't inside the enemy's overall rectangle - stop looking now
				if (enemy->Intersects(rect))
				{
					for (int eLine = 0; eLine < enemy->genes; eLine++)
					{
						//State of i can be effected within this loop
						if (enemy->state[eLine] > 0 && state[nLine] > 0)
						{
							enemy->GetLineRect(eLine, erect);

							// Do the lines rectangular regions cross
							if (rect.Intersects(erect))
							{
								GetLine(nLine, line);
								enemy->GetLine(eLine, eline);

								// Their rectangles may touch, but do the lines intersect!
								if (line.Intersect(eline, pt))
								{
									if (pObject == NULL)
									{
//										TRACE("Detection Biot %d ------------------------------------\n", m_id);
										pObject = m_collision.Record(dwEnvTime, enemy);
									}

//									TRACE("%d:%d colliding with %d:%d at (%d,%d) ", m_id, nLine, enemy->m_id, eLine, pt.x, pt.y);
									CCollisionPoint& point = pObject->Record(nLine, eLine);

									if (pObject->m_hits == 1)
									{
										CollisionVector(*enemy, pt, point);
										bCollided |= point.m_bCollide;
									}
									else
									{
										ArtificialCollision(*enemy, point);
									}
								}
							}
						}
					}
				}
			}
		}

		// If the first hit results in to real collisions then we still haven't collided
		// m_hits goes up every time we retrieve the record
		if (pObject && pObject->m_hits == 1 && !bCollided)
		{
			pObject->m_hits--;
		}
	}


	// Check for wall collisions
	if (!env.WithinBorders(*this))
	{
		CCollisionObject* pObject = NULL;

		for (int nLine = 0; nLine < genes; nLine++)
		{
			if (state[nLine] > 0)
			{
				GetLineRect(nLine, rect);

				// Check for wall collisions
				if (!env.WithinBorders(rect))
				{
					GetLine(nLine, line);

					if (env.IsIntersect(line, pt))
					{
						if (pObject == NULL)
							pObject = m_collision.Record(dwEnvTime, this);

//						TRACE("%d: Biot %d:%d colliding with wall at (%d,%d)", dwEnvTime, m_id, nLine, pt.x, pt.y);
						CCollisionPoint& point = pObject->Record(nLine, -1);
						if (pObject->m_hits == 1)
						{
							CollisionWall(pt, point);
						}
						else
						{
							ArtificialWall(point);
						}
					}
				}
			}
		}
	}

}


///////////////////////////////////////////////////////////////////////
// ArtificialCollision
//
//
void Biot::ArtificialCollision(Biot& enemy, CCollisionPoint& point)
{
	if (point.m_hits < 5)
	{
		point.m_dx = 0.0;
		point.m_dy = 0.0;
		point.m_dr = 0.0;

		if (point.m_hits == 1)
		{
			// Now caculate interaction even for artificial - as long as it is the first hit...
			point.m_bInteract = LineContact(enemy, point);
		}
		return;
	}

	BPoint ecenter;
	enemy.MassCenter(ecenter);

	BPoint center;
	MassCenter(center);

	int deltaX = center.x - ecenter.x;
	int deltaY = center.y - ecenter.y;
	double radius = vector.distance(deltaX, deltaY);
	double scale = 0.005 * double(point.m_hits);
	
	if (radius == 0)
	{
		// Must be concentric
		Randomizer rand;
		point.m_dx = 0.7 * rand.Sign() * scale;
		point.m_dy = 0.7 * rand.Sign() * scale;
		point.m_dr = 0.0;
	}
	else
	{
		point.m_dx = double(deltaX) * scale / radius;
		point.m_dy = double(deltaY) * scale / radius;
		point.m_dr = 0.0;
	}
	TRACE("ArtificialCollisionRate(%.3f, %.3f, %.3f)\n", point.m_dx, point.m_dy, point.m_dr);
}


///////////////////////////////////////////////////////////////////////
// CollisionVector
//
//
void Biot::CollisionVector(Biot& enemy, BPoint& pt, CCollisionPoint& point)
{
	// Calculate adjusted dx and dy for me
	double deltaX = double(pt.x - MassCenterX());
	double deltaY = double(pt.y - MassCenterY());
			
	double radius = vector.distance(deltaX, deltaY);
	ASSERT(radius < 1000);

	double DX, DY; 
	vector.RotatedDelta(DX, DY, deltaX, deltaY, radius);
	ASSERT(fabs(DX) < 1000);
	ASSERT(fabs(DY) < 1000);

	LimbRotatedDelta(pt.x, pt.y, point.m_nLine, DX, DY);

	// This step calculates the X and Y vector from this biot
	// at that point taking into consideration the
	// biots rotation and translational vectors

	// Calculate adjusted dx and dy for enemy
	double edeltaX = double(pt.x - enemy.MassCenterX());
	double edeltaY = double(pt.y - enemy.MassCenterY());

	double eradius = enemy.vector.distance(edeltaX, edeltaY);
	ASSERT(eradius < 1000);
			
	// This step calculates the X and Y vector from this biot
	// at that point taking into consideration the
	// biots rotation and translational vectors
	double eDX, eDY;
	enemy.vector.RotatedDelta(eDX, eDY, edeltaX, edeltaY, eradius);
	ASSERT(fabs(eDX) < 1000);
	ASSERT(fabs(eDY) < 1000);
	enemy.LimbRotatedDelta(pt.x, pt.y, point.m_eLine, eDX, eDY);

	// This step determines the effect of mass and calculates
	// the resultant vector to be imparted on each biot
	point.m_dx = vector.collisionResult(enemy.vector.mass, DX, eDX);
	point.m_dy = vector.collisionResult(enemy.vector.mass, DY, eDY);
	point.m_dr = vector.rotationComponent(radius, deltaX, deltaY, deltaX + point.m_dx, deltaY + point.m_dy);

	if (point.m_dr != 0)
	{
		double dv = vector.motionComponent(vector.distance(point.m_dx, point.m_dy), point.m_dr);
		point.m_dx = -vector.fraction(dv, deltaX, radius); 
		point.m_dy = -vector.fraction(dv, deltaY, radius); 
	}

	TRACE("CollisionVector(%.3f, %.3f, %.3f)\n", point.m_dx, point.m_dy, point.m_dr);

	// Now caculate interaction
	point.m_bInteract = LineContact(enemy, point);
}


///////////////////////////////////////////////////////////////////////
// CollisionWall
//
//
void Biot::CollisionWall(BPoint& pt, CCollisionPoint& point)
{
	// How long is the lever?
    double deltaX    = double(MassCenterX() - pt.x);
    double deltaY    = double(MassCenterY() - pt.y);
	double radius = vector.distance(deltaX, deltaY);

	// Determine the vector to apply by looking at our overall motion
	double dx, dy; 
	vector.RotatedDelta(dx, dy, deltaX, deltaY, radius);

	// Now add to this our relative limb motion
	LimbRotatedDelta(pt.x, pt.y, point.m_nLine, dx, dy);

	// Determine our resulting rotation from the collision at that point
    double dr = vector.rotationComponent(radius, deltaX, deltaY, deltaX + dx, deltaY + dy);
    if (dr != 0)
    {
		double dv = vector.motionComponent(vector.distance(dx, dy), dr);
		dx = vector.fraction(dv, deltaX, radius); 
		dy = vector.fraction(dv, deltaY, radius); 
    }
	point.m_dx = dx;
	point.m_dy = dy;
	point.m_dr = dr;
//	TRACE("CollisionWall(%.3f, %.3f, %.3f) for point(%d, %d)\n", point.m_dx, point.m_dy, point.m_dr, pt.x, pt.y);
}


///////////////////////////////////////////////////////////////////////
// ArtificialWall
//
//
void Biot::ArtificialWall(CCollisionPoint& point)
{
	point.m_dx = 0;
	point.m_dy = 0;
	point.m_dr = 0;

	if (m_top <= env.m_top)
		point.m_dy = 0.1;

	if (m_bottom >= env.m_bottom)
		point.m_dy = -0.1;
	
	if (m_left <= env.m_left)
		point.m_dx = 0.1;
	
	if (m_right >= env.m_right)
		point.m_dx = -0.1;

//	TRACE("ArtificialWall(%.3f, %.3f, %.3f)\n", point.m_dx, point.m_dy, point.m_dr);
}


///////////////////////////////////////////////////////////////////////
// Collide
//
//
void Biot::Collide()
{
	if (m_collision.GetSize() > 0)
	{
//		TRACE("Collide Biot %d --------------------------------------------------\n", m_id);
		vector.TraceDebug();

		VectorSum sum;
		VectorSum art;
		int collisions = 0;


		for (int nCol = 0; nCol < m_collision.GetSize(); nCol++)
		{
			CCollisionObject& object = m_collision[nCol];

			for(int i = 0; i < object.GetSize(); i++)
			{
				CCollisionPoint& point = object[i];
				if (CheckInteraction(point) == true)
				{
					if (object.m_hits > 1)
					{
						art.Add(point.m_dx, point.m_dy, point.m_dr);
					}
					else
					{
						ASSERT(object.m_hits != 0);
						sum.Add(point.m_dx, point.m_dy, point.m_dr);
					}
					collisions++;
				}
			}

			// If we are sick, make the enemy sick
			if (object.m_pBiot != this && collisions > 0 && m_nSick > 0)
			{
				if (!object.m_pBiot->m_nSick)
					object.m_pBiot->m_nSick = env.options.m_nSick;
			}
		}
		sum.Set(vector);
		art.Adjust(vector);
		vector.ValidateBounds(m_top < env.m_top, m_bottom > env.m_bottom, m_left < env.m_left, m_right > env.m_right);
	}
}


///////////////////////////////////////////////////////////////////////////////
// CheckInteraction
//
// Returns true if the collision should happen
//
bool Biot::CheckInteraction(CCollisionPoint& point)
{
	// Have we seen this collision before?
	if (point.m_bInteract)
	{
		energy += point.m_deltaEnergy;

		// Translate for flash color
		newType = env.options.newType[m_nType[point.m_nLine]];
		point.m_bInteract = false;

		//bool bCollide = 
		AdjustState(point.m_nLine, point.m_deltaLength);
//		ASSERT(point.m_bCollide == bCollide);
	}
	return point.m_bCollide;
}


///////////////////////////////////////////////////////////////////////
// LimbRotatedDelta
//
// Adds additional vector information based on a limb's relative motion
// starting from the segment colliding and working back to the origin.
//
void Biot::LimbRotatedDelta(int ptX, int ptY, int nLine, double& dx, double& dy)
{
	int nLimb = s_line2limb[nLine];
	int nSegment = s_line2segment[nLine];

	// This segment should be visible or something is not right
	ASSERT(trait.GetSegment(nLimb, nSegment).IsVisible());

	for (; nSegment >= 0; nSegment--)
	{
		GeneSegment& segment = trait.GetSegment(nLimb, nSegment);
		int nLine            = Line(nLimb, nSegment);

		if (segment.IsVisible())
		{
			ASSERT(state[nLine] > 0);
			int nAngleRate = m_angle[nLine] - m_angleDrawn[nLine];

			if (nAngleRate != 0)
			{
			    int deltaX    = ptX - x1(nLine);
			    int deltaY    = ptY - y1(nLine);
				double radius = vector.distance(deltaX, deltaY);

				// Current Velocity at this radius
				double Vr = RADIANS * radius * double(nAngleRate);

				dx += vector.deltaXr(Vr, deltaY, radius);
				dy += vector.deltaYr(Vr, deltaX, radius);
			}

			// This follows the reverse of symmetric
			if (segment.GetStart() < nSegment &&
				trait.GetSegment(nLimb, segment.GetStart()).IsVisible())
				nSegment = segment.GetStart();
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Move
//
// TODO: We don't check for collisions due to rotation directly.
// Although the rotation will affect the size of the rectangle for the
// next iteration.  Also, if lines are moved, the collision is not
// handled properly.  For instance, if a biot is perfectly still, and
// then moves its arm to hit another biot, it will be felt but the
// direction vector imparted to the other biot may not be right.
//
// Ideally, we would propose the new position and know whether it collided
// because of overall movement, or due to local arm movement.  One algorithm
// 
// Step 1:	Make move due to rotational and translation effects
// Step 2:  Check for collisions,
//		
//
// We could reduce the maximum rotation rate.  Thus changes in the rectangle
// size is limited.
//
// Step 1:  Look for wall collisions (don't make any move)
// Step 2:  Look for biot collisions
// Step 3:  Move and redraw biot
// Step 4:  Conduct brain and other functions
//
// BUG: Steps move significantly sometimes - appears that drawing doesn't
// happen for a while and then there is a big visual step...
void Biot::Move(void)
{
	// We must account for the center of mass in relation
	// to the origin.  We estimate the center of mass
	// to be the center of the bounding rectangle

	// Statistics
	m_age++;
	if ((m_age & 0x3F) == 0)
	{
		m_statEnergy[m_statIndex++] = PercentEnergy();
		if (m_statIndex >= MAX_ENERGY_HISTORY)
			m_statIndex = 0;
	}


	BPoint center;
	int dr = vector.tryRotate(origin, MassCenter(center));
	int dx = vector.tryStepX();
	int dy = vector.tryStepY();
	MoveBiot(dx, dy); 
	vector.makeStep();
	vector.friction(env.options.friction);

	bool bChangeSize;

	// If we die, we need to change size, or disappear
	// Handle the disappear case here.
	if (bDie)
	{
		genes -= 2;
		max_genes -= 2;
		if (genes <= 0)
		{
			env.PlayResource("PL.TooOld");
			Erase();
	        return;
		}
		bChangeSize = true;
	}
	else
	{
		// If we are not dying, we may need to add segments
		if (genes < max_genes && (m_age & 0x07) == 0x07)
		{
			genes += MAX_LINES/ MAX_SEGMENTS;
			bChangeSize = true;
		}
		else
		{
			bChangeSize = false;
		}
	}

	// Should we recalculate (top priority)
	if (redraw.ShouldRedraw() || dr)
	{
		EraseAndDraw(RECALCULATE);
	}
	else
	{
		if (bChangeSize)
		{
			EraseAndDraw(REFORM);
		}
		else
		{
			// Do we just need to reform the bitmap?
			if (lastType != newType || lastType != -1)
			{
				EraseAndDraw(REFORM);
			}
			else
			{
				if (dx || dy)
				{
					EraseAndDraw(NORMAL);
				}
			}
		}
	}


	if (m_nSick)
	{
		energy -= 2000;
		m_nSick--;

		if (!m_nSick)
			newType = -2;
	}
	else
	{
		energy += (turnBenefit - totalDistance);
		energy += (long) (m_dBonusRatio * turnBenefit);
	}


  // Add this turns cost
  // We are trying a method that has no cost
  //  energy += turnCost;

	// Did we die?
	if (energy <= 0 || totalDistance <= 0)
	{
		if (totalDistance <= 0 || energy >= 0)
			env.PlayResource("PL.Eaten");
		else
			env.PlayResource("PL.NoEnergy");

		Erase();
		return;
	}


	// Is it time to grow, die, or give birth?
	if ((m_age & 0x0F) == 0x0F)
	{
		CheckReproduction();

		if (ratio > trait.GetAdultRatio() && 
			energy > stepEnergy)
		{
			EraseAndDraw(GROW);
		}

		if (m_maxAge < m_age)
		{
			bDie = true;
		}
	}

	if (bInjured && (m_age & env.options.regenTime) == env.options.regenTime)
	{
		LONG regenEnergy  = childBaseEnergy >> 2;
		if (energy > regenEnergy)
		{
			bInjured = false;

			// Regenerate leaves
			for (int nLimb = 0; nLimb < MAX_LIMBS && energy > regenEnergy; nLimb++)
			{ 
				int j = nLimb;
				while (j < genes)
				{
					if (state[j] < distance[j]  && distance[j] > 0)
					{
						energy   -= env.options.regenCost; //env.leafRegen[m_nType[j]];
  
						state[j]++;
						bInjured = true;
						if (state[j] <= 0)
						{
							// we cant start growing the next branch unitl we are > 0
							break;
						}
						else
						{
							if (state[j] == distance[j] || state[j] == 1)
								newType = -2;

							if (m_nType[j] == LINE_LEAF)
								turnBenefit += env.options.m_leafEnergy;
      
							// How much does this effect our distance?
							colorDistance[m_nType[j]]++;
							totalDistance++;

							//!! We should change mass here
						}
					}
					j += MAX_LIMBS;
				}
			}
		}
	}

	Randomizer rand; 
	// Time to behave

	CBiotState sensor;

	sensor.SetInternalState(m_internalState);
	sensor.SetField(STATE_BIOT_MALE, trait.IsMale() != FALSE);
	sensor.SetField(STATE_BIOT_HUNGRY, energy < adultBaseEnergy);
	sensor.SetField(STATE_BIOT_SICK, m_nSick > 0);
	sensor.SetField(STATE_BIOT_STARVING, energy < adultBaseEnergy / 2);
	sensor.SetField(STATE_BIOT_DAMAGED, bInjured);
	sensor.SetField(STATE_BIOT_ADULT, (ratio == trait.GetAdultRatio()));
	sensor.SetField(STATE_BIOT_OLD,   (m_age * 2) > m_maxAge);
	sensor.SetField(STATE_BIOT_TOUCH, m_collision.GetSize() > 0);

	// Internal State in the last 8 bits only!!
	ASSERT((m_internalState & 0xFFFFFF00) == 0);

	//	TRACE("%d %d\n", m_id, m_internalState);
	for (int nLimb = 0; nLimb < MAX_LIMBS; nLimb++)
		m_store[nLimb].Execute(*this, sensor.GetState());
}


//////////////////////////////////////////////////////////////
// CheckReproduction
//
// 
void Biot::CheckReproduction()
{
	int i;

	// If it has enough energy, and is fertilized if sexual only -> reproduce
	if (energy >= (adultBaseEnergy << 1))
	{
		// Automatically lose excess energy above your base energy
		// Lose energy even if your children can't be placed - life is just not fair!
//		energy = adultBaseEnergy << 1;

		ASSERT(genes2 == 0 || trait.IsFemale());

		if (trait.IsFemale())
		{
			Biot *nBiot;
			int children = trait.GetNumberOfChildren();

			energy = adultBaseEnergy;

			for (i = 0; i < children; i++)
			{
				nBiot = new Biot(env);
				if (nBiot != NULL)
				{
					env.m_stats.m_births++;
					*nBiot = *this;

					if (!nBiot->PlaceNear(*this))
					{
						env.m_stats.m_deaths++;
						delete nBiot;
						break;
					}
					else
					{
						env.AddBiot(nBiot);
						nBiot->SetBonus();
					}
				}
			}

			if (i > 0)
			{
				env.PlayResource("PL.Birth");
				genes2 = 0;  
			}
		}
	}
}


 //////////////////////////////////////////////////////////////////////
//
// Copy operator =
//
// The copy operate automatically performs crossover, if genes2 is 
// filled in.
//
Biot& Biot::operator=(Biot& copyMe)
{
	ClearSettings();

	Randomizer rand;

	m_commandArray = copyMe.m_commandArray;

	trait = copyMe.trait;

	m_sName       = copyMe.m_sName;
	m_sWorldName  = copyMe.m_sWorldName;
	m_generation  = copyMe.m_generation + 1;

	if (copyMe.genes2)
	{
		trait.Crossover(copyMe.trait2);

		m_commandArray.Crossover(copyMe.m_commandArray2);

		if (rand.Bool())
		{
			m_generation = copyMe.m_fatherGeneration + 1;
			m_sName = copyMe.m_sFatherName;
			m_sWorldName = copyMe.m_sWorldName;
		}
//		env.PlayResource("PL.Mate");
	}

	trait.Mutate(env.options.chance);

	m_commandArray.Mutate(env.options.chance);

	max_genes = MAX_LINES;

	origin.x = copyMe.origin.x;
	origin.y = copyMe.origin.y;
        
	energy     = copyMe.adultBaseEnergy / copyMe.trait.GetNumberOfChildren();
	m_motherId = copyMe.m_id;
	m_fatherId = copyMe.m_mateId;

	copyMe.m_livingChildren++;
	copyMe.m_totalChildren++;

	trait.PickSex();


//  motionAge   = copyMe.motionAge - copyMe.age;
//  motionIndex = copyMe.motionIndex;
	Initialize();
	return *this;
}


///////////////////////////////////////////////////////////////////////////// 
// Determine the result of line contact
//
// Returns whether or not there was a meanful interaction
//
bool Biot::LineContact(Biot& enemy, CCollisionPoint& point) 
{
//int ePeno, int nPeno, int& delta, int& deltaEnergy)
	ASSERT(point.m_deltaLength == 0 && point.m_deltaEnergy == 0 && point.m_bInteract == false);

	// If the we are siblings or this is my child then we don't
	// mate or attack if our genetics say not to
	if (enemy.energy <= 0 || energy <= 0 ||
		(AreSiblings(enemy) && !SiblingsAttack(enemy)) ||
		(OneIsChild(enemy) && !AttackChildren(enemy)))
	{
		point.m_bCollide = true;
		return false;
	}

	int nPeno  = point.m_nLine;
	int nType  = m_nType[nPeno];
	int nState = state[nPeno];

	int ePeno  = point.m_eLine;
	int eType  = enemy.m_nType[ePeno];
	int eState = enemy.state[ePeno];

	// Is there an opportunity to Mate?
	if (nType == LINE_INJECTOR &&
		!enemy.trait.IsMale() &&
		enemy.ratio == enemy.trait.GetAdultRatio() &&
		SpeciesMatch(enemy.trait.GetSpecies()))
	{
		enemy.CopyGenes(*this);
		enemy.m_mateId = m_id;
		enemy.newType  = nType;
		env.PlayResource("PL.Mate");
		TRACE("Biot %d fertilized biot %d\n", m_id, enemy.m_id);
	}

	ContactLine& contact = env.Contact(nType, eType);
	switch (contact.m_nContact)
	{
	// Two lines touch but don't hurt each other
	case ContactLine::Ignore:
		point.m_bCollide = true;
		return false;

	// Our line gets eaten by their line!
	case ContactLine::Eaten:
		point.m_deltaLength = LengthLoss(nPeno, min(nState, contact.Weaken(eState)));
//		ASSERT(point.m_deltaLength <= totalDistance);
		//TODO:  Why does this assert fail so often!

		// We lose more energy than they get
		point.m_deltaEnergy = -(point.m_deltaLength * 2) * ((energy / totalDistance) + 1);

		// We can't lose more than we have
		if (energy + point.m_deltaEnergy < 0)
			point.m_deltaEnergy = -energy;

		point.m_bCollide = (point.m_deltaLength < nState);
		TRACE("Biot %d eaten by %d: Lost delta %d and lost %d energy\n", m_id, enemy.m_id, point.m_deltaLength, -point.m_deltaEnergy);
		break;
	
	// Our line gets damaged by their line (lose energy in proportion to the length loss)
	case ContactLine::Damaged:
		point.m_deltaLength = LengthLoss(nPeno, min(nState, contact.Weaken(eState)));
//		ASSERT(point.m_deltaLength <= totalDistance);
		//TODO:  Why does this assert fail so often!

		point.m_deltaEnergy = -point.m_deltaLength * ((energy / totalDistance) + 1);

		// We can't lose more than we have
		if (energy + point.m_deltaEnergy < 0)
			point.m_deltaEnergy = -energy;

		point.m_bCollide = (point.m_deltaLength < nState);
		TRACE("Biot %d damaged by %d: Lost delta %d and lost %d energy\n", m_id, enemy.m_id, point.m_deltaLength, -point.m_deltaEnergy);
		break;

	// Our line eats their line
	case ContactLine::Eat:
		point.m_deltaLength = enemy.LengthLoss(ePeno, min(eState, contact.Strengthen(nState)));
//		ASSERT(point.m_deltaLength <= totalDistance);
		//TODO:  Why does this assert fail so often!

		// Eat up to twice the percent length of the enemy, depending on how red you are
		point.m_deltaEnergy = (long) (/*PercentColor(LINE_MOUTH)*/ 0.75 * (double) (point.m_deltaLength * 2) * ((enemy.energy / enemy.totalDistance) + 1));

		// We can't eat more than they have
		if (point.m_deltaEnergy > enemy.energy)
			point.m_deltaEnergy = enemy.energy;

		point.m_bCollide = (point.m_deltaLength < eState);

		// We don't get damaged
		TRACE("Biot %d ate %d: Gained delta %d and gained %d energy\n", m_id, enemy.m_id, point.m_deltaLength, point.m_deltaEnergy);
		point.m_deltaLength = 0;
		break;

	// Our line damages their line (but gains no energy)
	case ContactLine::Damage:
		point.m_deltaLength = enemy.LengthLoss(ePeno, min(eState, contact.Strengthen(nState)));
//		ASSERT(point.m_deltaLength <= totalDistance);

		point.m_bCollide = (point.m_deltaLength < eState);

		TRACE("Biot %d damages %d\n", m_id, enemy.m_id);
		// Nothing happens to us, just the other biot.
		point.m_deltaLength = 0;
		break;

		// Red on red attacks can result in energy gain 
		// Was solely determined by the percent red of the enemy and yourself
		// Now it will be done on the biot which has the most red in percent
		// of total red  This will give larger biots a chance to add lines
		// without penalty

	// Our line damages theirs and their line damages us and someone might gain energy
	case ContactLine::Attack:
	{
		point.m_deltaLength = LengthLoss(nPeno, min(contact.Weaken(eState), nState));

		int eDeltaLength = enemy.LengthLoss(ePeno, min(contact.Strengthen(nState), eState));

//		double percentDifference = PercentColor(type) - enemy.PercentColor(enemyType);

		double percentDifference =	((double)(colorDistance[nType] - enemy.colorDistance[eType])) /
									((double)(colorDistance[nType] + enemy.colorDistance[eType]));						
		

		if (percentDifference > 0)
			point.m_deltaEnergy = (long) (percentDifference * (double) ((point.m_deltaLength * 2) * (enemy.energy / enemy.totalDistance) + 1));
		else
			point.m_deltaEnergy = (long) (percentDifference * (double) ((point.m_deltaLength * 2) * (energy / totalDistance) + 1));

		point.m_bCollide = (point.m_deltaLength < nState) && (eDeltaLength < eState);

		TRACE("Biot %d attacked by %d: Lost delta %d and lost %d energy\n", m_id, enemy.m_id, point.m_deltaLength, -point.m_deltaEnergy);
		break;
	}

	// Our line damages theirs and their line damages us and no one wins
	case ContactLine::Defend:
	{
		point.m_deltaLength = LengthLoss(nPeno, min(contact.Weaken(eState), nState));

		int eDeltaLength = enemy.LengthLoss(ePeno, min(contact.Strengthen(nState), eState));

//		double percentDifference = PercentColor(type) - enemy.PercentColor(enemyType);
		point.m_deltaEnergy = -point.m_deltaLength * ((energy / totalDistance) + 1);

		// We can't lose more than we have
		if (energy + point.m_deltaEnergy < 0)
			point.m_deltaEnergy = -energy;

		point.m_bCollide = (point.m_deltaLength < nState) && (eDeltaLength < eState);

		TRACE("Biot %d defending from %d: Lost delta %d and lost %d energy\n", m_id, enemy.m_id, point.m_deltaLength, -point.m_deltaEnergy);
		break;
	}

	}
	return true;
}


/////////////////////////////////////////////////////////
// AdjustState
//
// Returns true if a collision should happen
//
bool Biot::AdjustState(int nPeno, int delta)
{
	if (delta == 0)
		return true;

	if (delta > state[nPeno])
		delta = state[nPeno];

	state[nPeno]  -= delta;
	totalDistance -= delta;

	bInjured = true;

	// determine effect on energy production
	if (m_nType[nPeno] == LINE_LEAF)
		turnBenefit -= (delta * env.options.m_leafEnergy);

	// How much does this effect our distance?
	colorDistance[m_nType[nPeno]] -= delta;

	if (state[nPeno] <= 0)
	{
		state[nPeno] = -distance[nPeno];
		nPeno += MAX_LIMBS;
		while (nPeno < max_genes)
		{
			if (state[nPeno] > 0)
			{
				if (m_nType[nPeno] == LINE_LEAF)
					turnBenefit -= (state[nPeno] * env.options.m_leafEnergy);

				totalDistance -= state[nPeno];
				colorDistance[m_nType[nPeno]] -= state[nPeno];
			}
			state[nPeno] = -distance[nPeno];
			nPeno += MAX_LIMBS;
		}
		return false;
	}
	return true;
}


///////////////////////////////////////////////////////////////////////
// LengthLoss
//
// If the following physical line suffers damage, how much length will 
// be loss, accounting for damage done in the past.
//
// We assume state[nPeno] is positive
//
int Biot::LengthLoss(int nPeno, int delta)
{
	ASSERT(state[nPeno] > 0 && delta >= 0);

	int loss = min(delta, state[nPeno]);

	// Did we lose everything?
	if (loss == state[nPeno])
	{
		// Calculate how much line was actually lost
		// Put it doesn't actually lose the line here
		nPeno += MAX_LIMBS;
		while (nPeno < genes)
		{
			if (state[nPeno] > 0)
				loss += state[nPeno];

			nPeno += MAX_LIMBS;
		}
	}
	return loss;
}


//////////////////////////////////////////////////////////////////////
// InjectGenes
//
// If the emeny is a adult, and the right species and not a male,
// inject my genes if this is a while line.
// 
// Asexual males do not accept genes
// 
inline void Biot::InjectGenes(int type, Biot& enemy)
{
	if (type  == LINE_INJECTOR && 
		enemy.ratio == enemy.trait.GetAdultRatio() &&
		SpeciesMatch(enemy.trait.GetSpecies()) &&
		!enemy.trait.IsMale())
	{
		enemy.CopyGenes(*this);
		enemy.m_mateId = m_id;
		enemy.newType  = type;
		env.PlayResource("PL.Mate");
	}
}


//////////////////////////////////////////////////////////////////////
//
// ContactEnergy
//
//  
/*LONG Biot::ContactEnergy(int type, int etype, Biot* enemy)
{
LONG Energy;

  if (env.leafContact[type][etype] > 0)
  {
    // You gain a fraction of your base energy (no greater than the enemies)
    // This is further reduced by the % red you are
    Energy = ((childBaseEnergy / env.leafContact[type][etype]) * colorDistance[type]) / totalDistance;
    if (Energy > enemy->energy)
      Energy = enemy->energy;
  }
  else
  {
    // You lose a fraction of your enemies base energy 
    // This is further reduced by the % of the enemies color
    Energy = ((enemy->childBaseEnergy / env.leafContact[type][etype]) * enemy->colorDistance[etype]) / enemy->totalDistance;
  }  
  return Energy;
} */


//////////////////////////////////////////////////////////////////////
// LineContact
//
//  LINE_LEAF
//  LINE_SHIELD,
//  LINE_MOUTH,
//  LBLUE_LEAF,
//  LINE_INJECTOR,

/*LONG Biot::LineContact(int line, int eline, Biot* enemy, int* delta)
{
LONG contact     = env.options.leafContact[m_nType[line]][enemy->m_nType[eline]];
LONG energyTransfer;
double et;
//FIX
//double enemySize = ((double)enemy->totalDistance) / enemy->trait.GetLines();
//double yourSize  = ((double)totalDistance) / trait.GetLines();

double enemyTranslation = fabs(enemy->vector.getDeltaX()) + fabs(enemy->vector.getDeltaY()); 
double translation      = fabs(vector.getDeltaX()) + fabs(vector.getDeltaY());

//  translation      = vector.distance(vector.getDeltaX(), vector.getDeltaY());
//  enemyTranslation = vector.distance(enemy->vector.getDeltaX(), enemy->vector.getDeltaY());

  // Is he going to gain energy?
  if (contact > 0)
  {
    // We are going to gain energy, but how much?
    // We take the shortest line
    *delta = min(state[line], enemy->state[eline]);    

    // We assume the enemy will lose the length after his line
    *delta = enemy->LengthLoss(eline, *delta);

    if ((enemy->totalDistance - ((LONG)*delta)) <= 0)
      energyTransfer = enemy->energy;
    else
    {
      // The greater my relative size, the better
      // The chunk I bit off gains me energy related to the chunk percentage of the enemy
//      et = ((float)totalDistance / (float)(enemy->totalDistance + totalDistance) * ((float) contact * (float) enemy->energy * (float)(*delta) / (float) enemy->totalDistance));

//      et = ((double)totalDistance / (double)(enemy->totalDistance + totalDistance)) + 0.5;

//      if (enemySize + yourSize > 0)
//        et = (yourSize / (enemySize + yourSize)) + 0.5;
//      else
//        et = 1.0;

      et = ((double)enemy->energy * (double) (contact * (*delta)) / (double) enemy->totalDistance);

//      if ((translation + enemyTranslation) > 0)
//        et *= (0.5 + translation / (translation + enemyTranslation));

      if ((translation + enemyTranslation) > 0)
        et *= (0.5 + translation / (translation + enemyTranslation));

      energyTransfer = (LONG) et; //(contact * enemy->energy * ((LONG)*delta)) / enemy->totalDistance;
    }

    energyTransfer = (energyTransfer * 3)/ 4;

    *delta = 0;
  }
  else
  {
    // If your blue, you can half the impact of red
//    if (m_nType[line] == LINE_SHIELD && enemy->m_nType[eline] == LINE_MOUTH)
//      *delta = min(enemy->state[eline] >> 1, state[line]);
//    else
      // If your red, double the impact of blue
//      if (m_nType[line] == LINE_MOUTH && enemy->m_nType[eline] == LINE_SHIELD)
//        *delta = min(enemy->state[eline] << 1, state[line]);
//      else
//        if (m_nType[line] == LINE_SHIELD && enemy->m_nType[eline] == LINE_SHIELD)
//        {
//          *delta = min(enemy->state[eline] >> 1, state[line] >> 1);
//          if (*delta == 0)
//            (*delta)++;
//        }
//        else
          // I lose up to my own length or his length, which ever is shorter
          *delta = min(enemy->state[eline], state[line]);

    int totalLoss = LengthLoss(line, *delta);

    // We got a negative delta here and a totalDistance of zero
    // All lines in state were negative - it may be line zero was attacked
    // You pick your line, pick the enemies, possibly make your next line negative
    if ((totalDistance - ((LONG)totalLoss)) <= 0)
      energyTransfer = -energy;
    else
    {
      // The smaller my relative size, the bigger the loss
      // The larger the delta, as compared to my size, the worse I'm off
      et = ((double)energy * (double) (contact * (*delta)) / (double) totalDistance);

      // The faster my emeny, the more I lose
      if ((translation + enemyTranslation) > 0)
        et *= (0.5 + enemyTranslation / (translation + enemyTranslation));

      if (m_nType[line] == LINE_SHIELD)
        energyTransfer = 0;
      else
        energyTransfer = (LONG) et; //(contact * enemy->energy * ((LONG)*delta)) / enemy->totalDistance;
    }
//      energyTransfer = ((contact * energy * ((LONG)totalLoss)) / totalDistance);
  }
  return energyTransfer;
}*/


//////////////////////////////////////////////////////////////////////
// CopyGenes
//
//  
void Biot::CopyGenes(Biot& enemy)
{
	trait2 = enemy.trait;

	m_commandArray2 = enemy.m_commandArray;

	genes2 = enemy.genes;

	m_sFatherName      = enemy.m_sName;
	m_sFatherWorldName = enemy.m_sWorldName;
	m_fatherGeneration = enemy.m_generation;
}


//////////////////////////////////////////////////////////////////////
// Serialize
//
//
void Biot::Serialize(CArchive& ar)
{
	const BYTE archiveVersion = 11;
	long i;
	
	if (ar.IsLoading())
	{
		BYTE version;
		// Check version
		ar >> version;
		if (version != archiveVersion)
			AfxThrowArchiveException(CArchiveException::badSchema, _T("Biot"));
	}
	else
	{
		ar << archiveVersion;
	}

	// Store or load other objects
	trait.Serialize(ar);

	m_commandArray.Serialize(ar);
	for (i = 0; i < MAX_LIMBS; i++)
		m_store[i].Serialize(ar);

	trait2.Serialize(ar);

	m_commandArray2.Serialize(ar);

	vector.Serialize(ar);

	// Now handle biot level variables
	if (ar.IsLoading())
	{
		ar.Read((LPBYTE) state, sizeof(state));

		ar.Read((LPBYTE) m_retractDrawn, sizeof(m_retractDrawn));
		ar.Read((LPBYTE) m_retractRadius, sizeof(m_retractRadius));
		ar.Read((LPBYTE) m_retractSegment, sizeof(m_retractSegment));

		ar >> max_genes;
		ar >> genes;
		origin.Serialize(ar);
		ar >> energy;
		BoolArchive::Load(ar, bDie);
		ar >> m_id;
		ar >> m_motherId;
		ar >> genes2;
		ar >> stepEnergy;
		ar >> ratio;
		ar >> m_age;
		ar >> m_sName;
		ar >> m_sWorldName;
		ar >> m_sFatherName;
		ar >> m_sFatherWorldName;

		if (max_genes > MAX_LINES)
			max_genes = MAX_LINES;

		if (max_genes < 4)
			max_genes = 4;

		if (genes > max_genes)
			genes = max_genes;

		if (genes < 1)
			genes = 1;

		if (ratio > MAX_RATIO)
			ratio = MAX_RATIO;

		if (ratio < trait.GetAdultRatio())
			ratio = trait.GetAdultRatio();
	}
	else
	{
		ar.Write((LPBYTE)state, sizeof(state));

		ar.Write((LPBYTE) m_retractDrawn, sizeof(m_retractDrawn));
		ar.Write((LPBYTE) m_retractRadius, sizeof(m_retractRadius));
		ar.Write((LPBYTE) m_retractSegment, sizeof(m_retractSegment));

		ar << max_genes;
		ar << genes;
		origin.Serialize(ar);
		ar << energy;
		BoolArchive::Store(ar, bDie);
		ar << m_id;
		ar << m_motherId;
		ar << genes2;
		ar << stepEnergy;
		ar << ratio;
		ar << m_age;
		ar << m_sName;
		ar << m_sWorldName;
		ar << m_sFatherName;
		ar << m_sFatherWorldName;
	}
}


bool Biot::OnOpen()
{
	adultBaseEnergy = Symmetric(trait.GetAdultRatio()) * env.options.startEnergy;

	totalDistance   = Symmetric(ratio);
	childBaseEnergy = totalDistance * env.options.startEnergy;

	// Lets assume injury
	bInjured = true;

	SetScreenRect();
	SetBonus();

	m_id     = env.GetId();
	m_maxAge = trait.GetMaxAge();

//	if (env.WithinBorders(*this))
//	{
		FormBitmap();
		SetErasePosition();
//		return true;
//	}
	return false;
}


/////////////////////////////////////////////////////////////////////
// RetractSegment
//
// Retracts the tip segment on a particular limb.  
//
BYTE Biot::RetractSegment(int nSegment, int nLimb, int maxRadius)
{
   if (m_retractDrawn[nLimb] == m_retractRadius[nLimb] &&
	   m_retractDrawn[nLimb] < maxRadius)
   {
	   m_retractSegment[nLimb] = nSegment;
	   m_retractRadius[nLimb] += 1;
	   redraw.SetRedraw(true);
	   return (BYTE) 1;
   }
   return (BYTE) 0;
}


/////////////////////////////////////////////////////////////////////
// ExtendSegment
//
// Extends the tip segment on a particular limb.  
//
BYTE Biot::ExtendSegment(int nSegment, int nLimb)
{
   if (m_retractDrawn[nLimb] == m_retractRadius[nLimb] &&
	   m_retractDrawn[nLimb] > 0)
   {
	   m_retractSegment[nLimb] = nSegment;
	   m_retractRadius[nLimb] -= 1;
	   redraw.SetRedraw(true);
	   return (BYTE) 1;
   }
   return (BYTE) 0;
}


/////////////////////////////////////////////////////////////////////
// RetractLimbType
//
// Retracts all the limb tips on a biot for a particular limb type
//
// nSegment  - the segment of the limb
// nLimbType - which limb type?
// maxRadius - how long does this segment get for that limb type?
//
// Returns 1 or 0 for the distance retracted.
//
BYTE Biot::RetractLimbType(int nSegment, int nLimbType, int maxRadius)
{
	bool bOneLine = false;
	for (int i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
			if (m_retractDrawn[i] != m_retractRadius[i] ||
				m_retractDrawn[i] >= maxRadius)
				return 0;

			bOneLine = true;
		}
	}

	if (!bOneLine)
		return 0;

	for (i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
		   m_retractSegment[i] = nSegment;
		   m_retractRadius[i] += 1;
		}
	}
	redraw.SetRedraw(true);
	return (BYTE) 1;
}


/////////////////////////////////////////////////////////////////////
// ExtendLimbType
//
// Extends all the limb tips on a biot for a particular limb type
//
// Returns 1 or 0 for the distance extended.
//
BYTE Biot::ExtendLimbType(int nSegment, int nLimbType)
{
	bool bOneLine = false;
	for (int i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
			if (m_retractDrawn[i] != m_retractRadius[i] ||
				m_retractDrawn[i] <= 0)
				return 0;

			bOneLine = true;
		}
	}

	if (!bOneLine)
		return 0;

	for (i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
		   m_retractSegment[i] = nSegment;
		   m_retractRadius[i] -= 1;
		}
	}
	redraw.SetRedraw(true);
	return (BYTE) 1;
}


/////////////////////////////////////////////////////////////////////
// MaxAllowedRate
//
// nMaxRate should be set to s_nAngleRateLimit
// Finds the largest acceptable rate
//
inline void Biot::MaxAllowedRate(int& nMaxRate, int nDelta, int nRate)
{
	if (nRate < 0)
	{
		// If rate is negative the max allowed rate is 
		// bounded by -3 + 3 = 0, 3 + 3 = 6
		nMaxRate = min(nMaxRate, nDelta + s_nAngleRateLimit);
	}
	else
	{
		// If rate is positive the max allowed rate is 
		// bounded by 3 - -3 = 6, 3 - 3 = 0
		nMaxRate = min(nMaxRate, s_nAngleRateLimit - nDelta);
	}
}


/////////////////////////////////////////////////////////////////////
// MoveLimbTypeSegment
//
// Move a single segment on all the limbs of a particular type
// Returns the actual number of degrees moved.
//
int Biot::MoveLimbTypeSegment(int nSegment, int nLimbType, int nRate)
{
	ASSERT(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
	ASSERT(abs(nRate) <= s_nAngleRateLimit);
	ASSERT(nSegment < MAX_SEGMENTS && nSegment >= 0);

	// A rate of zero isn't too interesting
	if (nRate == 0)
		return 0;

	// Find the maximum rate allowed
	int nLimb;
	int nLine;
	int nMaxRate = min(s_nAngleRateLimit, abs(nRate));
	for (nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{
		if (trait.GetLineTypeIndex(nLimb) == nLimbType)
		{
			nLine = Line(nLimb, nSegment);
			MaxAllowedRate(nMaxRate, 
				m_angle[nLine] - m_angleDrawn[nLine],
				 trait.GetMirrorAngle(nRate, nLimb, nSegment));
		}
	}

	// A rate of zero isn't too interesting
	if (nMaxRate == 0)
		return 0;

	// nMaxRate is a positive representation of the max nRate allowed
	if (nRate < 0)
		nMaxRate = -nMaxRate;

	// Perform action
	for (nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{
		if (trait.GetLineTypeIndex(nLimb) == nLimbType)
		{
			nLine = Line(nLimb, nSegment);
			m_angle[nLine] += trait.GetMirrorAngle(nMaxRate, nLimb, nSegment);
			redraw.SetRedraw((m_angle[nLine] - m_angleDrawn[nLine]) != 0);
		}
	}
	return nMaxRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbTypeSegments
//
// Move all the segments on all the limbs of a particular type
//
int Biot::MoveLimbTypeSegments(int nLimbType, int nRate)
{
	ASSERT(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
	ASSERT(abs(nRate) <= s_nAngleRateLimit);

	// A rate of zero isn't too interesting
	if (nRate == 0)
		return 0;

	// Find the maximum rate allowed
	int nLimb;
	int nLine;
	int nSegment;
	int nMaxRate = min(s_nAngleRateLimit, abs(nRate));
	for (nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{
		if (trait.GetLineTypeIndex(nLimb) == nLimbType)
		{
			for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
			{
				nLine = Line(nLimb, nSegment);
				MaxAllowedRate(nMaxRate, 
				  m_angle[nLine] - m_angleDrawn[nLine],
				  trait.GetMirrorAngle(nRate, nLimb, nSegment));
			}
		}
	}

	// A rate of zero isn't too interesting
	if (nMaxRate == 0)
		return 0;

	// nMaxRate is a positive representation of the max nRate allowed
	if (nRate < 0)
		nMaxRate = -nMaxRate;

	// Perform action
	for (nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{
		if (trait.GetLineTypeIndex(nLimb) == nLimbType)
		{
			for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
			{
				nLine = Line(nLimb, nSegment);
				m_angle[nLine] += trait.GetMirrorAngle(nMaxRate, nLimb, nSegment);
				redraw.SetRedraw((m_angle[nLine] - m_angleDrawn[nLine]) != 0);
			}
		}
	}
	return nMaxRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbSegments
//
// Move all the segments of a specific limb
//
int Biot::MoveLimbSegments(int nLimb, int nRate)
{
	ASSERT(nLimb < MAX_LIMBS && nLimb >= 0);
	ASSERT(abs(nRate) <= s_nAngleRateLimit);

	// A rate of zero isn't too interesting
	if (nRate == 0)
		return 0;

	// Find the maximum rate allowed
	int nLine;
	int nSegment;
	int nMaxRate = min(s_nAngleRateLimit, abs(nRate));
	for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
	{
		nLine = Line(nLimb, nSegment);
		MaxAllowedRate(nMaxRate, 
			m_angle[nLine] - m_angleDrawn[nLine],
			trait.GetMirrorAngle(nRate, nLimb, nSegment));
	}

	// A rate of zero isn't too interesting
	if (nMaxRate == 0)
		return 0;

	// nMaxRate is a positive representation of the max nRate allowed
	if (nRate < 0)
		nMaxRate = -nMaxRate;

	// Perform action
	for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++)
	{
		nLine = Line(nLimb, nSegment);
		m_angle[nLine] += trait.GetMirrorAngle(nMaxRate, nLimb, nSegment);
		redraw.SetRedraw((m_angle[nLine] - m_angleDrawn[nLine]) != 0);
	}
	return nMaxRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbSegment
//
// Moves a segment on a limb
//
int Biot::MoveLimbSegment(int nSegment, int nLimb, int nRate)
{
	ASSERT(nLimb < MAX_LIMBS && nLimb >= 0);
	ASSERT(nSegment < MAX_SEGMENTS && nSegment >= 0);
	ASSERT(abs(nRate) <= s_nAngleRateLimit);

	// Find the maximum rate allowed
	int nLine = Line(nLimb, nSegment);
	int nMaxRate = min(s_nAngleRateLimit, abs(nRate));
	
	MaxAllowedRate(nMaxRate, 
		m_angle[nLine] - m_angleDrawn[nLine],
		trait.GetMirrorAngle(nRate, nLimb, nSegment));

	// nMaxRate is a positive representation of the max nRate allowed
	if (nRate < 0)
		nMaxRate = -nMaxRate;

	// Perform action
	m_angle[nLine] += trait.GetMirrorAngle(nMaxRate, nLimb, nSegment);

//	TRACE("Biot %d: MoveLimbSegment Line %d to angle %d\n", m_id, nLine, m_angle[nLine]);
	redraw.SetRedraw((m_angle[nLine] - m_angleDrawn[nLine]) != 0);

	return nMaxRate;
}


