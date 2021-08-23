#include "./AbstractBeanDB.h"

namespace org {
namespace jinsha {
namespace bean {

AbstractBeanDB::AbstractBeanDB()
    : m_world_(nullptr) 
{
}


AbstractBeanDB::~AbstractBeanDB()
{
}


int AbstractBeanDB::beginTransaction()
{
    int err = 0;
    if (!m_inTransaction_) {
        err = doBeginTransaction();
        if (!err) {
            m_inTransaction_ = true;
        }
    } 
    return err;
}


int AbstractBeanDB::commitTransaction()
{
    int err = 0;
    if (!m_inTransaction_) return -1;
    err = doCommitTransaction();
    if (!err) 
        m_inTransaction_ = false;
    return err;
}


int AbstractBeanDB::rollbackTransaction()
{
    int err = 0;
    if (!m_inTransaction_) return -1;
    err = doRollbackTransaction();
    if (!err) {
        m_inTransaction_ = false;
    }
    return err;
}


Bean* AbstractBeanDB::getBean(oidType id)
{
    BeanWorld *world = nullptr;
    if ((world = getWorld()) == nullptr) return nullptr;

    int err = 0;
    Bean* bean = world->getBean(id);
    if (bean == nullptr) {
        bean = world->createBean((oidType)id);
        err = bean->load();
        if (err) {
            world->removeBean(bean->getId());
            return nullptr;
        }
    } 
    return bean;
}


Property* AbstractBeanDB::newProperty(const char* name, 
    pidType id,
    Property::Type type,
    Property::ValueType valueType, 
    bool needIndex) const
{
    return new Property(name, id, type, valueType, needIndex);
}

Bean* AbstractBeanDB::newBean(oidType id) const
{
    return new Bean(id);
}


bool AbstractBeanDB::isDelayLoad(const Property& property)
{
    return property.isDelayLoad();
}


void AbstractBeanDB::setDelayLoad(Property&  property, bool isDelayLoad)
{
    property.setDelayLoad(isDelayLoad);
}


// int AbstractBeanDB::saveBean(Bean* bean)
// {
//     if (bean == nullptr) return -1;
//     if (!connected()) return -1;

//     int err = 0;
//     Json::Value value;
//     const char* pname = nullptr;
//     Property* property = nullptr;
//     Json::ArrayIndex arraySize = 0;
//     oidType beanId = bean->getId();
//     bool isArray = false;
//     Json::Value pstValue;
//     bool alreadyInTransaction = false;

//     alreadyInTransaction = inTransaction();
//     if (!alreadyInTransaction) {
//             if (beginTransaction() != 0) return -1;
//     }

//     err = saveBeanBase(bean);
//     if (err) goto _out;

//     for (const auto& pname : bean->m_pst_json_.getMemberNames()) {
//         property = getWorld()->getProperty(pname.c_str());
//         if (property == nullptr) {
//             //todo
//         } else {
//             if (property->getType() == Property::ArrayPrimaryType ||
//                 property->getType() == Property::ArrayRelationType) {
//                     isArray = true;
//                     arraySize = bean->m_pst_json_.size();
//                     pstValue = Json::arrayValue;
//                     for (int i = 0; i < arraySize; i++) {
//                     pstValue.append(bean->m_pst_json_[pname][i]);
//                 }                               
//             } else {
//                 isArray = false;
//                 arraySize = 1;
//                 pstValue = Json::arrayValue;
//                 pstValue.append(bean->m_pst_json_[pname]);
//             }
//         }

//         for (int i = 0; i < arraySize; i++) {
//             switch (pstValue[i].asInt()) {
//                 case Bean::PST_NSY:
//                 case Bean::PST_SYN:
//                     break; //no need to update
//                 case Bean::PST_NEW:
//                     err = insertBeanProperty(beanId, property, bean->m_json_[pname]);
//                     if (err) goto _out;
//                     break;
//                 case Bean::PST_MOD:
//                     if (isArray) {
//                         err = updateBeanProperty(beanId, property, i, bean->m_json_[pname]);
//                         if (err) goto _out;
//                     } else {
//                         err = updateBeanProperty(beanId, property, bean->m_json_[pname]);
//                         if (err) goto _out;
//                     }
//                     break;
//                 case Bean::PST_RMD:
//                     if (isArray) {
//                         err = deleteBeanProperty(beanId, property, i);
//                         if (err) goto _out;
//                     } else {
//                         err = deleteBeanProperty(beanId, property);
//                         if (err) goto _out;
//                     }
//                     break;
//                 default:
//                     break;
//             } 
//         }
//     }

// _out:
//    if (err) {
//         if (!alreadyInTransaction) {
//              err = rollbackTransaction();
//         }
//     } else {
//         if (!alreadyInTransaction) {
//             err = commitTransaction();
//         }
//     }
//     return err; 
// }


}
}
}