#pragma once

#include <string>
#include <unordered_map>
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
     * Does the bean have the named property/relation/
     * array property/array relation.
     */
    bool isMember(const char* name) const;
    // bool isMember(const std::string& name) const;

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
    bool hasProperty(pidType id) const;

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
    Json::Value getProperty(pidType id) const;

    /**
     * Set the value of a property of this bean. 
     * 
     * @param name name of the property
     * @param value value of the property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the value is null
     *                   -2: if no property is defined with the name
     *                   -3: if he value is of invalid type
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
    bool hasArrayProperty(pidType id) const;

    /**
     * Get size of an array property.
     * 
     * @param name the property name
     * @return the size
     * 
     * Note:
     * If this bean does not have the named array property,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArrayPropertySize(const char* name) const;
    Json::Value::ArrayIndex getArrayPropertySize(pidType id) const;

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
     * @param name name of the array property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if no  array property with the name is defined
     */
    int createArrayProperty(const char* name);
    int createArrayProperty(pidType id);

     /**
     * Append an item to the end of an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the value is null
     *                   -2: if no  array property is defined with the name
     *                   -3: if he value is of invalid type
     *                   -4: if the array property is not a member of this bean
     */
    int appendProperty(const char* name, const Json::Value& value); 
    int appendProperty(pidType id, const Json::Value& value); 

     /**
     * Set the value of an array property at specified index. 
     * 
     * This method must be called after createArrayProperty();
     * 
     * @param name name of the array property
     * @param index the index in the array
     * @param value value to be set
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the value is null
     *                   -2: if no  array property with the name is defined
     *                   -3: if the value is of invalid type
     *                   -4: if the array property is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setArrayProperty(const char* name, Json::Value::ArrayIndex index,
        const Json::Value& value);
    int setArrayProperty(pidType id, Json::Value::ArrayIndex index,
        const Json::Value& value);

    //  /**
    //  * Remove an item from an array property at specified index.
    //  * 
    //  * @param name name of the property
    //  * @param value value to be added
    //  * @return 0 if success, or an error code
    //  *                   error code:
    //  *                   -1: if no property is defined with the name
    //  *                   -5: if the index is invalid
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
     *                   -4: if the array property is not a member of this bean
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
    bool hasRelation(pidType id) const;

    /**
     * Check the bean has an array relation with the given name.
     * 
     * @param name the array relation name
     * @return true if it has such an array relation, false otherwise
     * 
     */
    bool hasArrayRelation(const char* name) const;
    bool hasArrayRelation(pidType id) const;

    /**
     * Get size of an array relation.
     * 
     * @param name the relation name
     * @return the size
     * 
     * Note:
     * If this bean does not have the named array relation,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArrayRelationSize(const char* name) const;
    Json::Value::ArrayIndex getArrayRelationSize(pidType id) const;

    /**
     * Get relation bean.
     * 
     * @param name the relation name
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getRelationBeanId(const char* name) const;
    oidType getRelationBeanId(pidType id) const;

    /**
     * Get relation bean of an array relation at specified index.
     * 
     * @param name the relation name
     * @param index the index in the array
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getRelationBeanId(const char* name, 
        Json::Value::ArrayIndex index) const;
    oidType getRelationBeanId(pidType id, 
        Json::Value::ArrayIndex index) const;

    /**
     * This method is used to set a one-to-one relationship between two beans, e.g.
     * father/mather, etc. 
     * 
     * @param name name of the relation proerty
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code:
     *                   -1: if the bean is null
     *                   -2: if no relation property with the name is defined
     */
    int setRelation(const char* name, Bean* bean);
    int setRelation(pidType id, Bean* bean);
   
     /**
     * Create an empty array relation for this bean.
     * 
     * This method must be called before items can be added into 
     * the array relation.
     * 
     * @param name name of the array relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if no  array relation with the name is defined
     */
    int createArrayRelation(const char* name);
    int createArrayRelation(pidType id);

    /**
     * This method is used to append an item to a one-to-many relationship.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by setRelation(const char* name).
     * 
     * @param name name of the array relation property
     * @param bean the counter part bean of the relation to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the bean is null
     *                   -2: if no  array relation with the name is defined
     *                   -4: if the array relation is not a member of this bean
     */
    int appendRelation(const char* name, Bean* bean);
    int appendRelation(pidType id, Bean* bean);
 
     /**
     * Set the relation of an array relation at specified index. 
     * 
     * This method must be called after createArrayRelation();
     * 
     * @param name name of the array relation
     * @param index the index in the array
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the bean is null
     *                   -2: if no  array relation with the name is defined
     *                   -4: if the array relation is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setRelation(const char* name, Json::Value::ArrayIndex index,
        Bean* bean);
    int setRelation(pidType id, Json::Value::ArrayIndex index,
        Bean* bean);

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
    Json::Value removeProperty(pidType id);

    /**
     * Remove relation from this bean.
     * 
     * @param name the relation name
     * @return
     */
    void removeRelation( const char* name);
    void removeRelation(pidType id);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;    
    virtual ~Bean();

    bool doHasProperty(pidType id, Property::Type type) const;

    Json::Value::ArrayIndex getArrayMemberSizeCommon_(pidType id, 
        bool isProperty_ = true) const;

    const Json::Value& getMemberRef(const char* key);

    // void swap(Json::Value &other);

    Json::Value m_propertyValues_;
    BeanWorld* m_world_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;

    oidType m_id_ = 0;
    // std::string m_name_;
    // std::string uri_;

friend class BeanWorld;
friend class Property;
};

}
}
}