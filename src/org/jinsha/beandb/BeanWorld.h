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
namespace beandb {

class Bean;
class Property;
class AbstractBeanDB;

/**
 * Bean world can be considered as a container of those beans
 * loaded into the memory. i.e. the container of cached beans.
 * 
 * A bean world can be retrieved by AstractBeanDB::getWorld().
 * which will work as a singleton. 
 * 
 * When AbstractDB::connect() is called, all properties will be loaded
 * automatically into the singleton world, for later operations;
 * and when AbstractDB::disconnect() is called, all previously
 * loaded properties will be unloaded (removed) from the bean world.
 */
class BeanWorld
{
public:
    /**
     * Load all data from data storage into this world (in memory).
     * 
     * This method is supposed to be used in all-load mode.
     * 
     * All-load mode means load all data from database into
     * the bean world, which is aimed to achieve the best time performance.
     * 
     * Notes:
     * - All previously loaded bean will be removed from this world
     *    before loading;
     * - All properties will be reloaded;
     * - All beans will be loaded;
     * - Any unsaved changes will be lost;
     * - Don't use all-load mode if memory is a concern;
     * 
     * @return 0 for success, or an error code
     */
    int loadAll();

    /**
     * Unload all data from this world.
     * 
     * This method is supposed to be used when user choose to
     * switch from all-load mode to partial-load mode.
     * 
     * Partial-load mode means only part of the data will be loaded 
     * from data storage into the bean world, when needed (see getBean()).
     * 
     * The partial-load mode is used when memory is not big
     * enough to hold all data.
     * 
     * Notes:
     * - All previously loaded beans will be removed;
     * - All properties will be reloaded;
     * - Any unsaved changes will be lost;
     * - No data will be deleted from the database;
     * 
     */
    void unloadAll();


public:
    /***********************************************************
     * property related
     ***********************************************************/
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
    Property* defineProperty(const char* name, Property::ValueType valueType);

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
    Property* defineArrayProperty(const char* name, Property::ValueType valueType);

    /**
     * Define a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @return the pointer to the property instance
     */
    Property* defineRelation(const char* name);

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @return the pointer to the property instance
     */
    Property* defineArrayRelation(const char* name);

    /**
     * Undefine a property.
     * 
     * CAUTION: 
     * The property value will also be removed from all beans 
     *  that have this property.
     * 
     * Notes:
     * - Use this method  to undefine either a property, an array property,
     *    a relation, or an array relation;
     * 
     * @param property the property to be undefined
     * @return 0 on success, or an error code
     */
    int undefineProperty(Property* property);

