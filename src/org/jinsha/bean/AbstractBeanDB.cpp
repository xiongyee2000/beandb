#include "./AbstractBeanDB.h"
#include "./common.h"
#include "./internal_common.hxx"

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
        if (!err) {
            m_world_->reloadProperties();
            m_connected_ = true;
        }
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
        if (err)  {
            elog("%s", "Failed to begin transaction\n");
        }  else {
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
    if (err)  {
        elog("%s", "Failed to commit transaction\n");
    }  else {
        m_inTransaction_ = false;
    }
    return err;
}


int AbstractBeanDB::rollbackTransaction()
{
    int err = 0;
    if (!m_inTransaction_) return -1;
    err = rollbackTransaction_();
    if (err)  {
        elog("%s", "Failed to rollback transaction\n");
    }  else {
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
    if (bean == nullptr) return 0;
    return bean->save();
}


int AbstractBeanDB::deleteBean(Bean* bean)
{
    if (bean == nullptr) return 0;
    
    int err = 0;
    err = deleteBean_(bean);
    if (!err) {
        m_world_->removeBean(bean->getId());
    }
    
    return err;
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
    return new Bean(id, getWorld());
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