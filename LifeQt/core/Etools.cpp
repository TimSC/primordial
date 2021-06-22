//////////////////////////////////////////////////////////////////////
//
// etools
//
// A collection of somewhat generic classes.
//
#include <stdio.h>
#include "Etools.h"

using namespace rapidjson;

////////////////////////////////////////////////////////////////////////////////////
// Class CLine
//
// 


//
// Set the line from a point and a slope
//
void CLine::Set(int X1, int Y1, double slope)
{
	m_slope = slope;
	SetState();

	switch (m_state)
	{
	case STATE_INFINITE:
		m_yIntercept = m_slope;
		m_x2 = X1;
		m_x1 = X1;
		m_y1 = -10000;
		m_y2 = 10000;
		break;

	case STATE_NORMAL:
		// b = y - mx;
		m_yIntercept = double(Y1) - m_slope * double(X1);

		// if the slope causes Y to increase faster than X, pick Y first
		if (m_slope > 1 || m_slope < -1)
		{
			m_y1 = -10000;
			m_y2 = 10000;

			// x = (y - b) / m
			m_x1 = Round((double(m_y1) - m_yIntercept) / m_slope);
			m_x2 = Round((double(m_y2) - m_yIntercept) / m_slope);
		}
		else
		{
			m_x1 = -10000;
			m_x2 = 10000;

			// y = mx + b
			m_y1 = Round((double(m_x1) * m_slope) + m_yIntercept);
			m_y2 = Round((double(m_x2) * m_slope) + m_yIntercept);
		}
		break;

	case STATE_NAN:
        assert(0);
		m_x2 = X1;
		m_x1 = X1;
		m_y2 = Y1;
		m_y1 = Y1;
		break;
	}
	MinMax();
}

void CLine::Set(int X1, int Y1, int X2, int Y2)
{
	m_x1 = X1;
	m_x2 = X2;
	m_y1 = Y1;
	m_y2 = Y2;

	MinMax();
	SetSlope();
}



