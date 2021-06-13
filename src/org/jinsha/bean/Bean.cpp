
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

Bean::Bean(BeanWorld* world) : 
m_json_(Json::ValueType::objectValue), 
m_world_(world)
{

}


Bean::~Bean()
{
    removeAllProperties();

    //handle relations: remove relation from subject that has
    //relation to this bean (as object)
    Bean* subject = nullptr;
    Property* property = nullptr;
    Json::Value* value = nullptr;
    auto iter = m_subjectMap_.begin();
    while (iter != m_subjectMap_.end())
    {
        subject = m_world_->getBean(iter->first);
        if (subject == nullptr) {
            iter = m_subjectMap_.erase(iter);
            continue;
        }
        property = iter->second;
        if (property->getType() == Property::RelationType)  {
            //use doRemoveProperty(property, true) to keep
            //m_subjectMap_ unchanged
            subject->doRemoveProperty(property, true);
        } else if (property->getType() == Property::ArrayRelationType) {
            value = subject->getMemberPtr(property);
            if (value == nullptr) {
                iter++;
                continue; //shall not be null
            }
            size_t size = value->size();
            //todo: O(n*n) complexity! How to improve performance?
            for (Json::ArrayIndex i = size; i > 0; i--)
                if (subject->getRelationBeanId(property, i - 1) == m_id_) {
                    subject->doRemoveProperty(property, i - 1, true); 
                }
        }
        iter++;
    }
    m_subjectMap_.clear();
}


// bool Bean::isMember(const string& key) const
// {
//     return isMember(key.c_str());
// }


bool Bean::isMember(const Property* property) const
{
    if (property == nullptr) return false;
    return m_json_.isMember(property->getName().c_str());
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
    return m_json_.isMember(name);
}


void Bean::removeAllProperties()
{
    for (auto& memberName : m_json_.getMemberNames())
    {
        removeProperty(m_world_->getProperty(memberName.c_str()));
    }
}


Json::Value Bean::getProperty(const Property* property) const
{
    Json::Value value(Json::nullValue);
    if (property == nullptr) return value;
    if (property->getType() != Property::PrimaryType) return value;
    value = m_json_.get(property->getName().c_str(), value);
    return value;
}


int Bean::setProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    Json::Value *oldValue = getMemberPtr(property);
    setPropertyBase_(property,  oldValue, value);

    //handle subject tracking
    property->addSubject(m_id_);
    return 0;
}


void Bean::setPropertyBase_(Property* property, 
    Json::Value *oldValue, const Json::Value&  newValue)
{
    const auto& pname = property->getName();
    if (oldValue == nullptr)
    {  //property has not been set before
        //set the property (regardless of value)
        oldValue = &m_json_[pname];
        //no old value, need to increment ref. count
        // property->m_refCount_++;
    }
    else
    {
        if (*oldValue == newValue) return; 
        //todo: do not index array property/relation for now
        // if (property->indexed())
        if (property->getType() != Property::ArrayPrimaryType && 
            property->getType() != Property::ArrayRelationType && 
             property->indexed())
            //remove index for previous value
            property->removeIndex(this, oldValue);
    }

    //set value for json object
    *oldValue  = newValue;

    //todo: do not index array property/relation for now
    // if (property->indexed())
    if (property->getType() != Property::ArrayPrimaryType && 
        property->getType() != Property::ArrayRelationType && 
        property->indexed())
        property->addIndex(this, *oldValue);
}


bool Bean::hasArrayProperty(const Property* property) const
{
    return doHasProperty(property, Property::ArrayPrimaryType);
}


Json::Value::ArrayIndex Bean::getArraySize(const Property* property) const
{
    if (property == nullptr) return 0;
    Json::Value* value = ((Bean*)this)->getMemberPtr(property);
    if (value == nullptr) return 0;
    if (!value->isArray()) return 0;
    return value->size();
}


Json::Value Bean::getArrayProperty(const Property* property,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value(Json::nullValue);
    if (!hasArrayProperty(property)) return value;
    const char* name = property->getName().c_str();
    if (index >= m_json_[name].size()) return value;
    return m_json_[name][index];
}


