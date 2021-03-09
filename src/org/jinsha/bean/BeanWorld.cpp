
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


pidType BeanWorld::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


pidType BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


pidType BeanWorld::defineRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


pidType BeanWorld::defineArrayRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


pidType BeanWorld::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap_.find(name);
    if (iter != m_propertyMap_.end()) return -2; 
    
    Property* property = new Property(this, name, type, valueType, needIndex);
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
        iter.second->removeProperty(property->m_id_);
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
    setPropertyCommon_(bean, property,  oldValue, value);
}


void BeanWorld::setArrayProperty( Bean* bean, Property* property, Json::Value::ArrayIndex index, const Json::Value& value)
{
    Json::Value* oldValue = (Json::Value*)&bean->getMemberRef(property->getName().c_str());
    oldValue = &(*oldValue)[index];
    setPropertyCommon_(bean, property,  oldValue, value);
}


void BeanWorld::setRelation(Property* property, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property->getName().c_str());
    Json::Value newValue(to->getId());
    setPropertyCommon_(from, property, oldValue, newValue);
}


void BeanWorld::setArrayRelation(Property* property, Json::Value::ArrayIndex index, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property->getName().c_str());
    oldValue = &(*oldValue)[index];
    Json::Value newValue(to->getId());
    setPropertyCommon_(from, property, oldValue, newValue);
}


void BeanWorld::setPropertyCommon_(Bean* bean,  Property* property, 
    Json::Value* oldValue, const Json::Value&  newValue)
{
    if ((*oldValue) == newValue) return;
    const string& pname = property->getName();
    if (oldValue->isNull())
    { 
        //no old value, need to increment ref. count
        // property->m_refCount_++;
        oldValue = &bean->m_propertyValues_[pname];

        if (property->getType() != Property::ArrayPrimaryType &&
             property->getType() != Property::ArrayRelationType )
            //keep track the bean who has this property
            property->addBean(bean);
    }
    else
    {
        //todo: do not index array property/relation for now
        // if (property->indexed())
        if (property->getType() != Property::ArrayPrimaryType && 
            property->getType() != Property::ArrayRelationType && 
             property->indexed())
            //remove index for previous value
            property->removeIndex(bean, *oldValue);
    }

    //set value for json object
    (*oldValue)  = newValue;

    //todo: do not index array property/relation for now
    // if (property->indexed())
    if (property->getType() != Property::ArrayPrimaryType && 
        property->getType() != Property::ArrayRelationType && 
            property->indexed())
    property->addIndex(bean, *oldValue);
}


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


void BeanWorld::findHas(const char* name,  std::list<Bean*>& beans)
{
    beans.clear();
    const Property* property = getProperty(name);
    if (property == nullptr) return;
    property->findHas(beans);
}


std::list<oidType>&& BeanWorld::findSubjects(Property* relation, oidType objectId)
{
    if (relation != nullptr) 
        return std::move(relation->findSubjects(objectId));
    else
        return std::move(std::list<oidType>());
}


void BeanWorld::findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_eq, propertyName, value, beans);
}


void BeanWorld::findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_le, propertyName, value, beans);
}


void BeanWorld::findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_ge, propertyName, value, beans);
}


void BeanWorld::findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_lt, propertyName, value, beans);
}


void BeanWorld::findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_gt, propertyName, value, beans);
}


void BeanWorld::findCommon_(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    beans.clear();
    if ((value.isNull() && opType != op_has) || value.isArray() || value.isObject()) return;
    const Property* property = getProperty(propertyName);
    if (property == nullptr) return;
    if (property->getValueType() != (Property::ValueType)value.type()) return;
    //todo: search on array not supported yet
    if (property->getType() == Property::ArrayPrimaryType || 
        property->getType() == Property::ArrayRelationType) return; 

    if (property->indexed())
    { //indexed by property, use index to improve performance
        property->findCommon_(opType, value, beans);
    }
    else
    { //no index, do trivial find
        trivialFind(opType, propertyName, value, beans);
    }
}


void BeanWorld::trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    beans.clear();
    if (value.isNull() || value.isArray() || value.isObject()) return;
    const Property* property = getProperty(propertyName);
    if (property == nullptr) return;
    if (property->getValueType() != (Property::ValueType)value.type()) return;
    //todo: search on array not supported yet
    if (property->getType() == Property::ArrayPrimaryType || 
        property->getType() == Property::ArrayRelationType) return; 

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


int BeanWorld::createIndex(pidType id)
{
    Property* property = (Property*)getProperty(id);
    if (property  == nullptr) return -1;
    return property->createIndex();
}


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
