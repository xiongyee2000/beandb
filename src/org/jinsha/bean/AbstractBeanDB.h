#pragma once

#include "./BeanWorld.h"
#include "./Page.hpp"
namespace org {
namespace jinsha {
namespace bean {

class BeanWorld;

class AbstractBeanDB
{
public:
    /**
     * Constructor
     */
    AbstractBeanDB();

    /**
     * Destructor
     */
    virtual ~AbstractBeanDB();

    /**
     * Connect to the database. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int connect();

    /**
     * Disonnect to the database. 
     * 
     * CAUTION: 
     * Once disconnected, all beans and properties
     * previously loaded will be unloaded from bean world.
     * 
     * @return 0 for success, or an error code
     */
    virtual int disconnect();

    /**
     * Get the attached world.
     * 
     * Notes:
     * - The world instance will be newed in the constructor, 
     *   and will be deleted in the destructor.
     * - The world will work as a singleton, so do NOT delete it.
     * 
     * @return the world
     */
    virtual BeanWorld* getWorld() const {return m_world_;};

    /**
     * Is the database connected.
     * 
     * @return true if connected, or false otherwise 
     */
    virtual bool connected() const;


    ////////////////////////////////////////////////////////////////////////
    //begin of pure virtual methods
     ////////////////////////////////////////////////////////////////////////

    /***********************************************************
     * connection related
     ***********************************************************/
public:
    /**
     * Clear all data in the data storage.
     * 
     * CAUTION: 
     * All data in the storage will be unrecoverably deleted!
     * 
     * @return 0 for success, or an error code
     */
    virtual int clear() = 0;

private:
    /**
     * Connect to the data storage. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int doConnect() = 0;

    /**
     * Disonnect to the data storage. 
     * 
     * @return 0 for success, or an error code
     */
    virtual int doDisconnect() = 0;


    /***********************************************************
     * property related
     ***********************************************************/
private:
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
     * @param pid [out] the id of the property in the database
     * @param delayLoad [out] set to true if want this property delay-load,
     *                                           or false otherwise
     * @return 0 on success, or an error code
     * 
     */
    virtual int defineProperty(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) = 0;

    /**
     * Undefine a property.
     * 
     * CAUTION:
     * The property value will also be removed from all beans 
     *  that have this property.
     * 
     * @param property the property to be undefined
     * @return 0 on success, or an error code
     */
    virtual int undefineProperty(Property* propety) = 0;

    /**
     * Load all defined properties from storage to memory.
     * 
     * Notes:
     * -  In out design, all properties will be loaded into memory
     *    before  any other operation. This is purposed to achieve
     *    the best performance.
     * 
     * @param properties the loaded properties
     * @return 0 for success, or an error code
     */
    virtual int loadProperties(std::unordered_map<std::string, Property*>& 
        properties) const = 0;


    /***********************************************************
     * bean related
     ***********************************************************/

    /**
     * Create a new bean instance. 
     * 
     * @param id [out] the id of the newly created bean
     * @return 0 for success, or an error code
     */
    virtual int createBean(oidType& id) = 0;

    /**
     * Delete a bean from the storage.
     * 
     * @param id the id of the bean to be deleted
     * @return 0 for success, or an error code
     */
    virtual int deleteBean(oidType id) = 0;

    /**
     * Load bean property. This method is used for delay-load.
     * 
     * @param id the bean id
     * @param property the property to be loaded
     * @param value the value loaded to
     * @return 0 on success, or an error code
     */
    virtual int loadBeanProperty(oidType id,  
        const Property* property, 
        Json::Value& value) = 0;

    /**
     * Load bean's native data. 
     * 
     * @param id the bean id
     * @param data the data loaded to
     * @return 0 on success, or an error code
     */
    virtual int loadBeanNativeData(oidType id, Json::Value& data) = 0;

    /**
     * Load basic bean data from database into memory.
     * 
     * Notes:
     * - All bean's properties must present in the output value;
     * - All bean's direct-load properies must be loaded into the 
     *   output value, while all delay-load properties shall be 
     *   set to null in the output value.
     * - The bean's native data is regarded as delay-load, 
     *   but can also be loaded into nativeData when nativeData is given.
     * 
     * @param id id of the bean to be loaded
     * @param value [out] the value that holds all bean's properties
     * @param nativeData [out] the value that holds the bean's nativeData
     *                    If set to null, ignore native data.
     * @return 0 on success, or an error code:
     *                   -1001: no such bean in database
     *                   others: other errors
     */
    virtual int loadBeanBase(oidType id, 
        Json::Value& value, 
        Json::Value* nativeData = nullptr) = 0;

