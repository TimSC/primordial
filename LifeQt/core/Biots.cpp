//////////////////////////////////////////////////////////////////////
// Biot Class
//
//
//
#include "Etools.h"
#include "Genotype.h"
#include "Environ.h"
#include "Biots.h"
#include <iostream>

using namespace rapidjson;

//////////////////////////////////////////////////////////////////////
// Biot Class
//
//
//
//
//
Biot::Biot(Environment& environment) : env(environment)
{
  max_genes = 1;
  ClearSettings();
  graphics = new QGraphicsItemGroup();
  boundingRect = nullptr;
  assert(env.m_scene != nullptr);
  env.m_scene->addItem(graphics);

}

Biot::~Biot(void)
{
    for(size_t i = 0; i < lines.size(); i ++)
    {
        //Remove unneeded line
        env.m_scene->removeItem(lines[i]);
        lines[i] = nullptr;
    }
    lines.clear();

    env.m_scene->removeItem(graphics);

//	FreeBitmaps();
}


//////////////////////////////////////////////////////////////////////
// GetName
//
std::string Biot::GetName()
{
    static const std::string vowels = ("aeiouy");
    static const std::string cons   = ("bcdfghjklmnpqrstvwx");

	Randomizer rand;
    if (m_sName.empty())
	{
        int max = 1 + rand.Integer(3);

		for (int i = 0; i < max; i++)
		{
            if (rand.Bool())
			{
                m_sName += vowels[rand.Integer(vowels.length())];
                m_sName += cons[rand.Integer(vowels.length())];
			}
			else
			{
                m_sName += cons[rand.Integer(vowels.length())];
                m_sName += vowels[rand.Integer(vowels.length())];
			}
		}

        if (rand.Bool())
            m_sName += vowels[rand.Integer(vowels.length())];

        m_sName[0] = (char) toupper(m_sName[0]);
	}
	return m_sName;
}


//////////////////////////////////////////////////////////////////////
// SetFirstName
//
void Biot::SetName(std::string sName)
{
	m_sName = sName;
}

//////////////////////////////////////////////////////////////////////
// GetFatherName
//
std::string Biot::GetFatherName()
{
    std::string sName;
	if (m_fatherId == 0)
	{
        sName = "No";
	}
	else
	{
		if (m_fatherGeneration > 0)
        {
            QString sNameTmp = QString::asprintf(":%lu", m_fatherGeneration);
            sName = sNameTmp.toStdString();
        }
        sName = m_sFatherName + sName;

        if (!m_sFatherWorldName.empty())
            sName += " of " + m_sFatherWorldName;
	}
    return sName;
}

