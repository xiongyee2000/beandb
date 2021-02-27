#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "./common.h"
#include "./Property.h"
#include "./Relation.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class Property;
class Relation;

class BeanWorld
{
public:
    BeanWorld();
    virtual ~BeanWorld();

    void clear();

    /**
     *  Create a bean.
     * 
     * @return the created bean
     */
    Bean *createBean();

    /**
     * Remove a bean.
     * 
     * @param id the id of the bean
     */
    void removeBean(oidType id);

    /**
     * Get total number of beans.
     * 
     * @return the total number of beans
     */
    int getNumOfBeans();

    /**
     * Get all beans.
     * 
     * @return a map containing all beans
     */
    const std::unordered_map<oidType, Bean*>& getBeans();

    /**
     * Get bean by id.
     * 
     * @param id the bean id
     * @return the bean, or nullptr
     */
    Bean* getBean(oidType id);

    /**
     * Get the id of a property. Later the property id 
     * can be used in other methods.
     * 
     * It is highly encouraged to use property id, for it
     * can reduce a lot of string calculation and achieve 
     * higher performance.
     *  
     * @param property name
     * @return property id
     */
    pidType getPropertyId(const char* name) const;

    /**
     * Get property by id.
     * 
     * @param id property id
     * @return property
     */
    const Property* getProperty(pidType id) const;
    Property* getProperty(pidType id);

    /**
     * Get property by name.
     * 
     * @param name property name
     * @return property
     */
    const Property* getProperty(const char* name) const;
    Property* getProperty(const char* name);

    /**
     * Get all properties.
     * 
     * @return a map containing all properties.
     */
    const std::unordered_map<std::string, pidType>& getProperties() const 
     {return m_propertyMap_;};

    /**
     * Get the id of a relation. Later the relation id 
     * can be used in other methods.
     * 
     * It is highly encouraged to use id over name, for it
     * can reduce a lot of string calculation and achieve 
     * higher performance.
     *  
     * @param name relation name
     * @return relation id
     */
    pidType getRelationId(const char* name) const;

    /**
     * Get relation by id.
     * 
     * @param id relation id
     * @return the relation, or null
     */
    const Relation* getRelation(pidType id) const;
    Relation* getRelation(pidType id);

    /**
     * Get relation by name.
     * 
     * @param name relation name
     * @return the relation, or null
     */
    const Relation* getRelation(const char* name) const;
    Relation* getRelation(const char* name);

    /**
     * Get all relations.
     * 
     * @return a map containing all relations.
     */
    const std::unordered_map<std::string, pidType>& getRelations() const 
     {return m_relationMap_;};

    /**
     * Find beans whose property values are equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are less equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are greater equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are less than the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are greater than the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

private:
    pidType setProperty( Bean* bean, const char* name, const Json::Value&  value);
    void setProperty( Bean* bean, pidType id, const Json::Value&  value);

    int setRelation(const char* name, Bean* from, Bean* to);
    void setRelation(pidType id, Bean* from, Bean* to);

    template<typename T>
    pidType addProperty(const char* name, 
        std::vector<T*>&properties, 
        std::unordered_map<std::string, pidType>& propertyMap);
    // pidType addProperty(const char* name);

    template<typename T>
    void removeProperty(T* property, 
        std::vector<T*>&properties, 
        std::unordered_map<std::string, pidType>& propertyMap);
    // void removeProperty(Property* property);

    template<typename T>
    void doSetProperty( Bean* bean, T* property, const Json::Value&  value);

    Json::Value removeProperty(Bean* bean, Property* property);


    void setRelation( Bean* bean, pidType id, const Json::Value&  value);
    void doSetRelation( Bean* bean, Property* property, const Json::Value&  value);

    void recreateIndex(Property* property);

    void findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    oidType generateBeanId();

private:
    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 0;

    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, pidType> m_propertyMap_; 

    std::vector<Relation*> m_relations_;
    //map from relation name to index
    std::unordered_map<std::string, pidType> m_relationMap_; 

friend class Bean;
friend class Property;
friend class Relation;
};

}
}
}