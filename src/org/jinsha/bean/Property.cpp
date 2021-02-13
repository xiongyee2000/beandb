#include <cstring>

#include "./Property.h"
#include "./Bean.h"

using namespace Json;

namespace org {
namespace jinsha {
namespace bean {

template<typename ValueT, typename MapT>
static void doFindEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static void doRemoveIndex(Bean* bean, const ValueT& value, MapT& map);

Property::~Property()
{
    clear();
}


void Property::clear()
{
    m_refCount_ = 0;
    m_trueValueMap_.clear();
    m_falseValueMap_.clear();
    m_intValueMap_.clear();
    m_uintValueMap_.clear();
    m_doubleValueMap_.clear();
}


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
    m_intValueMap_.insert(std::pair<Json::Int64, Bean*>(value, bean));
}


void Property::addIndex(Bean* bean, Json::UInt64 value)
{
    m_uintValueMap_.insert(std::pair<Json::UInt64, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, double value)
{
    m_doubleValueMap_.insert(std::pair<double, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, const char* value)
{
    m_strValueMap_.insert(std::pair<const char*, Bean*>(value, bean));
}

void Property::addIndex(Bean* bean, bool value)
{
    if (value)
    {
        m_trueValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
    }
    else
    {
        m_falseValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
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
    doRemoveIndex<Json::Int64, decltype(m_intValueMap_)>(bean, value, m_intValueMap_);
}


void Property::removeIndex(Bean* bean, Json::UInt64 value)
{
    doRemoveIndex<Json::UInt64, decltype(m_uintValueMap_)>(bean, value, m_uintValueMap_);
}


void Property::removeIndex(Bean* bean, double value)
{
    doRemoveIndex<double, decltype(m_doubleValueMap_)>(bean, value, m_doubleValueMap_);
}


void Property::removeIndex(Bean* bean, const char* value)
{
    doRemoveIndex<const char*, decltype(m_strValueMap_)>(bean, value, m_strValueMap_);
}


void Property::removeIndex(Bean* bean, bool value)
{
    if (value)
    {
        auto item = m_trueValueMap_.find(bean->getId());
        m_trueValueMap_.erase(item);
    }
    else
    {
        auto item = m_falseValueMap_.find(bean->getId());
        m_falseValueMap_.erase(item);
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


void Property::findEqual(Json::Int value, std::list<Bean*>& beans) const
{
    doFindEqual<Json::Int64, decltype(m_intValueMap_)>(value, m_intValueMap_, beans);
}


void Property::findEqual(Json::UInt value, std::list<Bean*>& beans) const
{
    doFindEqual<Json::UInt64, decltype(m_uintValueMap_)>(value, m_uintValueMap_, beans);
}


void Property::findEqual(Json::Int64 value, std::list<Bean*>& beans) const
{
    doFindEqual<Json::Int64, decltype(m_intValueMap_)>(value, m_intValueMap_, beans);
}


void Property::findEqual(Json::UInt64 value, std::list<Bean*>& beans) const
{
    doFindEqual<Json::UInt64, decltype(m_uintValueMap_)>(value, m_uintValueMap_, beans);
}


void Property::findEqual(double value, std::list<Bean*>& beans) const
{
    doFindEqual<double, decltype(m_doubleValueMap_)>(value, m_doubleValueMap_, beans);
}


void Property::findEqual(bool value, std::list<Bean*>& beans) const
{
    if (value)
    {
        for (auto& item : m_trueValueMap_)
        {
            beans.push_back(item.second);
        }
    }
    else
    {
        for (auto& item : m_falseValueMap_)
        {
            beans.push_back(item.second);
        }
    }
}


void Property::findEqual(const char* value, std::list<Bean*>& beans) const
{
    doFindEqual<const char*, decltype(m_strValueMap_)>(value, m_strValueMap_, beans);
}


bool Property::StrComparator::operator()(const char* const & a, const char* const & b) const
{
    return strcmp(a, b) < 0;
}


template<typename ValueT, typename MapT>
static void doRemoveIndex(Bean* bean, const ValueT& value, MapT& map)
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


template<typename ValueT, typename MapT>
static void doFindEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    auto ret = map.equal_range(value);
    for (auto& it = ret.first; it != ret.second; ++it)
    {
        beans.push_back(it->second);
    }
}


}
}
}