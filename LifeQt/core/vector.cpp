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
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    if (!v.HasMember("dx"))
        throw std::runtime_error("eror parsing json");
    dx = v["dx"].GetDouble();
    if (!v.HasMember("dy"))
        throw std::runtime_error("eror parsing json");
    dy = v["dy"].GetDouble();
    if (!v.HasMember("x"))
        throw std::runtime_error("eror parsing json");
    x = v["x"].GetDouble();
    if (!v.HasMember("y"))
        throw std::runtime_error("eror parsing json");
    y = v["y"].GetDouble();
    if (!v.HasMember("dr"))
        throw std::runtime_error("eror parsing json");
    dr = v["dr"].GetDouble();
    if (!v.HasMember("r"))
        throw std::runtime_error("eror parsing json");
    r = v["r"].GetDouble();
}
