
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


pidType BeanWorld::getRelationId(const char* name) const
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_relationMap_.find(name);
    if (iter == m_relationMap_.end()) return -1; //no such relation
    return (int)(iter->second);
}


const Relation* BeanWorld::getRelation(pidType id) const
{
    if (id < 0 || (size_t)id >= m_relations_.size())
        return nullptr;
    else
        return m_relations_[id];
}


Relation* BeanWorld::getRelation(pidType id)
{
    return (Relation*)((const BeanWorld*)this)->getRelation(id);
}


const Relation* BeanWorld::getRelation(const char* name) const
{
    return getRelation(getRelationId(name));
}

Relation* BeanWorld::getRelation(const char* name)
{
    return getRelation(getRelationId(name));
}


pidType BeanWorld::setProperty( Bean* bean, const char* name, const Json::Value& value)
{
    pidType  id = getPropertyId(name);
    if (id == -1)
    {
        id = addProperty<Property>(name, m_properties_, m_propertyMap_);
    }
    setProperty(bean, id, value);
    return id;
}


void BeanWorld::setProperty( Bean* bean, pidType id, const Json::Value& value)
{
    Property* property = m_properties_[id];
    if (property != nullptr)
        doSetProperty<Property>(bean, property,  value);
}


pidType BeanWorld::setRelation(const char* name, Bean* from, Bean* to)
{
    pidType  id = getRelationId(name);
    if (id == -1)
    {
        id = addProperty<Relation>(name, m_relations_, m_relationMap_);
    }
    setRelation(id, from, to);
    return id;
}


void BeanWorld::setRelation( pidType id, Bean* from, Bean* to)
{
    Relation* relation = m_relations_[id];
    if (relation != nullptr)
    {
        Json::Value value = to->getId();
        doSetProperty<Relation>(from, relation,  value);
    }
}


template<typename T>
void BeanWorld::doSetProperty(Bean* bean, T* property, 
    const Json::Value&  value)
{
    const string& pname = property->getName();
    Json::Value* oldValue = nullptr;
    if (typeid(property) == typeid((Property*)0)) 
    {
        oldValue = (Json::Value*)&bean->getPropertyRef(pname.c_str());
    } 
    else 
    {
        oldValue = (Json::Value*)&bean->getRelationRef(pname.c_str());
    }
    if (oldValue->isNull())
    { 
        //no old value, need to increment ref. count
        property->m_refCount_++;
        if (typeid(property) == typeid((Property*)0)) 
            oldValue = &bean->m_propertyValues_[pname];
        else
            oldValue = &bean->m_relationValues_[pname];
    }
    else
    {
        if (property->indexed())
            //remove index for previous value
            property->removeIndex(bean, *oldValue);
    }

    //set value for json object
    (*oldValue)  = value;
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
            property->removeIndex(bean, bean->getPropertyRef(pname));
        //remove member of json object
        value = bean->m_propertyValues_.removeMember(pname);
        property->m_refCount_--;
        if (property->m_refCount_ == 0)
        { //if this property is not used by any bean, remove it
            // removeProperty(property);
            removeProperty<Property>(property, m_properties_, m_propertyMap_);
        }
    }

    return value;
}

template<typename T>
pidType BeanWorld::addProperty(const char* name, 
    std::vector<T*>&properties, 
    std::unordered_map<std::string, pidType>& propertyMap)
{
    pidType id = getPropertyId(name);
    if (id == -1)
    {//no such property
        T* property = new T(this, name);
        properties.push_back(property);
        propertyMap[name] = properties.size() - 1;
        id = properties.size() - 1;
        property->m_pid_ = id;
    }
    return id;
}

// pidType BeanWorld::addProperty(const char* name)
// {
//     int id = getPropertyId(name);
//     if (id == -1)
//     {//no such property
//         Property* property = new Property(this, name);
//         m_properties_.push_back(property);
//         m_propertyMap_[name] = m_properties_.size() - 1;
//         id = m_properties_.size() - 1;
//         property->m_pid_ = id;
//     }
//     return id;
// }


template<typename T>
void BeanWorld::removeProperty(T* property,
    std::vector<T*>&properties, 
    std::unordered_map<std::string, pidType>& propertyMap)
{
    // if (property == nullptr) return;
    auto iter = propertyMap.find(property->getName());
    if (iter != propertyMap.end()) propertyMap.erase(iter);
    properties[property->m_pid_] = nullptr;
    delete property;
}

// void BeanWorld::removeProperty(Property* property)
// {
//     // if (property == nullptr) return;
//     auto iter = m_propertyMap_.find(property->getName());
//     if (iter != m_propertyMap_.end()) m_propertyMap_.erase(iter);
//     m_properties_[property->m_pid_] = nullptr;
//     delete property;
// }

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

// pidType BeanWorld::addRelation(Bean* from, Bean* to, const char* propertyName)
// {
//     // if (propertyName == nullptr) return -1;
//     // if (propertyName[0] == 0) return -1;

//     // pidType  id = addProperty(propertyName);
//     // (*from)[propertyName] = to->getId();
// }


// int BeanWorld::removeRelation(Bean* from, Bean* to, const char* propertyName)
// {
    // if (propertyName == nullptr) return -1;
    // if (propertyName[0] == 0) return -1;

    // from->removeProperty(propertyName);

    // pidType  id = getPropertyId(propertyName);
        //todo
        // m_propertyIndexTypes_[id] = Json::objectValue;
// }


void BeanWorld::recreateIndex(Property* property)
{
    Bean* bean = nullptr;
    const string& pname = property->getName();
    property->removeIndex();
    for (auto& iter : m_beans_)
    {
        bean = iter.second;
        if (bean->isMember(pname))
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
        const Json::Value& v = bean->getPropertyRef(propertyName);
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
