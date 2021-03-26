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
    AbstractBeanDB(BeanWorld& world) : m_world_(&world) {};

    /**
     * Destructor
     */
    virtual ~AbstractBeanDB() {};

    /**
     * Get the attached world.
     * 
     * @return the world
     */
    BeanWorld& getWorld() {return *m_world_;};

    /**
     * Load all data, including all beans, properties, from the storage 
     * into the world.
     * 
     * Notes:
     * - This method shall apply only to small scaled data storage.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadAll() = 0;

    /**
     * Save all data, including all beans, properties in the world, 
     * to the persistent storage.
     * 
     * @return 0 for success, or an error code
     */
    virtual int saveAll() = 0;

    /**
     * Load a single bean, including all its properties, from the storage 
     * into the world.
     * 
     * @param id the id of the bean
     * @return the pointer pointing to the bean, or null if no such
     *                   bean exist in the storage
     */
    virtual Bean* loadBean(oidType id) = 0;

    /**
     * Save a single bean into the storage.
     * 
     * @param bean the bean to be saved
     * @return 0 for success, or an error code
     */
    virtual int saveBean(Bean* bean) = 0;

    /**
     * Remove a single bean from the storage.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int removeBean(Bean* bean) = 0;

    /**
     * Load all properties from the storage into the world.
     * 
     * Notes:
     * - This method must be implemented for properties
     *    are supposed to be loaded all to world (memory) 
     *    before it can be used.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadProperties() = 0;

    /**
     * Save a property into the storage.
     * 
     * @param property the property
     * @return 0 for success, or an error code
     */
    virtual int saveProperty(Property* property) = 0;

    /**
     * Remove a property from the storage.
     * 
     * @param property the property
     * @return 0 for success, or an error code
     */
    virtual int removeProperty(Property* property) = 0;

private:
    BeanWorld *m_world_;
};

}
}
}