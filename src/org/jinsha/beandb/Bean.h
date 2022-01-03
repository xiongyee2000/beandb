#pragma once

#include <string>
#include <map>

#include "./common.h"
#include "./Property.h"

namespace org {
namespace jinsha {
namespace beandb {

class BeanWorld;


/**
 * In real world, many things can be represented as an object with multiple
 * properties. 
 * 
 * For example, "the 7 year old Peter is a boy" can be represented as:
 * 
 * <Peter, age, 7>
 * <Peter, gender, male>
 * 
 * In this sense, the class Bean is something like the subject in the
 * semantic ternary tuple.
 * 
 */
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
    oidType getId() const {return m_id_;};

    // std::string &getName() {return m_name_;};
    // std::string &getUri() {return uri_;};

    // void setClassId(otype id) {classId_ = id;};
    // void setClassName(const std::string& name) {className_ = name;};
    // void setClassUri(const std::string& uri) {classUri_ = uri;};

    // void setId(otype id) {m_id_ = id;};
    // void setName(const std::string& name) {m_name_ = name;};
    // void setUri(const std::string& uri) {uri_ = uri;};

    /**
     * Is the bean empty, i.e has no property/relation.
     */
    bool empty () const {return m_pst_json_.empty();};

    /**
     * Does the bean have the given property/relation/
     * array property/array relation.
     */
    bool isMember(const Property* property) const;

    /**
     * Does the bean have the given property/relation/
     * array property/array relation.
     */
    bool isMember(const std::string& name) const;

    /**
     * Does the bean have the given property/relation/
     * array property/array relation.
     */
    bool isMember(const char* name) const;

    /**
     * Get all property/relation names of this bean.
     * 
     * @return a collection of names
     */
    Json::Value::Members getMemberNames () const;

    /**
     * Remove all properties/relations of this bean.
     */
    void removeAllProperties();

    /**
     * Check if the bean has the given primary property.
     * 
     * @param property the property
     * @return true if it has such a property, false otherwise
     * 
     */
    bool hasPrimaryProperty(const Property* property) const;

    /**
     * Get property value.
     * 
     * @param property the property
     * @return the property value
     * 
     * Notes:
     * - The property must be of primary type, or
     *    json null value will be returned;
     * - If this bean does not have the given property,
     *    json null value will be returned;
     * - The caller of this method shall determine the type
     *    of the returned value itself;
     * 
     */
    Json::Value get(const Property* property) const;

    /**
     * Set property value.
     * 
     * Notes:
     * - You may need to do explicit type conversion for the value. 
     *    For example: set(p1, (Json::Uint)99).
     *    See more helpful hints in JsonCPP documentation.
     * 
     * @param property the property
     * @param value value of the property
     * @param saveAtOnce whether save change to database immediately
     *                   If set to false, the change will not be saved to database
     *                   until save() is called.
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if the property type is inconsistent to the value type
     */
    int set(Property* property, const Json::Value& value, bool saveAtOnce = true);

    /**
     * This is a special version of set() purposed to 
     * avoid string copying.
     */
    int set(Property* property, const char* value, bool saveAtOnce = true);

    /**
     * This is a special version of set() purposed to 
     * avoid string copying.
     */
    int set(Property* property, const std::string& value, bool saveAtOnce = true);

    /**
     * Does this bean has the given array property.
     * 
     * @param property the array property
     * @return true if it has the given array property, false otherwise
     */
    bool hasArrayProperty(const Property* property) const;

    /**
     * Get the size of the given array property.
     * 
     * Notes:
     * - Use this method to get array size for both primary/relation property. 
     * 
     * @param property the property
     * @return the array size
     * 
     * Notes:
     * - The property must be an array, or 0 will be returned;
     * - If this bean does not have the given array property,
     *   0 will be returned;
     */
    Json::Value::ArrayIndex size(const Property* property) const;

    /**
     * Get the value of an array property at specified index.
     * 
     * @param property the property
     * @param index the index in the array
     * @return the property value
     * 
     * Notes:
     * - If this bean does not have the given property,
     *   a json value of null will be returned;
     * - The property must be an array property, or a null json 
     *    value will be returned;
     * - If the index is invalid, a null json value will be returned;
     */
    Json::Value getAt(const Property* property, 
        Json::Value::ArrayIndex index) const;

     /**
     * Add an array property/relation to this bean.
     * 
     * This method must be called before elements can be appended into 
     * the array property/relation.
     * 
     * Notes:
     * - Once added, the property will be a member of this bean, 
     *   but will contain no element;
     * - Use append()/appendRelation() to add elements to this property/
     *   relation;
     * 
     * @param property the array property
     * @return 0 if success, or an error code:
     *                   -2: if property is null or invalid
     */
    int addArray(Property* property);

     /**
     * Append an element to the end of array property. 
     * 
     * @param property the property
     * @param value value to be added
     * @return 0 if success, or an error code:
     *                   -1: if value is null
     *                   -2: if property is not an array
     *                   -3: if he value is of invalid type
     *                   -4: if the property is not a member of this bean
     */
    int append(Property* property, const Json::Value& value); 

     /**
     * Set the value of an array property at specified index. 
     * 
     * This method must be called after addArray();
     * 
     * @param property the array property
     * @param index the index in the array
     * @param value value to be set
     * @param saveAtOnce whether save change to database immediately
     *                   If set to false, the change will not be saved to database
     *                   until save() is called.
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if the property or value is of invalid type
     *                   -4: if the array property is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setAt(Property* property, Json::Value::ArrayIndex index,
        const Json::Value& value, bool saveAtOnce = true);

    /**
     * This is a special version of setAt() purposed to 
     * avoid string copying.
     */
    int setAt(Property* property, Json::Value::ArrayIndex index,
        const char* value, bool saveAtOnce = true);

