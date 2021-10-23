#pragma once

#include <string>
#include <map>
#include <vector>

#include "./common.h"
#include "./Property.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanWorld;

class Bean
{
public:
    // otype getClassId() {return classId_;};
    // std::string getClassName() {return className_;};
    // std::string getClassUri() {return classUri_;};

    /**
     * Get bean id.
     * 
     * @return the bean id
     */
    inline oidType getId() const {return m_id_;} ;
    // std::string &getName() {return m_name_;};
    // std::string &getUri() {return uri_;};

    // void setClassId(otype id) {classId_ = id;};
    // void setClassName(const std::string& name) {className_ = name;};
    // void setClassUri(const std::string& uri) {classUri_ = uri;};

    // void setId(otype id) {m_id_ = id;};
    // void setName(const std::string& name) {m_name_ = name;};
    // void setUri(const std::string& uri) {uri_ = uri;};

    /**
     * Is the bean is empty, i.e has none property/relation.
     */
    bool empty () const {return m_pst_json_.empty();};

    /**
     * Does the bean have the named property/relation/
     * array property/array relation.
     */
    bool isMember(const Property* property) const;

    /**
     * Get all property/relation names of this bean.
     * 
     * @return a collection of names
     */
    Json::Value::Members getMemberNames () const 
        {return m_pst_json_.getMemberNames();};

    /**
     * Remove all properties/relations of this bean.
     */
    void removeAllProperties();

    /**
     * Check the bean has the given property.
     * 
     * @param property the property
     * @return true if it has such a property, false otherwise
     * 
     */
    bool hasProperty(const Property* property) const;

    /**
     * Get property value.
     * 
     * @param property the property
     * @return the property value
     * 
     * Note:
     * If this bean does not have the named property,
     * a json value of null will be returned.
     */
    Json::Value getProperty(const Property* property) const;

    /**
     * Set the value of a property of this bean. 
     * 
     * Notes:
     * - You may need to do explicit type conversion for the value. 
     *    For example: setProperty(p1, (Json::Uint)99).
     *    See more helpful hints in JsonCPP documentation.
     * 
     * @param property the property
     * @param value value of the property
     * @param saveAtOnce whether save change to database immediately
     *                   If set to false, the change will not be saved to database
     *                   until save().
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if the property or value is of invalid type
     */
    int setProperty(Property* property, const Json::Value& value, bool saveAtOnce = true);

    /**
     * This is a special version of setProperty() purposed to 
     * avoid string copying.
     */
    int setProperty(Property* property, const char* value, bool saveAtOnce = true);

    /**
     * This is a special version of setProperty() purposed to 
     * avoid string copying.
     */
    int setProperty(Property* property, const std::string& value, bool saveAtOnce = true);

    /**
     * Is the specified property an array property.
     * 
     * @param property the property
     * @return true if it is an array property, false otherwise
     */
    bool hasArrayProperty(const Property* property) const;

    /**
     * Get size of an array property.
     * 
     * Notes:
     * - Use this method to get array size for both primary and relation. 
     * 
     * @param property the property
     * @return the size
     * 
     * Note:
     * If this bean does not have the named array property,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArraySize(const Property* property) const;

    /**
     * Get value of an array property at specified index.
     * 
     * @param property the property
     * @param index the index in the array
     * @return the property value
     * 
     * Note:
     * If this bean does not have the given property,
     * a json value of null will be returned;
     * The property must be an array property, or a json 
     * value of null will be returned;
     * If the index is invalid, a json value of null will be returned;
     */
    Json::Value getArrayProperty(const Property* property, 
        Json::Value::ArrayIndex index) const;

     /**
     * Create an empty array property for this bean.
     * 
     * This method must be called before items can be added into 
     * the array property.
     * 
     * @param property the array property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if property is null or invalid
     */
    int createArrayProperty(Property* property);

     /**
     * Append an item to the end of an array property. 
     * 
     * @param property the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null or invalid
     *                   -3: if he value is of invalid type
     *                   -4: if the array property is not a member of this bean
     */
    int appendProperty(Property* property, const Json::Value& value); 

