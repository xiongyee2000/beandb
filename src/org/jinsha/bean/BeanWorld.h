#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"
#include "./AbstractBeanDB.h"
#include "./Page.hpp"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class Property;
class AbstractBeanDB;

class BeanWorld
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
     * Define a property.
     * 
     * Property must be defined before it can be used.
     * 
     * Notes:
     * - Property is type sensitive: i.e. setting the property with a value of type other
     *    than the one defined here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of property
     * @return the pointer to the property instance
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType);

    /**
     * Define an array property.
     * 
     * Array property must be defined before it can be used.
     * 
     * Notes:
     * - Array property is type sensitive: i.e. adding to the array property 
     *   with a value of type other than the one defined here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of the element of the array property
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType);

    /**
     * Define a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @return the pointer to the property instance
     */
    virtual Property* defineRelation(const char* name);

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayRelation(const char* name);

    /**
     * Undefine a property.
     * 
     * Notes:
     * - This method can be used to undefine either a property, an array property,
     *    a relation, or an array relation;
     * - CAUTION: All beans that have this property will remove the property with this id.
     * 
     * @param property the property to be undefined
     * @return 0 on success, or an error code
     */
    virtual int undefineProperty(Property* property);

    /**
     * Undefine a relation.
     * 
     * Notes:
     * - Same as undefineProperty() (just an alias).
     * 
     * @param relation the relation to be undefined
     * @return 0 on success, or an error code
     */
    virtual int undefineRelation(Property* relation) {return undefineProperty(relation);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return property
     */
    // virtual const Property* getProperty(const char* name) const;
    virtual Property* getProperty(const char* name);

    /**
     * Get property/relation/array property/array relation by id.
     * 
     * @param id property id
     * @return property
     */
    virtual Property* getProperty(pidType id);

    /**
     * Get relation.
     * 
     * Notes:
     * - Same as getProperty() (just an alias).
     * 
     * @param name the name of relation
     * @return 0 on success, or an error code
     */
    virtual Property* getRelation(const char* name) {return getProperty(name);};

    /**
     * Get all properties (including all properties/relations).
     * 
     * @return a map containing all properties.
     */
    virtual const std::unordered_map<std::string, Property*>& getProperties();


    /***********************************************************
     * bean related
     ***********************************************************/
public:
    /**
     * Creat an empty bean.
     * 
     * @return the pointer pointing to the bean, or nullptr if exception occurs.
     */
    virtual Bean* createBean();

    /**
     * Get bean by id.
     * 
     * @param id the id of the bean
     * @param loadFromDB if true, try to load the bean from db
     *                    when the bean has not been loaded into this world.
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist.
     */
    virtual Bean* getBean(oidType id, bool loadFromDB = true);

    /**
     * Unload a bean from this world. 
     * 
     * Notes:
     * - the memory allocated to bean will be deleted 
     *    so that it shall not be used any longer;
     * - however the bean will NOT be deleted from the 
     *    database;
     * 
     * @param bean the bean to be uloaded
     */
    virtual void unloadBean(Bean* bean);

    /**
     * Remove a single bean from the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int deleteBean(Bean* bean);

    /**
     * Reload all data from the storage.
     * 
     * Notes:
     * - all previously loaded data will be removed from this world
     *    before reloading;
     * 
     * @return 0 for success, or an error code
     */
    virtual int reloadAll();

    /**
     * Save all data into the storage.
     * 
     * @return 0 for success, or an error code
     */
    virtual int saveAll();

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
     * @ref BeanDBPIntf::getAllBeans()
     */
    virtual BeanIdPage* getAllBeans(unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf::findBeans()
     */
    virtual BeanIdPage* findEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf::findBeans()
     */
    virtual BeanIdPage* findLessEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf::findBeans()
     */
    virtual BeanIdPage* findLessThan(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf::findBeans()
     */
    virtual BeanIdPage* findGreaterEqual(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf::findBeans()
     */
    virtual BeanIdPage* findGreaterThan(const Property* property, const Json::Value& value, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf
     */
    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;

    /**
     * @ref BeanDBPIntf
     */
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = BeanDBIntf::DEFAULT_PAGE_SIZE) const;


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