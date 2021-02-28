#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "./common.h"

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
    inline oidType getId() {return m_id_;};
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
    bool empty () const {return m_propertyValues_.empty();};

    /**
     * Does the bean have the named property/relation.
     */
    bool isMember(const char* name) const;
    bool isMember(const std::string& name) const;

    /**
     * Get all property/relation names of this bean.
     * 
     * @return a collection of names
     */
    Json::Value::Members getMemberNames () const 
        {return m_propertyValues_.getMemberNames();};

    /**
     * Remove all properties/relations of this bean.
     */
    void clear();

    /**
     * Check the bean has a property with the given name.
     * 
     * @param name the property name
     * @return true if it has such a property, false otherwise
     * 
     */
    bool hasProperty(const char* name) const;

    /**
     * Get property value.
     * 
     * @param name the property name
     * @return the property value
     * 
     * Note:
     * If this bean does not have the named property,
     * a json value of null will be returned.
     */
    Json::Value getProperty(const char* name) const;

    /**
     * Set the value of a property of this bean. 
     * 
     * @param name name of the property
     * @param value value of the property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no property with the name is defined
     *                   -2: if the value is null
     *                   -3: if the property is not of primary type
     *                   -4: if the value type is invalid
     */
    int setProperty(const char* name, const Json::Value& value);
    int setProperty(pidType id, const Json::Value& value);

    /**
     * Is the specified property an array property.
     * 
     * @param name the property name
     * @return true if it is an array property, false otherwise
     */
    bool hasArrayProperty(const char* name) const;

    /**
     * Get size of an array property.
     * 
     * @param name the property name
     * @return the size
     * 
     * Note:
     * If this bean does not have the named property,
     * 0 will be returned;
     * If the property is not an array property, 
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArrayPropertySize(const char* name) const;

    /**
     * Get value of an array property at specified index.
     * 
     * @param name the property name
     * @param index the index in the array
     * @return the property value
     * 
     * Note:
     * If this bean does not have the named property,
     * a json value of null will be returned;
     * The property must be an array property, or a json 
     * value of null will be returned;
     * If the index is invalid, a json value of null will be returned;
     */
    Json::Value getArrayProperty(const char* name, 
        Json::Value::ArrayIndex index) const;

     /**
     * Create an empty array property for this bean.
     * 
     * This method must be called before items can be added into 
     * the array property.
     * 
     * @param name name of the property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no  array property with the name is defined
     */
    int setArrayProperty(const char* name);
    int setArrayProperty(pidType id);

     /**
     * Set the value of an array property at specified index. 
     * 
     * This method must be called after setArrayProperty();
     * 
     * @param name name of the property
     * @param index the index in the array
     * @param value value to be set
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no  array property with the name is defined
     *                   -2: if the value is null
     *                   -3: if the value is of invalid type
     *                   -4: if the index is invalid
     *                   -5: if the array property is not a member of this bean
     */
    int setArrayProperty(const char* name, Json::Value::ArrayIndex index,
        const Json::Value& value);
    int setArrayProperty(pidType id, Json::Value::ArrayIndex index,
        const Json::Value& value);

     /**
     * Append an item to the end of an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no  array property is defined with the name
     *                   -2: if the value is null
     *                   -3: if he value is of invalid type
     *                   -5: if the array property is not a member of this bean
     */
    int appendArrayProperty(const char* name, const Json::Value& value); 
    int appendArrayProperty(pidType id, const Json::Value& value); 

    //  /**
    //  * Remove an item from an array property at specified index.
    //  * 
    //  * @param name name of the property
    //  * @param value value to be added
    //  * @return 0 if success, or an error code
    //  *                   error code:
    //  *                   -1: if no property is defined with the name
    //  *                   -4: if the index is invalid
    //  */
    // int removeArrayProperty(const char* name, Json::Value::ArrayIndex index); 

     /**
     * Resize an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no array property is defined with the name
     *                   -5: if the array property is not a member of this bean
     */
    int resizeProperty(const char* name, Json::Value::ArrayIndex size);
    int resizeProperty(pidType id, Json::Value::ArrayIndex size);

    /**
     * Check the bean has a relation with the given name.
     * 
     * @param name the relation name
     * @return true if it has such a relation, false otherwise
     * 
     */
    bool hasRelation(const char* name) const;

    /**
     * Get relation bean.
     * 
     * @param name the relation name
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getBeanId(const char* name) const;

    /**
     * Get relation bean of an array relation at specified index.
     * 
     * @param name the relation name
     * @param index the index in the array
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getBeanId(const char* name, 
        Json::Value::ArrayIndex index) const;

    /**
     * This method is used to set a one-to-one relationship between two beans, e.g.
     * father/mather, etc. 
     * 
     * Notes:
     * - A relation is different than a property, so don't mix them up;
     * - If no relation with name has ever been created, a new one will be created;
     * - This method will internally set a property for this relation, using 
     *     "&"+"name"  as the key and the counter part bean's id as the value,
     *     so don't use this key  to set any value using setProperty();
     * 
     * @param name name of the relation
     * @param bean the counter part bean of the relation
     * @return the id of the relation if success, or an error code
     *                   (This is useful when the property is firstly set.)
     *                   error code:
     *                   -1: if no property with the name is defined
     *                   -2: if the bean is null
     */
    pidType setRelation(const char* name, Bean* bean);
    
   /**
     * This method is used to set a one-to-one relationship between two beans, e.g.
     * father/mather, etc. 
     * 
     * Notes:
     * - This method is provided for the sake of better performance.
     * 
     * @param rid id of the relation
     * @param bean the counter part bean of the relation
     * @return 0  if success, or an error code
     *                   error code:
     *                   -1: if the relation with the id does not exist
     *                   -2: if the bean is null
     */
    int setRelation(pidType rid, Bean* bean);

    /**
     * This method is used to create a one-to-many relationship, e.g.
     * friends, classMates, etc. 
     * 
     * Notes:
     * - A relation is different than a property, so don't mix them up;
     * - If no relation with name has ever been created, a new one 
     *    will be created;
     * - This method only "define" the relation, and user needs to 
     *    add related beans  using appendRelation();
     * - This method will internally create an json  array for this relation, using 
     *     "&"+"name"  as the key, so don't use this key  to set any value using 
     *     setProperty();
     * 
     * @param name name of the relation
     * @return The id of the relation if success, or an error code.
     *                   error code:
     *                   -1: if no property with the name is defined
     *                   -2: if the bean is null
     */
    pidType setRelation(const char* name);

    /**
     * This method is used to append an item to a one-to-many relationship.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by setRelation(const char* name).
     * 
     * @param name name of the relation
     * @param bean the counter part bean of the relation to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no property with the name is defined
     *                   -2: if the bean is null
     */
    int appendRelation(const char* name, Bean* bean);

    /**
     * This method is used to add an item to a one-to-many relationship.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to 
     *   the json array property created by setRelation(const char* name).
     * 
     * @param name name of the relation
     * @param size the size to be resized
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if no property with the name is defined
     *                   -2: if the bean is null
     */
    int resizeRelation(const char* name, Json::Value::ArrayIndex size);

    /**
     * Remove property from this bean.
     * 
     * @param name the property name
     * @return the removed item as json value
     */
    Json::Value removeProperty( const char* name);

    /**
     * Remove property from this bean.
     * 
     * Note the method is only effective when the id is valid, 
     * i.e. the property was previously set by setProperty().
     * 
     * @param id the property id
     * @return the removed item as json value, or null value if
     *                   the id is invalid.
     */
    Json::Value removeProperty(pidType id);

    /**
     * Remove relation from this bean.
     * 
     * @param name the relation name
     * @return
     */
    void removeRelation( const char* name);

    /**
     * Remove relation from this bean.
     * 
     * Note the method is only effective when the rid is valid, 
     * i.e. the relation was previously set by setRelation().
     * 
     * @param rid the relation id
     * @return 
     */
    void removeRelation(pidType rid);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;    
    virtual ~Bean();

    Json::Value::ArrayIndex doGetPropertySize(const char* name, 
        bool isProperty_ = true) const;

    const Json::Value& getPropertyRef(const char* key);
    const Json::Value& getRelationRef(const char* key);

    // void swap(Json::Value &other);

    Json::Value m_propertyValues_;
    Json::Value m_relationValues_;
    BeanWorld* m_world_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;

    oidType m_id_ = 0;
    // std::string m_name_;
    // std::string uri_;

friend class BeanWorld;
};

}
}
}