     /**
     * Set the value of an array property at specified index. 
     * 
     * This method must be called after createArrayProperty();
     * 
     * @param property the array property
     * @param index the index in the array
     * @param value value to be set
     * @param saveAtOnce whether save change to database immediately
     *                   If set to false, the change will not be saved to database
     *                   until save().
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if the property or value is of invalid type
     *                   -4: if the array property is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setArrayProperty(Property* property, Json::Value::ArrayIndex index,
        const Json::Value& value, bool saveAtOnce = true);

    /**
     * This is a special version of setArrayProperty() purposed to 
     * avoid string copying.
     */
    int setArrayProperty(Property* property, Json::Value::ArrayIndex index,
        const char* value, bool saveAtOnce = true);

    /**
     * This is a special version of setArrayProperty() purposed to 
     * avoid string copying.
     */
    int setArrayProperty(Property* property, Json::Value::ArrayIndex index,
        const std::string& value, bool saveAtOnce = true);

    //  /**
    //  * Remove an item from an array property at specified index.
    //  * 
    //  * @param property the property
    //  * @param value value to be added
    //  * @return 0 if success, or an error code
    //  *                   error code:
    //  *                   -1: if property is null
    //  *                   -5: if the index is invalid
    //  */
    // int removeArrayProperty(Property* property, Json::Value::ArrayIndex index); 

    /**
     * Check the bean has the given relation.
     * 
     * @param relation the relation
     * @return true if it has such a relation, false otherwise
     * 
     */
    bool hasRelation(const Property* relation) const;

    /**
     * Check the bean has the array relation.
     * 
     * @param relation the array relation
     * @return true if it has such an array relation, false otherwise
     * 
     */
    bool hasArrayRelation(const Property* relation) const;


    /**
     * Get the object bean id of the given relation.
     * 
     * @param relation the relation
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getObjectId(const Property* relation) const;

    /**
     * Get the object bean id of the given array relation at specified index.
     * 
     * Notes:
     * - bean id shall not be 0, which reprents an error.
     * 
     * @param relation the relation property
     * @param index the index in the array
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getObjectId(const Property* relation, 
        Json::Value::ArrayIndex index) const;

    /**
     * This method is used to set relation between two beans, e.g.
     * father/mather, etc. 
     * 
     * Notes:
     * - The caller of this method must ensure the objectId is a valid id
     *   of an existing bean.
     * 
     * @param relation the relation
     * @param objectBeanId the id of the counter part bean of the relation
     * @return 0 if success, or an error code:
     *                   -1: if bean is null
     *                   -2: if relation is null or invalid
     */
    int setRelation(Property* relation, oidType objectBeanId);

    /**
     * This method is used to set relation between two beans, e.g.
     * father/mather, etc. 
     * 
     * @param relation the relation
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code:
     *                   -1: if bean is null
     *                   -2: if property is null or invalid
     */
    int setRelation(Property* relation, Bean* bean);

     /**
     * Create an empty array relation for this bean.
     * 
     * This method must be called before items can be added into 
     * the array relation.
     * 
     * @param relation the array relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if property is null or invalid
     */
    int createArrayRelation(Property* relation);

    /**
     * This method is used to append an item to an array relation.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by setRelation(Property* property).
     * - The caller of this method must ensure the objectId is a valid id
     *   of an existing bean.
     * 
     * @param relation the array relation
     * @param objectId the id of the counter part bean of the relation 
     *                    to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if relation is null or invalid
     *                   -4: if the array relation is not a member of this bean
     */
    int appendRelation(Property* relation, oidType objectId);


    /**
     * This method is used to append an item to an array relation.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by createArrayRelation(Property* property).
     * 
     * @param relation the array relation
     * @param bean the counter part bean of the relation to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if relation is null or invalid
     *                   -4: if the array relation is not a member of this bean
     */
    int appendRelation(Property* relation, Bean* bean);

     /**
     * Set the relation of an array relation at specified index. 
     * 
     * @param relation the array relation
     * @param index the index in the array
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if relation is null or invalid
     *                   -4: if the array relation is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setArrayRelation(Property* relation, Json::Value::ArrayIndex index,
        Bean* bean);

     /**
     * Set the relation of an array relation at specified index. 
     * 
     * Notes:
     * - The caller of this method must ensure the objectId is a valid id
     *   of an existing bean.
     * 
     * @param relation the array relation
     * @param index the index in the array
     * @param objectId the id of the counter part bean of the relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if relation is null or invalid
     *                   -4: if the array relation is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setArrayRelation(Property* relation, 
        Json::Value::ArrayIndex index,
        oidType objectId);

    /**
     * Remove property from this bean.
     * 
     * Notes:
     * - Use this method to remove both property and relation.
     * 
     * @param property the property
     * @return 0 for souccess, or an error code
     */
    int removeProperty( Property* property);

