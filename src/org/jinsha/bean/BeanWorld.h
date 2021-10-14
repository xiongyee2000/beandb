#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"
#include "./BeanDBUserIntf.h"
#include "./AbstractBeanDB.h"
#include "./Page.hpp"

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
     * Remove all beans from this world.
     * 
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
     virtual int undefineProperty(Property* property) override;

    /**
     * @ref BeanDBUserIntf::getProperty(const char* name)
     */
    virtual Property* getProperty(const char* name) override;

    /**
     * @ref BeanDBUserIntf::getProperty(pidType pid)
     */
    virtual Property* getProperty(pidType id) override;

    /**
     * @ref BeanDBUserIntf::getProperties()
     */
    virtual const std::unordered_map<std::string, Property*>& getProperties() override;

    // /**
    //  * @ref BeanDBUserIntf::getRelations()
    //  */
    // virtual const std::unordered_map<std::string, Property*>& getRelations() override;


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
     * Get total number of beans chached in this world (in memory).
     * 
     * @return the total number of beans
     */
    virtual int getCachedNumOfBeans();

    /**
     * Get all beans chached in this world (in memory).
     * 
     * @return a map containing all beans
     */
    virtual const std::unordered_map<oidType, Bean*>& getCachedBeans();


    /***********************************************************
     * search related
     ***********************************************************/
    /**
     * @ref BeanDBPersistenceIntf::findBeans()
     */
    virtual BeanIdPage* findEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf::findBeans()
     */
    virtual BeanIdPage* findLessEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf::findBeans()
     */
    virtual BeanIdPage* findLessThan(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf::findBeans()
     */
    virtual BeanIdPage* findGreaterEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf::findBeans()
     */
    virtual BeanIdPage* findGreaterThan(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = BeanDBPersistenceIntf::DEFAULT_PAGE_SIZE) const;

private:
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

    int reloadProperties();
    Property* definePropertyCommon_(const char* name, 
        Property::Type type, 
        Property::ValueType valueType);
   BeanIdPage* findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize) const;

private:
    std::unordered_map<oidType, Bean*> m_beans_;

     //map from property name to property
    std::unordered_map<std::string, Property*> m_propertyMap_; 
    bool m_properties_loaded_ = false;

    AbstractBeanDB *m_db_;

friend class Bean;
friend class Property;
friend class AbstractBeanDB;
};

}
}
}