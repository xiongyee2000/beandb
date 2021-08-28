#include "./AbstractBeanDB.h"
#include "./common.h"

namespace org {
namespace jinsha {
namespace bean {


AbstractBeanDB::AbstractBeanDB()
    : m_world_(nullptr) 
{
    m_world_ = new BeanWorld(*this);
}


AbstractBeanDB::~AbstractBeanDB()
{
}


int AbstractBeanDB::reInit()
{
    m_world_->clear();
    return reInit_();
}


int AbstractBeanDB::connect()
{
    int err = 0;
    if (!m_connected_) {
        err = connect_();
        if (!err) m_connected_ = true;
    } 
    return err;
}


int AbstractBeanDB::disconnect()
{
    int err = 0;
    if (m_connected_) {
        err = disconnect_();
        if (!err) m_connected_ = false;
    } 
    return err;
}


bool AbstractBeanDB::connected() const
{
    return m_connected_;
}


int AbstractBeanDB::beginTransaction()
{
    int err = 0;
    if (!m_inTransaction_) {
        err = beginTransaction_();
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
    err = commitTransaction_();
    if (!err) 
        m_inTransaction_ = false;
    return err;
}


int AbstractBeanDB::rollbackTransaction()
{
    int err = 0;
    if (!m_inTransaction_) return -1;
    err = rollbackTransaction_();
    if (!err) {
        m_inTransaction_ = false;
    }
    return err;
}


bool AbstractBeanDB::inTransaction()
{
    return m_inTransaction_;
}


Property* AbstractBeanDB::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return m_world_->definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


Property* AbstractBeanDB::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return m_world_->definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


Property* AbstractBeanDB::defineRelation(const char* name, bool needIndex)
{
    return m_world_->definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


Property* AbstractBeanDB::defineArrayRelation(const char* name, bool needIndex)
{
    return m_world_->definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


int AbstractBeanDB::undefineProperty(const char* name)
{
    return m_world_->undefineProperty(name);
}


Property* AbstractBeanDB::getProperty(const char* name)
{
    return m_world_->getProperty(name);
}


const std::unordered_map<std::string, Property*>&  AbstractBeanDB::getProperties()
{
    return m_world_->getProperties();
}


Bean* AbstractBeanDB::createBean() 
{
    int err = 0;
    oidType id = 0;
    Bean* bean = nullptr;
    err  = createBean_(id);
    if (!err) {
        bean = m_world_->createBean(id);
    } 
    return bean;
}

Bean* AbstractBeanDB::getBean(oidType id)
{
    int err = 0;
    Bean* bean = m_world_->getBean(id);
    if (bean == nullptr) {
        bean = m_world_->createBean((oidType)id);
        err = bean->load();
        if (err) {
            m_world_->removeBean(bean->getId());
            return nullptr;
        }
    } 
    return bean;
}


int AbstractBeanDB::saveBean(Bean* bean)
{
    //todo
    return -1;
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


int AbstractBeanDB::deleteBean(Bean* bean)
{
    //todo:
    return -1;
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


int AbstractBeanDB::loadAll()
{
    //todo
    return -1;
}


int AbstractBeanDB::saveAll()
{
    //todo
    return -1;
}

}
}
}