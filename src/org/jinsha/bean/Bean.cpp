
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


bool Bean::isMember(const char* key) const
{
    return (isProperty(key) || isRelation(key));
}


bool Bean::isProperty(const char* key) const
{
    if (key == nullptr) return false;
    if (key[0] == 0) return false;
    return m_propertyValues_.isMember(key);
    // const Json::Value& v = m_jsonValue_.get(key, Json::Value::null);
    // return (v != Json::Value::null);
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


pidType Bean::setProperty(const char* name,  const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (value.isNull()) return -2;
    return m_world_->setProperty(this, name, value);
}


int Bean::setProperty(pidType pid,  const Json::Value& value)
{
    Property* property = (Property*)m_world_->getProperty(pid);
    if (property == nullptr) return -1;
    if (value.isNull()) return -2;
    m_world_->setProperty(this, pid, value);
    return 0;
}


bool Bean::isArrayProperty(const char* name) const
{
    if (name == nullptr) return false;
    if (name[0] == 0) return false;
    if (!isProperty(name)) return false;
    return  m_propertyValues_[name].isArray();   
}


Json::Value::ArrayIndex Bean::getPropertySize(const char* name) const
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
        if (!isProperty(name)) return 0;
        value = &m_propertyValues_[name];
    }
    else
    {
        if (!isRelation(name)) return 0;
        value = &m_relationValues_[name];
    }
    
    if (!value->isArray()) return 0;
    return value->size();
}


Json::Value Bean::getProperty(const char* name,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value;
    if (name == nullptr) return value;
    if (name[0] == 0) return value;
    if (!isProperty(name)) return value;
    if (!m_propertyValues_[name].isArray()) return value;
    if (index >= m_propertyValues_[name].size()) return value;
    return m_propertyValues_[name][index];
}


int Bean::setProperty(const char* name,  
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (!isProperty(name)) return -1;
    if (value.isNull()) return -2;
    if (!m_propertyValues_[name].isArray()) return -3;
    if (index >= m_propertyValues_[name].size()) return -4;
    m_propertyValues_[name][index] = value;
    return 0;
}


int Bean::appendProperty(const char* name,  const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (!isProperty(name)) return -1;
    if (value.isNull()) return -2;
    if (!m_propertyValues_[name].isArray()) return -3;
    m_propertyValues_[name].append(value);
    return 0;
}


int Bean::resizeProperty(const char* name,  Json::Value::ArrayIndex size)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (!isProperty(name)) return -1;
    if (!m_propertyValues_[name].isArray()) return -3;
    m_propertyValues_[name].resize(size);
    return 0;
}


bool Bean::isRelation(const char* key) const
{
    if (key == nullptr) return false;
    if (key[0] == 0) return false;
    return m_relationValues_.isMember(key);
    // const Json::Value& v = m_jsonValue_.get(key, Json::Value::null);
    // return (v != Json::Value::null);
}


bool Bean::isArrayRelation(const char* name) const
{
    if (name == nullptr) return false;
    if (name[0] == 0) return false;
    if (!isRelation(name)) return false;
    return  m_relationValues_[name].isArray();
}


Bean* Bean::getRelation(const char* name) const
{
    Json::Value value;
    value = m_relationValues_.get(name, value);
    if (value.isNull()) return nullptr;
    oidType oid = value.asUInt64();
    return m_world_->getBean(oid);
}


Json::Value::ArrayIndex Bean::getRelationSize(const char* name) const
{
    return doGetPropertySize(name, false);
}


Bean* Bean::getRelation(const char* name,  
    Json::Value::ArrayIndex index) const
{
    Bean* bean = nullptr;
    if (name == nullptr) return nullptr;
    if (name[0] == 0) return nullptr;
    if (!isRelation(name)) return nullptr;
    auto& value = m_relationValues_[name];
    if (!value.isArray()) return nullptr;
    if (index >= value.size()) return nullptr;
    oidType oid = value[index].asUInt64();
    return m_world_->getBean(oid);
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
    return (isProperty(key)) ? m_propertyValues_[key] :
    Json::Value::null;
}


const Json::Value& Bean::getRelationRef(const char* key)
{
    return (isRelation(key)) ? m_relationValues_[key] :
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