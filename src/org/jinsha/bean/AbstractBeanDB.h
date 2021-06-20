#pragma once

#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

class AbstractBeanDB
{
public:
    /**
     * Constructor
     * 
     * @param world the world attached to this storage
     */
    AbstractBeanDB() : m_world(nullptr) {};

    /**
     * Destructor
     */
    virtual ~AbstractBeanDB() {};

    /**
     * Get the attached world.
     * 
     * @return the world
     */
    virtual BeanWorld* getWorld() {return m_world;};

    /**
     * Connect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int connect() = 0;

    /**
     * Disonnect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int disconnect() = 0;

    /**
     * Clear the database, i.e. remove all data.
     * 
     * @return 0 for success, or an error code
     */
    virtual int clear() = 0;


    /**
     * Create a property.
     * 
     * Property must be created before it can be used.
     * 
     * Notes:
     * - Property is type sensitive: i.e. setting the property with a value of type other
     *    than the one created here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* createProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

    /**
     * Create an array property.
     * 
     * Array property must be created before it can be used.
     * 
     * Notes:
     * - Array property is type sensitive: i.e. adding to the array property 
     *   with a value of type other than the one created here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of the element of the array property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* createArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

    /**
     * Create a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* createRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Create an array relation property.
     * 
     * Array relation property must be created before it can be used.
     * 
     * @param name the name of array relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* createArrayRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Delete a property.
     * 
     * Notes:
     * - This method can be used to delete either a property, an array property,
     *    a relation, or an array relation;
     * - All beans that have this property will remove the property with this id.
     * 
     * @param name the name of property
     * @return 0 on success, or an error code
     */
    virtual int deleteProperty(const char* name) = 0;
    virtual int deleteRelation(const char* name) {return deleteProperty(name);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return property
     */
    virtual const Property* getProperty(const char* name) const = 0;
    virtual Property* getProperty(const char* name) = 0;

protected:
    /**
     * This method will be called in BeanWorld::BeanWold().
     * You can customize your initialization here.
     * 
     * @return void
     */
    virtual void init() {};

    /**
     * Load all data, including all beans, properties, from the storage 
     * into the world.
     * 
     * Notes:
     * - This method shall apply only to small scaled data storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadAll() = 0;


    /**
     * Save all data, including all beans, properties in the world, 
     * to the persistent storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @return 0 for success, or an error code
     */
    virtual int saveAll() = 0;

    /**
     * Load a single bean, including all its properties, from the storage 
     * into the world.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param id the id of the bean
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist in the storage
     */
    virtual Bean* loadBean(oidType id) = 0;

    /**
     * Save a single bean into the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be saved
     * @return 0 for success, or an error code
     */
    virtual int saveBean(Bean* bean) = 0;

    /**
     * Remove a single bean from the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int removeBean(Bean* bean) = 0;

    /**
     * Load all properties from the storage into the world.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * Notes:
     * -  In out design, all properties must be loaded into world (memory) 
     *    before the world is useful.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadProperties() = 0;

private:
    BeanWorld *m_world;

friend class BeanWorld;
};

}
}
}