    /**
     * Remove the item at given index from this bean's array property/relation.
     * 
     * Notes:
     * - Use this method to remove an array item for both property and relation.
     * 
     * @param property the array property
     * @param index the index
     * @return 0 for souccess, or an error code
     */
    int removeProperty( Property* property, Json::Value::ArrayIndex index);

    // /**
    //  * Remove relation from this bean.
    //  * 
    //  * @param relation the relation
    //  * @return
    //  */
    // void removeRelation( Property* relation);

    /**
     * Get native data of a bean.
     * 
     * @return the native data as json value
     */
    Json::Value& getNativeData();
    Json::Value& getNativeData() const;

    /**
     * Set native data of a bean.
     * 
     * Native data are data bound to this bean
     * but out of property control.
     * 
     * User can freely set/get native data
     * with setNativeData()/getNativeData()
     * without defining any property (actually 
     * the native data have nothing to do with
     * those defined properties).
     * 
     * This provides a way for user to easily 
     * store/access arbitrary data as they want.
     * However, the native data won't be helpful
     * in any search/find operations.
     *  
     * @param data the native data
     * @param saveAtOnce whether save change to database immediately
     *                   If set to false, the change will not be saved to database
     *                   until save().
     */
    int setNativeData(Json::Value& data, bool saveAtOnce = true);

    /**
     * Remove native data of this bean.
     * 
     * Notes:
     * - The native data will be directly removed from database.
     * 
     * @return 0 if success, or an error code
     */
    int removeNativeData();

    /**
     * Save data of this bean into database storage.
     * 
     * Note this method will call AbstractDB::saveBeanBase() firstly,
     * and then AbstractDB::saveBeanProperty() on each property
     * that has changes.
     * 
     * @return 0 on success, or an error code
     */
    int save();

    /**
     * Clear all data of this bean, including all properties and native data.
     * 
     * Notes:
     * - CAUTIOUS: this operation in unrevertable
     * 
     * @return 0 on success, or an error code
     */
    int clear();

private:
    // Bean(oidType id);
    Bean(oidType id, BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;    
    virtual ~Bean();

    bool doHasProperty(const Property* property, Property::Type type) const;

    Json::Value* getMemberPtr(const Property* property);
    int setPropertyBase_(Property* property, 
        Json::Value *oldValue,  
        const Json::Value&  newValue, 
        Json::Value::ArrayIndex index = (Json::Value::ArrayIndex)-1, bool saveAtOnce = true);

    int doRemoveProperty( Property* property, bool saveAtOnce = true);
    int doRemoveProperty( Property* property, Json::Value::ArrayIndex index, bool saveAtOnce = true);

    void addSubject(Bean* subject, Property* relation);
    void removeSubject(Bean* subject, Property* relation);
    // int pstTransition(int currentPst, int desiredPst);

    ////////////////////////////////////////////////////////////////
    //DB related methods
    ////////////////////////////////////////////////////////////////
private:
    int load(Json::Value& data);
    int unload();
    int loadProperty(const Property* property);
    // bool propertyLoaded(const Property* property);

    int doSetProperty(Property* property, const Json::Value& value, bool saveAtOnce);
    int doSetArrayProperty(Property* property, const Json::Value& value, bool saveAtOnce);
    int doSetRelation(Property* relation, oidType objectBeanId, bool saveAtOnce);
    int doCreateArrayProperty(Property* property, bool saveAtOnce = true);
    int doAppendProperty(Property* property, const Json::Value& value, bool saveAtOnce); 
    int doAppendRelation(Property* relation, oidType objectBeanId, bool saveAtOnce);

private:
    typedef enum {
        PST_NSY = 0,
        PST_SYN,
        PST_NEW,
        PST_MOD,
        PST_RMD,
        PST_MAX /*guard*/
    } pst_t;

private:
    Json::Value m_json_;
    Json::Value m_native_data_json_;
    Json::Value m_pst_json_;
    Json::Value m_native_data_pst_json_;
    BeanWorld* m_world_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;

    oidType m_id_ = 0;
    // std::string m_name_;
    // std::string uri_;

    std::multimap<oidType, Property*> m_subjectMap_;

friend class BeanWorld;
friend class Property;
friend class AbstractBeanDB;
};

}
}
}