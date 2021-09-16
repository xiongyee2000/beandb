#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"
#include "./BeanDBUserIntf.h"
#include "./AbstractBeanDB.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class Property;
class AbstractBeanDB;

class BeanWorld : public BeanDBUserIntf
{
public:
    /**
     * Constructor
     * 
     * @param db the database attached with this world
     */
    BeanWorld(AbstractBeanDB& db);

    /**
     * Destructor
     */
    virtual ~BeanWorld();

    /**
     * Delete all means/properties from the world.
     * 
     * CAUTION: If this world is attached with a database, 
     * all data will be erased from the attached database.
     */
    virtual void clear();


    /***********************************************************
     * property related
     ***********************************************************/
public:
    /**
     * @ref BeanDBUserIntf::defineProperty()
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType) override;

    /**
     * @ref BeanDBUserIntf::defineArrayProperty()
     */
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType) override;

    /**
     * @ref BeanDBUserIntf::defineRelation()
     */
    virtual Property* defineRelation(const char* name) override;

    /**
     * @ref BeanDBUserIntf::defineArrayRelation()
     */
    virtual Property* defineArrayRelation(const char* name) override;

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
    virtual Bean* getBean(oidType id, bool loadFromDB = true) override;

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

    /**
     * Remove a bean from this world. 
     * 
     * @param id the id of the bean
     */
    virtual void removeBean(oidType id) override;

    /**
     * Get total number of beans.
     * 
     * @return the total number of beans
     */
    virtual int getNumOfBeans();

    /**
     * Get all beans.
     * 
     * @return a map containing all beans
     */
    virtual const std::unordered_map<oidType, Bean*>& getBeans();


protected:
    int reloadProperties();
    Property* definePropertyCommon_(const char* name, 
        Property::Type type, 
        Property::ValueType valueType);
    Bean* getBean();

protected:
    std::unordered_map<oidType, Bean*> m_beans_;

     //map from property name to property
    std::unordered_map<std::string, Property*> m_propertyMap_; 
    bool m_properties_loaded_ = false;

    AbstractBeanDB *m_db;

friend class Bean;
friend class Property;
friend class AbstractBeanDB;
};

}
}
}