//////////////////////////////////////////////////////////////////////
//
// vector.cpp
//
// Implementation of vector covering motion and acceleration
//
#include <math.h>
//#include <stdio.h>
//#include <limits.h>
#include "PosAndSpeed.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace rapidjson;

PosAndSpeed::PosAndSpeed()
{
  dr = r = mass =  dy = x = y = dx = drx = dry = 0.0;
}

static void ValidateMotionValue(const char *context, const char *field, double value,
                                double minValue, double maxValue)
{
    if(!std::isfinite(value) || value < minValue || value > maxValue)
    {
        std::cerr << context << ": invalid " << field << " value " << value
                  << ", validRange=" << minValue << ".." << maxValue << std::endl;
        throw std::range_error("motion value out of range");
    }
}

void PosAndSpeed::ValidateRuntimeState(const char *context) const
{
    ValidateMotionValue(context, "dx", dx, -limit, limit);
    ValidateMotionValue(context, "dy", dy, -limit, limit);
    ValidateMotionValue(context, "dr", dr, -rlimit, rlimit);
    ValidateMotionValue(context, "r", r, -360.0, 360.0);
    ValidateMotionValue(context, "x", x, -100000.0, 100000.0);
    ValidateMotionValue(context, "y", y, -100000.0, 100000.0);
    ValidateMotionValue(context, "drx", drx, -1000.0, 1000.0);
    ValidateMotionValue(context, "dry", dry, -1000.0, 1000.0);
    ValidateMotionValue(context, "mass", mass, 0.0, 100000000.0);
}

////////////////////////////////////////////////////////////////////////////////////
// Class Vector
//
// Physics as interpreted by me.
//
//

void PosAndSpeed::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("dx", dx, allocator);
    v.AddMember("dy", dy, allocator);
    v.AddMember("x", x, allocator);
    v.AddMember("y", y, allocator);
    v.AddMember("dr", dr, allocator);
    v.AddMember("r", r, allocator);
}

void PosAndSpeed::SerializeJsonLoad(const rapidjson::Value& v)
{
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    dx = v["dx"].GetDouble();
    dy = v["dy"].GetDouble();
    x = v["x"].GetDouble();
    y = v["y"].GetDouble();
    dr = v["dr"].GetDouble();
    r = v["r"].GetDouble();
    drx = 0.0;
    dry = 0.0;

    ValidateRuntimeState("PosAndSpeed::SerializeJsonLoad");
}
