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
    static const unsigned int DEFAULT_PAGE_SIZE = 8u;

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
     * Find beans the property values of which meet the condition 
     * given by optype, property and value.
     * 
     * The condition could be understood as:
     * bean->getProperty(<property>) <optype> <value>
     * 
     * Notes:
     * - The property and the value shall be of consistent type;
     * - This method could be used to get relation subjects, 
     *   in which case optype, property, value  shall be op_eq, 
     *   a relation property, and object bean id respectively.
     * 
     * @param optype operation type  of the search criteria
     * @param property the property
     * @param value the value
     * @param pageSize the page size of the returned page
     */
    BeanIdPage* findBeans_(oidType optype, Property* property, Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE);

    // BeanIdPage* findBeans_(oidType optype, Property* property, Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE);

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