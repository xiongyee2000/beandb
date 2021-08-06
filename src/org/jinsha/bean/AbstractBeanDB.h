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
    AbstractBeanDB();

    /**
     * Destructor
     */
    virtual ~AbstractBeanDB();

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

    virtual bool connected() const = 0;

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
    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

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
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) = 0;

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
    virtual Property* defineRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Create an array relation property.
     * 
     * Array relation property must be created before it can be used.
     * 
     * @param name the name of array relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) = 0;

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
    virtual int undefineProperty(const char* name) = 0;
    virtual int undefineRelation(const char* name) {return undefineProperty(name);};

    /**
     * Get property/relation/array property/array relation by name.
     * 
     * @param name property name
     * @return property
     */
    virtual const Property* getProperty(const char* name) const = 0;
    virtual Property* getProperty(const char* name) = 0;

    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int beginTransaction();

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int commitTransaction();

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int rollbackTransaction();

    /**
     * Check if it is in a transaction
     * 
     * @return true if in a transaction, or false if not
     */
    virtual bool inTransaction() {return m_inTransaction;};


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
    virtual Bean* getBean(oidType id);

    /**
     * Save a single bean into the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be saved
     * @return 0 for success, or an error code
     */
    virtual int saveBean(Bean* bean);

    /**
     * Remove a single bean from the storage.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int deleteBean(Bean* bean) = 0;


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
     * Load bean properties. 
     * 
     * @param id the id of the bean
     * @return A list containing bean's property names
     */
    virtual std::list<std::string> getBeanProperties(oidType id) const = 0;

    /**
     * Load bean property. This method is used in case "delay-load"
     * is needed.
     * 
     * @param bean the bean
     * @param property the property to be loaded
     * @param value the value loaded to
     * @return 0 on success, or an error code
     */
    virtual int loadBeanProperty(oidType beanId,  const Property* property, Json::Value& value) = 0;

    virtual int loadUnmanagedValues(const Bean* bean, Json::Value& value) = 0;

    /**
     * Load all properties from the storage into the world.
     * 
     * This method is supposed to be called from class BeanWorld.
     * 
     * Notes:
     * -  In out design, all properties must be loaded into world (memory) 
     *    before the world is usable.
     * 
     * @return 0 for success, or an error code
     */
    virtual int loadProperties() = 0;

    /**
     * Load bean data into memory.
     * 
     * All bean's direct-load properies must be loaded into the value, 
     * while delay-load properties shall be set to null in the value.
     * 
     * Note the bean's unmanaged value is regarded as delay-load, 
     * but can also be loaded into unmanagedValue.
     * 
     * @param id id of the bean to be leaded
     * @param value the value that holds all bean's properties
     * @param unmanagedValue the value that holds the bean's unmanagedValue
     * @return 0 on success, or an error code
     */
    virtual int loadBean(oidType id, Json::Value& value, Json::Value& unmanagedValue) = 0;

    virtual int saveBeanBase(const Bean* bean) = 0;

    virtual int insertBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) = 0;

    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) = 0;

    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) = 0;

    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property) = 0;

    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) = 0;

    virtual int insertBeanUnmanagedValue(oidType beanId, 
        const Json::Value& value) = 0;

    virtual int updateUnmanagedValue(oidType beanId, 
        const Json::Value& value) = 0;

    virtual int deleteBeanUnmanagedValue(oidType beanId, 
        const Json::Value& value) = 0;


    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int doBeginTransaction() = 0;

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int doCommitTransaction() = 0;

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int doRollbackTransaction() = 0;

protected:
    BeanWorld *m_world;
    bool m_inTransaction = false;

friend class BeanWorld;
friend class Bean;
friend class Property;
};

}
}
}