#include <cstring>
#include <wchar.h>
#include "./Property.h"
#include "./Bean.h"
#include "./internal_common.hxx"

using namespace Json;

namespace org {
namespace jinsha {
namespace bean {

template<typename ValueT, typename MapT>
static void doFindCommon(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static bool doRemoveIndex(Bean* bean, const ValueT& value, MapT& map);

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


void Property::addIndex(Bean* bean, const Json::Value& value)
{
    switch (value.type()) 
    {
        case Json::intValue:
            m_intValueMap_.insert(std::pair<int_t, Bean*>((int_t)value.asInt64(), bean));
            break;
        case Json::uintValue:
            m_uintValueMap_.insert(std::pair<int_t, Bean*>((int_t)value.asUInt64(), bean));
            break;
        case Json::realValue:
            m_doubleValueMap_.insert(std::pair<double, Bean*>(value.asDouble(), bean));
            break;
        case Json::booleanValue:
            if (value == true)
            {
                m_trueValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
            }
            else
            {
                m_falseValueMap_.insert(std::pair<oidType, Bean*>(bean->getId(), bean));
            }
            break;
        case Json::stringValue:
            m_strValueMap_.insert(std::pair<const char*, Bean*>(value.asCString(), bean));
            break;
        default:
            elog("[%s:%d] value type %d not supported" ,  __FILE__, __LINE__, value.type());
            break;
    }
}


bool Property::removeIndex(Bean* bean, const Json::Value& value)
{
    bool rtn = false;
    switch (value.type()) 
    {
        case Json::intValue:
            rtn = doRemoveIndex<int_t, decltype(m_intValueMap_)>
                (bean, (int_t)value.asInt64(), m_intValueMap_);
            break;
        case Json::uintValue:
             rtn = doRemoveIndex<uint_t, decltype(m_uintValueMap_)>
                (bean, (uint_t)value.asUInt64(), m_uintValueMap_);
            break;
        case Json::realValue:
             rtn = doRemoveIndex<double, decltype(m_doubleValueMap_)>
                (bean, value.asDouble(), m_doubleValueMap_);
            break;
        case Json::stringValue:
             rtn = doRemoveIndex<const char*, decltype(m_strValueMap_)>
                (bean, value.asCString(), m_strValueMap_);
            break;
        case Json::booleanValue:
            if (value == true)
            {
                auto item = m_trueValueMap_.find(bean->getId());
                if (item != m_trueValueMap_.end())
                {
                     rtn = true;
                     m_trueValueMap_.erase(item);
                }
            }
            else
            {
                auto item = m_falseValueMap_.find(bean->getId());
                if (item != m_falseValueMap_.end())
                {
                    rtn = true;
                    m_falseValueMap_.erase(item);
                }
            }
            break;
        default:
            break;
    }
    return rtn;
}


void Property::findCommon(int opType, const Json::Value& value, std::list<Bean*>& beans) const
{
    switch (value.type()) {
        case Json::intValue:
            doFindCommon<int_t, decltype(m_intValueMap_)>
                (opType, value.asInt64(), m_intValueMap_, beans);
            break;
        case Json::uintValue:
            doFindCommon<uint_t, decltype(m_uintValueMap_)>
                (opType, value.asUInt64(), m_uintValueMap_, beans);
            break;
        case Json::realValue:
            doFindCommon<double, decltype(m_doubleValueMap_)>
                (opType, value.asDouble(), m_doubleValueMap_, beans);
            break;
        case Json::stringValue:
            doFindCommon<const char*, decltype(m_strValueMap_)>
                (opType, value.asCString(), m_strValueMap_, beans);
            break;
        case Json::booleanValue:
            if (opType == op_eq)
            { //only equal makes sense
                beans.clear();
                if (value == true)
                {
                    for (auto& item : m_trueValueMap_)
                        beans.push_back(item.second);
                }
                else
                {
                    for (auto& item : m_falseValueMap_)
                        beans.push_back(item.second);
                }
            }
            break;
        default:
            elog("[%s:%d] value type %d not supported" ,  __FILE__, __LINE__, value.type());
            break;
    }
}


bool Property::StrComparator::operator()(const char* const & a, const char* const & b) const
{
    if (a == b) return false;
    return strcmp(a, b) < 0;
}


template<typename ValueT, typename MapT>
static bool doRemoveIndex(Bean* bean, const ValueT& value, MapT& map)
{
    auto ret = map.equal_range(value);
    for (auto& it = ret.first; it != ret.second; ++it)
    {
        if (it->second == bean) 
        {
            map.erase(it);
            return true;
        }
    }
    return false;
}


template<typename ValueT, typename MapT>
static void doFindCommon(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto lowerBound = map.begin();
    auto upperBound = map.end();

    switch (opType) {
        case op_eq:
            lowerBound = map.lower_bound(value);
            upperBound = map.upper_bound(value);
            break;
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

    const auto keyComparator = map.key_comp();
    for (auto iter = lowerBound; iter != upperBound; iter++)
    {
         if  (opType == op_gt || opType == op_lt)
        {
            if (keyComparator(iter->first, value) ||
                keyComparator(value, iter->first) ) 
                //skip equal ones
                beans.push_back(iter->second);
        }
        else
        {
            beans.push_back(iter->second);
        }
    }
}


// void* Property::getMap(const Json::Value& value)
// {
//     void *rtn = nullptr;
//     switch (value.type()) 
//     {
//         case Json::intValue:
//             rtn = &m_intValueMap_;
//             break;
//         case Json::uintValue:
//             rtn = &m_uintValueMap_;
//             break;
//         case Json::realValue:
//             rtn = &m_doubleValueMap_;
//             break;
//         case Json::booleanValue:
//             if (value == true)
//             {
//                 rtn = &m_trueValueMap_;
//             }
//             else
//             {
//                 rtn = &m_falseValueMap_;
//             }
//             break;
//         case Json::stringValue:
//             return &m_strValueMap_;
//             break;
//         default:
//             break;
//     }
//     return rtn;
// }

}
}
}