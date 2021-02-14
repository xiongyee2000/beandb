#include <cstring>

#include "./Property.h"
#include "./Bean.h"
#include "./internal_common.hxx"

using namespace Json;

namespace org {
namespace jinsha {
namespace bean {

template<typename ValueT, typename MapT>
static void doFindEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static void doFindLessEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static void doFindGreaterEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static void doFindLessThan(const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static void doFindGreaterThan(const ValueT& value, const MapT& map, std::list<Bean*>& beans); 

template<typename ValueT, typename MapT>
static void doFindCommon(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans);

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


void Property::findCommon(int opType, const Json::Value& value, std::list<Bean*>& beans) const
{
    switch (value.type()) {
        case Json::intValue:
            doFindCommon<int_t, decltype(m_intValueMap_)>(opType, value.asInt64(), m_intValueMap_, beans);
            break;
        case Json::uintValue:
            doFindCommon<uint_t, decltype(m_uintValueMap_)>(opType, value.asUInt64(), m_uintValueMap_, beans);
            break;
        case Json::realValue:
            doFindCommon<double, decltype(m_doubleValueMap_)>(opType, value.asDouble(), m_doubleValueMap_, beans);
            break;
        case Json::stringValue:
            doFindCommon<const char*, decltype(m_strValueMap_)>(opType, value.asCString(), m_strValueMap_, beans);
            break;
        case Json::booleanValue:
            if (opType == op_eq)
            { //only equal makes sense
                beans.clear();
                if (value == true)
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
            break;
        default:
            //not supported
            break;
    }
}


void Property::findEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon(op_eq, value, beans);
}


void Property::findLessEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon(op_le, value, beans);
}


void Property::findGreaterEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon(op_ge, value, beans);
}


void Property::findLessThan(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon(op_lt, value, beans);
}


void Property::findGreaterThan(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon(op_gt, value, beans);
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
static void doFindCommon(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto lowerBound = map.begin();
    auto upperBound = map.end();
    auto iter = map.end();

    switch (opType) {
        case op_eq:
            lowerBound = map.lower_bound(value);
            upperBound = map.upper_bound(value);
        case op_le:
        case op_lt:
            upperBound = map.upper_bound(value);
            break;
        case op_ge:
        case op_gt:
           lowerBound = map.lower_bound(value);
            break;

        default:
            break;
    }

    for (auto iter = lowerBound; iter != upperBound; iter++)
    {
         if  (opType == op_gt || opType == op_lt)
        {
            if (iter->first != value) //skip equal ones
                beans.push_back(iter->second);
        }
        else
        {
            beans.push_back(iter->second);
        }
    }
}

template<typename ValueT, typename MapT>
static void doFindEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto ret = map.equal_range(value);
    for (auto& it = ret.first; it != ret.second; ++it)
    {
        beans.push_back(it->second);
    }
}


template<typename ValueT, typename MapT>
static void doFindLessEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto bound = map.upper_bound(value);
    for (auto& it = bound; it != map.end(); it--)
    {
        beans.push_back(it->second);
    }
}


template<typename ValueT, typename MapT>
static void doFindGreaterEqual(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto bound = map.lower_bound(value);
    for (auto& it = bound; it != map.end(); it++)
    {
        beans.push_back(it->second);
    }
}


template<typename ValueT, typename MapT>
static void doFindLessThan(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto bound = map.upper_bound(value);
    for (auto& it = bound; it != map.end(); it--)
    {
        if (it->first != value) beans.push_back(it->second);
    }
}


template<typename ValueT, typename MapT>
static void doFindGreaterThan(const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto bound = map.lower_bound(value);
    for (auto& it = bound; it != map.end(); it++)
    {
        if (it->first != value) beans.push_back(it->second);
    }
}

}
}
}