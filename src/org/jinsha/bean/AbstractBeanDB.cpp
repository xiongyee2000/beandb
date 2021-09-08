#include "./AbstractBeanDB.h"
#include "./common.h"
#include "./internal_common.hxx"

#include <assert.h>

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


int AbstractBeanDB::reInit()
{
    if (connected()) return -1;
    return reInit_();
}


int AbstractBeanDB::connect()
{
    int err = 0;
    if (!m_connected_) {
        err = connect_();
        if (err) {
            elog("Failed to connect (err:%d) \n", err);
        } else {
            m_world_ = new BeanWorld(*this);
            assert(m_world_);
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
        if (err) {
            elog("Failed to disconnect (err:%d) \n", err);
        } else {
            delete m_world_;
            m_connected_ = false;
        }
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
            elog("Failed to begin transaction (err=%d) \n", err);
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


Property* AbstractBeanDB::newProperty(const char* name, 
    pidType id,
    Property::Type type,
    Property::ValueType valueType, 
    bool delayLoad,
    bool needIndex) const
{
    return new Property(name, id, type, valueType, delayLoad, needIndex);
}

Bean* AbstractBeanDB::newBean(oidType id) const
{
    return new Bean(id, getWorld());
}


}
}
}