
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
    for (auto item : m_beans_) 
    {
        delete item.second;
    }
    m_beans_.clear();
    m_propertyMap_.clear();
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
    //todo: need to handle bean relations
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


Property* BeanWorld::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


Property* BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


Property* BeanWorld::defineRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


Property* BeanWorld::defineArrayRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


Property* BeanWorld::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
    if (name == nullptr) return nullptr;
    if (name[0] == 0) return nullptr;

    Property* property = nullptr;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end())
    {
        property = new Property(this, name, type, valueType, needIndex);
        m_propertyMap_[name] = property;
        //todo: current set to 0. Generate id in future...
        property->m_id_ = -1;
    }
    else
    {
        property = iter->second;
        if (property->getType() != type || property->getValueType() != valueType)
            property = nullptr; 
    }
    return property;
}


void BeanWorld::undefineProperty(const char* name)
{
    if (name == nullptr) return;
    if (name[0] == 0) return;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return;
    Property* property = iter->second;
    delete property;
    m_propertyMap_.erase(iter);
}


const Property* BeanWorld::getProperty(const char* name) const
{
    if (name == nullptr) return nullptr;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end())
        return nullptr;
    else
        return iter->second;
}


Property* BeanWorld::getProperty(const char* name)
{
    return const_cast<Property*>(((const BeanWorld*)this)->getProperty(name));
}

void BeanWorld::setProperty( Bean* bean, Property* property, const Json::Value& value)
{
    Json::Value* oldValue = (Json::Value*)&bean->getMemberRef(property);
    setPropertyCommon_(bean, property,  oldValue, value);
}


void BeanWorld::setArrayProperty( Bean* bean, Property* property, Json::Value::ArrayIndex index, const Json::Value& value)
{
    Json::Value* oldValue = (Json::Value*)&bean->getMemberRef(property);
    oldValue = &(*oldValue)[index];
    setPropertyCommon_(bean, property,  oldValue, value);
}


void BeanWorld::setRelation(Property* property, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property);
    Json::Value newValue(to->getId());
    setPropertyCommon_(from, property, oldValue, newValue);
}


void BeanWorld::setArrayRelation(Property* property, Json::Value::ArrayIndex index, Bean* from, Bean* to)
{
    Json::Value* oldValue =  (Json::Value*)&from->getMemberRef(property);
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
    property->removeIndex();
    for (auto& iter : m_beans_)
    {
        bean = iter.second;
        auto& value = bean->getMemberRef(property);
        if (!value.isNull())
        {
                property->addIndex(bean, value);
        }
    }
    property->m_indexed_ = true;
}


void BeanWorld::findHas(const Property* property,  std::list<Bean*>& beans)
{
    beans.clear();
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


void BeanWorld::findEqual(const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_eq, property, value, beans);
}


void BeanWorld::findLessEqual(const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_le, property, value, beans);
}


void BeanWorld::findGreaterEqual(const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_ge, property, value, beans);
}


void BeanWorld::findLessThan(const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_lt, property, value, beans);
}


void BeanWorld::findGreaterThan(const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon_(op_gt, property, value, beans);
}


void BeanWorld::findCommon_(int opType, const Property* property,  const Json::Value& value, std::list<Bean*>& beans)
{
    beans.clear();
    if ((value.isNull() && opType != op_has) || value.isArray() || value.isObject()) return;
    if (property == nullptr) return;
    if (property->getValueType() != (Property::ValueType)value.type()) return;
    //todo: search on array not supported yet
    if (property->getType() == Property::ArrayPrimaryType || 
        property->getType() == Property::ArrayRelationType) return; 

        property->findCommon_(opType, value, beans);
}


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