    /**
     * Undefine a relation.
     * 
     * Notes:
     * - Same as undefineProperty() (just an alias).
     * 
     * @param relation the relation to be undefined
     * @return 0 on success, or an error code
     */
    int undefineRelation(Property* relation) {return undefineProperty(relation);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return the pointer to the property, or null if no such property exists.
     */
    // const Property* getProperty(const char* name) const;
    Property* getProperty(const char* name) const;

    /**
     * Get property/relation/array property/array relation by id.
     * 
     * @param id property id
     * @return property
     */
    Property* getProperty(pidType id) const;

    /**
     * Get relation.
     * 
     * Notes:
     * - Same as getProperty() (just an alias).
     * 
     * @param name the name of relation
     * @return 0 on success, or an error code
     */
    Property* getRelation(const char* name) const {return getProperty(name);};

    /**
     * Get all properties (including all properties/relations).
     * 
     * @return a map containing all properties.
     */
    const std::unordered_map<std::string, Property*>& getProperties() const;

    /***********************************************************
     * bean related
     ***********************************************************/
public:
    /**
     * Creat a new bean.
     * 
     * Notes:
     * - A new bean instance will be created in the storage and loaded into
     *    this world;
     * - The new bean will have no property;
     * - The id of the bean will be the id generated in the storage;
     * 
     * @return the pointer pointing to the bean, or nullptr if exception occurs.
     */
    Bean* newBean();

    /**
     * Get bean by id.
     * 
     * Notes:
     * - Once loaded, the bean will kept in the world unless unloadBean() is called,
     *   which will remove the bean from this world and release the memory. So do 
     *   unloadBean() after you finish the job if you don't expect the bean kept in
     *   the world.
     * 
     * @param id the id of the bean
     * @param loadFromDB if true, try to load the bean from the storage
     *                    when it has not been loaded into this world.
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist.
     */
    Bean* getBean(oidType id, bool loadFromDB = true);

    /**
     * Unload a bean from this world. 
     * 
     * Notes:
     * - the memory allocated to this bean will be released;
     * - however the bean will NOT be deleted from the 
     *    storage;
     * 
     * @param bean the bean to be unloaded
     */
    void unloadBean(Bean* bean);

    /**
     * Unload all beans from this world.
     */
    void unloadAllBeans();

    /**
     * Delete a bean from the storage.
     * 
     * Notes:
     * - The bean will also be unloaded from this world;
     * 
     * @param bean the bean to be deleted
     * @return 0 for success, or an error code
     */
    int deleteBean(Bean* bean);

    /**
     * Save all changes into the storage.
     * 
     * Notes:
     * - Only changed data will be saved into storage;
     * - You can make some changes, and then use this method to 
     *    save them so as to achieve better performance.
     * 
     * @return 0 for success, or an error code
     */
    int saveAll();

    /**
     * Get total number of beans chached in this world (in memory).
     * 
     * @return the total number of beans
     */
    int getCachedNumOfBeans();

    /**
     * Get all beans chached in this world (in memory).
     * 
     * @return a map containing all beans
     */
    const std::unordered_map<oidType, Bean*>& getCachedBeans();


    /***********************************************************
     * search related
     ***********************************************************/
    /**
     * Load and return all beans from the database.
     */
    BeanIdPage* getAllBeans(unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * General Information of find method series:
     * 
     * These methods are used to find beans the property values of which 
     * meet the specified condition.
     * 
     * The condition could be represented as:
     * bean->getProperty(property) <optype> value
     * 
     * Where optype could be:
     * equal/not equal/lessThan/lessEqual/greater/greaterEqual/like/not like
     * 
     * Notes:
     * - when the given property is a relation, the value is interpreted as 
     *     the object bean id;
     * - argument value shall not be array or object;
     * - the search is type restricted, i.e. only those beans with the 
     *      property value having the same type will be accepted;
     * - user should delete the returned page after using it;
     * 
     * Notes:
     * - when the argument property is a relation, the value must represent 
     *   an object bean id;
     */
    BeanIdPage* findEqual(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     */
    BeanIdPage* findNotEqual(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     */
    BeanIdPage* findLessEqual(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     */
    BeanIdPage* findLessThan(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     */
    BeanIdPage* findGreaterEqual(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     */
    BeanIdPage* findGreaterThan(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     * - only applicable to string value type
     */
    BeanIdPage* findLike(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * @ref refer to findEqual()
     * 
     * Notes:
     * - not applicable to relations
     * - only applicable to string value type
     */
    BeanIdPage* findNotLike(const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * Find subject beans of a given relation property.
     * 
     * Notes:
     * - only applies to relation properties;
     * - user should delete the returned page after using it;
     * 
     * @param property the property
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     * 
     */
    BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;

    /**
     * Find object beans of a given relation property.
     * 
     * Notes:
     * - only applies to relation properties;
     * - user should delete the returned page after using it;
     * 
     * @param property the property
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     */
    BeanIdPage* findObjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;


private:
    BeanWorld(AbstractBeanDB& db);
    virtual ~BeanWorld();

    int doUnloadAll(bool reloadProperties);
    int loadProperties();
    void unloadProperties();
    Property* definePropertyCommon_(const char* name, 
        Property::Type type, 
        Property::ValueType valueType);
   BeanIdPage* findBeans(opType optype, 
        const Property* property, 
        const Json::Value& value, 
        unsigned int pageSize) const;

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