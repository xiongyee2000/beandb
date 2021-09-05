#pragma once

#include <string>
#include <unordered_map>

#include "./Property.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanDBPersistenceIntf
{
public:
    /**
     * Constructor
     */
    BeanDBPersistenceIntf() {};
    
    /**
     * Destructor
     */
    virtual ~BeanDBPersistenceIntf() {};

    /**
     * Re-initialize the database.
     * 
     * CAUTION: all data will be removed
     * 
     * @return 0 for success, or an error code
     */
    virtual int reInit_() = 0;

    /***********************************************************
     * connection related
     ***********************************************************/
    
    /**
     * Connect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int connect_() = 0;

    /**
     * Disonnect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int disconnect_() = 0;

    /***********************************************************
     * transaction related
     ***********************************************************/
    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int beginTransaction_() = 0;

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int commitTransaction_() = 0;

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int rollbackTransaction_() = 0;

    /***********************************************************
     * property related
     ***********************************************************/
public:

    /**
     * Define a property.
     * 
     * Notes:
     * - Property must be defined before it can be used.
     * - The argument valueType is ignored when it is a relation property.
     * 
     * @param name the name of property
     * @param type the type of property
     * @param valueType the value type of property
     * @param isArray if it is an array property
     * @param needIndex if index is needed
     * @return the id of the property if successful (non-negative), 
     *                   or a  negative number as error code
     */
    virtual pidType defineProperty_(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        bool needIndex = false) = 0;

    /**
     * Undefine a property.
     * 
     * Notes:
     * - CAUTION: the property value will also be removed from all beans 
     *   that have this property.
     * 
     * @param name the name of property
     * @return 0 on success, or an error code
     */
    virtual int undefineProperty_(const char* name) = 0;

    /**
     * Load all defined properties from storage to memory.
     * 
     * Notes:
     * -  In out design, all properties will be loaded into memory
     *    before  any other operation. This is purposed to achieve
     *    the best performance.
     * - The implementation of this method must new Property 
     *    instances. 
     * -  The caller method shall make sure properties is empty
     *    before this method is called.
     * - When error occurs, the mthod won't do cleaning work
     *    on properties. It is the caller's responsibility to do it.
     * 
     * @param properties the loaded properties
     * @return 0 for success, or an error code
     */
    virtual int loadProperties_(std::unordered_map<std::string, Property*>& properties) const = 0;


    /***********************************************************
     * bean related
     ***********************************************************/

    /**
     * Create a new bean instance. 
     * 
     * @param the id of the newly created bean
     * @return 0 for success, or an error code
     */
    virtual int createBean_(oidType& id) = 0;

    /**
     * Remove a single bean from the storage.
     * 
     * @param bean the bean to be removed
     * @return 0 for success, or an error code
     */
    virtual int deleteBean_(Bean* bean) = 0;

    /**
     * Load bean properties. 
     * 
     * @param id the id of the bean
     * @return A list containing bean's property names
     */
    virtual std::list<std::string> getBeanProperties_(oidType id) const = 0;

    /**
     * Load bean property. This method is used in case "delay-load"
     * is needed.
     * 
     * @param beanId the bean id
     * @param property the property to be loaded
     * @param value the value loaded to
     * @return 0 on success, or an error code
     */
    virtual int loadBeanProperty_(oidType beanId,  const Property* property, Json::Value& value) = 0;

    /**
     * Load bean's native data. 
     * 
     * @param beanId the bean id
     * @param data the data loaded to
     * @return 0 on success, or an error code
     */
    virtual int loadBeanNativeData_(oidType beanId, Json::Value& data) = 0;

    /**
     * Load bean data from database into memory.
     * 
     * All bean's direct-load properies must be loaded in this method, 
     * while delay-load properties shall be set to null in the value.
     * 
     * Note the bean's native data is regarded as delay-load, 
     * but can also be loaded into nativeData.
     * 
     * @param id id of the bean to be leaded
     * @param value the value that holds all bean's properties
     * @param nativeData the value that holds the bean's nativeData
     * @return 0 on success, or an error code
     */
    virtual int loadBeanBase_(oidType id, Json::Value& value, Json::Value& nativeData) = 0;

    /**
     * Save bean data into database.
     * 
     * All bean's direct-load properies must be saved in this method, 
     * while delay-load properties may or may not be saved in this method.
     * 
     * Note the bean's native data is regarded as delay-load, 
     * but can also be saved also.
     * 
     * @param id id of the bean to be leaded
     * @param data the data that holds all bean's property values
     * @param nativeData the value that holds the bean's nativeData
     * @return 0 on success, or an error code
     */
    virtual int saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value& nativeData) = 0;

    virtual int insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) = 0;

    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) = 0;

    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) = 0;

    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property) = 0;

    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) = 0;

    virtual int insertBeanNativeData_(oidType beanId, 
        const Json::Value& value) = 0;

    virtual int updateBeanNativeData_(oidType beanId, 
        const Json::Value& value) = 0;

    virtual int deleteBeanNativeData_(oidType beanId, 
        const Json::Value& value) = 0;

};

}
}
}