//////////////////////////////////////////////////////////////////////
// GetFullName
//
std::string Biot::GetFullName()
{
    std::string sName;
	if (m_generation > 0)
	{
        QString sNameTmp = QString::asprintf(":%lu", m_generation);
        sName = sNameTmp.toStdString();
	}
	sName = GetName() + sName;

    if (!GetWorldName().empty())
        sName += " of " + GetWorldName();

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
/*
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
*/

//////////////////////////////////////////////////////////////////////
// Clear Settings
//
//
void Biot::ClearSettings(void)
{
    bDie             = false;
	genes            = MAX_SYMMETRY; // We start out showing a little
	genes2           = 0;
	m_fatherId       = 0;
	m_mateId         = 0;
	m_livingChildren = 0;
	m_totalChildren  = 0;
	m_generation     = 0;

//	m_hBitmap        = NULL;
	m_bitmapWidth  = 0;
	m_bitmapHeight = 0;
	m_nSick        = 0;
	lastType       = BLACK_LEAF;
	newType        = -2;
	lastLeft       = lastTop   = 0;
	ratio          = 1;
	m_age          = 0;
	
    bInjured     = false;
	ClearCollisions();
    m_bDrawn     = false;
    m_bSelected  = false;

    for(int i=0; i<MAX_GENES; i++)
        m_angle[i] = 0;
    memset(m_angleLimbType,              0x00, sizeof(m_angleLimbType));
    memset(m_angleLimbTypeDrawn,         0x00, sizeof(m_angleLimbTypeDrawn));
    memset(m_angleLimbTypeSegmentDrawn,  0x00, sizeof(m_angleLimbTypeSegmentDrawn));
    memset(m_angleLimbTypeSegment,       0x00, sizeof(m_angleLimbTypeSegment));
    memset(m_angleDrawn,                 0x00, sizeof(m_angleDrawn));
    memset(m_angleLimb,                  0x00, sizeof(m_angleLimb));
    memset(m_angleLimbDrawn,             0x00, sizeof(m_angleLimbDrawn));

    memset(m_retractDrawn,               0x00, sizeof(m_retractDrawn));
    memset(m_retractRadius,              0x00, sizeof(m_retractRadius));

	for (int i = 0; i < MAX_SYMMETRY; i++)
		m_retractSegment[i] = -1;

    memset(state,    0x00, sizeof(state));

	// Statistics
    memset(m_statEnergy, 0x00, sizeof(m_statEnergy));
	m_statIndex = 0;

	int nPeno = 0;
	for (int nGene = 0; nGene < MAX_SEGMENTS; nGene++)
		for (int nLine = 0; nLine < MAX_SYMMETRY; nLine++)
		{
            geneNo[nPeno]   = (uint8_t) nGene;
            lineNo[nPeno++] = (uint8_t) nLine;
		}
}  


//////////////////////////////////////////////////////////////////////
// RandomCreate - creates a biot randomly
//
//
int Biot::RandomCreate(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
	max_genes = MAX_GENES;

	genes = MAX_GENES; // We show all of the biot on creation

	//TODO: Switch to Randomize
	trait.Randomize(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);
//	trait.Debug(nArmsPerBiot, nTypesPerBiot, nSegmentsPerArm);

	int i;

	m_commandArray.Randomize();

	// Initially we have no parent
	m_motherId = 0;

	vector.setDeltaY(0); 
	vector.setDeltaX(0);
	vector.setDeltaRotate(0);

    Initialize(true);

	i = PlaceRandom();
	SetBonus();

	GetName();

    this->graphics->setPos(this->origin);
    this->graphics->setRotation(this->vector.GetR()-90.0);

	return i;
}


//////////////////////////////////////////////////////////////////////
// Mutate
//
//
void Biot::Mutate(int chance)
{
	int i;

	max_genes = MAX_GENES;

	genes = MAX_GENES; // We show all of the biot on creation

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
	for (i = 0; i < MAX_GENES; i++)
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
	adultBaseEnergy = Symmetric(trait.GetAdultRatio()) * env.options.startEnergy;
	if (bRandom || energy <= 0)
		energy = adultBaseEnergy;

	SetRatio();
	totalDistance   = Symmetric(ratio);

	childBaseEnergy = totalDistance * env.options.startEnergy;
  
	// If not loaded, we need a new ID
	m_Id     = env.GetID();
	m_maxAge = trait.GetMaxAge();

	// Set a fully charged initial state
    for (int i = 0; i < MAX_GENES; i++)
		state[i] = distance[i];

    for (int i = 0; i < MAX_SYMMETRY; i++)
    {
        int nLimbType = trait.GetLineTypeIndex(i);
        assert(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
        m_store[i].Initialize(nLimbType, i, *this);
    }

	return 0;
}


//////////////////////////////////////////////////////////////////////
//
// PlaceRandom
//
//
int Biot::PlaceRandom(void)
{
	int i;

	Randomizer rand;


	for (i = 0; i < 24; i++)
	{
		//BUG: Value passed in might be zero, causing divide error in Integer
        origin.setX(rand.Integer((env.Width()) + leftX - rightX)  - leftX);
        origin.setY(rand.Integer((env.Height()) + topY  - bottomY) - topY);
		SetScreenRect();

//		BRectSortPos pos;
//		pos.
		BRectSortPos pos;
		pos.FindRectsIntersecting(*this);

		if (env.HitCheck(this, pos) == NULL)
			break;
		// TODO:  Are we even sorted at this time?
	}
       
  if (i > 8)
    env.NoRoomToGiveBirth();

  vector.setX(origin.x());
  vector.setY(origin.y());

  UpdateGraphics();
  SetErasePosition();
  return i;
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

      origin.setX(parent.origin.x() + parent.Width() * side[nPos][0]);
      origin.setY(parent.origin.y() + parent.Height() * side[nPos][1]);
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
  env.NoRoomToGiveBirth();
  return false;
}


//////////////////////////////////////////////////////////////////////
// Symmetric
//
//
int64_t Biot::Symmetric(int aRatio)
{
int64_t  dist    = 0;
double X,Y;
int i, nPeno;
//int nSet = 0;
//bool bFirstLine = true;
int  nLastGene = -1;

	colorDistance[GREEN_LEAF]   = 
	colorDistance[RED_LEAF  ]   =
	colorDistance[BLUE_LEAF ]   =
	colorDistance[WHITE_LEAF]   = 
	colorDistance[LBLUE_LEAF]   = 0;

	leftX   =
	topY    =
	rightX  =
	bottomY = 0;

	turnBenefit    = 0;
	redraw.ClearRedraw();

    memset(distance, 0x00, sizeof(distance));
    memset(stopPt,   0x00, sizeof(stopPt));
    memset(startPt,  0x00, sizeof(startPt));
    memset(nType,    0x00, sizeof(nType));

	for (int nLimb = 0; nLimb < trait.GetLines(); nLimb++)
	{	
		nLastGene = -1;
		int lineType    = trait.GetLineTypeIndex(nLimb);
		short nTypeAngle  = m_angleLimbType[lineType];
		short nLineAngle  = m_angleLimb[nLimb];

		m_angleLimbTypeDrawn[lineType] = nTypeAngle;
		m_angleLimbDrawn[nLimb]        = nLineAngle;

		X = 0.0;
		Y = 0.0;

        for (int nGene = 0; nGene < MAX_SEGMENTS; nGene++)
		{
			m_angleLimbTypeSegmentDrawn[lineType][nGene] = m_angleLimbTypeSegment[lineType][nGene];

			GeneSegment& segment = trait.GetSegment(nLimb, nGene);

			if (segment.IsVisible())
			{
				nPeno = nLimb + nGene * MAX_SYMMETRY;
				m_angleDrawn[nPeno] = m_angle[nPeno];

				if (nLastGene < 0)
				{
                    startPt[nPeno].setX(0);
                    startPt[nPeno].setY(0);
				}
				else
				{
					//TODO: Branching involves setting new start points
//					startPt[nPeno] = stopPt[nPeno - ((nGene - nLastGene) * MAX_SYMMETRY)];
					if (segment.GetStart() < nGene &&
						trait.GetSegment(nLimb, segment.GetStart()).IsVisible())
						startPt[nPeno] = stopPt[nPeno - ((nGene - segment.GetStart()) * MAX_SYMMETRY)];
					else
						startPt[nPeno] = stopPt[nPeno - ((nGene - nLastGene) * MAX_SYMMETRY)];


				}
				nLastGene = nGene;
				
				double radius = segment.GetRadius();

				if (nGene == m_retractSegment[nLimb])
				{
					radius -= (double) m_retractRadius[nLimb];
					m_retractDrawn[nLimb] = m_retractRadius[nLimb];
				}

				distance[nPeno] = Translate(segment.GetRadius(), X, Y,
					trait.GetAngle(nLimb, nGene) + 
					vector.getRotate()           + 
					m_angleDrawn[nPeno]            +
					trait.GetCompressedToggle(nTypeAngle, nLimb, nGene) +
					nLineAngle +
					trait.GetCompressedToggle(m_angleLimbTypeSegment[lineType][nGene], nLimb, nGene),
					aRatio);

				// Pass doubles in and out to keep track of the fractional portion
				// We are getting lots of jitter during rotation.  To smooth it out
				// we must pass on the fraction portion for each limb
                stopPt[nPeno].setX(startPt[nPeno].x() + int(X));
                stopPt[nPeno].setY(startPt[nPeno].y() + int(Y));

				X -= int(X);
				Y -= int(Y);

                if (stopPt[nPeno].x() < leftX)
                    leftX = stopPt[nPeno].x();

                if (stopPt[nPeno].x() > rightX)
                    rightX = stopPt[nPeno].x();

                if (stopPt[nPeno].y() < topY)
                    topY = stopPt[nPeno].y();

                if (stopPt[nPeno].y() > bottomY)
                    bottomY = stopPt[nPeno].y();

				dist += distance[nPeno];

				nType[nPeno] = segment.GetColor(trait.IsMale());

				if (nType[nPeno] == WHITE_LEAF && (!trait.IsMale() || trait.IsAsexual()))
					nType[nPeno] = GREEN_LEAF;
       
				if (nType[nPeno] == GREEN_LEAF)
					turnBenefit += (env.options.m_leafEnergy * distance[nPeno]);

				colorDistance[nType[nPeno]] += distance[nPeno];
 			}
		}
	}

    vector.setMass(0.0);

	for (i = GREEN_LEAF; i <= WHITE_LEAF; i++)
        vector.addMass(colorDistance[i] * env.options.leafMass[i]);

	return dist;
}

//***********************************************************************
// Translate                                                           
//
// Takes a line and scales/rotates it. Returns the new radius and
// starting coordinates.
//
inline short Biot::Translate(double radius, double& newX, double& newY, int degrees, int aRatio)
{
static double scale[MAX_RATIO] = {0.70, 0.76, 0.84, 0.92,
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

	return (short) (radius + .5);
}



/////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 
// Distance                                                           
//
// d=square root of (x1 - x2) sqaured + (y1 - y2) squared
//
int64_t Biot::Distance(QPoint& start, QPoint& stop)
{
double d = double(((start.x() - stop.x()) * (start.x() - stop.x())) +
                    ((start.y() - stop.y()) * (start.y() - stop.y())));
  return (int64_t) sqrt(d);
}


//////////////////////////////////////////////////////////////////////
// Draw 
//
// Draws the biot during a WM_PAINT
//
//
/*
void Biot::Draw(void)
{
	HBITMAP hOld;

	hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap); 
    assert(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
	VERIFY(BitBlt(env.m_hScreenDC, m_left, m_top, Width(), Height(), env.m_hMemoryDC, 0, 0, SRCPAINT));
	SelectObject(env.m_hMemoryDC, hOld);

	SetErasePosition();
    m_bDrawn = true;
}
*/

//////////////////////////////////////////////////////////////////////
// EraseAndDraw - draws the biot
//
//
/*
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
            assert(lastWidth <= m_bitmapWidth && lastHeight <= m_bitmapHeight);
			VERIFY(BitBlt(hMemPadDC, lastLeft - region.left, lastTop - region.top,
				lastWidth,
				lastHeight, env.m_hMemoryDC, 0, 0, DSTERASE));
			SelectObject(env.m_hMemoryDC, hOld);
		}


		PrepareDraw(operation);
 
		hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
        assert(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
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
            assert(lastWidth <= m_bitmapWidth && lastHeight <= m_bitmapHeight);
 			VERIFY(BitBlt(env.m_hScreenDC, lastLeft, lastTop, lastWidth, lastHeight, env.m_hMemoryDC, 0, 0, DSTERASE));
			SelectObject(env.m_hMemoryDC, hOld);
		}

		PrepareDraw(operation);

		hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
        assert(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
		VERIFY(BitBlt(env.m_hScreenDC, m_left, m_top, Width(), Height(), env.m_hMemoryDC, 0, 0, SRCPAINT));
		SelectObject(env.m_hMemoryDC, hOld);

        m_bDrawn = true;
	}

	SetErasePosition();
	newType = -1;
}
*/

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
			for (int i = 0; i < MAX_GENES; i++)
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
/*
void Biot::CheckBitmapSize(int width, int height)
{
	if (width > m_bitmapWidth || height > m_bitmapHeight)
	{
		FreeBitmaps();

		if (width > m_bitmapWidth)
			m_bitmapWidth = BRect::NextSize(width);

		if (height > m_bitmapHeight)
			m_bitmapHeight = BRect::NextSize(height);

        assert(m_hBitmap == NULL);

		m_hBitmap = ::CreateCompatibleBitmap(env.m_hScreenDC, m_bitmapWidth, m_bitmapHeight);
	}
    assert(m_hBitmap);
}
*/

//////////////////////////////////////////////////////////////////////
// Erase - erases the biot if it was drawn
//
//
/*
void Biot::Erase(void)
{
	if (m_bDrawn)
	{
        m_bDrawn = false;
		HBITMAP hOld = (HBITMAP) SelectObject(env.m_hMemoryDC, m_hBitmap);
        assert(Width() <= m_bitmapWidth && Height() <= m_bitmapHeight);
		VERIFY(BitBlt(env.m_hScreenDC, lastLeft, lastTop, Width(), Height(), env.m_hMemoryDC, 0, 0, DSTERASE));//SRCAND));
		SelectObject(env.m_hMemoryDC, hOld);
	}
}
*/

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
// UpdateGraphics
//
//

void Biot::UpdateGraphics()
{
    QRect rc(0, 0, Width(), Height());

	if (env.BiotShouldBox(m_Id))
	{
        if (boundingRect == nullptr)
            boundingRect = new QGraphicsRectItem(rc, graphics);
        else
            boundingRect->setRect(rc);
    }
    else if (boundingRect != nullptr)
    {
        env.m_scene->removeItem(boundingRect);
        boundingRect = nullptr;
    }

    size_t lineCount = 0;

    if (m_nSick and false)
	{
        /*
		if (pen > -1)
			hOldPen = (HPEN) ::SelectObject(env.m_hMemoryDC, env.options.hPen[pen]);
		else
			hOldPen = (HPEN) ::SelectObject(env.m_hMemoryDC, env.options.hPen[PURPLE_LEAF]);
*/
        for (int i = 0; i < genes; i++)
		{
			if (state[i] > 0)
			{
				//State should be zero if distance is
                assert(distance[i] > 0);


/*
				VERIFY(::MoveToEx(env.m_hMemoryDC, startPt[i].x - leftX, startPt[i].y - topY, NULL));
				VERIFY(::LineTo(env.m_hMemoryDC, stopPt[i].x - leftX, stopPt[i].y - topY));
                */
			}
		}
//		::SelectObject(env.m_hMemoryDC, hOldPen);
	}
	else
	{
/*		int aPen = -1;
        int lastX = -999999;
        int lastY = -999999;
        int startX = 0;
        int startY = 0;

//		hOldPen = (HPEN) SelectObject(env.m_hMemoryDC, env.options.hPen[0]);*/
        for (int i = 0; i < genes; i++)
		{
			if (state[i] > 0)
			{
                short aPen = nType[i];

				if (state[i] != distance[i])
					aPen += DIM_COLOR;
/*
				if (aPen != pen)
				{
					pen = aPen;
					SelectObject(env.m_hMemoryDC, env.options.hPen[pen]);
                }*/

                if (m_nSick)
                    aPen = PURPLE_LEAF;

                //startX = startPt[i].x() - leftX;
                //startY = startPt[i].y() - topY;

                //if (lastX != startX || lastY != startY)
                //	VERIFY(::MoveToEx(env.m_hMemoryDC, startX, startY, NULL));

                //lastX = stopPt[i].x() - leftX; lastY = stopPt[i].y() - topY;
                //VERIFY(::LineTo(env.m_hMemoryDC, lastX, lastY));

                QGraphicsLineItem *line = nullptr;
                if(lineCount >= lines.size())
                {
                    line = new QGraphicsLineItem(startPt[i].x(), startPt[i].y(), stopPt[i].x(), stopPt[i].y(), graphics);
                    lines.append(line);
                }
                else
                {
                    line = lines[lineCount];
                    line->setLine(startPt[i].x(), startPt[i].y(), stopPt[i].x(), stopPt[i].y());
                }
                line->setPen(env.options.pens[aPen]);
                lineCount += 1;
			}
		}
//		::SelectObject(env.m_hMemoryDC, hOldPen);
	}

    for(size_t i = lineCount; i < lines.size(); i ++)
    {
        //Remove unneeded line
        env.m_scene->removeItem(lines[i]);
        lines[i] = nullptr;
    }
    size_t originalNumLines = lines.size();
    for(size_t i = lineCount; i < originalNumLines; i ++)
    {
        lines.removeLast();
    }

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


//////////////////////////////////////////////////////////////////////
// WallBounce
//
// Use the center of the rectangle as the center of gravity
// Translate origin to effect true
//
// Point (x, y) represent the collision point
//
void Biot::WallBounce(int x, int y)
{
	// How long is the lever?
//    int deltaX = x - origin.x;
//    int deltaY = y - origin.y;
    int deltaX    = x - CenterX();
    int deltaY    = y - CenterY();
	double radius = vector.distance(deltaX, deltaY);

	// Determine the vector to apply by looking at our motion
    double dx, dy; 
    vector.RotatedDelta(dx, dy, deltaX, deltaY, radius);

	// Determine our resulting rotation from the collision at that point
    double dr = vector.rotationComponent((double)deltaX, (double)deltaY, deltaX + dx, deltaY + dy);
    if (dr != 0)
    {
		double dv = vector.motionComponent(vector.distance(dx, dy), dr);
		dx = vector.fraction(dv, deltaX, radius); 
		dy = vector.fraction(dv, deltaY, radius); 

    }
    
	// Put Validate Movement back in.  Perhaps it
	// could keep sticky collisions from happening.
//    ValidateBorderMovement(dx, dy);//JJS!
    vector.setDeltaX(-dx);
    vector.setDeltaY(-dy);
    vector.setDeltaRotate(-dr);
}


///////////////////////////////////////////////////////////////////////
//
// ValidateBorderMovement()  JJS!
//
void Biot::ValidateBorderMovement(double& dx, double& dy)
{
  // If we are over the top, and heading north, go south
  if (m_top <= env.m_top)
  {
    if (dy <= 0)
      dy = -dy;

    // Encourage migration back into the field
    if (dy < 0.1)
      dy = 0.1;
  }

  if (m_bottom >= env.m_bottom)
  {
    if (dy >= 0)
      dy = -dy;

    // Encourage migration back into the field
    if (dy > -0.1)
     dy = -0.1;
  }

  
  if (m_left <= env.m_left)
  {
    if (dx <= 0)
      dx = -dx;

    // Encourage migration back into the field
    if (dx < 0.1)
      dx = 0.1;
  }

  if (m_right >= env.m_right)
  {
    if (dx >= 0)
      dx = -dx;

     // Encourage migration back into the field
    if (dx > -0.1)
      dx = -0.1;
  }
}


/////////////////////////////////////////////////////////////////////
// Motion
//
void Biot::Motion(const double deltaX, const double deltaY, double Vx, double Vy, const double radius)
{
	double dr = vector.rotationComponent(deltaX, deltaY, deltaX + Vx, deltaY + Vy);
	if (dr != 0)
	{
		double dv = vector.motionComponent(vector.distance(Vx, Vy), dr);
		Vx = vector.fraction(dv, (int) deltaX, radius); 
		Vy = vector.fraction(dv, (int) deltaY, radius);
		dr=-dr;
	}

	vector.setDeltaX(Vx);
	vector.setDeltaY(Vy);
	vector.setDeltaRotate(dr);
}


//////////////////////////////////////////////////////////////////////
// Move
//
//TODO: When we calculate the rotation using the center, we need to
//effectively rotate it around the center of the rectangle, not the 
// center of the biot.  However, we are not doing this, but in error
// are applying the rotation determined from the center to the origin.
//
bool Biot::Move(void)
{
Biot* enemy;
int i;
BRect lineRect;
CLine cLine;

	m_age++;

//    vector.accelerateY(.5);
    QPoint center;

	// We must account for the center of mass in relation
	// to the origin.  We estimate the center of mass
	// to be the center of the bounding rectangle
	int dr = vector.tryRotate(origin, Center(center));
	int dx = vector.tryStepX();
	int dy = vector.tryStepY();


	// Statistics
	if ((m_age & 0x3F) == 0)
	{
		m_statEnergy[m_statIndex++] = PercentEnergy();
		if (m_statIndex >= MAX_ENERGY_HISTORY)
			m_statIndex = 0;
	}

	//TODO: Move doesn't take into account rotation!!
	//TODO: Sometimes huge rotation steps are taken!!
    // Make a move
    MoveBiot(dx, dy);

    // Is the biot trying to leave the environment?
	if (!env.WithinBorders(*this))  //IsContainedBy(env))    // JJS!
	{
		for (i = 0; i < genes; i++)
		{
			if (state[i] > 0)
			{
				// Create the line
				lineRect.Set(x1(i), y1(i), x2(i), y2(i));

				// If it isn't inside the enemy's overall rectangle - stop looking now
				if (!env.WithinBorders(lineRect))
				{
					cLine.Set(x1(i), y1(i), x2(i), y2(i));
					int x, y;

					if (env.IsIntersect(cLine, x, y))
					{
						WallBounce(x, y);

						MoveBiot(-dx, -dy);

						dr = vector.tryRotate(origin, Center(center));
						dx = vector.tryStepX();
						dy = vector.tryStepY();

						MoveBiot(dx, dy);

//						RequestNextEvent(GeneEvent::EVENT_WALL, lineNo[i]); 
						break;
					}
					else
					{
						double tempDX = vector.getDeltaX();
						double tempDY = vector.getDeltaY();

                        assert(tempDX < 1000 && tempDX > -1000);

						ValidateBorderMovement(tempDX, tempDY);

						vector.setDeltaX((float)tempDX);
						vector.setDeltaY((float)tempDY);

						MoveBiot(-dx, -dy);
						dr = vector.tryRotate(origin, Center(center));
						dx = vector.tryStepX();
						dy = vector.tryStepY();

						MoveBiot(dx, dy);
						break;
					}
				}
			}
		}
    }


	// Does this biot's gross rectangular region cross anothers?
	i = 0;
	int x, y;
	BRectSortPos pos;
	pos.FindRectsIntersecting(*this);

	while ((enemy = env.HitCheck(this, pos)) != NULL)
    {
		// Yes it does, but does it actually touch?
		if (Contacter(enemy, dx, dy, x, y))
		{
			int him = FindCollision(enemy->m_Id);
            int me = 0;

			// Take a step back
			MoveBiot(-dx, -dy);

			env.m_stats.m_collisionCount++;

			// We found him
			if (him < MAX_COLLISIONS)
			{
				collider[him].setSeen(m_age);
				if (collider[him].addHit() > 1)
				{
					float boost = (float) 0;

                    if (enemy->origin.x() > origin.x())
						boost = (float)-0.05;

                    if (enemy->origin.x() < origin.x())
						boost = (float)0.05;

					vector.adjustDeltaX(boost);
					enemy->vector.adjustDeltaX(-boost);

					boost = (float)0;
                    if (enemy->origin.y() > origin.y())
						boost = (float)-0.05;

                    if (enemy->origin.y() < origin.y())
						boost = (float)0.05;

					vector.adjustDeltaY(boost);
					enemy->vector.adjustDeltaY(-boost);

					dx = vector.tryStepX();
					dy = vector.tryStepY();

					MoveBiot(dx, dy); 
				}
			}
			else
			{
				// New guy
				him = AddCollision(enemy->m_Id);
				if (him < MAX_COLLISIONS)
				{                      
					collider[him].setId(enemy->m_Id);
					collider[him].setSeen(m_age);
					me = enemy->AddCollision(m_Id);

					if (me < MAX_COLLISIONS)
					{
						enemy->collider[me].setId(m_Id);

						// Calculate adjusted dx and dy for me
						int deltaX = x - CenterX();//origin.x;
						int deltaY = y - CenterY(); //origin.y;
						double radius = vector.distance(deltaX, deltaY);
                        assert(radius < 1000);

						// This step calculates the X and Y vector from this biot
						// at that point taking into consideration the
						// biots rotation and translational vectors
						double DX, DY; 
						vector.RotatedDelta(DX, DY, deltaX, deltaY, radius);
                        assert(DX < 1000 && DX > -1000);
                        assert(DY < 1000 && DY > -1000);

						// Calculate adjusted dx and dy for enemy
						int edeltaX = enemy->CenterX() - x;//enemy->origin.x - x;
						int edeltaY = enemy->CenterY() - y;//enemy->origin.y - y;
						double eradius = enemy->vector.distance(edeltaX, edeltaY);
                        assert(eradius < 1000);
           
						// This step calculates the X and Y vector from this biot
						// at that point taking into consideration the
						// biots rotation and translational vectors
                        double eDX = 0.0, eDY = 0.0;
						enemy->vector.RotatedDelta(eDX, eDY, edeltaX, edeltaY, eradius);
                        assert(eDX < 1000 && eDX > -1000);
                        assert(eDY < 1000 && eDY > -1000);

						// This step determines the effect of mass and calculates
						// the resultant vector to be imparted on each biot
						double Vx = vector.collisionResult(enemy->vector.mass, DX, eDX);
						double Vy = vector.collisionResult(enemy->vector.mass, DY, eDY);

						double eVx = enemy->vector.collisionResult(vector.mass, eDX, DX);
						double eVy = enemy->vector.collisionResult(vector.mass, eDY, DY);

						// We have the resultant collision vector, now we need to
						// break it into a rotational vector and X and Y translational vectors.
						enemy->Motion(edeltaX, edeltaY, eVx, eVy, eradius);

                        assert(DX < 1000 && DX > -1000);
                        assert(DY < 1000 && DY > -1000);
						Motion(deltaX, deltaY, Vx, Vy, radius);
              
						dx = vector.tryStepX();
						dy = vector.tryStepY();
						MoveBiot(dx, dy); 
					}
				} 
			}
		}
    }

	RemoveCollisions(m_age);

	vector.makeStep();
//	vector.friction(env.options.friction);

	// Time to behave
	for (i = 0; i < MAX_SYMMETRY; i++)
		m_store[i].Execute(*this, 0xFFFFFFFF);

    bool bChangeSize = false;

	// If we die, we need to change size, or disappear
	// Handle the disappear case here.
	if (bDie)
	{
		genes -= 2;
		max_genes -= 2;
		if (genes <= 0)
		{
			env.PlayResource("PL.TooOld");
            //Erase();
            return false;
		}
        bChangeSize = true;
	}
	else
	{
		// If we are not dying, we may need to add segments
		if (genes < max_genes && (m_age & 0x07) == 0x07)
		{
			genes += MAX_GENES/ MAX_SEGMENTS;
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
        //EraseAndDraw(RECALCULATE);
	}
	else
	{
		if (bChangeSize)
		{
            //EraseAndDraw(REFORM);
		}
		else
		{
			// Do we just need to reform the bitmap?
			if (lastType != newType || lastType != -1)
			{
                //EraseAndDraw(REFORM);
			}
			else
			{
				if (dx || dy)
				{
                    //EraseAndDraw(NORMAL);
				}
			}
		}
	}


	if (m_nSick)
	{
		energy -= 2000;
        m_nSick = 0;

		if (!m_nSick)
			newType = -2;
	}
	else
	{
		energy += (turnBenefit - totalDistance);
        energy += (int64_t) (m_dBonusRatio * turnBenefit);
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

        //Erase();
        return false;
	}


	// Is it time to grow, die, or give birth?
	if ((m_age & 0x0F) == 0x0F)
	{
		CheckReproduction();

		if (ratio > trait.GetAdultRatio() && 
			energy > stepEnergy)
		{
            //EraseAndDraw(GROW);
		}

		if (m_maxAge < m_age)
		{
            bDie = true;
		}
	}

	if (bInjured && (m_age & env.options.regenTime) == env.options.regenTime)
	{
        int64_t regenEnergy  = childBaseEnergy >> 2;
		if (energy > regenEnergy)
		{
            bInjured = false;

			// Regenerate leaves
			for (i = 0; i < MAX_SYMMETRY && energy > regenEnergy; i++)
			{ 
				int j = i;
				while (j < genes)
				{
					if (state[j] < distance[j]  && distance[j] > 0)
					{
						energy   -= env.options.regenCost; //env.leafRegen[nType[j]];
  
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

							if (nType[j] == GREEN_LEAF)
								turnBenefit += env.options.m_leafEnergy;
      
							// How much does this effect our distance?
							colorDistance[nType[j]]++;
							totalDistance++;

							//!! We should change mass here
						}
					}
					j += MAX_SYMMETRY;
				}
			}
		}
	}

    UpdateGraphics();
    this->graphics->setPos(this->origin);
    this->graphics->setRotation(this->vector.GetR()-90.0);

    return true;
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

		if ((genes2 > 0 && !trait.IsMale()) ||
				trait.IsAsexual())
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


////////////////////////////////////////////////////////////////
// FindCollision
//
//
int Biot::FindCollision(int enemyId)
{
  for (int i = 0; i < MAX_COLLISIONS; i++)
    if (collider[i].id == enemyId)
      return i;

  return MAX_COLLISIONS;
}


////////////////////////////////////////////////////////////////
// AddCollision
//
//
int Biot::AddCollision(int /*enemyId*/)
{
  for (int i = 0; i < MAX_COLLISIONS; i++)
    if (collider[i].id == -1)
      return i;

  return MAX_COLLISIONS;
}


////////////////////////////////////////////////////////////////
// RemoveCollisions
//
//
void Biot::RemoveCollisions(uint32_t age)
{
  for (int i = 0; i < MAX_COLLISIONS; i++)
    if (!collider[i].wasSeen(age))
      collider[i].id = -1;
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

	max_genes = MAX_GENES;

    origin.setX(copyMe.origin.x());
    origin.setY(copyMe.origin.y());
        
	energy     = copyMe.adultBaseEnergy / copyMe.trait.GetNumberOfChildren();
	m_motherId = copyMe.m_Id;
	m_fatherId = copyMe.m_mateId;

	copyMe.m_livingChildren++;
	copyMe.m_totalChildren++;

	trait.PickSex();


//  motionAge   = copyMe.motionAge - copyMe.age;
//  motionIndex = copyMe.motionIndex;
	Initialize();
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Contacter
//
// Determines the effect of collisions.
//
// Returns the number of contacts with another biot.
//
int Biot::Contacter(Biot* enemy, int dx, int dy, int& x, int& y)
{
	BRect line;
	BRect eline;
	CLine e;
	CLine l;
	int i, j;
	int nContacts = 0;
	int setX, setY;


	for (i = 0; i < genes; i++)
	{
		if (state[i] > 0)
		{
			// Create the line
			line.Set(x1(i), y1(i), x2(i), y2(i));

			// If it isn't inside the enemy's overall rectangle - stop looking now
			if (enemy->Touches(line))
			{
				for (j= 0; j < enemy->genes; j++)
				{
					//State of i can be effected within this loop
					if (enemy->state[j] > 0 && state[i] > 0)
					{
						eline.Set(enemy->x1(j), enemy->y1(j), enemy->x2(j), enemy->y2(j));

						// Do the lines rectangular regions cross
						if (line.Touches(eline))
						{
							l.Set(x1(i) + dx, y1(i) + dy, x2(i) + dx, y2(i) + dy);
							e.Set(enemy->x1(j), enemy->y1(j), enemy->x2(j), enemy->y2(j));

							// There rectangles may touch, but do the lines intersect!
							if (l.Intersect(e, setX, setY))
							{
								short delta, enemyDelta;
                                int64_t  deltaEnergy, enemyDeltaEnergy;

                                bool bInteract = ContactLine(*enemy, j, i, delta, deltaEnergy);
								bInteract |= enemy->ContactLine(*this, i, j, enemyDelta, enemyDeltaEnergy);

								if (bInteract)
								{
									enemy->energy +=  enemyDeltaEnergy;
                                    bool bNoContact = enemy->AdjustState(j, enemyDelta);

									// Translate for flash color
									enemy->newType = env.options.newType[enemy->nType[j]];

									energy += deltaEnergy;
									bNoContact |= AdjustState(i, delta);

									// Translate for flash color
									newType = env.options.newType[nType[i]];

									if (bNoContact)
										nContacts--;
								}

//								ChangeColorEvent(i, enemy->nType[j]);
//								enemy->ChangeColorEvent(j, nType[i]);

								if (m_nSick)
								{
									if (!enemy->m_nSick)
										enemy->m_nSick = env.options.m_nSick;
								}
								else
								{
									if (enemy->m_nSick)
										m_nSick = env.options.m_nSick;
								}
								nContacts++;
								x = setX;
								y = setY;
							}
						}
					}
				}
			}
		}
    }
	return nContacts;
}


bool Biot::ContactLine(Biot& enemy, int nEnemyPeno, int nPeno, short& delta, int64_t& deltaEnergy)
{
	delta = 0;
	deltaEnergy = 0;

	if (enemy.energy <= 0 || energy <= 0 ||
		(AreSiblings(enemy) && !SiblingsAttack(enemy)) ||
		(OneIsChild(enemy) && !AttackChildren(enemy)))
        return false;

	int type      = nType[nPeno];
	int enemyType = enemy.nType[nEnemyPeno];

	// Mating?
	if (type == WHITE_LEAF &&
		!enemy.trait.IsMale() &&
		enemy.ratio == enemy.trait.GetAdultRatio() &&
		SpeciesMatch(enemy.trait.GetSpecies()))
	{
		enemy.CopyGenes(*this);
		enemy.m_mateId = m_Id;
		enemy.newType  = type;
		env.PlayResource("PL.Mate");
	}

	switch (env.options.leafContact[type][enemyType])
	{
	case CONTACT_IGNORE:
        return false;

	case CONTACT_EAT:
        delta = LengthLoss(nPeno, std::min(enemy.state[nEnemyPeno], state[nPeno]));
        assert(delta <= totalDistance);

		// Eat up to twice the percent length of the enemy, depending on how red you are
        deltaEnergy = (int64_t) (PercentColor(RED_LEAF) * (double) (delta << 1) * ((enemy.energy / enemy.totalDistance) + 1));

		if (deltaEnergy > enemy.energy)
			deltaEnergy = enemy.energy;

		delta = 0;
		break;

 	case CONTACT_EATEN:
        delta = LengthLoss(nPeno, std::min(enemy.state[nEnemyPeno], state[nPeno]));
        assert(delta <= totalDistance);
	
		deltaEnergy = (delta << 1) * ((energy / totalDistance) + 1);

		if (deltaEnergy > energy)
			deltaEnergy = energy;

		deltaEnergy = -deltaEnergy;
		break;
	
	case CONTACT_DESTROY:
		// Nothing happens to us, just the other biot.
		break;

		// Red on red attacks can result in energy gain 
		// Was solely determined by the percent red of the enemy and yourself
		// Now it will be done on the biot which has the most red in percent
		// of total red  This will give larger biots a chance to add lines
		// without penalty
	case CONTACT_ATTACK:
	{
        delta = LengthLoss(nPeno, std::min(enemy.state[nEnemyPeno], state[nPeno]));

//		double percentDifference = PercentColor(type) - enemy.PercentColor(enemyType);

		double percentDifference =	((double)(colorDistance[RED_LEAF] - enemy.colorDistance[RED_LEAF])) /
									((double)(colorDistance[RED_LEAF] + enemy.colorDistance[RED_LEAF]));						
		
		if (percentDifference > 0)
            deltaEnergy = (int64_t) (percentDifference * (double) ((delta << 1) * (enemy.energy / enemy.totalDistance) + 1));
		else
            deltaEnergy = (int64_t) (percentDifference * (double) ((delta << 1) * (energy / totalDistance) + 1));
		break;
	}

		// Double defense for blue
	case CONTACT_DEFEND:
		delta = LengthLoss(nPeno, minShort((enemy.state[nEnemyPeno] + 1) >> 1, state[nPeno]));
		break;

	case CONTACT_DEFENDED:
		delta = LengthLoss(nPeno, minShort(enemy.state[nEnemyPeno] << 1, state[nPeno]));
		break;

	case CONTACT_DESTROYED:
        delta = LengthLoss(nPeno, std::min(enemy.state[nEnemyPeno], state[nPeno]));
        assert(delta <= totalDistance);
		break;
	}
    return true;
}


/////////////////////////////////////////////////////////
// AdjustState
//
//
bool Biot::AdjustState(int nPeno, short delta)
{
	if (delta > state[nPeno])
		delta = state[nPeno];

	state[nPeno] -= delta;
	totalDistance -= delta;

    bInjured = true;

	// determine effect on energy production
	if (nType[nPeno] == GREEN_LEAF)
		turnBenefit -= (delta * env.options.m_leafEnergy);

	// How much does this effect our distance?
	colorDistance[nType[nPeno]] -= delta;

	if (state[nPeno] <= 0)
	{
		state[nPeno] = -distance[nPeno];
		nPeno += MAX_SYMMETRY;
		while (nPeno < max_genes)
		{
			if (state[nPeno] > 0)
			{
				if (nType[nPeno] == GREEN_LEAF)
					turnBenefit -= (state[nPeno] * env.options.m_leafEnergy);

				totalDistance -= state[nPeno];
				colorDistance[nType[nPeno]] -= state[nPeno];
			}
			state[nPeno] = -distance[nPeno];
			nPeno += MAX_SYMMETRY;
		}
        return true;
	}
    return false;
}


///////////////////////////////////////////////////////////////////////
// LengthLoss
//
// If the following physical line suffers damage, how much length will 
// be loss, accounting for damage done in the past.
//
// We assume state[nPeno] is positive
//
short Biot::LengthLoss(int nPeno, short delta)
{
    short loss = std::min(delta, state[nPeno]);

	if (loss == state[nPeno])
	{
		nPeno += MAX_SYMMETRY;
		while (nPeno < genes)
		{
			if (state[nPeno] > 0)
				loss += state[nPeno];

			nPeno += MAX_SYMMETRY;
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
	if (type  == WHITE_LEAF && 
		enemy.ratio == enemy.trait.GetAdultRatio() &&
		SpeciesMatch(enemy.trait.GetSpecies()) &&
		!enemy.trait.IsMale())
	{
		enemy.CopyGenes(*this);
		enemy.m_mateId = m_Id;
		enemy.newType  = type;
		env.PlayResource("PL.Mate");
	}
}


//////////////////////////////////////////////////////////////////////
//
// ContactEnergy
//
//  
/*int64_t Biot::ContactEnergy(int type, int etype, Biot* enemy)
{
int64_t Energy;

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

#if 0 //Unused?
//////////////////////////////////////////////////////////////////////
// LineContact
//
//  GREEN_LEAF
//  BLUE_LEAF,
//  RED_LEAF,
//  LBLUE_LEAF,
//  WHITE_LEAF,

int64_t Biot::LineContact(int line, int eline, Biot* enemy, short* delta)
{
int64_t contact     = env.options.leafContact[nType[line]][enemy->nType[eline]];
int64_t energyTransfer;
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
    *delta = std::min(state[line], enemy->state[eline]);

    // We assume the enemy will lose the length after his line
    *delta = enemy->LengthLoss(eline, *delta);

    if ((enemy->totalDistance - ((int64_t)*delta)) <= 0)
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

      energyTransfer = (int64_t) et; //(contact * enemy->energy * ((int64_t)*delta)) / enemy->totalDistance;
    }

    energyTransfer = (energyTransfer * 3)/ 4;

    *delta = 0;
  }
  else
  {
    // If your blue, you can half the impact of red
//    if (nType[line] == BLUE_LEAF && enemy->nType[eline] == RED_LEAF)
//      *delta = min(enemy->state[eline] >> 1, state[line]);
//    else
      // If your red, double the impact of blue
//      if (nType[line] == RED_LEAF && enemy->nType[eline] == BLUE_LEAF)
//        *delta = min(enemy->state[eline] << 1, state[line]);
//      else
//        if (nType[line] == BLUE_LEAF && enemy->nType[eline] == BLUE_LEAF)
//        {
//          *delta = min(enemy->state[eline] >> 1, state[line] >> 1);
//          if (*delta == 0)
//            (*delta)++;
//        }
//        else
          // I lose up to my own length or his length, which ever is shorter
          *delta = std::min(enemy->state[eline], state[line]);

    short totalLoss = LengthLoss(line, *delta);

    // We got a negative delta here and a totalDistance of zero
    // All lines in state were negative - it may be line zero was attacked
    // You pick your line, pick the enemies, possibly make your next line negative
    if ((totalDistance - ((int64_t)totalLoss)) <= 0)
      energyTransfer = -energy;
    else
    {
      // The smaller my relative size, the bigger the loss
      // The larger the delta, as compared to my size, the worse I'm off
      et = ((double)energy * (double) (contact * (*delta)) / (double) totalDistance);

      // The faster my emeny, the more I lose
      if ((translation + enemyTranslation) > 0)
        et *= (0.5 + enemyTranslation / (translation + enemyTranslation));

      if (nType[line] == BLUE_LEAF)
        energyTransfer = 0;
      else
        energyTransfer = (int64_t) et; //(contact * enemy->energy * ((int64_t)*delta)) / enemy->totalDistance;
    }
//      energyTransfer = ((contact * energy * ((int64_t)totalLoss)) / totalDistance);
  }
  return energyTransfer;
}
#endif

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

void  Biot::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    const uint8_t archiveVersion = 11;
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("archiveVersion", Value(archiveVersion), allocator);

    // Store or load other objects
    Value traitJson(kObjectType);
    trait.SerializeJson(d, traitJson);
    v.AddMember("trait", traitJson, allocator);

    Value commJson(kObjectType);
    m_commandArray.SerializeJson(d, commJson);
    v.AddMember("m_commandArray", commJson, allocator);

    Value storeArrJson(kArrayType);
    for (int i = 0; i < MAX_SYMMETRY; i++)
    {
        Value storeJson(kObjectType);
        m_store[i].SerializeJson(d, storeJson, *this);
        storeArrJson.PushBack(storeJson, allocator);
    }
    v.AddMember("m_store", storeArrJson, allocator);

    Value trait2Json(kObjectType);
    trait2.SerializeJson(d, trait2Json);
    v.AddMember("trait2", trait2Json, allocator);

    Value comm2Json(kObjectType);
    m_commandArray2.SerializeJson(d, comm2Json);
    v.AddMember("m_commandArray2", comm2Json, allocator);

    Value vectorJson(kObjectType);
    vector.SerializeJson(d, vectorJson);
    v.AddMember("vector", vectorJson, allocator);

    //short    state[MAX_GENES];
    Value stateArray(kArrayType);
    for(int i=0; i<MAX_GENES; i++)
        stateArray.PushBack(state[i], allocator);
    v.AddMember("state", stateArray, allocator);

    Value m_retractDrawnArray(kArrayType);
    Value m_retractRadiusArray(kArrayType);
    Value m_retractSegmentArray(kArrayType);
    for(int i=0; i<MAX_SYMMETRY; i++)
    {
        m_retractDrawnArray.PushBack(m_retractDrawn[i], allocator);
        m_retractRadiusArray.PushBack(m_retractRadius[i], allocator);
        m_retractSegmentArray.PushBack(m_retractSegment[i], allocator);
    }
    v.AddMember("m_retractDrawn", m_retractDrawnArray, allocator);
    v.AddMember("m_retractRadius", m_retractRadiusArray, allocator);
    v.AddMember("m_retractSegment", m_retractSegmentArray, allocator);

    v.AddMember("max_genes", max_genes, allocator);
    v.AddMember("genes", genes, allocator);
    v.AddMember("origin_x", origin.x(), allocator);
    v.AddMember("origin_y", origin.y(), allocator);
    v.AddMember("energy", energy, allocator);
    v.AddMember("bDie", bDie, allocator);
    v.AddMember("m_Id", m_Id, allocator);
    v.AddMember("m_motherId", m_motherId, allocator);
    v.AddMember("genes2", genes2, allocator);
    v.AddMember("stepEnergy", stepEnergy, allocator);
    v.AddMember("ratio", ratio, allocator);
    v.AddMember("m_age", m_age, allocator);
    v.AddMember("m_sName", Value(m_sName.c_str(), allocator), allocator);
    v.AddMember("m_sWorldName", Value(m_sWorldName.c_str(), allocator), allocator);
    v.AddMember("m_sFatherName", Value(m_sFatherName.c_str(), allocator), allocator);
    v.AddMember("m_sFatherWorldName", Value(m_sFatherWorldName.c_str(), allocator), allocator);
}

void Biot::SerializeJsonLoad(const rapidjson::Value& v)
{

    const uint8_t archiveVersion = 11;
    assert (v["archiveVersion"].GetInt() == archiveVersion);

    // Store or load other objects
    trait.SerializeJsonLoad(v["trait"]);

    m_commandArray.SerializeJsonLoad(v["m_commandArray"]);

    const Value &st = v["m_store"];
    for (int i = 0; i < st.Size(); i++)
        m_store[i].SerializeJsonLoad(st[i]);

    trait2.SerializeJsonLoad(v["trait2"]);

    m_commandArray2.SerializeJsonLoad(v["m_commandArray2"]);

    vector.SerializeJsonLoad(v["vector"]);

    // Now handle biot level variables
    const Value &stateJson = v["state"];
    for(int i=0; i<stateJson.Size(); i++)
        state[i] = stateJson[i].GetInt();

    const Value &retractDrawn = v["m_retractDrawn"];
    const Value &retractRadius = v["m_retractRadius"];
    const Value &retractSegment = v["m_retractSegment"];

    for(int i=0; i<retractDrawn.Size(); i++)
        m_retractDrawn[i] = retractDrawn[i].GetInt();
    for(int i=0; i<retractRadius.Size(); i++)
        m_retractRadius[i] = retractRadius[i].GetInt();
    for(int i=0; i<retractSegment.Size(); i++)
        m_retractSegment[i] = retractSegment[i].GetInt();

    max_genes = v["max_genes"].GetInt();
    genes = v["genes"].GetInt();
    origin.setX(v["origin_x"].GetInt());
    origin.setY(v["origin_y"].GetInt());
    energy = v["energy"].GetInt();
    bDie = v["bDie"].GetBool();
    m_Id = v["m_Id"].GetInt();
    m_motherId = v["m_motherId"].GetInt();
    genes2 = v["genes2"].GetInt();
    stepEnergy = v["stepEnergy"].GetInt();
    ratio = v["ratio"].GetInt();
    m_age = v["m_age"].GetInt();
    m_sName = v["m_sName"].GetString();
    m_sWorldName = v["m_sWorldName"].GetString();
    m_sFatherName = v["m_sFatherName"].GetString();
    m_sFatherWorldName = v["m_sFatherWorldName"].GetString();

    if (max_genes > MAX_GENES)
        max_genes = MAX_GENES;

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

bool Biot::OnOpen()
{
	adultBaseEnergy = Symmetric(trait.GetAdultRatio()) * env.options.startEnergy;

	totalDistance   = Symmetric(ratio);
	childBaseEnergy = totalDistance * env.options.startEnergy;

	// Lets assume injury
    bInjured = true;

	SetScreenRect();
	SetBonus();

	m_Id     = env.GetID();
	m_maxAge = trait.GetMaxAge();

//	if (env.WithinBorders(*this))
//	{
    UpdateGraphics();
    SetErasePosition();
//		return true;
//	}
    return false;
}

/////////////////////////////////////////////////////////
// MoveArm
//
// Moves a biots are starting at one segment and
// moving outward.  Returns true if the arm
// should be redrawn.
//
void Biot::MoveArm(int nPeno, short degree)
{
	for (int i = nPeno; i < MAX_GENES; i += MAX_SYMMETRY)
	{
		IncreaseAngle(i, degree);
	}
}


void Biot::MoveSegment(int nPeno, short degree)
{
	GeneSegment& segment = trait.GetSegment(lineNo[nPeno], geneNo[nPeno]);

	if (segment.IsVisible())
	{
		m_angle[nPeno] += degree;
		redraw.SetRedraw(segment.ShouldRedraw(m_angle[nPeno] - m_angleDrawn[nPeno]));
	}
}


///////////////////////////////////////////////////////////////
// MoveLineType
//
// Moves a lineType and returns true when
// the line is in the requested position.  Returns false
// if the line is not yet in the requested position.
//
// We need to reserve line type operations so commands don't
// fight to complete.
//
// rate should always be positive.  offset can be negative or
// positive.
//
bool Biot::MoveLineType(int nLineType, short rate, short offset)
{
    assert(nLineType < MAX_LIMB_TYPES && nLineType >= 0);

	short& nAngle = m_angleLimbType[nLineType];
	if (offset != nAngle)
	{
		if (offset > nAngle)
		{
			nAngle += rate;
			if (nAngle > offset)
				nAngle = offset;
		}
		else
		{
			nAngle -= rate;
			if (nAngle - rate < offset)
				nAngle = offset;
		}
		redraw.SetRedraw(abs(m_angleLimbType[nLineType] - m_angleLimbTypeDrawn[nLineType]) > 3);

		return (offset == nAngle);
	}
    return true;
}


///////////////////////////////////////////////////////////////
// MoveLine
//
// Moves a lineType and returns true when
// the line is in the requested position.  Returns false
// if the line is not yet in the requested position.
//
// We need to reserve line type operations so commands don't
// fight to complete.
//
// rate should always be positive.  offset can be negative or
// positive.
//
bool Biot::MoveLine(int nLine, short rate, short offset)
{
    assert(nLine < MAX_SYMMETRY && nLine >= 0);

	short& nAngle = m_angleLimb[nLine];
	if (offset != nAngle)
	{
		if (offset > nAngle)
		{
			nAngle += rate;
			if (nAngle > offset)
				nAngle = offset;
		}
		else
		{
			nAngle -= rate;
			if (nAngle - rate < offset)
				nAngle = offset;
		}
		redraw.SetRedraw(abs(m_angleLimb[nLine] - m_angleLimbDrawn[nLine]) > 3);

		return (offset == nAngle);
	}
    return true;
}


///////////////////////////////////////////////////////////////
// MoveSegmentType
//
// Moves a segment of a lineType and returns true when
// the line is in the requested position.  Returns false
// if the line is not yet in the requested position.
//
// We need to reserve line type operations so commands don't
// fight to complete.
//
// rate should always be positive.  offset can be negative or
// positive.
//
bool Biot::MoveSegmentType(int nLineType, int nSegment, short rate, short offset)
{
    assert(nLineType < MAX_LIMB_TYPES && nLineType >= 0);
    assert(nSegment < MAX_SEGMENTS && nSegment >= 0);

	GeneSegment& segment = trait.GetSegmentType(nLineType, nSegment);
	short& nAngle = m_angleLimbTypeSegment[nLineType][nSegment];
	if (offset != nAngle)
	{
		if (offset > nAngle)
		{
			nAngle += rate;
			if (nAngle > offset)
				nAngle = offset;
		}
		else
		{
			nAngle -= rate;
			if (nAngle - rate < offset)
				nAngle = offset;
		}
		redraw.SetRedraw(segment.ShouldRedraw(m_angleLimbTypeSegmentDrawn[nLineType][nSegment] - nAngle));

		return (offset == nAngle);
	}
    return true;
}


//////////////////////////////////////////////////////
// SeekArm
//
//
void Biot::SeekArm(int nPeno, short rate, short offset)
{
	short& nAngle = m_angle[nPeno];
	if (trait.GetSegment(lineNo[nPeno], geneNo[nPeno]).IsVisible() && offset != nAngle)
	{
		if (offset > nAngle)
		{
			if (nAngle + rate > offset)
				rate = offset - nAngle;
			
			MoveArm(nPeno, rate);
		}
		else
		{
			
			if (nAngle - rate < offset)
				rate = nAngle - offset;

			MoveArm(nPeno, -rate);
		}
	}
}


//////////////////////////////////////////////////////
// SeekSegment
//
// If no more seeking is required, it returns true
//
void Biot::SeekSegment(int nPeno, short rate, short offset)
{
	short& nAngle = m_angle[nPeno];
	if (offset != nAngle)
	{
		if (offset > nAngle)
		{
			if (nAngle + rate > offset)
				IncreaseAngle(nPeno, offset - nAngle);
			else
				IncreaseAngle(nPeno, rate);
		}
		else
		{
			if (nAngle - rate < offset)
				IncreaseAngle(nPeno, offset - nAngle);
			else
				IncreaseAngle(nPeno, -rate);
		}
	}
}


/////////////////////////////////////////////////////////////////////
// IncreaseAngle
//
//
void Biot::IncreaseAngle(int nPeno, short rate)
{
	GeneSegment& segment = trait.GetSegment(lineNo[nPeno], geneNo[nPeno]);

	// Is the m_angle visible?
	if (segment.IsVisible())
	{
		// First increase the angle
		m_angle[nPeno] += rate;

		redraw.SetRedraw(segment.ShouldRedraw(m_angle[nPeno] - m_angleDrawn[nPeno]));
	}
}


/////////////////////////////////////////////////////////////////////
// RetractLine
//
// Retracts the tip segment on a particular limb.  
//
uint8_t Biot::RetractLine(int nSegment, int nLimb, int maxRadius)
{
   if (m_retractDrawn[nLimb] == m_retractRadius[nLimb] &&
	   m_retractDrawn[nLimb] < maxRadius)
   {
	   m_retractSegment[nLimb] = nSegment;
	   m_retractRadius[nLimb] += 1;
       redraw.SetRedraw(true);
       return (uint8_t) 1;
   }
   return (uint8_t) 0;
}


/////////////////////////////////////////////////////////////////////
// ExtendLine
//
// Extends the tip segment on a particular limb.  
//
uint8_t Biot::ExtendLine(int nSegment, int nLimb)
{
   if (m_retractDrawn[nLimb] == m_retractRadius[nLimb] &&
	   m_retractDrawn[nLimb] > 0)
   {
	   m_retractSegment[nLimb] = nSegment;
	   m_retractRadius[nLimb] -= 1;
       redraw.SetRedraw(true);
       return (uint8_t) 1;
   }
   return (uint8_t) 0;
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
uint8_t Biot::RetractLimbType(int nSegment, int nLimbType, int maxRadius)
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

	for (int i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
		   m_retractSegment[i] = nSegment;
		   m_retractRadius[i] += 1;
		}
	}
    redraw.SetRedraw(true);
    return (uint8_t) 1;
}


/////////////////////////////////////////////////////////////////////
// ExtendLimbType
//
// Extends all the limb tips on a biot for a particular limb type
//
// Returns 1 or 0 for the distance extended.
//
uint8_t Biot::ExtendLimbType(int nSegment, int nLimbType)
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

	for (int i = 0; i < trait.GetLines(); i++)
	{
		if (nLimbType == trait.GetLineTypeIndex(i))
		{
		   m_retractSegment[i] = nSegment;
		   m_retractRadius[i] -= 1;
		}
	}
    redraw.SetRedraw(true);
    return (uint8_t) 1;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbTypeSegment
//
// Move a single segment on all the limbs of a particular type
//
short Biot::MoveLimbTypeSegment(int nSegment, int nLimbType, int nRate)
{
	static const short maxRate = 3;
    assert(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
    assert(nSegment < MAX_SEGMENTS && nSegment >= 0);
    assert(nRate <= maxRate && nRate >= -maxRate);

	short delta = m_angleLimbTypeSegment[nLimbType][nSegment] - m_angleLimbTypeSegmentDrawn[nLimbType][nSegment];

	if (nRate < 0)
	{
		if (delta <= -maxRate)
			return 0;

        nRate = std::max(nRate, -maxRate - delta);
	}
	else
	{
		if (delta >= maxRate)
			return 0;

        nRate = std::min(nRate, maxRate - delta);
	}

	m_angleLimbTypeSegment[nLimbType][nSegment] += nRate;     
	redraw.SetRedraw((delta + nRate) >= maxRate || (delta + nRate) <= -maxRate);
	return nRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbTypeSegments
//
// Move all the segments on all the limbs of a particular type
//
short Biot::MoveLimbTypeSegments(int nLimbType, int nRate)
{
	static const short maxRate = 3;
    assert(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
    assert(nRate <= maxRate && nRate >= -maxRate);

	short delta = m_angleLimbType[nLimbType] - m_angleLimbTypeDrawn[nLimbType];

	if (nRate < 0)
	{
		if (delta <= -maxRate)
			return 0;

        nRate = std::max(nRate, -maxRate - delta);
	}
	else
	{
		if (delta >= maxRate)
			return 0;

        nRate = std::min(nRate, maxRate - delta);
	}

	m_angleLimbType[nLimbType] += nRate;     
	redraw.SetRedraw((delta + nRate) >= maxRate || (delta + nRate) <= -maxRate);
	return nRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbSegments
//
// Move all the segments of a specific limb
//
short Biot::MoveLimbSegments(int nLimb, int nRate)
{
	static const short maxRate = 3;
    assert(nLimb < MAX_SYMMETRY && nLimb >= 0);
    assert(nRate <= maxRate && nRate >= -maxRate);

	short delta = m_angleLimb[nLimb] - m_angleLimbDrawn[nLimb];

	if (nRate < 0)
	{
		if (delta <= -maxRate)
			return 0;

        nRate = std::max(nRate, -maxRate - delta);
	}
	else
	{
		if (delta >= maxRate)
			return 0;

        nRate = std::min(nRate, maxRate - delta);
	}

	m_angleLimb[nLimb] += nRate;     
	redraw.SetRedraw((delta + nRate) >= maxRate || (delta + nRate) <= -maxRate);
	return nRate;
}


/////////////////////////////////////////////////////////////////////
// MoveLimbSegment
//
// Moves a segment on a limb
//
short Biot::MoveLimbSegment(int nSegment, int nLimb, int nRate)
{
	static const short maxRate = 3;
    assert(nLimb < MAX_SYMMETRY && nLimb >= 0);
    assert(nSegment < MAX_SEGMENTS && nSegment >= 0);
    assert(nRate <= maxRate && nRate >= -maxRate);

	int nPeno = nLimb + nSegment * MAX_SYMMETRY;
	short delta = m_angle[nPeno] - m_angleDrawn[nPeno];

	if (nRate < 0)
	{
		if (delta <= -maxRate)
			return 0;

        nRate = std::max(nRate, -maxRate - delta);
	}
	else
	{
		if (delta >= maxRate)
			return 0;

        nRate = std::min(nRate, maxRate - delta);
	}

	m_angleLimb[nPeno] += nRate;     
	redraw.SetRedraw((delta + nRate) >= maxRate || (delta + nRate) <= -maxRate);
	return nRate;
}

