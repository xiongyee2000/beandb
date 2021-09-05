
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

// Bean::Bean(oidType id) : 
// m_world_(nullptr)
// , m_id_(id)
// {
//     m_json_= Json::Value(Json::ValueType::objectValue);
//     m_native_data_json_ = Json::Value(Json::ValueType::objectValue);
//     m_pst_json_ = Json::Value(Json::ValueType::objectValue);
//     m_native_data_pst_json_ = PST_NSY;
// }


Bean::Bean(oidType id, BeanWorld* world) : 
m_id_(id)
, m_world_(world)
{
    m_json_= Json::Value(Json::ValueType::objectValue);
    m_native_data_json_ = Json::Value(Json::ValueType::objectValue);
    m_pst_json_ = Json::Value(Json::ValueType::objectValue);
    m_native_data_pst_json_ = PST_NSY;
}


Bean::~Bean()
{
}


// bool Bean::isMember(const string& key) const
// {
//     return isMember(key.c_str());
// }


bool Bean::isMember(const Property* property) const
{
    if (property == nullptr) return false;
    return m_json_.isMember(property->getName());
}


bool Bean::hasProperty(const Property* property) const
{
    return doHasProperty(property, Property::PrimaryType);
}


bool Bean::doHasProperty(const Property* property, Property::Type type) const
{
    if (property == nullptr) return false;
    if (property->getType() != type) return false;
    return m_pst_json_.isMember(property->getName());
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
    if (!hasProperty(property)) return Json::Value::nullRef;

    const auto& pname = property->getName();
    if (m_pst_json_[pname].asInt() == PST_NSY) {
        //not loaded, load it
        if (0 != ((Bean*)this)->loadProperty(property)) return Json::Value::nullRef;
    } 
    return m_json_[pname];
}


int Bean::setProperty(Property* property,  const Json::Value& value)
{
    return doSetProperty(property, value, true);
}


int Bean::doSetProperty(Property* property,  const Json::Value& value, bool syncToDB)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -3;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    int err = 0;
    const auto& pname = property->getName();
    Json::Value *oldValue = nullptr;
    if (m_pst_json_.isMember(pname)) {
        oldValue = getMemberPtr(property);
    }
    // if (oldValue != nullptr && (*oldValue) == value) return 0;

    err = setPropertyBase_(property,  oldValue, value, -1, syncToDB);
    if (err) {
        return err;
    } else {
        if ( syncToDB)
            m_pst_json_[pname] = PST_SYN;
        else 
            m_pst_json_[pname] = PST_MOD;
    }
    
    //handle subject tracking
    property->addSubject(m_id_);
    return 0;
}


