#pragma once

#include "./BeanDBUserIntf.h"
#include "./BeanDBPersistenceIntf.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

class AbstractBeanDB : public BeanDBUserIntf, public BeanDBPersistenceIntf
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
     * @ref BeanDBUserIntf::reInit()
     */
    virtual int reInit() override;

    /***********************************************************
     * connection related
     ***********************************************************/
public:
    /**
     * @ref BeanDBUserIntf::connect()
     */
    virtual int connect() override;

    /**
     * @ref BeanDBUserIntf::disconnect()
     */
    virtual int disconnect() override;

    /**
     * @ref BeanDBUserIntf::connected()
     */
    virtual bool connected() const override;

    /***********************************************************
     * transaction related
     ***********************************************************/
    /**
     * @ref BeanDBUserIntf::beginTransaction()
     */
    virtual int beginTransaction() override;

    /**
     * @ref BeanDBUserIntf::commitTransaction()
     */
    virtual int commitTransaction() override;

    /**
     * @ref BeanDBUserIntf::rollbackTransaction()
     */
    virtual int rollbackTransaction() override;

    /**
     * @ref BeanDBUserIntf::inTransaction()
     */
    virtual bool inTransaction() override;


    /***********************************************************
     * property related
     ***********************************************************/
public:
    /**
     * @ref BeanDBUserIntf::defineProperty()
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;

    /**
     * @ref BeanDBUserIntf::defineArrayProperty()
     */
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;

    /**
     * @ref BeanDBUserIntf::defineRelation()
     */
    virtual Property* defineRelation(const char* name, bool needIndex = false) override;

    /**
     * @ref BeanDBUserIntf::defineArrayRelation()
     */
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) override;

    /**
     * @ref BeanDBUserIntf::undefineProperty()
     */
     virtual int undefineProperty(const char* name) override;

    /**
     * @ref BeanDBUserIntf::undefineRelation()
     */
    virtual int undefineRelation(const char* name) override {return undefineProperty(name);};

    /**
     * @ref BeanDBUserIntf::getProperty()
     */
    // virtual const Property* getProperty(const char* name) const;
    virtual Property* getProperty(const char* name) override;

    /**
     * @ref BeanDBUserIntf::getProperties()
     */
    virtual const std::unordered_map<std::string, Property*>& getProperties() override;


    /***********************************************************
     * bean related
     ***********************************************************/
public:
    /**
     * @ref BeanDBUserIntf::createBean()
     */
    virtual Bean* createBean() override;

    /**
     * @ref BeanDBUserIntf::getBean()
     */
    virtual Bean* getBean(oidType id) override;

    /**
     * @ref BeanDBUserIntf::saveBean()
     */
    virtual int saveBean(Bean* bean) override;

    /**
     * @ref BeanDBUserIntf::deleteBean()
     */
    virtual int deleteBean(Bean* bean) override;


    /**
     * @ref BeanDBUserIntf::loadAll()
     */
    virtual int loadAll() override;

    /**
     * @ref BeanDBUserIntf::saveAll()
     */
    virtual int saveAll() override;

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
        bool needIndex = false) const;

    /**
     * This method is used for the implemenation method of 
     * AbstractBeanDB::getBean() to create new Property
     * instance.
     */
    Bean* newBean(oidType id) const;

    /**
     * This method is used for the derived class to get delayLoad
     * flag of a property.
     */
    static bool isDelayLoad(const Property& property);

    /**
     * This method is used for the derived class to set delayLoad
     * flag of a property.
     */
    static void setDelayLoad(Property& property, bool isDelayLoad);

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