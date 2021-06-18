//////////////////////////////////////////////////////////////////////
//
// vector.cpp
//
// Implementation of vector covering motion and acceleration
//
#include <math.h>
//#include <stdio.h>
//#include <limits.h>
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
void Vector::Serialize(QDataStream& ar)
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