int Bean::setPropertyBase_(Property* property, 
    Json::Value *oldValue, const Json::Value&  newValue, 
    Json::Value::ArrayIndex index,
    bool syncToDB)
{
    int err = 0;
    const auto& pname = property->getName();
    if (oldValue == nullptr)
    {   //property has not been set before
            if (syncToDB) {
                // insert property record first
                if (m_world_->m_db != nullptr) {
                    err = m_world_->m_db->insertBeanProperty_(m_id_, property, newValue);
                    if (err)
                        return err;
                }
        }
        //set the property (regardless of value)
        oldValue = &m_json_[pname];
        //no old value, need to increment ref. count

        // property->m_refCount_++;
    }
    else
    {
        if (*oldValue == newValue) return 0; 
        if (syncToDB) {
            //update db first
            if (m_world_->m_db != nullptr) {
                if (index == (Json::Value::ArrayIndex)-1) {
                    err = m_world_->m_db->updateBeanProperty_(m_id_, property, newValue);
                    if (err)
                        return err;
                } else {
                    err = m_world_->m_db->updateBeanProperty_(m_id_, property, index, newValue);
                    if (err)
                        return err;
                }
            }
        }

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

    return 0;
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
    if (!hasArrayProperty(property)) return Json::Value::nullRef;
    const auto& pname = property->getName();

    if (!m_pst_json_[pname].isArray()) { 
        //not loaded, load it
        if (0 != ((Bean*)this)->loadProperty(property)) return Json::Value::nullRef;
    }
    if (index < m_json_[pname].size()) {
        return m_json_[pname][index];
    }
    else {
        return Json::Value::nullRef;
    }
}


int Bean::createArrayProperty(Property* property)
{
    return doCreateArrayProperty(property, false);
}


int Bean::doCreateArrayProperty(Property* property, bool syncToDB)
{
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (hasArrayProperty(property)) return 0;
    const auto& pname = property->getName();
    //todo: save to db first
    //...
    m_json_[pname] = Json::Value(Json::arrayValue);
    m_pst_json_[pname] = PST_NEW;
    property->addSubject(m_id_);
    return 0;
}


int Bean::setProperty(Property* property,  
    Json::Value::ArrayIndex index, const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -3;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return -4;
    if (m_pst_json_[pname] == PST_NEW) return -4;
    if (!m_pst_json_[pname].isArray()) { 
        //delay load
        if (0 != loadProperty(property)) {
            return -11;
        }
    }
    Json::Value* array = getMemberPtr(property);
    if (index >= array->size()) return -5;
    Json::Value *oldValue = &((*array)[index]);
    int err = setPropertyBase_(property,  oldValue, value, index, true);
    if (!err) m_pst_json_[pname][index] = PST_SYN;
    return err;
}


int Bean::appendProperty(Property* property,  const Json::Value& value)
{
    return doAppendProperty(property, value, true);
}


int Bean::doAppendProperty(Property* property,  const Json::Value& value, bool syncToDB)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return -4;
    int err = 0;
    if (!m_pst_json_[pname].isArray()) {
        if (m_pst_json_[pname].asInt() == PST_NEW) {
            //new created array and first time append
            m_pst_json_[pname] = Json::Value(Json::arrayValue);
        } else {
            //delay load
            err = loadProperty(property);
            if (err) return -11;
        }
    } else {
        //already loaded
    }
    Json::Value * array =  getMemberPtr(property);
    //insert property record first
    if (m_world_->m_db != nullptr && syncToDB) {
        if (0 != m_world_->m_db->insertBeanProperty_(m_id_, property, value)) {
            if (m_pst_json_.isArray() && m_pst_json_[pname].size() == 0)
                m_pst_json_[pname] = PST_NEW; //resume to PST_NEW
            return -11;
        }
    }
    array->append(value);
    if (syncToDB) {
        m_pst_json_[pname].append(PST_SYN);
    } else {
        m_pst_json_[pname].append(PST_NEW);
    }
    
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


int Bean::setRelation(Property* relation, oidType objectId)
{
    return doSetRelation(relation, objectId, true);
}


int Bean::doSetRelation(Property* relation, oidType objectId, bool syncToDB)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::RelationType) return -2;
    Json::Value *oldValue = nullptr;
    const auto& pname = relation->getName();
    if (m_pst_json_.isMember(pname)) {
        oldValue = getMemberPtr(relation);
    }
    int err =setPropertyBase_(relation, oldValue, objectId, -1, syncToDB);
    if (err) {
        return err;
    } else {
        if ( syncToDB)
            m_pst_json_[pname] = PST_SYN;
        else 
            m_pst_json_[pname] = PST_MOD;
    }

    //handle object tracking
    if (oldValue != nullptr) {
        oidType oldObjectId = (*oldValue).asUInt64();
        if (oldObjectId == objectId) return 0; //the same bean, do nothing
        Bean* object = m_world_->getBean(oldObjectId);
        if (object != nullptr) {
            object->removeSubject(this, relation);
        }
        relation->removeObject(oldObjectId);
    }
    relation->addObject(objectId);

    //handle subject tracking
    Bean* objectBean = m_world_->getBean(objectId);
    if (objectBean != nullptr)
        objectBean->addSubject(this, relation);
    relation->addSubject(m_id_);
    
    return 0;
}


int Bean::setRelation(Property* relation, Bean* bean)
{
    if (bean == nullptr) return -1;
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::RelationType) return -3;
    Json::Value *oldValue = nullptr;
    const auto& pname = relation->getName();
    if (m_pst_json_.isMember(pname)) {
        oldValue = getMemberPtr(relation);
    }
    int err = setPropertyBase_(relation, oldValue, bean->getId(), -1, true);
    if (err) {
        return err;
    } else {
        m_pst_json_[pname] = PST_SYN;
    }

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
    return doCreateArrayRelation(relation, true);
}


