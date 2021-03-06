
#include <assert.h>
#include <cstring>
#include <locale.h>

#include "./BeanWorld.h"
#include "./internal_common.hxx"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {


BeanWorld::BeanWorld()
{
    setlocale(LC_ALL, "");
}


BeanWorld::~BeanWorld()
{
    clear();
};


void BeanWorld::clear()
{
    m_propertyMap_.clear();

    for (auto item : m_properties_)
    {
        delete item;
    }
    m_properties_.clear();

    for (auto item : m_beans_) 
    {
        delete item.second;
    }
    m_beans_.clear();
}


Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateBeanId();
    m_beans_[bean->m_id_] = bean;
    return bean;
};


void BeanWorld::removeBean(oidType id)
{
    auto iter = m_beans_.find(id);
    if (iter != m_beans_.end())
    {
       delete iter->second;
        m_beans_.erase(iter);
    }
}


int BeanWorld::getNumOfBeans()
{
    return m_beans_.size();
}

const unordered_map<oidType, Bean*>& BeanWorld::getBeans()
{
    return m_beans_;
}


Bean* BeanWorld::getBean(oidType id)
{
    auto iter = m_beans_.find(id);
    if (iter == m_beans_.end())
    {
        return nullptr;
    }
    else
    {
        return iter->second;
    }
}


pidType BeanWorld::defineProperty(const char* name, Property::ValueType valueType)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType);
}


pidType BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType);
}


pidType BeanWorld::defineRelation(const char* name)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType);
}


pidType BeanWorld::defineArrayRelation(const char* name)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType);
}


pidType BeanWorld::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool createIndex)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap_.find(name);
    if (iter != m_propertyMap_.end()) return -2; 
    
    Property* property = new Property(this, name, type, valueType, createIndex);
    m_properties_.push_back(property);
    m_propertyMap_[name] = m_properties_.size() - 1;
    pidType id = m_properties_.size() - 1;
    property->m_id_ = id;
    return id;
}


void BeanWorld::undefineProperty(const char* name)
{
    if (name == nullptr) return;
    if (name[0] == 0) return;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return;
    pidType id = -1;
    id = iter->second;
    Property* property = m_properties_[id];

    //remove all beans that have this property
    //todo: can be optimized by checking the property
    //instance for how many beans have this property 
    for (auto& iter : m_beans_)
    {
        removeProperty(iter.second, property);
    }

    m_properties_[id] = nullptr;
    m_propertyMap_.erase(iter);
    delete property;
}

pidType BeanWorld::getPropertyId(const char* name) const
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return -1; //no such property
    return (int)(iter->second);
}


const Property* BeanWorld::getProperty(pidType id) const
{
    if (id < 0 || (size_t)id >= m_properties_.size())
        return nullptr;
    else
        return m_properties_[id];
}


Property* BeanWorld::getProperty(pidType id)
{
    return (Property*)((const BeanWorld*)this)->getProperty(id);
}


const Property* BeanWorld::getProperty(const char* name) const
{
    return getProperty(getPropertyId(name));
}

Property* BeanWorld::getProperty(const char* name)
{
    return getProperty(getPropertyId(name));
}


void BeanWorld::setProperty( Bean* bean, Property* property, const Json::Value& value)
{
    Json::Value* oldValue = (Json::Value*)&bean->getMemberRef(property->getName().c_str());
    doSetProperty(bean, property,  oldValue, value);
}


void BeanWorld::setArrayProperty( Bean* bean, Property* property, Json::Value::ArrayIndex index, const Json::Value& value)
{
    Json::Value* oldValue = (Json::Value*)&bean->getMemberRef(property->getName().c_str());
    oldValue = &(*oldValue)[index];
    doSetProperty(bean, property,  oldValue, value);
}


void BeanWorld::setRelation(Property* property, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property->getName().c_str());
    Json::Value newValue(to->getId());
    doSetProperty(from, property, oldValue, newValue);
}


void BeanWorld::setArrayRelation(Property* property, Json::Value::ArrayIndex index, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property->getName().c_str());
    oldValue = &(*oldValue)[index];
    Json::Value newValue(to->getId());
    doSetProperty(from, property, oldValue, newValue);
}


void BeanWorld::doSetProperty(Bean* bean,  Property* property, 
    Json::Value* oldValue, const Json::Value&  newValue)
{
    if ((*oldValue) == newValue) return;
    const string& pname = property->getName();
    if (oldValue->isNull())
    { 
        //no old value, need to increment ref. count
        property->m_refCount_++;
        oldValue = &bean->m_propertyValues_[pname];
    }
    else
    {
        if (property->indexed())
            //remove index for previous value
            property->removeIndex(bean, *oldValue);
    }

    //set value for json object
    (*oldValue)  = newValue;
    if (property->indexed())
        property->addIndex(bean, *oldValue);
}


Json::Value BeanWorld::removeProperty(Bean* bean,  Property* property)
{
    const char* pname = property->getName().c_str();
    Json::Value value;
    if (bean->isMember(pname))
    {
        if (property->indexed())
           //remove index first
            property->removeIndex(bean, bean->getMemberRef(pname));
        //remove member of json object
        value = bean->m_propertyValues_.removeMember(pname);
        property->m_refCount_--;
        // if (property->m_refCount_ == 0)
        // { //if this property is not used by any bean, remove it
        //     undefineProperty(pname);
        // }
    }

    return value;
}


// pidType BeanWorld::addRelation( const char* name)
// {
//     pidType id = getRelationId(name);
//     if (id == -1)
//     {//no such relation
//         Relation* relation = new Relation(this, name);
//         m_relations_.push_back(relation);
//         m_relationMap_[name] = m_relations_.size() - 1;
//         id = m_properties_.size() - 1;
//         property->m_pid_ = id;
//     }
//     return id;
// }


void BeanWorld::recreateIndex(Property* property)
{
    Bean* bean = nullptr;
    const string& pname = property->getName();
    property->removeIndex();
    for (auto& iter : m_beans_)
    {
        bean = iter.second;
        if (bean->isMember(pname.c_str()))
        {
                Json::Value& value = bean->m_propertyValues_[pname];
                property->addIndex(bean, value);
        }
    }
    property->m_indexed_ = true;
}


void BeanWorld::findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_eq, propertyName, value, beans);
}


void BeanWorld::findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_le, propertyName, value, beans);
}


void BeanWorld::findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_ge, propertyName, value, beans);
}


void BeanWorld::findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_lt, propertyName, value, beans);
}


void BeanWorld::findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_gt, propertyName, value, beans);
}


void BeanWorld::findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    beans.clear();
   const Property* property = getProperty(propertyName);
   if (property == nullptr) return;
    if (property->indexed())
    { //indexed by property, use index to improve performance
        // switch (type)
        property->findCommon(opType, value, beans);
    }
    else
    { //no index, do trivial find
        trivialFind(opType, propertyName, value, beans);
    }
}


void BeanWorld::trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    if (value.isNull() || value.isArray() || value.isObject()) return;
    beans.clear();
    Bean* bean = nullptr;
    for (auto& item : m_beans_)
    {
        bean = item.second;
        const Json::Value& v = bean->getMemberRef(propertyName);
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


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
