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
     * @return the pointer to the property instance
     */
    virtual Property* defineProperty(const char* name, Property::ValueType valueType) = 0;

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
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType) = 0;

    /**
     * Define a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @return the pointer to the property instance
     */
    virtual Property* defineRelation(const char* name) = 0;

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayRelation(const char* name) = 0;

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
    virtual int undefineProperty(Property* property) = 0;

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
    virtual Property* getProperty(const char* name) = 0;

    /**
     * Get property/relation/array property/array relation by id.
     * 
     * @param id property id
     * @return property
     */
    virtual Property* getProperty(pidType id) = 0;

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

    // /**
    //  * Get all relations.
    //  * 
    //  * @return a map containing all properties.
    //  */
    // virtual const std::unordered_map<std::string, Property*>& getRelations() = 0;

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
     * @param id the id of the bean
     * @param loadFromDB if true, try to load the bean from db
     *                    when the bean has not been loaded into this world.
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist.
     */
    virtual Bean* getBean(oidType id, bool loadFromDB = true) = 0;

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
    virtual void unloadBean(Bean* bean) = 0;

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
     * Reload all data from the storage.
     * 
     * Notes:
     * - all previously loaded data will be removed from this world
     *    before reloading;
     * 
     * @return 0 for success, or an error code
     */
    virtual int reloadAll() = 0;

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