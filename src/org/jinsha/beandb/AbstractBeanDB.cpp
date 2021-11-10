#include "./AbstractBeanDB.h"
#include "./common.h"
#include "./internal_common.hxx"

#include <assert.h>

namespace org {
namespace jinsha {
namespace beandb {


AbstractBeanDB::AbstractBeanDB()
    : m_world_(nullptr) 
{
    m_world_ = new BeanWorld(*this);
}


AbstractBeanDB::~AbstractBeanDB()
{
    delete m_world_;
}


int AbstractBeanDB::connect()
{
    int err = 0;
    if (!m_connected_) {
        err = doConnect();
        if (err) {
            elog("Failed to connect (err:%d). \n", err);
        } else {
            m_connected_ = true;
            err = m_world_->loadProperties();
            if (err) {
                doDisconnect();
                m_connected_ = false;
                elog("Failed to connect due to fail to load properties (err:%d). \n", err);
            } 
        }
    } 
    return err;
}


int AbstractBeanDB::disconnect()
{
    int err = 0;
    if (m_connected_) {
        err = doDisconnect();
        if (err) {
            elog("Failed to disconnect (err:%d) \n", err);
        } else {
            m_world_->doUnloadAll(false);
            m_connected_ = false;
        }
    }
    return err;
}


bool AbstractBeanDB::connected() const
{
    return m_connected_;
}


Property* AbstractBeanDB::newProperty(const char* name, 
    pidType id,
    Property::Type type,
    Property::ValueType valueType, 
    bool delayLoad) const
{
    return new Property(getWorld(), name, id, type, valueType, delayLoad);
}


}
}
}