int Bean::createArrayProperty(Property* property)
{
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (isMember(property)) return 0;
    const char* pname = property->getName().c_str();
    m_json_[pname] = Json::Value(Json::arrayValue);
    property->addSubject(m_id_);
    return 0;
}


int Bean::setProperty(Property* property,  
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName().c_str();
    if (!m_json_.isMember(pname)) return -4;
    auto& array = m_json_[pname]; 
    if (index >= array.size()) return -5;
    Json::Value *oldValue = &array[index];
    setPropertyBase_(property,  oldValue, value);
    return 0;
}


int Bean::appendProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    if (!hasArrayProperty(property)) return -4;
    const auto& pname = property->getName();
    auto& arrayValue = m_json_[pname];
    arrayValue.append(value);
    //todo: do not index array property/relation for now
    // if (property->indexed())
    //     property->addIndex(this, value);
    return 0;
}


bool Bean::hasRelation(const Property* relation) const
{
    return doHasProperty(relation, Property::RelationType);
}


bool Bean::hasArrayRelation(const Property* relation) const
{
    return doHasProperty(relation, Property::ArrayRelationType);
}


oidType Bean::getRelationBeanId(const Property* relation) const
{
    if (!hasRelation(relation)) return 0;
    auto pname = relation->getName().c_str();
    auto& value = m_json_[pname];
    return value.asUInt64();
    
}


oidType Bean::getRelationBeanId(const Property* relation,  
    Json::Value::ArrayIndex index) const
{
    if (!hasArrayRelation(relation)) return 0;
    auto pname = relation->getName().c_str();
    auto& value = m_json_[pname];
    if (!value.isArray()) return 0;
    if (index >= value.size()) return 0;
    return value[index].asUInt64();
}


int Bean::setRelation(Property* relation, Bean* bean)
{
    if (bean == nullptr) return -1;
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::RelationType) return -2;
    Json::Value *oldValue =  getMemberPtr(relation);
    setPropertyBase_(relation, oldValue, bean->getId());

    //handle object tracking
    if (oldValue != nullptr) {
        oidType objectId = (*oldValue).asUInt64();
        if (objectId == bean->m_id_) return 0; //the same bean, do nothing
        Bean* object = m_world_->getBean(objectId);
        if (object != nullptr) {
            object->removeSubject(this, relation);
        }
        relation->removeObject(objectId);
    }
    relation->addObject(bean->getId());

    //handle subject tracking
    bean->addSubject(this, relation);
    relation->addSubject(m_id_);
    
    return 0;
}


int Bean::createArrayRelation(Property* relation)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (hasArrayRelation(relation)) return 0;
    const char* pname = relation->getName().c_str();
    m_json_[pname] = Json::Value(Json::arrayValue);
    relation->addSubject(m_id_);
    return 0;
}


int Bean::appendRelation(Property* relation,  Bean* bean)
{
    if (bean == nullptr) return -1;
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(relation)) return -4;
    const auto& pname = relation->getName().c_str();
    auto& arrayValue = m_json_[pname];
    arrayValue.append(bean->getId());
    relation->addObject(bean->getId());
    //todo: do not index array property/relation for now
    // if (property->indexed())
    //     property->addIndex(this, value);
    bean->addSubject(this, relation);
    return 0;
}


int Bean::setRelation(Property* relation,  
    Json::Value::ArrayIndex index, Bean* bean)
{
    if (bean == nullptr) return -1;
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    const auto& pname = relation->getName().c_str();
    if (!m_json_.isMember(pname)) return -4;
    auto& array = m_json_[pname]; 
    if (index >= array.size()) return -5;
    Json::Value *oldValue = &array[index];
    setPropertyBase_(relation, oldValue, bean->getId());

    //handle object tracking
    if (oldValue != nullptr) {
        oidType objectId = (*oldValue).asUInt64();
        if (objectId == bean->m_id_) return 0; //the same bean, do nothing
        Bean* object = m_world_->getBean(objectId);
        if (object != nullptr) {
            object->removeSubject(this, relation);
        }
        relation->removeObject(objectId);
    }
    relation->addObject(bean->getId());

    //handle subject tracking
    bean->addSubject(this, relation);

    return 0;
}


