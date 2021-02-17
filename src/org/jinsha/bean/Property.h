#pragma once

#include <string>
#include <list>
#include <unordered_map>

#include "common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

class Property
{
public:
    /**
     * Constructor
     * 
     * @param name property name (similar to json key)
     */
    Property(const std::string& name) : m_name_(name) {};
    Property(const char* name) : m_name_(name) {};
    virtual ~Property();

    /**
     * Get the name of the property
     * 
     * @return property name (key)
     */
    const std::string& getName() const {return m_name_;};

private:
    void clear();

private:
    void addIndex(Bean* bean, const Json::Value& value);
    bool removeIndex(Bean* bean, const Json::Value& value);

    void findCommon(int type, const Json::Value& value, std::list<Bean*>& beans) const; 

private:
    std::string m_name_;
    pidType m_pid_ = -1;
    unsigned int m_refCount_ = 0;
    bool m_indexed_ = false;
    std::unordered_map<oidType, Bean*> m_trueValueMap_;
    std::unordered_map<oidType, Bean*> m_falseValueMap_;
    std::multimap<int_t, Bean*> m_intValueMap_;
    std::multimap<uint_t, Bean*> m_uintValueMap_;
    std::multimap<double, Bean*> m_doubleValueMap_;

     struct StrComparator
    {
        bool operator()(const char* const & a, const char* const & b) const;
    };
    std::multimap<const char*, Bean*, StrComparator> m_strValueMap_;


friend class BeanWorld;
};

}
}
}