    /**
     * Save bean data into database.
     * 
     * All bean's direct-load properies must be saved in this method, 
     * while delay-load properties may or may not be saved in this method.
     * 
     * Notes:
     * - The bean's native data is regarded as delay-load, 
     *   but can also be saved in this method when nativeData is given.
     * 
     * @param id id of the bean to be saved
     * @param data the data that holds all bean's property values
     * @param nativeData the value that holds the bean's nativeData
     *                    If set to null, ignore native data.
     * @return 0 on success, or an error code
     */
    virtual int saveBeanBase(oidType id, 
        const Json::Value& data, 
        const Json::Value* nativeData = nullptr) = 0;

    /**
     * Insert bean's property value. 
     * 
     * Notes:
     * - This method is used when the property is set for the first time.
     * 
     * @param id id of the bean to be inserted
     * @param property the property
     * @param value the value to set
     * @return 0 on success, or an error code
     */
    virtual int insertBeanProperty(oidType id, 
        const Property* property, 
        const Json::Value& value) = 0;

    /**
     * Update bean's property value. 
     * 
     * Notes:
     * - This method is used when the property has previously been set 
     *   with value.
     * 
     * @param id id of the bean to be updated
     * @param property the property
     * @param value the value to set
     * @return 0 on success, or an error code
     */
    virtual int updateBeanProperty(oidType id, 
        const Property* property, 
        const Json::Value& value) = 0;

    /**
     * Update bean's property value at given index.
     * 
     * Notes:
     * - This method is used when the property has previously been 
     *   set with value.
     * 
     * @param id id of the bean to be updated
     * @param property the property
     * @param index the index to be set at
     * @param value the value to set
     * @return 0 on success, or an error code
     */
    virtual int updateBeanProperty(oidType id, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) = 0;

    /**
     * Delete bean's property value. 
     * 
     * @param id id of the bean to be deleted
     * @param property the property to be deleted
     * @return 0 on success, or an error code
     */
    virtual int deleteBeanProperty(oidType id, 
        const Property* property) = 0;

    /**
     * Delete bean's property value at given index
     * 
     * @param id id of the bean to be loaded
     * @param property the property to be deleted
     * @param index the index to be deleted at
     * @return 0 on success, or an error code
     */
    virtual int deleteBeanProperty(oidType id, 
        const Property* property, 
        Json::Value::ArrayIndex index) = 0;

    /**
     * Update bean's native data.
     * 
     * @param id id of the bean to be deleted
     * @param value the value to set
     * @return 0 on success, or an error code
     */
    virtual int updateBeanNativeData(oidType id, 
        const Json::Value& value) = 0;

    /**
     * Delete bean's native data.
     * 
     * @param id id of the bean to be deleted
     * @return 0 on success, or an error code
     */
    virtual int deleteBeanNativeData(oidType id) = 0;


    /***********************************************************
     * search related
     ***********************************************************/
    /**
     * Find beans the property values of which meet the condition 
     * given by optype, property and value.
     * 
     * The condition could be represented as:
     * bean->getProperty(<property>) <optype> <value>
     * 
     * Notes:
     * - when the given property is a relation, the value is interpreted as 
     *     the object bean id;
     * - argument value shall not be array or object;
     * - the search is type restricted, i.e. only those beans with the 
     *      property value having the same type will be accepted;
     * 
     * @param property the property
     * @param value the value of the property
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     * 
     */
   virtual BeanIdPage* findBeans(opType optype, 
        const Property* property, 
        const Json::Value& value, 
        unsigned int pageSize = DEFAULT_PAGE_SIZE) const = 0;

    /**
     * Find subject beans of a given relation property.
     * 
     * Notes:
     * 1. only applies to relation properties;
     * 
     * @param property the property
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     * 
     */
    virtual BeanIdPage* findSubjects(const Property* property, 
        unsigned int pageSize = DEFAULT_PAGE_SIZE) const = 0;

    /**
     * Find object beans of a given relation property.
     * 
     * Notes:
     * 1. only applies to relation properties;
     * 
     * @param property the property
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     */
    virtual BeanIdPage* findObjects(const Property* property, 
        unsigned int pageSize = DEFAULT_PAGE_SIZE) const = 0;

    /**
     * Get all beans in this database.
     * @param pageSize the page size of the returned BeanIdPage
     * @return the first page of the result, or null if error occurs
     */
    virtual BeanIdPage* getAllBeans(unsigned int pageSize = DEFAULT_PAGE_SIZE) 
        const = 0;
    ////////////////////////////////////////////////////////////////////////
    //end of pure virtual methods
     ////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////
    // Other methods that can be used by derived classes
     ////////////////////////////////////////////////////////////////////////
protected:
    /**
     * This method is used for the implemenation method of 
     * AbstractBeanDB::loadProperties() to create new Property
     * instance.
     */
    Property* newProperty(const char* name, 
        pidType id,
        Property::Type type,
        Property::ValueType valueType, 
        bool delayLoad) const;

    ////////////////////////////////////////////////////////////////////////
    // member variables that can be used by derived classes
     ////////////////////////////////////////////////////////////////////////
protected:
    BeanWorld *m_world_;
    bool m_connected_ = false;


friend class BeanWorld;
friend class Bean;
friend class Property;
};

}
}
}