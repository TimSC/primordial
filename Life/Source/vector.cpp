//////////////////////////////////////////////////////////////////////
//
// vector.cpp
//
// Implementation of vector covering motion and acceleration
//
#include "stdafx.h"
#include <math.h>
//#include <stdio.h>
//#include <limits.h>
#include "vector.h"

using namespace rapidjson;

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

void Vector::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("dx", dx, allocator);
    v.AddMember("dy", dy, allocator);
    v.AddMember("x", x, allocator);
    v.AddMember("y", y, allocator);
    v.AddMember("dr", dr, allocator);
    v.AddMember("r", r, allocator);
}

void Vector::SerializeJsonLoad(const rapidjson::Value& v)
{
    dx = v["dx"].GetDouble();
    dy = v["dy"].GetDouble();
    x = v["x"].GetDouble();
    y = v["y"].GetDouble();
    dr = v["dr"].GetDouble();
    r = v["r"].GetDouble();
}
