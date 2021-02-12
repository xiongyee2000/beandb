#include <cstring>

#include "./Property.h"
#include "./Bean.h"

using namespace Json;

namespace org {
namespace jinsha {
namespace bean {

Property::~Property()
{
    clear();
}


void Property::clear()
{
    refCount_ = 0;
    trueValueMap_.clear();
    falseValueMap_.clear();
    intValueMap_.clear();
    uintValueMap_.clear();
    doubleValueMap_.clear();
}

template<typename ValueT, typename MapT>
static void doRemoveIndex(Bean* bean, ValueT value, MapT map);

void Property::addIndex(Bean* bean, Json::Int value)
{
    addIndex(bean, (Json::Int64)value);
}


void Property::addIndex(Bean* bean, Json::UInt value)
{
    addIndex(bean, (Json::UInt64)value);
}


void Property::addIndex(Bean* bean, Json::Int64 value)
{
    intValueMap_.insert(std::pair<Json::Int64, Bean*>(value, bean));
}


void Property::addIndex(Bean* bean, Json::UInt64 value)
{
    uintValueMap_.insert(std::pair<Json::UInt64, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, double value)
{
    doubleValueMap_.insert(std::pair<double, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, const char* value)
{
    strValueMap_.insert(std::pair<const char*, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, bool value)
{
    if (value)
    {
        trueValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
    }
    else
    {
        falseValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
    }
}
 

void Property::removeIndex(Bean* bean, Json::Int value)
{  
    removeIndex(bean, (Json::Int64)value);
}


void Property::removeIndex(Bean* bean, Json::UInt value)
{
    removeIndex(bean, (Json::UInt64)value);
}


void Property::removeIndex(Bean* bean, Json::Int64 value)
{
    doRemoveIndex<Json::Int64, decltype(intValueMap_)>(bean, value, intValueMap_);
}


void Property::removeIndex(Bean* bean, Json::UInt64 value)
{
    doRemoveIndex<Json::UInt64, decltype(uintValueMap_)>(bean, value, uintValueMap_);
}


void Property::removeIndex(Bean* bean, double value)
{
    doRemoveIndex<double, decltype(doubleValueMap_)>(bean, value, doubleValueMap_);
}


void Property::removeIndex(Bean* bean, const char* value)
{
    doRemoveIndex<const char*, decltype(strValueMap_)>(bean, value, strValueMap_);
}


void Property::removeIndex(Bean* bean, bool value)
{
    if (value)
    {
        auto item = trueValueMap_.find(bean->getId());
        trueValueMap_.erase(item);
    }
    else
    {
        auto item = falseValueMap_.find(bean->getId());
        falseValueMap_.erase(item);
    }
}


void Property::removeIndex(Bean* bean, const Json::Value& value)
{
    if (value.isInt())
    {
        return removeIndex(bean, value.asInt());
    }
    else if (value.isUInt())
    {
        return removeIndex(bean, value.asUInt());
    }
    else if (value.isInt64())
    {
        return removeIndex(bean, value.asInt64());
    }
    else if (value.isUInt64())
    {
        return removeIndex(bean, value.asUInt64());
    }
    else if (value.isDouble())
    {
        return removeIndex(bean, value.asDouble());
    }
    else if (value.isString())
    {
        return removeIndex(bean, value.asCString());
    }
    else if (value.isBool())
    {
        return removeIndex(bean, value.asBool());
    }
}

bool Property::StrComparator::operator()(const char* const & a, const char* const & b) const
{
    return strcmp(a, b) < 0;
}


template<typename ValueT, typename MapT>
static void doRemoveIndex(Bean* bean, ValueT value, MapT map)
{
    auto ret = map.equal_range(value);
    for (auto& it = ret.first; it != ret.second; ++it)
    {
        if (it->second == bean) 
        {
            map.erase(it);
            return;
        }
    }
}

}
}
}