int Bean::doCreateArrayRelation(Property* relation, bool syncToDB)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (hasArrayRelation(relation)) return 0;
    const char* pname = relation->getName().c_str();
    m_json_[pname] = Json::Value(Json::arrayValue);
    m_pst_json_[pname] = PST_NEW;
    if (syncToDB) {
        //todo: save to db first
    }
    relation->addSubject(m_id_);
    return 0;
}


int Bean::appendRelation(Property* relation,  oidType objectBeanId)
{
    return doAppendRelation(relation, objectBeanId, true);
}

int Bean::doAppendRelation(Property* relation,  oidType objectBeanId, bool syncToDB)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(relation)) return -4;
    const auto& pname = relation->getName();

    // if (!m_pst_json_.isMember(pname)) return -4;
    int err = 0;
    if (!m_pst_json_[pname].isArray()) {
        if (m_pst_json_[pname].asInt() == PST_NEW) {
            //new created array and first time append
            m_pst_json_[pname] = Json::Value(Json::arrayValue);
        } else {
            //delay load
            err = loadProperty(relation);
            if (err) return -11;
        }
    } else {
        //already loaded
    }

    //insert property record first
    if (m_world_->m_db != nullptr && syncToDB) {
        if (0 != m_world_->m_db->insertBeanProperty_(m_id_, relation, Json::Value(objectBeanId))) {
            if (m_pst_json_[pname].size() == 0)
                m_pst_json_[pname] = PST_NEW; //resume to PST_NEW
            return -11;
        }
    }

    auto& arrayValue = m_json_[pname];
    arrayValue.append(objectBeanId);
    if (syncToDB) {
        m_pst_json_[pname].append(PST_SYN);
    } else {
        m_pst_json_[pname].append(PST_NEW);
    }
    relation->addObject(objectBeanId);
    //todo: do not index array property/relation for now
    // if (property->indexed())
    //     property->addIndex(this, value);
    Bean* objectBean = m_world_->getBean(objectBeanId);
    if (objectBean != nullptr) 
        objectBean->addSubject(this, relation);
    return 0;
}


int Bean::appendRelation(Property* relation,  Bean* bean)
{
    if (bean == nullptr) return -1;
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(relation)) return -4;

    const auto& pname = relation->getName();
    // if (!m_pst_json_.isMember(pname)) return -4;
    int err = 0;
    if (!m_pst_json_[pname].isArray()) {
        if (m_pst_json_[pname].asInt() == PST_NEW) {
            //new created array and first time append
            m_pst_json_[pname] = Json::Value(Json::arrayValue);
        } else {
            //delay load
            err = loadProperty(relation);
            if (err) return -11;
        }
    } else {
        //already loaded
    }

    //insert property record first
    if (m_world_->m_db != nullptr) {
        if (0 != m_world_->m_db->insertBeanProperty_(m_id_, relation, Json::Value(bean->m_id_)))
            return -11;
    }

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
    if (relation->getType() != Property::ArrayRelationType) return -3;
    const auto& pname = relation->getName();
    if (!m_pst_json_.isMember(pname)) return -4;
    if (m_pst_json_[pname] == PST_NEW) return -4;
    if (!m_pst_json_[pname].isArray()) { 
        //delay load
        if (0 != loadProperty(relation)) {
            return -11;
        }
    }
    auto& array = m_json_[pname]; 
    if (index >= array.size()) return -5;
    Json::Value *oldValue = &array[index];
    int err = setPropertyBase_(relation, oldValue, bean->getId(), index, true);
    if (err) {
        return err;
    } else {
        m_pst_json_[pname][index] = PST_SYN;
    }

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
    if (property == nullptr) return Json::Value::nullRef;
    return doRemoveProperty(property, false, true);
}


