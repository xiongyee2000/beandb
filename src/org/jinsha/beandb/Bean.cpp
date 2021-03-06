
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"
#include "internal_common.hxx"

using namespace std;

namespace org {
namespace jinsha {
namespace beandb {


Bean::Bean(oidType id, BeanWorld* world) : 
m_id_(id)
, m_world_(world)
{
    m_json_= Json::Value(Json::ValueType::objectValue);
    m_native_data_json_ = Json::Value::nullRef;
    m_pst_json_ = Json::Value(Json::ValueType::objectValue);
    m_native_data_pst_json_ = PST_NSY;
}


Bean::~Bean()
{
}


bool Bean::isMember(const Property* property) const
{
    if (property == nullptr) return false;
    return m_json_.isMember(property->getName());
}


bool Bean::isMember(const char* name) const
{
    if (name == nullptr || name[0] == 0) return false;
    return m_json_.isMember(name);
}


bool Bean::isMember(const string& name) const
{
    if (name.empty()) return false;
    return m_json_.isMember(name);
}


Json::Value::Members Bean::getMemberNames () const 
{
    return m_pst_json_.getMemberNames();
};

bool Bean::hasPrimaryProperty(const Property* property) const
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
    //todo: put them into  transaction, and handle exception
    for (auto& memberName : m_json_.getMemberNames())
    {
        remove(m_world_->getProperty(memberName.c_str()));
    }
}


Json::Value Bean::get(const Property* property) const
{
    Json::Value value(Json::nullValue);
    if (!hasPrimaryProperty(property)) return Json::Value::nullRef;

    const auto& pname = property->getName();
    if (m_pst_json_[pname].asInt() == PST_NSY) {
        //delay load
        if (0 != ((Bean*)this)->loadProperty(property)) return Json::Value::nullRef;
    } 
    return m_json_[pname];
}


int Bean::set(Property* property,  const Json::Value& value, bool saveAtOnce)
{
    return doSet(property, value, saveAtOnce);
}


int Bean::set(Property* property,  const char* value, bool saveAtOnce)
{
    Json::StaticString sv(value);
    Json::Value v(sv);
    return doSet(property, v, saveAtOnce);
}


int Bean::set(Property* property,  const std::string& value, bool saveAtOnce)
{
    Json::StaticString sv(value.c_str());
    Json::Value v(sv);
    return doSet(property, v, saveAtOnce);
}


int Bean::doSet(Property* property,  const Json::Value& value, bool saveAtOnce)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -3;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;

    int err = 0;
    bool newSet = false;
    const auto& pname = property->getName();
    Json::Value *oldValue = nullptr;
    if (m_pst_json_.isMember(pname)) {
        oldValue = getMemberPtr(property);
    }

    if (oldValue == nullptr ) 
        newSet = true;
    err = setPropertyBase_(property,  oldValue, value, -1, saveAtOnce);
    if (err) {
        return err;
    } else {
        if ( saveAtOnce)
            m_pst_json_[pname] = PST_SYN;
        else {
            if (newSet) 
                m_pst_json_[pname] = PST_NEW;
            else 
                m_pst_json_[pname] = PST_MOD;
        }
    }
    
