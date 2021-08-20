#pragma once

#include "./BeanWorld.h"
#include <unordered_map>

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

    /***********************************************************
     * Connection related
     ***********************************************************/
public:
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
     * Is the database connected.
     * 
     * @return true if connected, or false otherwise 
     */
    virtual bool connected() const = 0;

    /**
     * Clear the database, i.e. remove all data.
     * 
     * CAUTIOUS: the operation is not revertable.
     * 
     * @return 0 for success, or an error code
     */
    virtual int clear() = 0;

    /***********************************************************
     * property related
     ***********************************************************/
public:
    /**
     * Define a property.
     * 
     * Notes:
     * - Property must be defined before it can be used.
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
     * Notes:
     * - Array property must be defined before it can be used.
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
     * Notes:
     * - Relation is a special kind of property, which represents the relation between
     * two beans, e.g. friend/colleague etc.
     * 
     * @param name the name of relation property
     * @param needIndex if index is needed
     * @return the pointer to the property instance
     */
    virtual Property* defineRelation(const char* name, bool needIndex = false) = 0;

    /**
     * Define an array relation property.
     * 
     * Notes:
     * - Array relation property must be define before it can be used.
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
     * - This method can be used to delete either a property, an array property;
     * - CAUTOUS: the properties will also be removed from all beans that have this property.
     * 
     * @param name the name of property
     * @return 0 on success, or an error code
     */
    virtual int undefineProperty(const char* name) = 0;

    /**
     * Undefine a relation.
     * 
     * Notes:
     * - This method can be used to delete either a relation, or an array relation;
     * - CAUTOUS: the relations will also be removed from all beans that have this relation.
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
     Property* getProperty(const char* name);

    /**
     * Get all properties.
     * 
     * @return a map containing all properties.
     */
    const std::unordered_map<std::string, Property*>& getProperties() const 
     {return m_propertyMap_;};

    /***********************************************************
     * transaction related
     ***********************************************************/
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
    virtual Bean* getBean(oidType id);

    // /**
    //  * Save a single bean into the storage.
    //  * 
    //  * Notes:
    //  * - This method will 
    //  * 
    //  * @param bean the bean to be saved
    //  * @return 0 for success, or an error code
    //  */
    // virtual int saveBean(Bean* bean);

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
     * Load all defined properties from storage to memory.
     * 
     * Notes:
     * -  In out design, all properties will be loaded into memory
     *    before  any other operation. This is purposed to achieve
     *    the best performance.
     * 
     * @param names the property names
     * @param type the property types
     * @param valueType the property valueTypes
     * @param indices the property index flags
     * @return 0 for success, or an error code
     */
    virtual int loadProperties(std::vector<std::string>& names, 
        std::vector<Property::Type>& types, 
        std::vector<Property::ValueType>& valueTypes,
        std::vector<bool>& indices) const = 0;

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

    virtual int loadUnmanagedValue(oidType beanId, Json::Value& value) = 0;

    /**
     * Load bean data from database into memory.
     * 
     * All bean's direct-load properies must be loaded in this method, 
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
    virtual int loadBeanBase(oidType id, Json::Value& value, Json::Value& unmanagedValue) = 0;

    /**
     * Save bean data into database.
     * 
     * All bean's direct-load properies must be saved in this method, 
     * while delay-load properties may or may not be saved in this method.
     * 
     * Note the bean's unmanaged value is regarded as delay-load, 
     * but can also be saved also.
     * 
     * @param id id of the bean to be leaded
     * @param value the value that holds all bean's properties
     * @param unmanagedValue the value that holds the bean's unmanagedValue
     * @return 0 on success, or an error code
     */
    virtual int saveBeanBase(oidType beanId, const Json::Value& managedValue, const Json::Value& unmanagedValue) = 0;

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


    /**
     * Get the attached world.
     * 
     * @return the world
     */
    virtual BeanWorld* getWorld() const {return m_world;};

protected:
    BeanWorld *m_world;
    bool m_inTransaction = false;
    bool m_propertyLoaded_ = false;
    std::unordered_map<std::string, Property*> m_propertyMap_;

friend class BeanWorld;
friend class Bean;
friend class Property;
};

}
}
}