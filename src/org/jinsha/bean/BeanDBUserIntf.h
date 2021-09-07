#pragma once

#include <string>
#include <unordered_map>

#include "./Property.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanDBUserIntf
{
public:
    /**
     * Constructor
     */
    BeanDBUserIntf() {};
    
    /**
     * Destructor
     */
    virtual  ~BeanDBUserIntf() {};

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
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

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
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

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
    virtual Property* defineRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Undefine a property.
     * 
     * Notes:
     * - This method can be used to undefine either a property, an array property,
     *    a relation, or an array relation;
     * - CAUTION: All beans that have this property will remove the property with this id.
     * 
     * @param name the name of property
     * @return 0 on success, or an error code
     */
     virtual int undefineProperty(const char* name) = 0;

    /**
     * Undefine a relation.
     * 
     * Notes:
     * - Same as undefineProperty() (just an alias).
     * 
     * @param name the name of relation
     * @return 0 on success, or an error code
     */
    virtual int undefineRelation(const char* name) {return undefineProperty(name);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return property
     */
    // virtual const Property* getProperty(const char* name) const;
    virtual Property* getProperty(const char* name) = 0;

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
    virtual const std::unordered_map<std::string, Property*>& getProperties() = 0;


    /***********************************************************
     * bean related
     ***********************************************************/
public:
    /**
     * Creat an empty bean.
     * 
     * @return the pointer pointing to the bean, or nullptr if exception occurs.
     */
    virtual Bean* createBean() = 0;

    /**
     * Get bean by id.
     * 
     * The method will firstly try to get the bean by calling BeanWorld::getBean(),
     * if it fails, it will then try to call loadBean().
     * 
     * @param id the id of the bean
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist.
     */
    virtual Bean* getBean(oidType id) = 0;

    /**
     * Remove a bean from this world. 
     * 
     * Notes:
     * - The bean will NOT be deleted from the database.
     * 
     * @param id the id of the bean
     */
    virtual void removeBean(oidType id) = 0;

    /**
     * Remove a single bean from the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int deleteBean(Bean* bean) = 0;

    /**
     * Load all data from the storage.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadAll() = 0;

    /**
     * Save all data into the storage.
     * 
     * @return 0 for success, or an error code
     */
    virtual int saveAll() = 0;
};

}
}
}