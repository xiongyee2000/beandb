#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "./common.h"
#include "./Relation.h"

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

    ////////////////////////////////////////////////////////////////
    //Below  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////
    bool 	empty () const {return m_propertyValues_.empty();};

    //Note it is equal to (isPropery() || isRelation())
    bool isMember (const char* key) const;
    bool isMember (const std::string &key) const;

    Json::Value::Members getMemberNames () const 
        {return m_propertyValues_.getMemberNames();};

    void clear();
    ////////////////////////////////////////////////////////////////
    //Above  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////

    /**
     * Check the bean has a property with the given name.
     * 
     * @param name the property name
     * @return true if it has such a property, false otherwise
     * 
     */
    bool isProperty(const char* name) const;

    /**
     * Get property value.
     * 
     * @param name the property name
     * @return the property value
     * 
     * Note:
     * If this bean does not have property with name,
     * a json value of null will be returned.
     */
    Json::Value getProperty(const char* name) const;

    /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as bean['key'] = value, excpect
     * that the index will be updated if the property is indexed. But also
     * note that index won't apply to property values of list/object type.
     * 
     * Notes:
     * 1. the index created is based on the type of the value, so that only 
     *      those beans that have the same property type will be returned.
     * 
     * @param name name of the property
     * @param value value of the property
     * @return the pid of the property if success, or an error code
     *                   (This is useful when the property is firstly set.)
     *                   error code:
     *                   -1: if the property name is null or empty
     *                   -2: if the value is null
     */
    pidType setProperty(const char* name, const Json::Value& value);
    
     /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as bean['key'] = value, excpect
     * that the index will be updated if the property is indexed. But also
     * note that index won't apply to property values of list/object type.
     * 
     * @param pid id of the property (see BeanWorld::getPropertyId())
     * @param value value of the property
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the property with the id does not exist
     *                   -2: if the value is null
     */
    int setProperty(pidType pid, const Json::Value& value);

    /**
     * Is the specified property an array property.
     * 
     * @param name the property name
     * @return true if it is an array property, false otherwise
     * 
     * Note:
     * If this bean does not have property with name,
     * false will be returned;
     * If the current property value is not a json array, 
     * false will be returned;
     */
    bool isArrayProperty(const char* name) const;

    /**
     * Get size of an array property.
     * 
     * @param name the property name
     * @return the size
     * 
     * Note:
     * If this bean does not have property with name,
     * 0 will be returned;
     * The current property value must be a json array, 
     * or 0 will be returned;
     */
    Json::Value::ArrayIndex getPropertySize(const char* name) const;

    /**
     * Get value of an array property at specified index.
     * 
     * @param name the property name
     * @param index the index in the array
     * @return the property value
     * 
     * Note:
     * If this bean does not have property with name,
     * a json value of null will be returned;
     * The current property value must be a json array, or a json 
     * value of null will be returned;
     * If the index is invalid, a json value of null will be returned;
     */
    Json::Value getProperty(const char* name, 
        Json::Value::ArrayIndex index) const;

     /**
     * Set the value of an array property at specified index. 
     * 
     * Note this method must be called after the array property
     * is created, say be setProperty() with  json value of array type.
     * 
     * @param name name of the property
     * @param index the index in the array
     * @param value value to be set
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the property name is null or empty
     *                   -2: if the value is null
     *                   -3: if he current property value is not a json array
     *                   -4: if the index is invalid
     */
    int setProperty(const char* name, Json::Value::ArrayIndex index,
        const Json::Value& value);

     /**
     * Append an item to the end of an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the property with the name does not exist
     *                   -2: if the value is null
     *                   -3: if he current property value is not a json array
     */
    int appendProperty(const char* name, const Json::Value& value); 

     /**
     * Resize an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code
     *                   error code:
     *                   -1: if the property with the name does not exist
     *                   -3: if he current property value is not a json array
     */
    int resizeProperty(const char* name, Json::Value::ArrayIndex size);

    /**
     * Check the bean has a relation with the given name.
     * 
     * @param name the relation name
     * @return true if it has such a relation, false otherwise
     * 
     */
    bool isRelation(const char* name) const;

    /**
     * Check the bean has an array relation with the given name.
     * 
     * @param name the relation name
     * @return true if it has such an array relation, false otherwise
     * 
     */
    bool isArrayRelation(const char* name) const;

    /**
     * Get relation bean.
     * 
     * @param name the relation name
     * @return the relation bean, or null if it does not exist.
     * 
     */
    Bean* getRelation(const char* name) const;

    /**
     * Get size of an array relation.
     * 
     * @param name the relation name
     * @return the size
     * 
     * Note:
     * If this bean does not have relation with the given name,
     * 0 will be returned;
     */
    Json::Value::ArrayIndex getRelationSize(const char* name) const;

    /**
     * Get relation bean of an array relation at specified index.
     * 
     * @param name the relation name
     * @param index the index in the array
     * @return the relation bean, or null
     * 
     */
    Bean* getRelation(const char* name, 
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
     * @return the pid of the relation if success, or an error code
     *                   (This is useful when the property is firstly set.)
     *                   error code:
     *                   -1: if the property name is null or empty
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
     * @return The pid of the relation if success, or an error code.
     *                   error code:
     *                   -1: if the property name is null or empty
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
     *                   -1: if the property name is null or empty
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
     *                   -1: if the property name is null or empty
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
     * Note the method is only effective when the pid is valid, 
     * i.e. the property was previously set by setProperty().
     * 
     * @param pid the property id
     * @return the removed item as json value, or null value if
     *                   the pid is invalid.
     */
    Json::Value removeProperty(pidType pid);

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