    /**
     * This is a special version of setAt() purposed to 
     * avoid string copying.
     */
    int setAt(Property* property, Json::Value::ArrayIndex index,
        const std::string& value, bool saveAtOnce = true);

    /**
     * Does this bean have the given relation.
     * 
     * @param relation the relation
     * @return true if it has such a relation, false otherwise
     * 
     */
    bool hasRelation(const Property* relation) const;

    /**
     * Dos this bean have the array relation.
     * 
     * @param relation the array relation
     * @return true if it has such an array relation, false otherwise
     * 
     */
    bool hasArrayRelation(const Property* relation) const;

    /**
     * Get the value (object bean id) of the given relation.
     * 
     * @param relation the relation
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getRelation(const Property* relation) const;

    /**
     * Get the value (object bean id) of the given array relation at specified index.
     * 
     * Notes:
     * - bean id shall not be 0, which reprents an error.
     * 
     * @param relation the relation property
     * @param index the index in the array
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getReationAt(const Property* relation, 
        Json::Value::ArrayIndex index) const;

    /**
     * This method is used to set relation between two beans, e.g.
     * father/mather, etc. 
     * 
     * @param relation the relation
     * @param bean the object bean of the relation
     * @return 0 if success, or an error code:
     *                   -1: if bean is null
     *                   -2: if property is null or invalid
     */
    int setRelation(Property* relation, Bean* bean);

    /**
     * This is a special version of setRelation().
     * 
     * Notes:
     * - The caller of this method must ensure the objectId is a valid id
     *   of an existing bean.
     * 
     */
    int setRelation(Property* relation, oidType objectBeanId);

    /**
     * This method is used to append an item to an array relation.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by addArray(Property* property).
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
     * This is a special version of appendRelation().
     * 
     * Notes:
     * - The caller of this method must ensure the objectId is a valid id
     *   of an existing bean.
     */
    int appendRelation(Property* relation, oidType objectId);

     /**
     * Set the relation of an array relation at specified index. 
     * 
     * @param relation the array relation
     * @param index the index in the array
     * @param bean the object bean of the relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if relation is null or invalid
     *                   -4: if the array relation is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setRelationAt(Property* relation, Json::Value::ArrayIndex index,
        Bean* bean);

     /**
     * This is a special version of setRelationAt().
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
    int setRelationAt(Property* relation, 
        Json::Value::ArrayIndex index,
        oidType objectId);

    /**
     * Remove property from this bean.
     * 
     * Notes:
     * - Use this method to remove both property and relation.
     * 
     * @param property the property/relation
     * @return 0 for souccess, or an error code
     */
    int remove(Property* property);

    /**
     * Remove the element at given index from this bean's 
     * array property/relation.
     * 
     * Notes:
     * - Use this method to remove an array element for 
     *   both property and relation.
     * 
     * @param property the array property
     * @param index the index
     * @return 0 for souccess, or an error code
     */
    int removeAt( Property* property, Json::Value::ArrayIndex index);

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
     *                   until save() is called.
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
     * Notes:
     * - Only changed data will be saved;
     * - Native data, if changed, will also be saved;
     * 
     * @return 0 on success, or an error code
     */
    int save();

    /**
     * Clear all data of this bean, including all properties and native data.
     * 
     * CAUTION: This operation in unrevertable.
     * 
     * Notes:
     * - All properties/relations will be deleted from storage;
     * - Native data will be deleted from storage;
     * 
     * @return 0 on success, or an error code
     */
    int clear();

private:
    Bean(oidType id, BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;    
    virtual ~Bean();

    Json::Value* getMemberPtr(const Property* property);
    int setPropertyBase_(Property* property, 
        Json::Value *oldValue,  
        const Json::Value&  newValue, 
        Json::Value::ArrayIndex index = (Json::Value::ArrayIndex)-1, bool saveAtOnce = true);

    bool doHasProperty(const Property* property, Property::Type type) const;
    int doRemoveProperty( Property* property, bool saveAtOnce = true);
    int doRemoveProperty( Property* property, Json::Value::ArrayIndex index, bool saveAtOnce = true);   
    int doSet(Property* property, const Json::Value& value, bool saveAtOnce);
    int doSetAt(Property* property, const Json::Value& value, bool saveAtOnce);
    int doSetRelation(Property* relation, oidType objectBeanId, bool saveAtOnce);
    int doAddArray(Property* property, bool saveAtOnce = true);
    int doAppend(Property* property, const Json::Value& value, bool saveAtOnce); 
    int doAppendRelation(Property* relation, oidType objectBeanId, bool saveAtOnce);
    void addSubject(Bean* subject, Property* relation);
    void removeSubject(Bean* subject, Property* relation);
    int load(Json::Value& data);
    int unload();
    int loadProperty(const Property* property);
    // int pstTransition(int currentPst, int desiredPst);


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
    oidType m_id_ = 0;
    Json::Value m_json_;
    Json::Value m_native_data_json_;
    Json::Value m_pst_json_;
    Json::Value m_native_data_pst_json_;
    BeanWorld* m_world_;
    std::multimap<oidType, Property*> m_subjectMap_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;
    // std::string m_name_;
    // std::string uri_;

friend class BeanWorld;
friend class Property;
friend class AbstractBeanDB;
};

}
}
}