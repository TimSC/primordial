//////////////////////////////////////////////////////////////////////
//
// vector.cpp
//
// Implementation of vector covering motion and acceleration
//
#include "stdafx.h"
#include "vector.h"

Vector::Vector()
{
  dr = r = mass =  dy = x = y = dx = drx = dry = 0.0;
}

////////////////////////////////////////////////////////////////////////////////////
// Class Vector
//
// Physics as interpreted by me.
//
//
void Vector::Serialize(CArchive& ar)
{
	if (ar.IsLoading())
	{
		ar >> dx;
		ar >> dy;
		ar >> x;
		ar >> y;
		ar >> dr;
		ar >> r;
	}
	else
	{
		ar << dx;
		ar << dy;
		ar << x;
		ar << y;
		ar << dr;
		ar << r;
	}
}

void Vector::TraceDebug()
{
//	TRACE("vector pos(%d,%d,%.3f) rate(%.3f, %.3f, %.3f)\n", int(x),int(y),r,dx,dy,dr);
}


