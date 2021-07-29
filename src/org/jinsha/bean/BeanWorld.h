#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"
#include "./AbstractBeanDB.h"

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
     * Constructor
     * 
     * @param db the database attached with this world
     * 
     * Notes:
     * Parameter db can be NULL, in which case no data persistency 
     * will be available.
     *  
     */
    BeanWorld(AbstractBeanDB *db = nullptr);

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

    /**
     *  Create a bean.
     * 
     * @return the created bean
     */
    virtual Bean *createBean(oidType id = 0);

    /**
     * Remove a bean from this world. 
     * 
     * @param id the id of the bean
     */
    virtual void removeBean(oidType id);

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

    /**
     * Get bean by id.
     * 
     * @param id the bean id
     * @return the bean, or nullptr
     */
    virtual Bean* getBean(oidType id);

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
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false);

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
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false);

    /**
     * Define a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineRelation(const char* name, bool needIndex = false);

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false);

    /**
     * Undefine a property.
     * 
     * Notes:
     * - This method can be used to undefine either a property, an array property,
     *    a relation, or an array relation;
     * - All beans that have this property will remove the property with this id.
     * 
     * @param name the name of property
     * @return 
     */
    virtual void undefineProperty(const char* name);
    virtual void undefineRelation(const char* name) {undefineProperty(name);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return property
     */
    virtual const Property* getProperty(const char* name) const;
    virtual Property* getProperty(const char* name);

    /**
     * Get all properties.
     * 
     * @return a map containing all properties.
     */
    virtual const std::unordered_map<std::string, Property*>& getProperties() const 
     {return m_propertyMap_;};

public:
    /**
     * See AbstractBeanDB::loadAll().
     */
    int loadAll();

    /**
     * Remove all beans/properties from this world.
     */
    int unloadAll();

    /**
     * See AbstractBeanDB::loadProperties().
     */
    int loadProperties();

    /**
     * See AbstractBeanDB::loadBean().
     */
    Bean* loadBean(oidType id);

    /**
     * Remove all beans from this world.
     */
    int unloadBean(oidType id);

protected:
    Property* definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex = false);

    oidType generateBeanId();

protected:
    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 1;

    //map from property name to property
    std::unordered_map<std::string, Property*> m_propertyMap_; 

    AbstractBeanDB *m_db;

friend class Bean;
friend class Property;
};

}
}
}