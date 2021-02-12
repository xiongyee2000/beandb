#pragma once

#include <string>
#include <unordered_map>

#include "jsoncpp/json/value.h"

#include "common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

class Property
{
public:
    Property(const std::string& name) : name_(name) {};
    Property(const char* name) : name_(name) {};
    virtual ~Property();

    const std::string& getName() const {return name_;};

private:
    void clear();

private:
    void addIndex(Bean* bean, Json::Int value);
    void addIndex(Bean* bean, Json::UInt value);
    void addIndex(Bean* bean, Json::Int64 value);
    void addIndex(Bean* bean, Json::UInt64 value);
    void addIndex(Bean* bean, double value);
    void addIndex(Bean* bean, const char* value);
    void addIndex(Bean* bean, bool value);
    void removeIndex(Bean* bean, Json::Int value);
    void removeIndex(Bean* bean, Json::UInt value);
    void removeIndex(Bean* bean, Json::Int64 value);
    void removeIndex(Bean* bean, Json::UInt64 value);
    void removeIndex(Bean* bean,  double value);
    void removeIndex(Bean* bean, const char* value);
    void removeIndex(Bean* bean, bool value);
    void removeIndex(Bean* bean, const Json::Value& value);

    std::string name_;
    unsigned int refCount_ = 0;
    std::unordered_map<oidType, Bean*> trueValueMap_;
    std::unordered_map<oidType, Bean*> falseValueMap_;
    std::multimap<Json::Int64, Bean*> intValueMap_;
    std::multimap<Json::Int64, Bean*> uintValueMap_;
    std::multimap<double, Bean*> doubleValueMap_;

     struct StrComparator
    {
        bool operator()(const char* const & a, const char* const & b) const;
    };
    std::multimap<const char*, Bean*, StrComparator> strValueMap_;


friend class BeanWorld;
};

}
}
}