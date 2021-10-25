#pragma once

#include <string>
#include <list>
#include <map>

#include "./common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

/**
 * In real world, many facts can be represented as following ternary 
 * tuples: <subject, predicate, object>. 
 * 
 * For example, Romeo loves Juliet can be represented as: 
 * 
 * <Romeo, loves, Juliet>
 * 
 * In this sense, the class Property is something like the poredicate in the  
 * semantic ternary tuple. It basically represents the relationship between
 * the subject and the object.
 */
class Property
{
public:
    enum Type
    {
        /**
         * the property value represents  a primary type (see  ValueType)
         */
        PrimaryType = 0,

        /**
         * the property is an array, the element of which is of ValueType
         */
        ArrayPrimaryType,

        /**
         * the property represents a relation between two beans
         */
        RelationType,

        /**
         * the property is an array of relations, the element of which is
         * a bean id
         */
        ArrayRelationType
    };

    enum ValueType
    {
        IntType = Json::intValue,
        UIntType = Json::uintValue,
        RealType = Json::realValue,
        StringType = Json::stringValue,
        BoolType = Json::booleanValue
    } ;

public:
    /**
     * Get the name of the property.
     * 
     * Notes:
     * - the property is unique across the bean world.
     * @return property name
     */
    const std::string& getName() const {return m_name_;};

    /**
     * If this property is an array property.
     * 
     * @return true if it is an array property, or false otherwise
     */
    bool isArray() const;

    /**
     * If this property is a relation.
     * 
     * @return true if it is a relation, or false otherwise
     */
    bool isRelation() const;

    /**
     * Get the id of the property
     * 
     * @return property id
     */
    pidType getId() const {return m_id_;};

    /**
     * Get the type of this property.
     */
    Type getType() const {return  m_propertyType_ ;};

    /**
     * Get the value type of this property.
     * 
     * This method is meaningful only for property of PrimaryType.
     */
    ValueType getValueType() const {return m_valueType_;};

    /**
     * If this property is a delay-load property.
     * 
     * The delay-load property is a property the value of which
     * won't be loaded from storage into memory (BeanWorld) 
     * at loading phase (inside BeanWorld::getBean()), but will be 
     * delayed to when getProperty() is called. 
     * 
     * The database backend provider shall determine whether a 
     * property should be a direct-load or delay-load property 
     * based on its implementation. This is purposed to achieve better 
     * performance. 
     * 
     * General speaking, a simple and fixed lengthed property 
     * (e.g. of int type) should be direct load, while a complex and 
     * dynamic lengthed property (e.g.string property) should be 
     * delay-load.
     * 
     * Note relation properties are designed to be direct-load.
     * 
     * @return true if this property is delay load property, or false.
     */
    bool isDelayLoad() const {return m_delayLoad_;};

////////////////////////////////////////////////////////////////
// All-load mode related features. 
// Make them private for now.
////////////////////////////////////////////////////////////////
private:
    /**
     * Is property indexed.
     * 
     * @return true if indexed, false otherwise
     */
    bool indexed() const {return m_indexed_;};

    /**
     * Create index for a property.
     * 
     * Notes:
     * - This method is aimed to improve time performance during search, 
     *   yet by using more memory space as a price. So create index when 
     *   it is really needed.
     * - This method will create index entries for all beans that have this 
     *   property. So it may result the time complexity of this method O(n) 
     *   where n is the number of beans in this world.
     * - Once created, the index will be updated each time when  the property 
     *    is set.
     * - You can call this method at any time. However If index is desired 
     *    for a property, It is strongly  sugguested to call this method right 
     *    after the property is dfined.
     * - Currently index is supported only on property of PrimaryType.
     * 
     * @return 0 if success, or  an error code
     *                   -1: unable to create index
     */
    int createIndex();

    /**
     * Remove index of a property.
     * 
     * Note this method will delete all index entries, which may result 
     * the time complexity of this method O(n) where n is the number 
     * of beans in this world.
     * 
     * @return 0 for success, or -1 if the index doesn't exist
     */
    int removeIndex();

    /**
     * Get number of subjects.
     */
    size_t getNumOfSubjects() const;

    /**
     * Get number of objects.
     * 
     * Notes:
     * - only used for property of RelationType/ArrayRelationType
     * - if this property is an array, all elements in the array will be  
     *    counted
     */
    size_t getNumOfObjects() const;

    /**
     * Find all subjects that have the given property/relation.
     * 
     * @param beans the results
     * 
     */
    void getSubjects(std::list<oidType>& beans) const;

    /**
     * Find all beans that act as objects of this property.
     * 
     * Notes:
     * - only used for property of RelationType/ArrayRelationType
     * 
     * @param beans the results
     * 
     */
    void getObjects(std::list<oidType>& beans) const;

    /**
     * Find beans (as subject) which have given relation (as predicate) 
     * to the given bean (as object).
     * 
     * @param objectId the id of the relation bean
     * @return a list containing ids of beans
     * 
     */
    void findSubjects(oidType objectId, std::list<oidType>& beans) const;

    /**
     * Find beans whose property values are equal to the given one.
     * 
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findEqual(const Json::Value& value, std::list<oidType>& beans) const;

    /**
     * Find beans whose property values are less equal to the given one.
     * 
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessEqual(const Json::Value& value, std::list<oidType>& beans) const;

    /**
     * Find beans whose property values are greater equal to the given one.
     * 
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterEqual(const Json::Value& value, std::list<oidType>& beans) const;

    /**
     * Find beans whose property values are less than the given one.
     * 
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessThan(const Json::Value& value, std::list<oidType>& beans) const;

    /**
     * Find beans whose property values are greater than the given one.
     * 
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterThan(const Json::Value& value, std::list<oidType>& beans) const;
////////////////////////////////////////////////////////////////
// End of load-all-into-memory related features.
////////////////////////////////////////////////////////////////


private:
    Property(BeanWorld* world, const char* name,  pidType id,
        Type type, ValueType valueType, bool delayLoad);

    virtual ~Property();

    void addSubject(oidType id); 
    void removeSubject(oidType id); 
    void addObject(oidType id); 
    void removeObject(oidType id); 

    void addIndex(Bean* bean, const Json::Value& value);
    bool removeIndex(Bean* bean, const Json::Value& value);

    void findCommon_(int type, const Json::Value& value, std::list<oidType>& beans) const;
    void trivialFind(int opType,  const Json::Value& value, std::list<oidType>& beans) const;

private:
    BeanWorld* m_world_;
    std::string m_name_;
    int m_id_ =  -1;
    Type m_propertyType_;
    ValueType m_valueType_ ;
    unsigned int m_refCount_ = 0;
    bool m_indexed_ = false;
    bool m_delayLoad_;
    
    //keep all subject beans for better performance
    std::map<oidType, unsigned int> m_subjectMap_; 

    //keep all object beans for better performance
    std::map<oidType, unsigned int> m_objectMap_; 

    //index for true values
    std::map<oidType, Bean*> m_trueValueMap_;

    //index for false values
    std::map<oidType, Bean*> m_falseValueMap_;

    //index for int values
    std::multimap<int_t, Bean*> m_intValueMap_;

    //index for uint values
    std::multimap<uint_t, Bean*> m_uintValueMap_;

    //index for double values
    std::multimap<double, Bean*> m_realValueMap_;

    //index for string values
     struct StrComparator
    {
        bool operator()(const char* const & a, const char* const & b) const;
    };
    std::multimap<const char*, Bean*, StrComparator> m_strValueMap_;


friend class BeanWorld;
friend class Bean;
friend class AbstractBeanDB;
};

}
}
}