Json::Value Bean::doRemoveProperty(Property* property, bool internal, bool syncToDB)
{
    const auto& pname = property->getName();
    Json::Value rtnValue;
   Json::Value* valuePtr = getMemberPtr(property);
    if (valuePtr == nullptr) return Json::Value::nullRef;
    Json::Value& pstValue = m_pst_json_[pname];
    if (pstValue.isArray()) {
        if (m_json_[pname].isNull() || !m_json_[pname].isArray()) 
            return Json::Value::nullRef; //inconsistent value
        Json::Value& array = m_json_[pname];
        rtnValue = *valuePtr;
        Json::Value::ArrayIndex size = array.size();
        for (Json::Value::ArrayIndex i = 0; i < size; i++) {
            doRemoveProperty(property, i, false, syncToDB);
        }
    }
    else  if (pstValue.asInt() == PST_NEW) {
        // a new created array property that contains no item
        // so no need to later stuff.
            m_pst_json_.removeMember(pname);
            //need to do this for createProperty() call addSubject()
            property->removeSubject(m_id_); 
            return Json::Value::nullRef;
    }

    if (syncToDB) {
        //delete from db first
        if (m_world_->m_db != nullptr) {
            if (0 != m_world_->m_db->deleteBeanProperty_(m_id_, property))
                return Json::Value::nullRef;
        }
    }

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
            if (m_pst_json_[pname].asInt() != PST_NSY)  {
                property->removeIndex(this, value);
            }
        }
    }

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
    m_pst_json_.removeMember(pname);
    if (property->getType() == Property::ArrayPrimaryType || 
        property->getType() == Property::ArrayRelationType) {
            m_json_.removeMember(pname);
            return rtnValue;
    }
    return m_json_.removeMember(pname);
}


Json::Value Bean::removeProperty(Property* property, Json::Value::ArrayIndex index)
{
    if (property == nullptr) return Json::Value::nullRef;
    return doRemoveProperty(property, index, false, true);
}