Json::Value Bean::removeProperty(Property* property)
{
    return doRemoveProperty(property);
}


Json::Value Bean::doRemoveProperty(Property* property, bool internal)
{
    if (property == nullptr) return Json::Value();
    Json::Value* valuePtr = getMemberPtr(property);
    if (valuePtr == nullptr) return Json::Value();
    Json::Value& value = *valuePtr;
    property->removeSubject(m_id_);
    if (property->indexed())
    { //remove index first
        if (property->getType() == Property::ArrayPrimaryType ||
            property->getType() == Property::ArrayRelationType) 
        {
            //todo: index not supported for array currently
            // Json::Value& array = *value;
            // for (Json::ArrayIndex i = 0; i< array.size(); i++)
            // {
            //     property->removeIndex(this, array[i]);
            // }
        }
        else
        {
            property->removeIndex(this, value);
        }
    }

    const auto& pname = property->getName();

    //remove object record if the property is relation
    if (property->getType() == Property::RelationType)
    {
            oidType oid = value.asUInt64();
            property->removeObject(oid);

            if (!internal) {
                Bean* objectBean = m_world_->getBean(oid);
                if (objectBean != nullptr)
                    objectBean->removeSubject(this, property);
            }
    }
    else if (property->getType() == Property::ArrayRelationType)
    {
        auto& array = value;
        oidType oid = 0;
        for (Json::ArrayIndex index = 0; index < array.size(); index++)
        {
            oid = array[index].asUInt64();
            property->removeObject(oid);

            if (!internal) {
                Bean* objectBean = m_world_->getBean(oid);
                if (objectBean == nullptr) continue;
                objectBean->removeSubject(this, property);
            }
        }
    }
    
    //remove member of json object
    return this->m_json_.
        removeMember(pname);
}


Json::Value Bean::removeProperty(Property* property, Json::Value::ArrayIndex index)
{
    return doRemoveProperty(property, index);
}

Json::Value Bean::doRemoveProperty(Property* property, Json::Value::ArrayIndex index, bool internal)
{
    Json::Value rtn; //null
    if (property == nullptr) return rtn;
    Json::Value* arrayPtr = getMemberPtr(property);
    if (arrayPtr == nullptr) return rtn; 
    Json::Value& array = *arrayPtr;
    if (!array.isArray()) return rtn; //if property is array, array.isArray() shall be true
    if (index >= array.size()) return rtn;
    rtn = array[index];

    //todo: index not supported for array currently
    // if (property->indexed())
    // { //remove index first
    //     property->removeIndex(this, array[index]);
    // }

    oidType oid = 0;
    Bean* objectBean = nullptr;
    //remove object record if the property is array relation
    if (property->getType() == Property::ArrayRelationType)
    {
        oid = array[index].asUInt64();
        property->removeObject(oid);
        if (!internal) {
            objectBean = m_world_->getBean(oid);
            if (objectBean != nullptr)
                objectBean->removeSubject(this, property);
        }
    }
    
    //create a new array and replace the old
    Json::Value newArray(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < array.size(); i++) {
        if (i != index) {
            newArray.append(array[i]);
        }
    }

    array = newArray;
    return rtn;
}


// void Bean::removeRelation( Property* relation)
// {
//     removeProperty(relation);
// }

void Bean::addSubject(Bean* subject, Property* relation)
{
    m_subjectMap_.insert(std::pair<oidType, Property*>(subject->getId(), relation));
}


void Bean::removeSubject(Bean* subject, Property* relation)
{
    auto range = m_subjectMap_.equal_range(subject->getId());
    for (auto it = range.first; it != range.second;) {
        if (it->second == relation)
            it = m_subjectMap_.erase(it);
        else
            it++;
    }
}


Json::Value* Bean::getMemberPtr(const Property* property)
{
    const auto& pname = property->getName();
    return (m_json_.isMember(pname)) ? &m_json_[pname] : nullptr;
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