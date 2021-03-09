
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

Bean::Bean(BeanWorld* world) : 
m_propertyValues_(Json::ValueType::objectValue), 
m_world_(world)
{

}


Bean::~Bean()
{
    clear();
}


// bool Bean::isMember(const string& key) const
// {
//     return isMember(key.c_str());
// }


bool Bean::isMember(const char* name) const
{
    if (name == nullptr) return false;
    return m_propertyValues_.isMember(name);
}


bool Bean::hasProperty(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return hasProperty(pid);
}


bool Bean::hasProperty(pidType id) const
{
    return doHasProperty(id, Property::PrimaryType);
}


bool Bean::doHasProperty(pidType id, Property::Type type) const
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return false;
    if (property->getType() != type) return false;
    const char* name = property->getName().c_str();
    return m_propertyValues_.isMember(name);
}


void Bean::clear()
{
    for (auto& memberName : m_propertyValues_.getMemberNames())
    {
        removeProperty(memberName.c_str());
    }
}


Json::Value Bean::getProperty(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return getProperty(pid);
}

Json::Value Bean::getProperty(pidType id) const
{
    Json::Value value(Json::nullValue);
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return value;
    if (property->getType() != Property::PrimaryType) return value;
    value = m_propertyValues_.get(property->getName().c_str(), value);
    return value;
}


int Bean::setProperty(const char* name,  const Json::Value& value)
{
    pidType pid = m_world_->getPropertyId(name);
    return setProperty(pid, value);
}


int Bean::setProperty(pidType id,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    m_world_->setProperty(this, property, value);
    return 0;
}



bool Bean::hasArrayProperty(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return hasArrayProperty(pid);
}


bool Bean::hasArrayProperty(pidType id) const
{
    return doHasProperty(id, Property::ArrayPrimaryType);
}


Json::Value::ArrayIndex Bean::getArrayPropertySize(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return getArrayPropertySize(pid);
}


Json::Value::ArrayIndex Bean::getArrayPropertySize(pidType id) const
{
    return getArrayMemberSizeCommon_(id, true);
}


Json::Value::ArrayIndex Bean::getArrayMemberSizeCommon_(pidType id, 
        bool isProperty_) const
{
    const char* pname = nullptr;
    const Json::Value* value = nullptr;
    if (isProperty_)
    {
        if (!hasArrayProperty(id)) return 0;
        pname = m_world_->getProperty(id)->getName().c_str();
        value = &m_propertyValues_[pname];
    }
    else
    {
        if (!hasArrayRelation(id)) return 0;
        pname = m_world_->getProperty(id)->getName().c_str();
        value = &m_propertyValues_[pname];
    }
    
    if (!value->isArray()) return 0;
    return value->size();
}


Json::Value Bean::getArrayProperty(const char* name,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value(Json::nullValue);
    if (name == nullptr) return value;
    if (name[0] == 0) return value;
    if (!hasArrayProperty(name)) return value;
    if (index >= m_propertyValues_[name].size()) return value;
    return m_propertyValues_[name][index];
}


int Bean::createArrayProperty(const char* name)
{
    pidType pid = m_world_->getPropertyId(name);
    return createArrayProperty(pid);
}


int Bean::createArrayProperty(pidType id)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    const char* pname = property->getName().c_str();
    if (isMember(pname)) return 0;
    m_propertyValues_[pname] = Json::Value(Json::arrayValue);
    property->addBean(this);
    return 0;
}


int Bean::setArrayProperty(const char* name,  
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    pidType pid = m_world_->getPropertyId(name);
    return setArrayProperty(pid, index, value);
}


int Bean::setArrayProperty(pidType id, 
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    if (value.isNull()) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName().c_str();
    if (!hasArrayProperty(pname)) return -4;
    if (index >= m_propertyValues_[pname].size()) return -5;

    // m_propertyValues_[pname][index] = value;
    m_world_->setArrayProperty(this, property, index, value);
    return 0;
}


int Bean::appendProperty(const char* name,  const Json::Value& value)
{
    pidType pid = m_world_->getPropertyId(name);
    return appendProperty(pid, value);
}


int Bean::appendProperty(pidType id,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    const auto& pname = property->getName().c_str();
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    if (!hasArrayProperty(pname)) return -4;
    auto& arrayValue = m_propertyValues_[pname];
    arrayValue.append(Json::Value(property->getValueType()));
    m_world_->setArrayProperty(this, property, arrayValue.size() - 1, value);
    return 0;
}


int Bean::resizeProperty(const char* name,  Json::Value::ArrayIndex size)
{
    pidType pid = m_world_->getPropertyId(name);
    return resizeProperty(pid, size);
}


int Bean::resizeProperty(pidType id,  Json::Value::ArrayIndex size)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -1;
    if (property->getType() != Property::ArrayPrimaryType) return -1;
    const auto& pname = property->getName().c_str();
    if (!hasArrayProperty(pname)) return -1;
    m_propertyValues_[pname].resize(size);
    return 0;
}


bool Bean::hasRelation(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return hasRelation(pid);
}


bool Bean::hasRelation(pidType id) const
{
    return doHasProperty(id, Property::RelationType);
}


bool Bean::hasArrayRelation(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return hasArrayRelation(pid);
}


