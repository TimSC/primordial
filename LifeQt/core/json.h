#ifndef JSON_H
#define JSON_H
#include <stdexcept>
//Always include this file before any rapidjson includes

#define RAPIDJSON_ASSERT(x) (throw std::runtime_error("json parse error"))

#endif // JSON_H
