
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


bool Bean::isMember(const Property* property) const
{
    if (property == nullptr) return false;
    return m_propertyValues_.isMember(property->getName().c_str());
}


bool Bean::hasProperty(const Property* property) const
{
    return doHasProperty(property, Property::PrimaryType);
}


bool Bean::doHasProperty(const Property* property, Property::Type type) const
{
    if (property == nullptr) return false;
    if (property->getType() != type) return false;
    const char* name = property->getName().c_str();
    return m_propertyValues_.isMember(name);
}


void Bean::clear()
{
    for (auto& memberName : m_propertyValues_.getMemberNames())
    {
        removeProperty(m_world_->getProperty(memberName.c_str()));
    }
}


Json::Value Bean::getProperty(const Property* property) const
{
    Json::Value value(Json::nullValue);
    if (property == nullptr) return value;
    if (property->getType() != Property::PrimaryType) return value;
    value = m_propertyValues_.get(property->getName().c_str(), value);
    return value;
}


int Bean::setProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    m_world_->setProperty(this, property, value);
    return 0;
}



bool Bean::hasArrayProperty(const Property* property) const
{
    return doHasProperty(property, Property::ArrayPrimaryType);
}


Json::Value::ArrayIndex Bean::getArrayPropertySize(const Property* property) const
{
    return getArrayMemberSizeCommon_(property, true);
}


Json::Value::ArrayIndex Bean::getArrayMemberSizeCommon_(const Property* property, 
        bool isProperty_) const
{
    const char* pname = nullptr;
    const Json::Value* value = nullptr;
    if (isProperty_)
    {
        if (!hasArrayProperty(property)) return 0;
        pname = property->getName().c_str();
        value = &m_propertyValues_[pname];
    }
    else
    {
        if (!hasArrayRelation(property)) return 0;
        pname = property->getName().c_str();
        value = &m_propertyValues_[pname];
    }
    
    if (!value->isArray()) return 0;
    return value->size();
}


Json::Value Bean::getArrayProperty(const Property* property,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value(Json::nullValue);
    if (!hasArrayProperty(property)) return value;
    const char* name = property->getName().c_str();
    if (index >= m_propertyValues_[name].size()) return value;
    return m_propertyValues_[name][index];
}


int Bean::createArrayProperty(Property* property)
{
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (isMember(property)) return 0;
    const char* pname = property->getName().c_str();
    m_propertyValues_[pname] = Json::Value(Json::arrayValue);
    property->addBean(this);
    return 0;
}


int Bean::setArrayProperty(Property* property,  
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    if (!hasArrayProperty(property)) return -4;
    const auto& pname = property->getName().c_str();
    if (index >= m_propertyValues_[pname].size()) return -5;
    // m_propertyValues_[pname][index] = value;
    m_world_->setArrayProperty(this, property, index, value);
    return 0;
}


int Bean::appendProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    if (!hasArrayProperty(property)) return -4;
    const auto& pname = property->getName().c_str();
    auto& arrayValue = m_propertyValues_[pname];
    arrayValue.append(Json::Value(property->getValueType()));
    m_world_->setArrayProperty(this, property, arrayValue.size() - 1, value);
    return 0;
}


int Bean::resizeProperty(Property* property,  Json::Value::ArrayIndex size)
{
    if (property == nullptr) return -1;
    if (property->getType() != Property::ArrayPrimaryType) return -1;
    if (!hasArrayProperty(property)) return -1;
    const auto& pname = property->getName().c_str();
    m_propertyValues_[pname].resize(size);
    return 0;
}


bool Bean::hasRelation(const Property* property) const
{
    return doHasProperty(property, Property::RelationType);
}


bool Bean::hasArrayRelation(const Property* property) const
{
    return doHasProperty(property, Property::ArrayRelationType);
}


Json::Value::ArrayIndex Bean::getArrayRelationSize(const Property* property) const
{
    return getArrayMemberSizeCommon_(property, false);
}


oidType Bean::getRelationBeanId(const Property* property) const
{
    if (!hasRelation(property)) return 0;
    auto pname = property->getName().c_str();
    auto& value = m_propertyValues_[pname];
    return value.asUInt64();
    
}


oidType Bean::getRelationBeanId(const Property* property,  
    Json::Value::ArrayIndex index) const
{
    if (!hasArrayRelation(property)) return 0;
    auto pname = property->getName().c_str();
    auto& value = m_propertyValues_[pname];
    if (!value.isArray()) return 0;
    if (index >= value.size()) return 0;
    return value[index].asUInt64();
}


int Bean::setRelation(Property* property, Bean* bean)
{
    if (bean == nullptr) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::RelationType) return -2;
    m_world_->setRelation(property, this, bean);
    return 0;
}


int Bean::createArrayRelation(Property* property)
{
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    if (hasArrayRelation(property)) return 0;
    const char* pname = property->getName().c_str();
    m_propertyValues_[pname] = Json::Value(Json::arrayValue);
    property->addBean(this);
    return 0;
}


int Bean::appendRelation(Property* property,  Bean* bean)
{
    if (bean == nullptr) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(property)) return -4;
    const auto& pname = property->getName().c_str();
    auto& arrayValue = m_propertyValues_[pname];
    arrayValue.append(bean->getId());
    m_world_->setArrayRelation(property, arrayValue.size() - 1, this, bean);
    return 0;
}


int Bean::setRelation(Property* property,  
    Json::Value::ArrayIndex index, Bean* bean)
{
    if (bean == nullptr) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(property)) return -4;
    const auto& pname = property->getName().c_str();
    if (index >= m_propertyValues_[pname].size()) return -5;
    // Json::Value &oldValue = m_propertyValues_[pname][index];
    // m_propertyValues_[pname][index] = bean.getId();
    m_world_->setArrayRelation(property, index, this, bean);
    return 0;
}


Json::Value Bean::removeProperty(Property* property)
{
    if (property == nullptr) return Json::Value();
    Json::Value& value = const_cast<Json::Value&>(getMemberRef(property));
    if (value.isNull()) return Json::Value();
    property->removeBean(this);
    if (property->indexed())
    { //remove index first
        if (property->getType() == Property::ArrayPrimaryType ||
            property->getType() == Property::ArrayRelationType) 
        {
            auto& array = value;
            for (Json::ArrayIndex i = 0; i< array.size(); i++)
            {
                property->removeIndex(this, array[i]);
            }
        }
        else
        {
            property->removeIndex(this, value);
        }
    }

    //remove member of json object
    return this->m_propertyValues_.
        removeMember(property->getName().c_str());
}


void Bean::removeRelation( Property* property)
{
    removeProperty(property);
}


const Json::Value& Bean::getMemberRef(const Property* property)
{
    // return (isMember(key)) ? m_propertyValues_[key] :
    // Json::Value::null;
    const char* pname = property->getName().c_str();
    if (m_propertyValues_.isMember(pname))
        return m_propertyValues_[pname];
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