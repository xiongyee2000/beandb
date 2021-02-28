
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


bool Bean::isMember (const string& key) const
{
    return isMember(key.c_str());
}


bool Bean::isMember(const char* name) const
{
    return (hasProperty(name) || hasRelation(name));
}


bool Bean::hasProperty(const char* name) const
{
    if (name == nullptr) return false;
    if (name[0] == 0) return false;
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
    Json::Value value;
    value = m_propertyValues_.get(name, value);
    return value;
}


int Bean::setProperty(const char* name,  const Json::Value& value)
{
    pidType pid = m_world_->getPropertyId(name);
    return setProperty(pid, value);
}


int Bean::setProperty(pidType id,  const Json::Value& value)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -1;
    if (value.isNull()) return -2;
    if (property->getType() != Property::PrimaryType) return -3;
    if (property->getValueType() != value.type()) return -4;
    m_world_->setProperty(this, property, value);
    return 0;
}


bool Bean::hasArrayProperty(const char* name) const
{
    if (name == nullptr) return false;
    if (name[0] == 0) return false;
    if (!hasProperty(name)) return false;
    return  m_propertyValues_[name].isArray();   
}


Json::Value::ArrayIndex Bean::getArrayPropertySize(const char* name) const
{
    return doGetPropertySize(name);
}


Json::Value::ArrayIndex Bean::doGetPropertySize(const char* name, 
        bool isProperty_) const
{
    if (name == nullptr) return 0;
    if (name[0] == 0) return 0;
    const Json::Value* value = nullptr;
    if (isProperty_)
    {
        if (!hasProperty(name)) return 0;
        value = &m_propertyValues_[name];
    }
    else
    {
        if (!hasRelation(name)) return 0;
        value = &m_propertyValues_[name];
    }
    
    if (!value->isArray()) return 0;
    return value->size();
}


Json::Value Bean::getArrayProperty(const char* name,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value;
    if (name == nullptr) return value;
    if (name[0] == 0) return value;
    if (!hasArrayProperty(name)) return value;
    if (index >= m_propertyValues_[name].size()) return value;
    return m_propertyValues_[name][index];
}


int Bean::setArrayProperty(const char* name)
{
    pidType pid = m_world_->getPropertyId(name);
    return setArrayProperty(pid);
}


int Bean::setArrayProperty(pidType id)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -1;
    if (property->getType() != Property::PrimaryArrayType) return -1;
    m_propertyValues_[property->getName()] = Json::Value(Json::arrayValue);
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
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -1;
    if (property->getType() != Property::PrimaryArrayType) return -1;
    const auto& pname = property->getName().c_str();
    if (!hasArrayProperty(pname)) return -5;
    if (value.isNull()) return -2;
    if (property->getValueType() != value.type()) return -3;
    if (index >= m_propertyValues_[pname].size()) return -4;
    m_propertyValues_[pname][index] = value;
    return 0;
}


int Bean::appendArrayProperty(const char* name,  const Json::Value& value)
{
    pidType pid = m_world_->getPropertyId(name);
    return appendArrayProperty(pid, value);
}


int Bean::appendArrayProperty(pidType id,  const Json::Value& value)
{
    Property* property = (Property*)m_world_->getProperty(id);
    if (property == nullptr) return -1;
    if (property->getType() != Property::PrimaryArrayType) return -1;
    const auto& pname = property->getName().c_str();
    if (!hasArrayProperty(pname)) return -5;
    if (value.isNull()) return -2;
    if (property->getValueType() != value.type()) return -3;
    m_propertyValues_[pname].append(value);
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
    if (property->getType() != Property::PrimaryArrayType) return -1;
    const auto& pname = property->getName().c_str();
    if (!hasArrayProperty(pname)) return -1;
    m_propertyValues_[pname].resize(size);
    return 0;
}


bool Bean::hasRelation(const char* name) const
{
    if (!hasProperty(name)) return false;
    Property* property = m_world_->getProperty(name);
    return property->getType() == Property::RelationType;
}


oidType Bean::getBeanId(const char* name) const
{
    Json::Value value;
    value = m_propertyValues_.get(name, value);
    if (value.isNull()) return 0;
    return value.asUInt64();
}


oidType Bean::getBeanId(const char* name,  
    Json::Value::ArrayIndex index) const
{
    if (!hasRelation(name)) return 0;
    auto& value = m_propertyValues_[name];
    if (!value.isArray()) return 0;
    if (index >= value.size()) return 0;
    return value[index].asUInt64();
}


pidType Bean::setRelation(const char* name, Bean* bean)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (bean == nullptr) return -2;
    // return m_world_->setRelation(name, this, bean);
    return 0;
}


int Bean::setRelation(pidType rid,  Bean* bean)
{
    // Relation* relation = (Relation*)m_world_->getRelation(rid);
    // if (relation == nullptr) return -1;
    // if (bean == nullptr) return -2;
    // m_world_->setRelation(this, rid, bean);
    return 0;
}


Json::Value Bean::removeProperty( const char* name)
{
    if (name == nullptr) return Json::Value();
    if (name[0] == 0) return Json::Value();
    Property* property = (Property*)m_world_->getProperty(name);
    if (property == nullptr) 
        return m_propertyValues_.removeMember(name); 
    else
        return m_world_->removeProperty(this, property);
}


Json::Value Bean::removeProperty(pidType pid)
{
    Property* property = (Property*)m_world_->getProperty(pid);
    if (property == nullptr) return Json::Value();
    return m_world_->removeProperty(this, property);
}


const Json::Value& Bean::getPropertyRef(const char* key)
{
    return (hasProperty(key)) ? m_propertyValues_[key] :
    Json::Value::null;
}


const Json::Value& Bean::getRelationRef(const char* key)
{
    return (hasRelation(key)) ? m_propertyValues_[key] :
    Json::Value::null;
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