//
// Intersect - determines if and where segmented lines intersect
//
bool CLine::Intersect(CLine &l, int& x, int& y) const
{
	if (m_state == STATE_NORMAL)
	{
		if (l.m_state == STATE_NORMAL)
		{
			// Slopes differ
			if (m_slope != l.m_slope)
			{
				x = Round((l.m_yIntercept - m_yIntercept) / (m_slope - l.m_slope));
				y = Round((l.m_yIntercept * m_slope - m_yIntercept * l.m_slope) / (m_slope - l.m_slope));
				return !(outOfRangeX(x) || l.outOfRangeX(x) || outOfRangeY(y) || l.outOfRangeY(y));
			}
			else
			{
				// If the slopes are the same, or nearly the same
				// pick an intersection point
				if (fabs(m_yIntercept - l.m_yIntercept) <= .5)
				{
                    y = std::max(m_minY, l.m_minY) + (std::min(m_maxY, l.m_maxY) - std::max(m_minY, l.m_minY)) / 2;
                    x = std::max(m_minX, l.m_minX) + (std::min(m_maxX, l.m_maxX) - std::max(m_minX, l.m_minX)) / 2;

					return !(outOfRangeY(y) || l.outOfRangeY(y) || outOfRangeX(x) || l.outOfRangeX(x));
				}
				return false;
			}
		}

		if (l.m_state == STATE_INFINITE)
		{
			y = Round(l.m_x1 * m_slope + m_yIntercept);
			x = l.m_x1;
			return !(l.outOfRangeY(y) || outOfRangeX(x));
		}
	}

	if (m_state == STATE_INFINITE)
	{
		if (l.m_state == STATE_NORMAL)
		{
			y = Round(m_x1 * l.m_slope + l.m_yIntercept);
			x = m_x1;
			return !(outOfRangeY(y) || l.outOfRangeX(x));
		}

		if (l.m_state == STATE_INFINITE)
		{
			// Both must have infinite slope
			if (m_x1 == l.m_x1)
			{
				x = m_x1;
                y = std::max(m_minY, l.m_minY) + (std::min(m_maxY, l.m_maxY) - std::max(m_minY, l.m_minY)) / 2;
				return !(outOfRangeY(y) || l.outOfRangeY(y));
			}
			return false;
		}
	}

	// This line is really a point
	if (m_state == STATE_NAN)
	{
		x = m_x1;
		y = m_y1;

		switch(l.m_state)
		{
		case STATE_NAN:
			return (m_x1 == l.m_x1 && m_y1 == l.m_y1);

		case STATE_NORMAL:
			// b = y - mx
			return (fabs((m_y1 - l.m_slope * m_x1) - l.m_yIntercept) <= .5);

		case STATE_INFINITE:
			return (m_x1 == l.m_x1);

		default:
            assert(0);
			return false;
		}
	}

	// This line is really a point
	if (l.m_state == STATE_NAN)
	{
		x = l.m_x1;
		y = l.m_y1;

		switch(m_state)
		{
		case STATE_NAN:
			return (m_x1 == l.m_x1 && m_y1 == l.m_y1);

		case STATE_NORMAL:
			// b = y - mx
			return (fabs((l.m_y1 - m_slope * l.m_x1) - m_yIntercept) <= .5);

		case STATE_INFINITE:
			return (m_x1 == l.m_x1);

		default:
            assert(0);
			return false;
		}
	}

    assert(0);

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
// Class BRect
//
// Wrote a replacement for Borland's TRect.
//
//

////////////////////////////////////////////////////////////////////////////////////
// Normalize
//
// Ensures top < bottom and left < right
//
//
void BRect::Normalize(void)
{
	if (m_top > m_bottom)
	{
		int tmp = m_top;

		m_top    = m_bottom;
		m_bottom = tmp;
	}

	if (m_left > m_right)
	{
		int tmp = m_left;

		m_left  = m_right;
		m_right = tmp;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Contains
//
// Determines if a rectangle contains a point. If so returns the distance from
// the center or -1 if not contained
//
//
int BRect::Contains(int x, int y)
{
  if (x >= m_left && x <= m_right &&
      y <= m_bottom && y >= m_top)
  {
    return (int) sqrt( (double) ( ( (int64_t)(x - CenterX()) * (int64_t) (x - CenterX()) ) +
                         ((int64_t)(y - CenterY()) * (int64_t) (y - CenterY()) ) ) );
  }
  return -1;
}


///////////////////////////////////////////////////////////////
// Serialize
//
// Takes or sets the state of the randmizer
//

void BRect::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_left", m_left, allocator);
    v.AddMember("m_right", m_right, allocator);
    v.AddMember("m_top", m_top, allocator);
    v.AddMember("m_bottom", m_bottom, allocator);
}

void BRect::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_left = v["m_left"].GetInt();
    m_right = v["m_right"].GetInt();
    m_top = v["m_top"].GetInt();
    m_bottom = v["m_bottom"].GetInt();
}

//////////////////////////////////////////////////////////////////
// NextSizeTwo
//
// Rounds a number up a bitmap dimension to the next size so we
// don't have to reallocate that bitmap too often.
//
int BRect::NextSize(int value)
{
	static int size[9] = {15, 31, 63, 95, 127, 191, 255, 511, 2047};
	int start = 0;

	while (value > size[start])
		start++;

	// It should never be larger than 511
    assert(start < 8);

	return size[start];
}



BRectSort::BRectSort() 
	: m_bIncrementalSort(true)
{
	m_pArray[ARRAY_LEFT]   = &m_arrayLeft;
	m_pArray[ARRAY_RIGHT]  = &m_arrayRight;
	m_pArray[ARRAY_TOP]    = &m_arrayTop;
	m_pArray[ARRAY_BOTTOM] = &m_arrayBottom;
}

// Add a rectangle
void BRectSort::Add(BRectItem* pItem)
{
	// We care about speed - I'm not checking for NULL in Release
    assert(pItem != NULL);

	// Can we do the fast add?
	if (!m_bIncrementalSort)
	{
		for (int i = 0; i < ARRAY_MAX; i++)
		{
			m_pArray[i]->AddRect(pItem);
		}
		return;
	}

	for (int i = 0; i < ARRAY_MAX; i++)
	{
		m_pArray[i]->InsertRect(pItem);
	}
}


bool BRectSort::SortAll()
{
	bool bSorted = false;

	for (int i = 0; i < ARRAY_MAX; i++)
	{
		bSorted |= m_pArray[i]->Sort();

		m_pArray[i]->TraceDebug(i);
	}
	return bSorted;
}


void BRectSort::TraceDebug()
{
	for (int i = 0; i < ARRAY_MAX; i++)
		m_pArray[i]->TraceDebug(i);
}


// Indicate a rectangle has moved, but we don't know how
void BRectSort::Move(BRectItem* pItem, BRect* pOrigRect)
{
	if (pItem->m_left != pOrigRect->m_left)
		m_arrayLeft.MoveRect(pItem);

	if (pItem->m_right != pOrigRect->m_right)
		m_arrayRight.MoveRect(pItem);

	if (pItem->m_top != pOrigRect->m_top)
		m_arrayTop.MoveRect(pItem);

	if (pItem->m_bottom != pOrigRect->m_bottom)
		m_arrayBottom.MoveRect(pItem);
}


void BRectSort::Move(BRectItem* pItem)
{
	for (int i = 0; i < ARRAY_MAX; i++)
		m_pArray[i]->MoveRect(pItem);
}

// Remove a rectangle
void BRectSort::Remove(BRectItem* pItem)
{
	for (int i = 0; i < ARRAY_MAX; i++)
		m_pArray[i]->RemoveRect(pItem);
}


void BRectSort::FreeAll()
{
	for (int i = 0; i < ARRAY_MAX; i++)
        m_pArray[i]->clear();
}

// Searching methods
// Find all rectangles that contain a point
BRectItem* BRectSort::IterateRects(BRectSortPos& sortPos)
{
	switch (sortPos.m_nType)
	{
	default:
	case BRectSortPos::TYPE_INVALID:
        assert(0);
		return NULL;

	case BRectSortPos::TYPE_POINT:
		if (!FindShortestArray(sortPos, sortPos.m_left, sortPos.m_top, sortPos.m_left, sortPos.m_top))
			return NULL;
		break;

	case BRectSortPos::TYPE_CONTAINED:
		if (!FindShortestArray(sortPos, sortPos.m_left, sortPos.m_top, sortPos.m_right, sortPos.m_bottom))
			return NULL;
		break;

	case BRectSortPos::TYPE_INTERSECT:
		if (!FindShortestArray(sortPos, sortPos.m_right, sortPos.m_bottom, sortPos.m_left, sortPos.m_top))
			return NULL;
		break;
	}

	BRectArray* pArray = m_pArray[sortPos.m_nShortIndex];

	// If X is between the left and right and
	// Y is between top and bottom then we have a match

	if (sortPos.m_nShortIndex <= ARRAY_TOP)
	{
		for (int i = pArray->GetPos(&sortPos) - 1; i >= 0; i--)
		{
            BRectItem* pItem = 	pArray->at(i);
			if (pItem->m_indexBottom >= sortPos.m_indexBottom &&	
				pItem->m_indexRight  >= sortPos.m_indexRight  &&
				pItem->m_indexLeft   <  sortPos.m_indexLeft   &&
				pItem->m_indexTop    <  sortPos.m_indexTop)
			{
				pArray->SetPos(&sortPos, i - 1);
				return pItem;
			}
		}
	}
	else
	{
        for (int i = pArray->GetPos(&sortPos); i < pArray->size(); i++)
		{
            BRectItem* pItem = 	pArray->at(i);
			if (pItem->m_indexBottom >= sortPos.m_indexBottom &&	
				pItem->m_indexRight  >= sortPos.m_indexRight  &&
				pItem->m_indexLeft   <  sortPos.m_indexLeft   &&
				pItem->m_indexTop    <  sortPos.m_indexTop)
			{
				pArray->SetPos(&sortPos, i + 1);
				return pItem;
			}
		}
	}
	// A point is considered inside a rect
	// Find the 

//	return !(x < m_left || x > m_right || y > m_bottom || y < m_top);
	return NULL;

}


bool BRectSort::FindShortestArray(BRectSortPos& sortPos, int nLeft, int nTop, int nRight, int nBottom)
{
	int nSmallestArraySize;

	if (!sortPos.m_bInitialized)
	{
		sortPos.m_indexRight   = m_arrayRight.GreaterPos(nRight);
        nSmallestArraySize    = m_arrayRight.size() - sortPos.m_indexRight;
		sortPos.m_nShortIndex = ARRAY_RIGHT;
		if (nSmallestArraySize == 0)
			return false;

		sortPos.m_indexBottom    = m_arrayBottom.GreaterPos(nBottom);
        if (nSmallestArraySize > m_arrayBottom.size() - sortPos.m_indexBottom)
		{
            nSmallestArraySize = m_arrayBottom.size() - sortPos.m_indexBottom;
			sortPos.m_nShortIndex = ARRAY_BOTTOM;
			if (nSmallestArraySize == 0)
				return false;
		}

		sortPos.m_indexLeft = m_arrayLeft.GreaterOrEqualPos(nLeft);
		if (nSmallestArraySize > sortPos.m_indexLeft)
		{
			nSmallestArraySize = sortPos.m_indexLeft;
			sortPos.m_nShortIndex = ARRAY_LEFT;
			if (nSmallestArraySize == 0)
				return false;
		}

		sortPos.m_indexTop = m_arrayTop.GreaterOrEqualPos(nTop);
		if (nSmallestArraySize > sortPos.m_indexTop)
		{
			nSmallestArraySize = sortPos.m_indexTop;
			sortPos.m_nShortIndex = ARRAY_TOP;
			if (nSmallestArraySize == 0)
				return false;
		}
	}
	sortPos.m_bInitialized = true;
	return true;
}


void BRectArray::TraceDebug(int nIndex)
{
    for (int j = 0; j < size(); j++)
	{
        //TRACE("Array %d, index %d, rectIndex %d, value %d, biot%x\n", nIndex, j,
        //    GetPos(at(j)), Coordinate(at(j)), at(j));
        assert(ValidateRect(j, at(j)));
	}
}


bool BRectArray::Sort(int nStartIndex /*=0*/, int nStopIndex /*=GetSize()*/)
{
	int nNextStartIndex = -1;
	int nNextStopIndex  = 0;

	for (int i = nStartIndex; i < nStopIndex - 1; i++)
	{
        if (Coordinate(at(i)) > Coordinate(at(i + 1)))
		{
            BRectItem* pItem = at(i);
			SetPos(pItem, i + 1);
            SetPos(at(i+1), i);

            (*this)[i] = at(i+1);
            (*this)[i+1] = pItem;

			if (nNextStartIndex == -1)
				nNextStartIndex = i - 1;

			nNextStopIndex = i + 1;
		}
	}

	if (nNextStartIndex != -1)
		return Sort(nNextStartIndex, nNextStopIndex);

    return (nStartIndex == 0 && nStopIndex == size());
}


// For an array 1 4 5 6 6 7 8 9
// a coordinate of 6 would return the first 6's position
int BRectArray::GreaterPos(int nCoordinate)
{
    int nStop  = size();
	int nStart = 0;
	int nPos   = nStop / 2;

	while (nStop != nStart)
	{
		// The coordinate is greater than or equal, move down the array
        if (nCoordinate > Coordinate(at(nPos)))
		{
			nStart = nPos + 1;
		}
		else
		{
			nStop = nPos;
		}
		nPos = nStart + ((nStop - nStart) / 2);
	}
	return nPos;
}

// For an array 1 4 5 6 6 7 8 9
// a coordinate of 6 would return 7's position
int BRectArray::GreaterOrEqualPos(int nCoordinate)
{
    int nStop  = size();
	int nStart = 0;
	int nPos   = nStop / 2;

	while (nStop != nStart)
	{
		// The coordinate is greater than or equal, move down the array
        if (nCoordinate >= Coordinate(at(nPos)))
		{
			nStart = nPos + 1;
		}
		else
		{
			nStop = nPos;
		}
		nPos = nStart + ((nStop - nStart) / 2);
	}
	return nPos;
}

void BRectArray::InsertRect(BRectItem* pItem)
{
	int nPos = GreaterPos(Coordinate(pItem));
    this->insert(nPos, pItem);

	// Update all our external references
    for (int i = nPos; i < size(); i++)
        SetPos(at(i), i);
}

// The rect said it moved - re-sort
void BRectArray::MoveRect(BRectItem* pItem)
{
    assert(size() != 0);

    int nTopItem = size() - 1;

	// We need at least two items 
	if (nTopItem < 1)
		return;

	int nPos    = GetPos(pItem);

	// A -1 indicates the rect is not in the array
    assert(nPos != -1);
	// The pointer better be the same as in the array
//	BRectItem* pItem2 = GetAt(nPos);


    assert(pItem == at(nPos));
	
    while (nPos < nTopItem && Coordinate(pItem) > Coordinate(at(nPos + 1)))
	{
        (*this)[nPos] = at(nPos + 1);
        SetPos(at(nPos), nPos);
		nPos++;
	}

    while (nPos > 0 && Coordinate(pItem) < Coordinate(at(nPos - 1)))
	{
        (*this)[nPos] = at(nPos - 1);
        SetPos(at(nPos), nPos);
		nPos--;
	}

    (*this)[nPos] = pItem;
    SetPos(at(nPos), nPos);
}


void BRectArray::RemoveRect(BRectItem* pItem)
{
	int nPos = GetPos(pItem);
    assert(nPos != -1);

	// Set this so we don't get confused it was removed
	SetPos(pItem, -1);

	// Adjust the array
    this->removeAt(nPos);

	// Update all our external references
    for (int i = nPos; i < size(); i++)
        SetPos(at(i), i);
}


// Add an item to the array without worring about sorting
void BRectArray::AddRect(BRectItem* pItem)
{
	// If it doesn't equal -1, the array may have been added before
    assert(GetPos(pItem) == -1);
    this->append(pItem);
    SetPos(pItem, this->size()-1);
}


// Just checks to make sure they are pointing at each other
// For catching bugs
bool BRectArray::ValidateRect(int nPos, BRectItem* pItem)
{
    return (at(nPos) == pItem && GetPos(pItem) == nPos);

}

