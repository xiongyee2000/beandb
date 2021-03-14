#include <cstring>
#include <wchar.h>
#include "./Property.h"
#include "./Bean.h"
#include "./Property.h"
#include "./internal_common.hxx"

using namespace Json;

namespace org {
namespace jinsha {
namespace bean {

template<typename ValueT, typename MapT>
static void doFindCommon_(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans);

template<typename ValueT, typename MapT>
static bool doRemoveIndex(Bean* bean, const ValueT& value, MapT& map);

Property::~Property()
{
    removeIndex();
    //remove property from beans that have this property
    for (auto& iter : m_beanMap_)
    {
        iter.first->m_propertyValues_.removeMember(m_name_.c_str());
    }
    m_beanMap_.clear();
}


int Property::createIndex()
{
    if (m_indexed_) return -1;
    if (m_propertyType_ == ArrayPrimaryType ||
         m_propertyType_ == ArrayRelationType) 
        return -1;
    Bean* bean = nullptr;
    for (auto& iter : m_beanMap_)
    {
        bean = iter.first;
        auto& value = bean->getMemberRef(this);
        if (!value.isNull())
                addIndex(bean, value);
    }
    m_indexed_ = true;
    return 0;
}


int Property::removeIndex()
{
    if (!m_indexed_) return -1;
    if (m_propertyType_ == ArrayPrimaryType ||
         m_propertyType_ == ArrayRelationType) 
        return -1;
    m_trueValueMap_.clear();
    m_falseValueMap_.clear();
    m_intValueMap_.clear();
    m_uintValueMap_.clear();
    m_doubleValueMap_.clear();
    m_strValueMap_.clear();
    m_indexed_ = false;
    return 0;
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


void Property::addBean(Bean* bean)
{
    m_beanMap_[bean] = 0;
}


void Property::removeBean(Bean* bean)
{
        auto iter = m_beanMap_.find(bean);
        if (iter != m_beanMap_.end())
            m_beanMap_.erase(iter);
}


void Property::addIndex(Bean* bean, const Json::Value& value)
{
    //todo:
    // if (m_propertyType_ == ArrayPrimaryType ||
    //     m_propertyType_ == ArrayRelationType)
    // {
    //     auto iter = m_beanMap_.find(bean);
    //     if (iter != m_beanMap_.end())
    //         iter->second++;
    // }    

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

    //todo:
    // if (m_propertyType_ == ArrayPrimaryType ||
    //     m_propertyType_ == ArrayRelationType)
    // {
    //     auto iter = m_beanMap_.find(bean);
    //     if (iter != m_beanMap_.end())
    //         iter->second--;
    // }

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


void Property::findEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon_(op_eq, value, beans);
}


void Property::findLessEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon_(op_le, value, beans);
}


void Property::findGreaterEqual(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon_(op_ge, value, beans);
}


void Property::findLessThan(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon_(op_lt, value, beans);
}


void Property::findGreaterThan(const Json::Value& value, std::list<Bean*>& beans) const
{
    findCommon_(op_gt, value, beans);
}


void Property::findHas(std::list<Bean*>& beans) const
{
    beans.clear();
    if (m_propertyType_ == ArrayPrimaryType || 
        m_propertyType_ == ArrayRelationType)
    {
            for (auto& item : m_beanMap_)
            {
                beans.push_back(item.first);
            }
    }
    else
    {
        if (m_indexed_)
        { //when indexed, take use of the index
            //make a fake value to reuse findCommon_() 
            Json::Value value;
            switch (m_valueType_)
            {
                case IntType:
                    value = 0;
                    break;
                case UIntType:
                    value = 0U;
                    break;
                case RealType:
                    value = 0.0f;
                    break;
                case StringType:
                    value = "";
                    break;
                case BoolType:
                    value = true;
                    break;
                default:
                    break;
            }
            findCommon_(op_has, value, beans);
        }
        else
        {
            for (auto& item : m_beanMap_)
            {
                beans.push_back(item.first);
            } 
        }
    }
}


std::list<oidType>&& Property::findSubjects(oidType objectId) const
{
    std::list<oidType>&& beans = std::list<oidType>();
    if (m_propertyType_ == RelationType)
    {
        if (m_indexed_)
        {
            //todo
        }
        else
        {
            for (auto& item : m_beanMap_)
            {
                auto& bean = item.first;
                const auto& value = bean->getMemberRef(this);
                oidType objectId_ = value.asUInt64();
                if (objectId_ == objectId)
                    beans.push_back(bean->getId());
            }
        }
    }
    else if (m_propertyType_ == ArrayRelationType)
    {
        //todo: currently just search all beans
        //take use of the index in future
        for (auto& item : m_beanMap_)
        {
            auto& bean = item.first;
            const auto& value = bean->getMemberRef(this);
            for (Json::ArrayIndex index = 0; index < value.size(); index++)
            {
                oidType objectId_ = value[index].asUInt64();
                if (objectId_ == objectId)
                    beans.push_back(bean->getId());
            }
        }
    }
    return std::move(beans);
}
    

void Property::findCommon_(int opType, const Json::Value& value, std::list<Bean*>& beans) const
{  
    beans.clear();
    if ((value.isNull() && opType != op_has) || value.isArray() || value.isObject()) return;
    if (getValueType() != (Property::ValueType)value.type()) return;
    //todo: search on array not supported yet
    if (getType() == Property::ArrayPrimaryType || 
        getType() == Property::ArrayRelationType) return; 

    if (m_indexed_)
    {     
        switch (value.type()) {
            case Json::intValue:
                doFindCommon_<int_t, decltype(m_intValueMap_)>
                    (opType, value.asInt64(), m_intValueMap_, beans);
                break;
            case Json::uintValue:
                doFindCommon_<uint_t, decltype(m_uintValueMap_)>
                    (opType, value.asUInt64(), m_uintValueMap_, beans);
                break;
            case Json::realValue:
                doFindCommon_<double, decltype(m_doubleValueMap_)>
                    (opType, value.asDouble(), m_doubleValueMap_, beans);
                break;
            case Json::stringValue:
                doFindCommon_<const char*, decltype(m_strValueMap_)>
                    (opType, value.asCString(), m_strValueMap_, beans);
                break;
            case Json::booleanValue:
                if (opType == op_has)
                { //add all beans
                    for (auto& item : m_trueValueMap_)
                        beans.push_back(item.second);
                    for (auto& item : m_falseValueMap_)
                        beans.push_back(item.second);
                }
                else if (opType == op_eq)
                {
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
                else
                {
                    //other operations don't make sense
                }
                break;
            default:
                elog("[%s:%d] value type %d not supported" ,  __FILE__, __LINE__, value.type());
                break;
        }
    }
    else
    {
            trivialFind(opType, value, beans);
    }
}



template<typename ValueT, typename MapT>
static void doFindCommon_(int opType, const ValueT& value, const MapT& map, std::list<Bean*>& beans) 
{
    beans.clear();
    auto lowerBound = map.begin();
    auto upperBound = map.end();

    switch (opType) {
        case op_has:
            //need to add all items
            // lowerBound = map.begin();
            // upperBound = map.end();
            break; 
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
            //make the output beans empty 
            lowerBound = map.end();
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


void Property::trivialFind(int opType,  const Json::Value& value, std::list<Bean*>& beans) const
{
    beans.clear();
    if (value.isNull() || value.isArray() || value.isObject()) return;
    if (getValueType() != (Property::ValueType)value.type()) return;
    //todo: search on array not supported yet
    if (getType() == Property::ArrayPrimaryType || 
         getType() == Property::ArrayRelationType) return; 

    Bean* bean = nullptr;
    for (auto& item : m_beanMap_)
    {
        bean = item.first;
        const Json::Value& v = bean->getMemberRef(this);
        if (v.isNull()) continue; //not found or null
        switch (opType) {
            case op_eq:
                if (v == value) beans.push_back(bean);
                break;
            case op_le:
                if (v <= value) beans.push_back(bean);
                break;
            case op_ge:
                if (v >= value) beans.push_back(bean);
                break;
            case op_lt:
                if (v < value) beans.push_back(bean);
                break;
            case op_gt:
                if (v > value) beans.push_back(bean);
                break;
            default:
                break;
        }
    }
}


bool Property::StrComparator::operator()(const char* const & a, const char* const & b) const
{
    if (a == b) return false;
    return strcmp(a, b) < 0;
}



}
}
}