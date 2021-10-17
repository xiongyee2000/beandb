#pragma once

#include "./BeanDBIntf.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

class AbstractBeanDB : public BeanDBIntf
{

    ////////////////////////////////////////////////////////////////////////
    // public methods at user level
     ////////////////////////////////////////////////////////////////////////

public:
    /**
     * Constructor
     */
    AbstractBeanDB();

    /**
     * Destructor
     */
    virtual ~AbstractBeanDB();

    /**
     * @ref BeanDBPIntf::reInit()
     */
    virtual int reInit() override;



    /***********************************************************
     * connection related
     ***********************************************************/
public:
    /**
     * Connect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int connect() override;

    /**
     * Disonnect to the database. 
     * 
     * CAUTION: 
     * Once disconnected, all beans and properties
     * previously loaded will be deleted!
     * 
     * @return 0 for success, or an error code
     */
    virtual int disconnect() override;

    /**
     * Is the database connected.
     * 
     * @return true if connected, or false otherwise 
     */
    virtual bool connected() const override;


    /***********************************************************
     * transaction related
     ***********************************************************/
    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int beginTransaction();

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int commitTransaction();

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int rollbackTransaction();

    /**
     * Check if it is in a transaction
     * 
     * @return true if in a transaction, or false if not
     */
    virtual bool inTransaction();


    ////////////////////////////////////////////////////////////////////////
    // Other methods that derived class may use
     ////////////////////////////////////////////////////////////////////////

protected:
    /**
     * This method is used for the implemenation method of 
     * AbstractBeanDB::loadProperties() to create new Property
     * instance.
     */
    Property* newProperty(const char* name, 
        pidType id,
        Property::Type type,
        Property::ValueType valueType, 
        bool delayLoad) const;

    // /**
    //  * This method is used for the implemenation method of 
    //  * AbstractBeanDB::getBean() to create new Property
    //  * instance.
    //  */
    // Bean* newBean(oidType id) const;

    /**
     * Get the attached world.
     * 
     * @return the world
     */
    virtual BeanWorld* getWorld() const {return m_world_;};

protected:
    BeanWorld *m_world_;
    bool m_connected_ = false;
    bool m_inTransaction_ = false;

private:

friend class BeanWorld;
friend class Bean;
friend class Property;
};

}
}
}