bool Bean::hasArrayRelation(pidType id) const
{
    return doHasProperty(id, Property::ArrayRelationType);
}


Json::Value::ArrayIndex Bean::getArrayRelationSize(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return getArrayRelationSize(pid);
}


Json::Value::ArrayIndex Bean::getArrayRelationSize(pidType id) const
{
    return getArrayMemberSizeCommon_(id, false);
}


oidType Bean::getRelationBeanId(const char* name) const
{
    pidType pid = m_world_->getPropertyId(name);
    return getRelationBeanId(pid);
}

oidType Bean::getRelationBeanId(pidType id) const
{
    if (!hasRelation(id)) return 0;
    auto pname = m_world_->getProperty(id)->getName().c_str();
    auto& value = m_propertyValues_[pname];
    return value.asUInt64();
    
}


oidType Bean::getRelationBeanId(const char* name,  
    Json::Value::ArrayIndex index) const
{
    pidType pid = m_world_->getPropertyId(name);
    return getRelationBeanId(pid, index);
}


oidType Bean::getRelationBeanId(pidType id,  
    Json::Value::ArrayIndex index) const
{
    if (!hasArrayRelation(id)) return 0;
    auto pname = m_world_->getProperty(id)->getName().c_str();
    auto& value = m_propertyValues_[pname];
    if (!value.isArray()) return 0;
    if (index >= value.size()) return 0;
    return value[index].asUInt64();
}


pidType Bean::setRelation(const char* name, Bean* bean)
{
    pidType pid = m_world_->getPropertyId(name);
    return setRelation(pid, bean);
}


int Bean::setRelation(pidType id,  Bean* bean)
{
    if (bean == nullptr) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::RelationType) return -2;
    m_world_->setRelation(property, this, bean);
    return 0;
}


int Bean::createArrayRelation(const char* name)
{
    pidType pid = m_world_->getPropertyId(name);
    return createArrayRelation(pid);
}


int Bean::createArrayRelation(pidType id)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    const char* pname = property->getName().c_str();
    if (isMember(pname)) return 0;
    m_propertyValues_[pname] = Json::Value(Json::arrayValue);
    property->addBean(this);
    return 0;
}


int Bean::appendRelation(const char* name,  Bean* bean)
{
    pidType pid = m_world_->getPropertyId(name);
    return appendRelation(pid, bean);
}


int Bean::appendRelation(pidType id,  Bean* bean)
{
    if (bean == nullptr) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    const auto& pname = property->getName().c_str();
    if (!hasArrayRelation(pname)) return -4;
    auto& arrayValue = m_propertyValues_[pname];
    arrayValue.append(bean->getId());
    m_world_->setArrayRelation(property, arrayValue.size() - 1, this, bean);
    return 0;
}


int Bean::setRelation(const char* name,  
    Json::Value::ArrayIndex index, Bean* bean)
{
    pidType pid = m_world_->getPropertyId(name);
    return setRelation(pid, index, bean);
}


int Bean::setRelation(pidType id, 
    Json::Value::ArrayIndex index, Bean* bean)
{
    if (bean == nullptr) return -1;
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    const auto& pname = property->getName().c_str();
    if (!hasArrayRelation(pname)) return -4;
    if (index >= m_propertyValues_[pname].size()) return -5;
    // Json::Value &oldValue = m_propertyValues_[pname][index];
    // m_propertyValues_[pname][index] = bean.getId();
    m_world_->setArrayRelation(property, index, this, bean);
    return 0;
}


Json::Value Bean::removeProperty( const char* name)
{
    pidType pid = m_world_->getPropertyId(name);
    return removeProperty(pid);
}


Json::Value Bean::removeProperty(pidType id)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return Json::Value();
    const char* pname = property->getName().c_str();
    if (!isMember(pname)) return Json::Value();
    property->removeBean(this);
    if (property->indexed())
    { //remove index first
        if (property->getType() == Property::ArrayPrimaryType ||
            property->getType() == Property::ArrayRelationType) 
        {
            auto& array = getMemberRef(pname);
            for (int i = 0; i< array.size(); i++)
            {
                property->removeIndex(this, array[i]);
            }
        }
        else
        {
            property->removeIndex(this, getMemberRef(pname));
        }
    }

    //remove member of json object
    Json::Value&& value = this->m_propertyValues_.removeMember(pname);
    return value;
}


void Bean::removeRelation( const char* name)
{
    pidType pid = m_world_->getPropertyId(name);
    return removeRelation(pid);
}


void Bean::removeRelation(pidType id)
{
    removeProperty(id);
}


const Json::Value& Bean::getMemberRef(const char* key)
{
    // return (isMember(key)) ? m_propertyValues_[key] :
    // Json::Value::null;
    if (isMember(key)) return m_propertyValues_[key];
    return Json::Value::null;
}


// class Iterator
// {
// public:
//     Iterator& begin();
//     Iterator& end();
//     Bean& operator*();
//     Bean* operator&();
//     Iterator& operator++();
//     Iterator& operator--();

// private:
//     Iterator(const Iterator& other) = delete;
//     Iterator& operator=(const Iterator& other) = delete;
//     static const Iterator end_;
//     std::list<Bean*> items_;
// }

}
}
}