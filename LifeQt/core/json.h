#ifndef JSON_H
#define JSON_H
//Always include this file before any rapidjson includes

#include <stdexcept>
#define RAPIDJSON_ASSERT(x) {if ((x)==0) throw std::runtime_error("json parse error");}

#endif // JSON_H