Json::Value Bean::doRemoveProperty(Property* property, Json::Value::ArrayIndex index, bool internal, bool syncToDB)
{
    Json::Value rtn; //null
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return Json::Value::nullRef; //bean has no such property value
    Json::Value& pstValue = m_pst_json_[pname];
    if (!pstValue.isArray()) 
    {
        if (pstValue.asInt() == PST_NEW) { 
        // a new created array property that contains no item,
        // so no need to do later stuff.
            m_pst_json_.removeMember(pname);
            property->removeSubject(m_id_);
            return Json::Value::nullRef;
        }
    }
    Json::Value* arrayPtr = getMemberPtr(property);
    if (arrayPtr == nullptr) return Json::Value::nullRef; 
    Json::Value& array = *arrayPtr;
    if (!array.isArray()) return Json::Value::nullRef; //if property is array, array.isArray() shall be true
    if (index >= array.size()) return Json::Value::nullRef;

    if (syncToDB) {
        //delete from db first
        if (m_world_->m_db != nullptr) {
            if (0 != m_world_->m_db->deleteBeanProperty_(m_id_, property, index))
                return Json::Value::nullRef;
        }
    }

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
    Json::Value newPstArray(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < array.size(); i++) {
        if (i != index) {
            newArray.append(array[i]);
            newPstArray.append(pstValue[i]);
        }
    }

    array = newArray;
    m_pst_json_[pname] = newPstArray;
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

Json::Value Bean::getNativeData()
{       
    if (m_native_data_pst_json_.asInt() == PST_NSY) {
    //not loaded yet, load it first
        if (m_world_->m_db != nullptr) {
            int err = m_world_->m_db->
                loadBeanNativeData_(m_id_, m_native_data_json_);
            if (!err) {
                m_native_data_pst_json_ = PST_SYN;
            } else {
                m_native_data_json_ = Json::Value(Json::ValueType::objectValue);
            }
        } 
    } 

    return m_native_data_json_;
}

Json::Value Bean::getNativeData() const
{
    return ((Bean*)this)->getNativeData();
}

int Bean::setNativeData(Json::Value& data, bool syncToDB)
{
    int err = 0;
    if (syncToDB) {
        if (m_world_->m_db != nullptr) {
            //save to db
            err = m_world_->m_db->updateBeanNativeData_(m_id_, data);
            if (!err) {
                m_native_data_json_ = data;
                m_native_data_pst_json_ = PST_SYN;
            } 
        } else {
            err = -1;
        }
    }   else
    {
        m_native_data_json_ = data;
        m_native_data_pst_json_ = PST_MOD;
    }
    
    return err;
}


int Bean::pstTransition(int currentPst, int desiredPst)
{
    if (currentPst < 0 || currentPst >= PST_MAX)  return -1;
    if (desiredPst < 0 || desiredPst >= PST_MAX)  return -1;

    //transition table from curent pst to target pst.
    //row index: current PST
    //col index: argument status
    //value: result status
    //
    //Example:
    //When current pst is PST_NEW and status is  PST_MOD,
    //the transition result will be PST_NEW, i.e. row #1 col. #2 
    //will be 1 (index 0 based).
    //This is rational because after modification, it will be 
    //taken as a new property from DB perspective since
    //it has not been synced to DB.
    //
    //Note -1 means to remove the pst record.
    static const int pstTransitionTable[5][5]= {
        {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD}, /*CURRENT PST_NSY*/
        {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD},  /*current PST_SYN*/
        {              -1, PST_SYN,                  -2, PST_NEW,                 -1},  /*current PST_NEW*/
        {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD}, /*current PST_MOD*/
        {PST_NSY,                -1, PST_MOD, PST_RMD, PST_RMD}  /*current PST_RMD*/
    };

    return pstTransitionTable[currentPst][desiredPst];
}


int Bean::load()
{
    if (m_world_->m_db == nullptr) return -1;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    Json::Value data, nativeData;
    Json::Value* pstValuePtr = nullptr;

    //unload first
    unload();

    err = m_world_->m_db->loadBeanBase_(m_id_, data, m_native_data_json_);
    if (err) {
        m_json_ = Json::Value(Json::ValueType::objectValue);
        m_native_data_json_ = Json::Value(Json::ValueType::objectValue);
    } else {
        //todo: 
        //check data validity, e.g.: property name consistency, 
        //delay load attribute etc...

        //set proper property value
        auto pnames = data.getMemberNames();
        for (auto& pname : pnames) {
            property = m_world_->getProperty(pname.c_str());
            if (property == nullptr) continue; //it's not a defined property

            //set pst value
            if (property->isDelayLoad()) {
                m_pst_json_[pname] = PST_NSY;
            } else {
                m_pst_json_[pname] = PST_SYN;
            }

            pstValuePtr = &data[pname];

            if (pstValuePtr->isNull()) { //delay load
                m_json_[pname] = Json::Value::nullRef;
                m_pst_json_[pname] = PST_NSY;
            } else {
                switch (property->getType()) {
                    case Property::PrimaryType:
                        m_pst_json_.removeMember(pname);
                        doSetProperty(property,  *pstValuePtr, false);
                        m_pst_json_[pname] = PST_SYN;
                        break;
                    case Property::RelationType:
                        m_pst_json_.removeMember(pname);
                        doSetRelation(property, pstValuePtr->asUInt64(), false);         
                        m_pst_json_[pname] = PST_SYN;
                        break;
                    case Property::ArrayPrimaryType:
                        //override pst value
                        m_pst_json_.removeMember(pname);                 
                        doCreateArrayProperty(property, false);
                        size = pstValuePtr->size();
                        for (int i = 0; i < size; i++) {
                            doAppendProperty(property,  (*pstValuePtr)[i], false);
                            //set it again to ensure it's PST_SYN
                            m_pst_json_[pname][i] = PST_SYN;
                        }
                        break;
                    case Property::ArrayRelationType:
                        //override pst value
                        m_pst_json_.removeMember(pname);         
                        doCreateArrayRelation(property, false);
                        size = pstValuePtr->size();
                        for (int i = 0; i < size; i++) {
                            doAppendRelation(property,  (*pstValuePtr)[i].asUInt64(), false);
                            //set it again to ensure it's PST_SYN
                            m_pst_json_[pname][i] = PST_SYN;
                        } 
                        break;
                    default: 
                        //shall not reach here
                        break;
                }
            }
        }

        if (nativeData.isNull()) { //delay load
            nativeData = Json::Value(Json::ValueType::objectValue);
            m_native_data_pst_json_ = PST_NSY;
        } else {
            m_native_data_pst_json_ = PST_SYN;
        }
    }

    return err;
}


int Bean::unload()
{
    int err = 0;
   Property* property = nullptr;

   //remove all properties
    for (auto& pname : m_pst_json_.getMemberNames()) {
        property = m_world_->getProperty(pname.c_str());
        if (property == nullptr) continue;
        doRemoveProperty(property, false, false); 
    }
    m_json_ = Json::Value(Json::objectValue);
    m_pst_json_ = Json::Value(Json::objectValue);

    //remove all native data
   m_native_data_json_ = Json::Value(Json::objectValue);
   m_native_data_pst_json_ = PST_NSY;

    return err;
}


int Bean::loadProperty(const Property* property)
{
    if (m_world_->m_db == nullptr) return -1;

    int err = 0;
    const auto& pname = property->getName();

    if (property->m_propertyType_ == Property::ArrayPrimaryType ||
         property->m_propertyType_ == Property::ArrayRelationType) {
        err = m_world_->m_db->loadBeanProperty_(m_id_, property, (Json::Value&)m_json_[pname]);
        if (err) {
            m_json_[pname] = Json::Value::nullRef; //reset to null
            m_pst_json_[pname] = PST_NSY;
        } else {
            m_pst_json_[pname] = Json::Value(Json::arrayValue);
            auto& v = (Json::Value&)m_pst_json_[pname];
            for (int i = 0; i < m_json_.size(); i++) {
                v.append(PST_SYN);
            }
        }
    } else {
        err = m_world_->m_db->loadBeanProperty_(m_id_, property, (Json::Value&)m_json_[pname]);
        if (err) {
            m_json_[pname] = Json::Value::nullRef; //reset to null
            m_pst_json_[pname] = PST_NSY;
        } else {
            m_pst_json_[pname] = PST_SYN;
        }
    }

    return err;
}

int Bean::save()
{
    if (m_world_->m_db == nullptr) return -1;
    if (m_pst_json_.isNull()) return -2;

    int err = 0;
    int i = 0;
    int size = 0;
    Json::Value* pstValuePtr = nullptr;
    Property* property = nullptr;

    err = m_world_->m_db->beginTransaction();
    if (err) return -3;

    err = m_world_->m_db->saveBeanBase_(m_id_, m_json_, m_native_data_json_);
    if (err) goto out;

    for (auto& pname : m_pst_json_.getMemberNames()) {
        property = m_world_->getProperty(pname.c_str());
        if (property == nullptr) continue;
        if (property->isDelayLoad()) {
            pstValuePtr = &m_pst_json_[pname];
            if (!pstValuePtr->isNull()) {
                if (pstValuePtr->isArray()) {
                    size = pstValuePtr->size();
                    for (i = 0; i < size; i++) {
                        switch ((*pstValuePtr)[i].asInt()) {
                            case PST_NSY:
                                break;
                            case PST_SYN:
                                break;
                            case PST_MOD:
                                err = m_world_->m_db->updateBeanProperty_(m_id_, property, i, m_json_[pname][i]);
                                if (err) goto out;
                                break;
                            // case PST_NEW:
                            //     err = m_world_->m_db->insertBeanProperty(m_id_, property, m_json_[pname][i]);
                            //     if (err) goto out;
                            //     break;
                            default:
                                break;
                        }
                    }
                } else if (pstValuePtr->asInt() == PST_NEW) {
                    //todo: do anything?
                } else if (pstValuePtr->asInt() == PST_MOD) {
                    err = m_world_->m_db->updateBeanProperty_(m_id_, property, m_json_[pname]);
                    if (err) goto out;
                } else {
                    //do nothing
                }
            }
        } else {
            //don't do anything for non-delay property here
        }
    }

    if (m_native_data_pst_json_.asInt() == PST_MOD) {
            err = m_world_->m_db->updateBeanNativeData_(m_id_, m_native_data_json_);
            if (err) goto out;
            m_native_data_pst_json_ = PST_SYN;
    }

out:
    if (err) {
        m_world_->m_db->rollbackTransaction();
    } else {
        m_world_->m_db->commitTransaction();

        //set pst value
        for (auto& pname : m_pst_json_.getMemberNames()) {
            property = m_world_->getProperty(pname.c_str());
            if (property == nullptr) continue;
            pstValuePtr = &m_pst_json_[pname];
            if (pstValuePtr->isArray()) {
                size = pstValuePtr->size();
                for (i = 0; i < size; i++) {
                    switch ((*pstValuePtr)[i].asInt()) {
                        case PST_NSY:
                            break;
                        case PST_SYN:
                            break;
                        case PST_MOD:
                            (*pstValuePtr)[i] = PST_SYN;
                            break;
                        // case PST_NEW:
                        //     err = m_world_->m_db->insertBeanProperty(m_id_, property, m_json_[pname][i]);
                        //     if (err) goto out;
                        //     break;
                        default:
                            break;
                    }
                }
            } else if (pstValuePtr->asInt() == PST_NEW) {
                //todo: do anything?
            } else if (pstValuePtr->asInt() == PST_MOD) {
                (*pstValuePtr)[i] = PST_SYN;
            } else {
                //do nothing
            }
        }
    }
    
    return err;
}

}
}
}