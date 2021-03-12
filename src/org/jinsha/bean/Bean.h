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
    bool isMember(const Property* property) const;

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
     * @param property the property
     * @param value value of the property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if he value is of invalid type
     */
    int setProperty(Property* property, const Json::Value& value);

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
     * @param property the property
     * @return the size
     * 
     * Note:
     * If this bean does not have the named array property,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArrayPropertySize(const Property* property) const;

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
     *                   -2: if property is null
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
     *                   -2: if property is null
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
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if value is null
     *                   -2: if property is null
     *                   -3: if the value is of invalid type
     *                   -4: if the array property is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setProperty(Property* property, Json::Value::ArrayIndex index,
        const Json::Value& value);

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
     * Resize an array property. 
     * 
     * @param property the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if property is null
     *                   -4: if the array property is not a member of this bean
     */
    int resizeProperty(Property* property, Json::Value::ArrayIndex size);

    /**
     * Check the bean has the given relation.
     * 
     * @param property the relation property
     * @return true if it has such a relation, false otherwise
     * 
     */
    bool hasRelation(const Property* property) const;

    /**
     * Check the bean has the array relation.
     * 
     * @param property the array relation property
     * @return true if it has such an array relation, false otherwise
     * 
     */
    bool hasArrayRelation(const Property* property) const;

    /**
     * Get size of an array relation.
     * 
     * @param property the relation property
     * @return the size
     * 
     * Note:
     * If this bean does not have the named array relation,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getArrayRelationSize(const Property* property) const;

    /**
     * Get relation bean.
     * 
     * @param property the relation property
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getRelationBeanId(const Property* property) const;

    /**
     * Get relation bean of an array relation at specified index.
     * 
     * @param property the relation property
     * @param index the index in the array
     * @return the bean id, or 0 if it does not exist.
     * 
     */
    oidType getRelationBeanId(const Property* property, 
        Json::Value::ArrayIndex index) const;

    /**
     * This method is used to set a one-to-one relationship between two beans, e.g.
     * father/mather, etc. 
     * 
     * @param property the relation property
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code:
     *                   -1: if bean is null
     *                   -2: if property is null
     */
    int setRelation(Property* property, Bean* bean);
   
     /**
     * Create an empty array relation for this bean.
     * 
     * This method must be called before items can be added into 
     * the array relation.
     * 
     * @param property the array relation property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -2: if property is null
     */
    int createArrayRelation(Property* property);

    /**
     * This method is used to append an item to a one-to-many relationship.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to the 
     *    json array property created by setRelation(Property* property).
     * 
     * @param property the array relation property property
     * @param bean the counter part bean of the relation to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if property is null
     *                   -4: if the array relation is not a member of this bean
     */
    int appendRelation(Property* property, Bean* bean);
 
     /**
     * Set the relation of an array relation at specified index. 
     * 
     * This method must be called after createArrayRelation();
     * 
     * @param property the array relation property
     * @param index the index in the array
     * @param bean the counter part bean of the relation
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if bean is null
     *                   -2: if property is null
     *                   -4: if the array relation is not a member of this bean
     *                   -5: if the index is invalid
     */
    int setRelation(Property* property, Json::Value::ArrayIndex index,
        Bean* bean);

    /**
     * This method is used to add an item to a one-to-many relationship.
     * 
     * Notes:
     * - This method will internally add the counter part bean's id to 
     *   the json array property created by setRelation(Property* property).
     * 
     * @param property the relation property
     * @param size the size to be resized
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if property is null
     *                   -2: if bean is null
     */
    int resizeRelation(Property* property, Json::Value::ArrayIndex size);

    /**
     * Remove property from this bean.
     * 
     * @param property the property
     * @return the removed item as json value
     */
    Json::Value removeProperty( Property* property);

    /**
     * Remove relation from this bean.
     * 
     * @param property the relation property
     * @return
     */
    void removeRelation( Property* property);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;    
    virtual ~Bean();

    bool doHasProperty(const Property* property, Property::Type type) const;

    Json::Value::ArrayIndex 
    getArrayMemberSizeCommon_(const Property* property, 
        bool isProperty_ = true) const;

    const Json::Value& getMemberRef(const Property* property);
    void setPropertyBase_(Property* property, 
        const Json::Value& oldValue,  const Json::Value&  newValue);

private:
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