    //handle subject tracking
    property->addSubject(m_id_);
    return 0;
}


int Bean::setPropertyBase_(Property* property, 
    Json::Value *oldValue, const Json::Value&  newValue, 
    Json::Value::ArrayIndex index,
    bool saveAtOnce)
{
    int err = 0;
    
    const auto& pname = property->getName();
    if (oldValue == nullptr)
    {   //property has not been set before
        if (saveAtOnce) {
            // insert property record first
            if (m_world_->m_db_ != nullptr) {
                err = m_world_->m_db_->insertBeanProperty(m_id_, property, newValue);
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
        if (saveAtOnce) {
            //update db first
            if (m_world_->m_db_ != nullptr) {
                if (index == (Json::Value::ArrayIndex)-1) {
                    err = m_world_->m_db_->updateBeanProperty(m_id_, property, newValue);
                    if (err)
                        return err;
                } else {
                    err = m_world_->m_db_->updateBeanProperty(m_id_, property, index, newValue);
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


Json::Value::ArrayIndex Bean::size(const Property* property) const
{
    if (property == nullptr) return 0;
    if (!property->isArray())  return 0;
    const std::string& pname = property->getName();
    if (property->isDelayLoad() && !m_pst_json_[pname].isArray()) {
        //delay load
        if (0 != ((Bean*)this)->loadProperty(property)) {
            elog("Failed to delay load property: %s \n", pname.c_str());
            return 0;
        }        
    }
    Json::Value* value = ((Bean*)this)->getMemberPtr(property);
    if (value == nullptr) return 0;

    return value->size();
}


Json::Value Bean::getAt(const Property* property,  
    Json::Value::ArrayIndex index) const
{
    Json::Value value(Json::nullValue);
    if (!hasArrayProperty(property)) return Json::Value::nullRef;
    const auto& pname = property->getName();

    if (!m_pst_json_[pname].isArray()) { 
        //delay load
        if (0 != ((Bean*)this)->loadProperty(property)) 
            return Json::Value::nullRef;
    }
    if (index < m_json_[pname].size()) {
        return m_json_[pname][index];
    }
    else {
        return Json::Value::nullRef;
    }
}


int Bean::addArray(Property* property)
{
    if (property == nullptr) return -2;
    if (!property->isArray()) return -2;
    
    if (isMember(property)) return 0;
    return doAddArray(property, true);
}


int Bean::doAddArray(Property* property, bool saveAtOnce)
{
    int err = 0;
    const auto& pname = property->getName();
    m_json_[pname] = Json::Value(Json::arrayValue);
    m_pst_json_[pname] = Json::Value(Json::arrayValue);
    if (saveAtOnce) {
        err = m_world_->m_db_->saveBeanBase(m_id_, m_json_);
        if (err) {
            m_json_.removeMember(pname);
            m_pst_json_.removeMember(pname);
        }
    }
    property->addSubject(m_id_); //todo: need this line?
    return 0;
}


int Bean::setAt(Property* property,  
    Json::Value::ArrayIndex index, const Json::Value& value, bool saveAtOnce)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -3;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return -4;
    if (!m_pst_json_[pname].isArray()) { 
        //delay load
        if (0 != loadProperty(property)) {
            return -11;
        }
    }
    Json::Value* array = getMemberPtr(property);
    if (index >= array->size()) return -5;
    Json::Value *oldValue = &((*array)[index]);
    int err = setPropertyBase_(property,  oldValue, value, index, saveAtOnce);
    if (!err) {
        if (saveAtOnce) 
            m_pst_json_[pname][index] = PST_SYN;
        else 
            m_pst_json_[pname][index] = PST_MOD;
    }
    return err;
}


int Bean::setAt(Property* property,  
    Json::Value::ArrayIndex index, const char* value, bool saveAtOnce)
{
    Json::StaticString sv(value);
    Json::Value v(sv);
    return setAt(property, index, v, saveAtOnce);
}


int Bean::setAt(Property* property,  
    Json::Value::ArrayIndex index, const std::string& value, bool saveAtOnce)
{
    Json::StaticString sv(value.c_str());
    Json::Value v(sv);
    return setAt(property, index, v, saveAtOnce);
}


int Bean::append(Property* property,  const Json::Value& value)
{
    return doAppend(property, value, true);
}


int Bean::doAppend(Property* property,  const Json::Value& value, bool saveAtOnce)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return -4;

    int err = 0;
    if (!m_pst_json_[pname].isArray()) {
        //delay load
        err = loadProperty(property);
        if (err) return -11;
    } else {
        //do nothing
    }

    //insert property record first
    if (m_world_->m_db_ != nullptr && saveAtOnce) {
        if (0 != m_world_->m_db_->insertBeanProperty(m_id_, property, value)) {
            return -11;
        }
    }
    if (saveAtOnce) {
        m_pst_json_[pname].append(PST_SYN);
    } else {
        m_pst_json_[pname].append(PST_NEW);
    }

    Json::Value * array =  getMemberPtr(property);
    array->append(value);
    
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


oidType Bean::getRelation(const Property* relation) const
{
    if (!hasRelation(relation)) return 0;
    auto pname = relation->getName().c_str();
    //currently relation is direct-load
    // if (m_pst_json_[pname].asInt() == PST_NSY) {
    //     //delay load
    //     if (0 != ((Bean*)this)->loadProperty(relation)) return 0;
    // } 
    auto& value = m_json_[pname];
    return value.asUInt64();
    
}


oidType Bean::getReationAt(const Property* relation,  
    Json::Value::ArrayIndex index) const
{
    if (!hasArrayRelation(relation)) return 0;
    auto pname = relation->getName().c_str();
    auto& value = m_json_[pname];
    //currently relation is direct-load
    // if (!m_pst_json_[pname].isArray()) {
    //     //delay load
    //     if (0 != ((Bean*)this)->loadProperty(relation)) 
    //         return 0;
    // }
    if (!value.isArray()) return 0;
    if (index >= value.size()) return 0;
    return value[index].asUInt64();
}


int Bean::setRelation(Property* relation, Bean* bean)
{
    if (bean == nullptr) return -1;
    return setRelation(relation, bean->getId());
}


int Bean::setRelation(Property* relation, oidType objectId)
{
    if (relation == nullptr) return -2;
    if (objectId == 0) return -3;
    return doSetRelation(relation, objectId, true);
}


int Bean::doSetRelation(Property* relation, oidType objectId, bool saveAtOnce)
{
    if (relation->getType() != Property::RelationType) return -2;
    Bean* object = nullptr;
    Json::Value *oldValue = nullptr;
    oidType oldObjectId = 0;
    int err = 0;
    const auto& pname = relation->getName();
    if (m_pst_json_.isMember(pname)) {
        oldValue = getMemberPtr(relation);
    }

    if (oldValue != nullptr) {
        oldObjectId = (*oldValue).asUInt64();
        object = m_world_->getBean(oldObjectId, false);
    }

    if (!saveAtOnce && oldObjectId == objectId)  return 0;
    
    err =setPropertyBase_(relation, oldValue, objectId, -1, saveAtOnce);
    if (err) return err;

    m_pst_json_[pname] = PST_SYN;

    if (oldObjectId != objectId) {
        //handle object tracking
        if (object != nullptr) {
            object->removeSubject(this, relation);
        }

        relation->removeObject(oldObjectId);
        relation->addObject(objectId);
        relation->addSubject(m_id_);

        //handle subject tracking
        object = m_world_->getBean(objectId, false);
        if (object != nullptr) 
            object->addSubject(this, relation);
    }
    
    return 0;
}


int Bean::appendRelation(Property* relation,  oidType objectBeanId)
{
    return doAppendRelation(relation, objectBeanId, true);
}


int Bean::doAppendRelation(Property* relation,  oidType objectBeanId, bool saveAtOnce)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(relation)) return -4;
    const auto& pname = relation->getName();

    // if (!m_pst_json_.isMember(pname)) return -4;
    int err = 0;

    //currently relation is direct-load
    // if (!m_pst_json_[pname].isArray()) {
    //     //delay load
    //     err = loadProperty(relation);
    //     if (err) return -11;
    // } else {
    //     //already loaded
    // }

    //insert property record first
    if (m_world_->m_db_ != nullptr && saveAtOnce) {
        if (0 != m_world_->m_db_->insertBeanProperty(m_id_, relation, Json::Value(objectBeanId))) {
            return -11;
        }
    }

    auto& arrayValue = m_json_[pname];
    arrayValue.append(objectBeanId);
    if (saveAtOnce) {
        m_pst_json_[pname].append(PST_SYN);
    } else {
        m_pst_json_[pname].append(PST_NEW);
    }
    relation->addObject(objectBeanId);
    //todo: do not index array property/relation for now
    // if (property->indexed())
    //     property->addIndex(this, value);
    Bean* objectBean = m_world_->getBean(objectBeanId, false);
    if (objectBean != nullptr) 
        objectBean->addSubject(this, relation);
    return 0;
}


int Bean::appendRelation(Property* relation,  Bean* bean)
{
    if (bean == nullptr) return -1;
    return appendRelation(relation, bean->getId());
}


int Bean::setRelationAt(Property* relation,  
    Json::Value::ArrayIndex index, Bean* bean)
{
    if (bean == nullptr) return -1;
    return setRelationAt(relation, index, bean->getId());
}


int Bean::setRelationAt(Property* relation,  
    Json::Value::ArrayIndex index, oidType objectId)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    const auto& pname = relation->getName();
    if (!m_pst_json_.isMember(pname)) return -4;

    //currently relation is direct-load
    // if (!m_pst_json_[pname].isArray() && 
    //     m_pst_json_[pname].asInt() == PST_NSY) { 
    //     //delay load
    //     if (0 != loadProperty(relation)) {
    //         return -11;
    //     }
    // }

    auto& array = m_json_[pname]; 
    if (index >= array.size()) return -5;

    Json::Value *oldValue = &array[index];
     oidType oldObjectId = 0;
     int err = 0;
    Bean* object = nullptr;

    oldObjectId = (*oldValue).asUInt64();
    if (oldObjectId == objectId) return 0;

    object = m_world_->getBean(objectId, false);

    err = setPropertyBase_(relation, oldValue, objectId, index, true);
    if (err) {
        return err;
    } else {
        m_pst_json_[pname][index] = PST_SYN;
    }

    //handle object tracking
    if (object != nullptr) {
        object->removeSubject(this, relation);
    }

    relation->removeObject(oldObjectId);
    relation->addObject(objectId);

    //handle subject tracking
    object = m_world_->getBean(objectId, false);
    if (object != nullptr) 
        object->addSubject(this, relation);

    return 0;
}


int Bean::remove(Property* property)
{
    if (property == nullptr) return -1;
    return doRemoveProperty(property, true);
}


int Bean::removeNativeData()
{
    int err = 0;
    if (m_world_->m_db_ != nullptr) {
        err = m_world_->m_db_->deleteBeanNativeData(m_id_);
        if (err) {
            elog("Failed to remove native data of bean (id=%ld) \n", m_id_);
            return err;
        }
    }
    m_native_data_json_ = Json::Value(Json::ValueType::nullValue);
    m_native_data_pst_json_ = PST_SYN;
    return 0;
}


int Bean::doRemoveProperty(Property* property, bool saveAtOnce)
{
    const auto& pname = property->getName();
   Json::Value* valuePtr = getMemberPtr(property);
    if (valuePtr == nullptr) return -1; //bean has no such property value
    Json::Value& pstValue = m_pst_json_[pname];
    if (pstValue.isArray()) 
        if (m_json_[pname].isNull() || !m_json_[pname].isArray()) 
            return -1; //inconsistent value

    if (saveAtOnce) {
        //delete from db first
        if (m_world_->m_db_ != nullptr) {
            if (0 != m_world_->m_db_->deleteBeanProperty(m_id_, property))
                return -1;
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
            Bean* objectBean = m_world_->getBean(oid, false);
            if (objectBean != nullptr)
                objectBean->removeSubject(this, property);
    }
    else if (property->getType() == Property::ArrayRelationType)
    {
        auto& array = value;
        oidType oid = 0;
        for (Json::ArrayIndex index = 0; index < array.size(); index++)
        {
            oid = array[index].asUInt64();
            property->removeObject(oid);
            Bean* objectBean = m_world_->getBean(oid, false);
            if (objectBean == nullptr) continue;
            objectBean->removeSubject(this, property);
        }
    }
    
    //remove member of json object
    m_pst_json_.removeMember(pname);
    m_json_.removeMember(pname);
    return 0;
}


int Bean::removeAt(Property* property, Json::Value::ArrayIndex index)
{
    if (property == nullptr) return -1;
    return doRemoveProperty(property, index, true);
}

int Bean::doRemoveProperty(Property* property, Json::Value::ArrayIndex index, bool saveAtOnce)
{
    const auto& pname = property->getName();
    if (!m_pst_json_.isMember(pname)) return -1; //bean has no such property value
    Json::Value& pstValue = m_pst_json_[pname];
    Json::Value* arrayPtr = getMemberPtr(property);
    if (arrayPtr == nullptr) return -1; 
    Json::Value& array = *arrayPtr;
    if (!array.isArray()) return -1; //if property is array, array.isArray() shall be true
    if (index >= array.size()) return -1;

    if (saveAtOnce) {
        //delete from db first
        if (m_world_->m_db_ != nullptr) {
            if (0 != m_world_->m_db_->deleteBeanProperty(m_id_, property, index))
                return -1;
        }
    }

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
        objectBean = m_world_->getBean(oid, false);
        if (objectBean != nullptr)
            objectBean->removeSubject(this, property);
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
    return 0;
}


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

Json::Value& Bean::getNativeData()
{       
    if (m_native_data_pst_json_.asInt() == PST_NSY) {
    //delay load
        if (m_world_->m_db_ != nullptr) {
            int err = m_world_->m_db_->
                loadBeanNativeData(m_id_, m_native_data_json_);
            if (!err) {
                m_native_data_pst_json_ = PST_SYN;
            } else {
                m_native_data_json_ = Json::Value(Json::ValueType::objectValue);
            }
        } 
    } 

    return m_native_data_json_;
}

Json::Value& Bean::getNativeData() const
{
    return ((Bean*)this)->getNativeData();
}

int Bean::setNativeData(Json::Value& data, bool saveAtOnce)
{
    int err = 0;
    if (saveAtOnce) {
        if (m_world_->m_db_ != nullptr) {
            //save to db
            err = m_world_->m_db_->updateBeanNativeData(m_id_, data);
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


// int Bean::pstTransition(int currentPst, int desiredPst)
// {
//     if (currentPst < 0 || currentPst >= PST_MAX)  return -1;
//     if (desiredPst < 0 || desiredPst >= PST_MAX)  return -1;

//     //transition table from curent pst to target pst.
//     //row index: current PST
//     //col index: argument status
//     //value: result status
//     //
//     //Example:
//     //When current pst is PST_NEW and status is  PST_MOD,
//     //the transition result will be PST_NEW, i.e. row #1 col. #2 
//     //will be 1 (index 0 based).
//     //This is rational because after modification, it will be 
//     //taken as a new property from DB perspective since
//     //it has not been synced to DB.
//     //
//     //Note -1 means to remove the pst record.
//     static const int pstTransitionTable[5][5]= {
//         {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD}, /*CURRENT PST_NSY*/
//         {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD},  /*current PST_SYN*/
//         {              -1, PST_SYN,                  -2, PST_NEW,                 -1},  /*current PST_NEW*/
//         {PST_NSY, PST_SYN,                  -2, PST_MOD, PST_RMD}, /*current PST_MOD*/
//         {PST_NSY,                -1, PST_MOD, PST_RMD, PST_RMD}  /*current PST_RMD*/
//     };

//     return pstTransitionTable[currentPst][desiredPst];
// }


int Bean::load(Json::Value& data)
{
    if (m_world_->m_db_ == nullptr) return -1;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    Json::Value* dataValuePtr = nullptr;

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

        dataValuePtr = &data[pname];

        if (dataValuePtr->isNull()) { //delay load
            m_json_[pname] = Json::Value::nullRef;
            m_pst_json_[pname] = PST_NSY;
        } else {
            switch (property->getType()) {
                case Property::PrimaryType:
                    m_pst_json_.removeMember(pname);
                    doSet(property,  *dataValuePtr, false);
                    m_pst_json_[pname] = PST_SYN;
                    break;
                case Property::RelationType:
                    m_pst_json_.removeMember(pname);
                    doSetRelation(property, dataValuePtr->asUInt64(), false);         
                    m_pst_json_[pname] = PST_SYN;
                    break;
                case Property::ArrayPrimaryType:
                    //override pst value
                    m_pst_json_.removeMember(pname);                 
                    doAddArray(property, false);
                    size = dataValuePtr->size();
                    for (int i = 0; i < size; i++) {
                        doAppend(property,  (*dataValuePtr)[i], false);
                        //set it again to ensure it's PST_SYN
                        m_pst_json_[pname][i] = PST_SYN;
                    }
                    break;
                case Property::ArrayRelationType:
                    //override pst value
                    m_pst_json_.removeMember(pname);         
                    doAddArray(property, false);
                    size = dataValuePtr->size();
                    for (int i = 0; i < size; i++) {
                        doAppendRelation(property,  (*dataValuePtr)[i].asUInt64(), false);
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

    return err;
}


int Bean::unload()
{
    int err = 0;
   Property* property = nullptr;
   Property* tmpProperty = nullptr;
   Bean* subject = nullptr;
   Bean* object = nullptr;
   Json::Value* objectValue = nullptr;
   oidType objectId = 0;
   Json::Value helperArrayValue = Json::Value(Json::arrayValue);
   Json::Value* tmpv = nullptr;

    for (auto& pname : m_json_.getMemberNames()) {
        property = m_world_->getProperty(pname.c_str());
        if (property == nullptr) continue;

        if (property->isRelation()) {
            objectValue = getMemberPtr(property);
            if (objectValue != nullptr) {
                if (objectValue->isArray()) {
                    tmpv = objectValue;
                } else {
                    helperArrayValue.append(objectValue->asUInt64());
                    tmpv = &helperArrayValue;
                }
                for (Json::ArrayIndex i = 0; i < tmpv->size(); i++) {
                    objectId = ((*tmpv)[i]).asUInt64();
                    object = m_world_->getBean(objectId, false);
                    if (object != nullptr) {
                        //remove subject reference from relation objects
                        object->removeSubject(this, property);
                    }
                }

                //remove object reference from property
                property->removeObject(m_id_);
            }
        }

        //remove subject reference from property
        property->removeSubject(m_id_);

        //remove property (remove index)
        doRemoveProperty(property, false);
    }

    //reset data
    m_json_ = Json::Value(Json::objectValue);
    m_pst_json_ = Json::Value(Json::objectValue);

    //reset native data
   m_native_data_json_ = Json::Value::nullRef;
   m_native_data_pst_json_ = PST_NSY;

   //reset subject map
   m_subjectMap_.clear();

    return err;
}


int Bean::loadProperty(const Property* property)
{
    if (m_world_->m_db_ == nullptr) return -1;

    int err = 0;
    const auto& pname = property->getName();

    if (property->isArray()) {
        err = m_world_->m_db_->loadBeanProperty(m_id_, property, (Json::Value&)m_json_[pname]);
        if (err) {
            m_json_[pname] = Json::Value::nullRef; //reset to null
            m_pst_json_[pname] = PST_NSY;
        } else {
            if (m_json_[pname].isNull()) { //empty array in db
                m_json_[pname] = Json::Value(Json::arrayValue);
            }
            m_pst_json_[pname] = Json::Value(Json::arrayValue);
            auto& v = (Json::Value&)m_pst_json_[pname];
            for (int i = 0; i < m_json_.size(); i++) {
                v.append(PST_SYN);
            }
        }
    } else {
        err = m_world_->m_db_->loadBeanProperty(m_id_, property, (Json::Value&)m_json_[pname]);
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
    if (m_world_->m_db_ == nullptr) return -1;
    if (m_pst_json_.isNull()) return -2;

    int err = 0;
    int i = 0;
    int size = 0;
    Json::Value* pstValuePtr = nullptr;
    Property* property = nullptr;

    for (auto& pname : m_pst_json_.getMemberNames()) {
        property = m_world_->getProperty(pname.c_str());
        if (property == nullptr) continue; //shall not happen
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
                        err = m_world_->m_db_->updateBeanProperty(m_id_, property, i, m_json_[pname][i]);
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
        } else { 
            if (pstValuePtr->asInt() == PST_MOD) {
                err = m_world_->m_db_->updateBeanProperty(m_id_, property, m_json_[pname]);
                if (err) goto out;
            } else if (pstValuePtr->asInt() == PST_NEW) {
                err = m_world_->m_db_->insertBeanProperty(m_id_, property, m_json_[pname]);
            }
        }            
    }

    err = m_world_->m_db_->saveBeanBase(m_id_, m_json_, &m_native_data_json_);
    if (err) goto out;

    if (m_native_data_pst_json_.asInt() == PST_MOD) {
            err = m_world_->m_db_->updateBeanNativeData(m_id_, m_native_data_json_);
            if (err) goto out;
            m_native_data_pst_json_ = PST_SYN;
    }

out:
    if (err) {
        elog("Failed to save bean (id=%llu) \n", m_id_);
    } else {
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
            } else if (pstValuePtr->asInt() == PST_MOD) {
                (*pstValuePtr) = PST_SYN;
            } else {
                //do nothing
            }
        }
    }
    
    return err;
}


int Bean::clear()
{
    //todo: put them into transaction and handle exception
    removeAllProperties();
    removeNativeData();
    return 0;
}

}
}
}