
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
    unload();
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
    return m_json_.isMember(property->getName());
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
    if (property == nullptr) return Json::nullValue;
    if (property->getType() != Property::PrimaryType) return Json::nullValue;

    const char* pname = property->getName().c_str();
    if (!m_json_.isMember(pname)) return Json::nullValue; //no such property

    if (property->m_delay_load) {
        if (m_delay_load_json_.isMember(pname)) {
            //already loaded
            return m_delay_load_json_[pname];
        } else {
            //not loaded, load it
            Json::Value* v = ((Bean*)this)->delayLoad(property);
            if (v != nullptr) {
                return *v;
            } else {
                return Json::nullValue;
            }
        }
    } else {
            value = m_json_.get(pname, value);
    }
    return value;
}


int Bean::setProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::PrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    Json::Value *oldValue = getMemberPtr(property);
    if (oldValue == nullptr) {
        setPropertyBase_(property,  oldValue, value);
    }

    //handle subject tracking
    property->addSubject(m_id_);
    return 0;
}


void Bean::setPropertyBase_(Property* property, 
    Json::Value *oldValue, const Json::Value&  newValue, Json::Value::ArrayIndex index)
{
    const auto& pname = property->getName();
    if (oldValue == nullptr)
    {   //property has not been set before
        //insert property record first
        // if (m_world_->m_db != nullptr) {
        //     if (0 != m_world_->m_db->insertBeanProperty(this, property, newValue))
        //         return;
        // }
        //set the property (regardless of value)
        oldValue = &m_json_[pname];
        //no old value, need to increment ref. count
        // property->m_refCount_++;
    }
    else
    {
        if (*oldValue == newValue) return; 
        // //update property record first
        // if (m_world_->m_db != nullptr) {
        //     if (index == (Json::Value::ArrayIndex)-1) {
        //         if (0 != m_world_->m_db->updateBeanProperty(this, property, newValue))
        //             return;
        //     } else {
        //         if (0 != m_world_->m_db->updateBeanProperty(this, property, index, newValue))
        //             return;                
        //     }
        // }

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
    if (!hasArrayProperty(property)) return Json::nullValue;
    const char* pname = property->getName().c_str();
    if (index >= m_json_[pname].size()) return Json::nullValue;

    if (property->m_delay_load) {
        if (m_delay_load_json_.isMember(pname)) {
            //already loaded
             if (index < m_delay_load_json_[pname].size()) {
                return m_delay_load_json_[pname][index];
             }
        } else {
            //not loaded, load it
            Json::Value* v = ((Bean*)this)->delayLoad(property);
            if (v != nullptr && v->isArray()) {
                    if (index < v->size()) {
                        return (*v)[index];
                    } else {
                        value = Json::nullValue;
                    }
            } else {
                value = Json::nullValue;
            }
        }
    } else {
        return m_json_[pname][index];
    }
    return value;
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
    const auto& pname = property->getName();
    if (!m_json_.isMember(pname)) return -4;
    Json::Value* array = getMemberPtr(property);
    if (array == nullptr) return -4; 
    if (index >= array->size()) return -5;
    Json::Value *oldValue = &((*array)[index]);
    setPropertyBase_(property,  oldValue, value, index);
    return 0;
}


int Bean::appendProperty(Property* property,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    if (property == nullptr) return -2;
    if (property->getType() != Property::ArrayPrimaryType) return -2;
    if (property->getValueType() != (Property::ValueType)value.type()) return -3;
    if (!hasArrayProperty(property)) return -4;
    Json::Value * array =  getMemberPtr(property);
    if (array == nullptr) return -4; 
    // //insert property record first
    // if (m_world_->m_db != nullptr) {
    //     if (0 != m_world_->m_db->insertBeanProperty(this, property, value))
    //         return -11;
    // }
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
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::RelationType) return -2;
    Json::Value *oldValue =  getMemberPtr(relation);
    setPropertyBase_(relation, oldValue, objectId);

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


int Bean::appendRelation(Property* relation,  oidType objectBeanId)
{
    if (relation == nullptr) return -2;
    if (relation->getType() != Property::ArrayRelationType) return -2;
    if (!hasArrayRelation(relation)) return -4;
    const auto& pname = relation->getName();
    auto& arrayValue = m_json_[pname];
    // //insert property record first
    // if (m_world_->m_db != nullptr) {
    //     if (0 != m_world_->m_db->insertBeanProperty(this, relation, Json::Value(objectBeanId)))
    //         return -11;
    // }
    arrayValue.append(objectBeanId);
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
    // //insert property record first
    // if (m_world_->m_db != nullptr) {
    //     if (0 != m_world_->m_db->insertBeanProperty(this, relation, Json::Value(bean->m_id_)))
    //         return -11;
    // }
    const auto& pname = relation->getName();
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
    setPropertyBase_(relation, oldValue, bean->getId(), index);

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
    if (property == nullptr) return Json::nullValue;
    return doRemoveProperty(property);
}


Json::Value Bean::doRemoveProperty(Property* property, bool internal)
{
    Json::Value* valuePtr = getMemberPtr(property);
    if (valuePtr == nullptr) return Json::nullValue;

    // //delete from db first
    // if (m_world_->m_db != nullptr) {
    //     if (0 != m_world_->m_db->deleteBeanProperty(this, property))
    //         return Json::nullValue;
    // }

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
    if (property->m_delay_load) {
        this->m_json_.
        removeMember(pname);
        return this->m_delay_load_json_.
            removeMember(pname);
    } else {
        return this->m_json_.
            removeMember(pname);
    }
}


Json::Value Bean::removeProperty(Property* property, Json::Value::ArrayIndex index)
{
    if (property == nullptr) return Json::nullValue;
    return doRemoveProperty(property, index);
}

Json::Value Bean::doRemoveProperty(Property* property, Json::Value::ArrayIndex index, bool internal)
{
    Json::Value rtn; //null
    Json::Value* arrayPtr = getMemberPtr(property);
    if (arrayPtr == nullptr) return rtn; 
    Json::Value& array = *arrayPtr;
    if (!array.isArray()) return rtn; //if property is array, array.isArray() shall be true
    if (index >= array.size()) return rtn;

    // //delete from db first
    // if (m_world_->m_db != nullptr) {
    //     if (0 != m_world_->m_db->deleteBeanProperty(this, property, index))
    //         return Json::nullValue;
    // }

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
    if (property->m_delay_load) {
        if (m_delay_load_json_.isMember(pname)) {
            return &m_delay_load_json_[pname];
        } else {
            return delayLoad(property);
        }
    } else {
        return (m_json_.isMember(pname)) ? &m_json_[pname] : nullptr;
    }
}

Json::Value Bean::getUnmanagedValue(const char* name)
{
    if (name == nullptr || name[0] == 0) return Json::Value(nullptr);
    return m_unmanaged_json_[name];
}

Json::Value Bean::getUnmanagedValue(const char* name) const
{
    return ((Bean*)this)->getUnmanagedValue(name);
}

void Bean::setUnmanagedValue(const char* name, Json::Value& value)
{
    if (name == nullptr || name[0] == 0) return;
    m_unmanaged_json_[name] = value;
}

Json::Value* Bean::delayLoad(const Property* property)
{
    if (m_world_->m_db != nullptr) return nullptr;
    
    bool connected = m_world_->m_db->connected();
    if (!connected) {
        if (0 != m_world_->m_db->connect()) {
            return nullptr;
        }
    }

    Json::Value* rtn = nullptr;
    Json::Value value = m_world_->m_db->getBeanProperty(this, property);
    if (!value.isNull()) { 
        //loaded, and then set the value
        ((Bean*)this)->m_delay_load_json_[property->getName()] = value;
        rtn = &m_delay_load_json_[property->getName()];
    } 
    if (!connected) { 
        //if previously not connected, resume it to disconnected
        m_world_->m_db->disconnect();
    }
    return rtn;
}

void Bean::setPst(Property* property, int status)
{
    if (property == nullptr) return;
    if (status < 0 || status >= PST_MAX)  return;

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
    const auto& pname = property->getName();
        if (m_pst_json_.isMember(pname)) {
            int newStatus = pstTransitionTable[m_pst_json_[pname].asInt()][status];
            if (newStatus == -1) {
                m_pst_json_.removeMember(pname);
            } else if (newStatus == -2) {
                //shall not reach here
            } else {
                m_pst_json_[pname] = newStatus;
            }
        } else {
            m_pst_json_[pname] = PST_NEW;
        }
}


int Bean::load()
{
    int err = 0;
    if (m_world_->m_db == nullptr) return -1;
    return m_world_->m_db->loadBean(this);
}


int Bean::unload()
{
    int err = 0;
   removeAllProperties();
   m_unmanaged_json_ = Json::nullValue;
   m_delay_load_json_ = Json::nullValue;
   for (auto& pname : m_pst_json_.getMemberNames()) {
       if (m_pst_json_[pname].isNull()) {
           m_pst_json_.removeMember(pname);
       } else {
           if (m_pst_json_[pname].asInt() == PST_NEW ) {
               m_pst_json_.removeMember(pname);
               auto iter = m_subjectMap_.find(m_id_);
           } else {
               m_pst_json_[pname] = PST_NSY;
           }
       }
   }
    return err;
}


int Bean::loadProperty(Property* property, const Json::Value& value)
{
    int err = 0;
    err = setProperty(property, value);
    if (err) return err;

    return 0;
}

int Bean::save()
{
    int err = 0;
    if (m_world_->m_db != nullptr) {
        err = m_world_->m_db->saveBean(this);
    }
    return err;
}


}
}
}