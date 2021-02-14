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
    Property(const std::string& name) : m_name_(name) {};
    Property(const char* name) : m_name_(name) {};
    virtual ~Property();

    const std::string& getName() const {return m_name_;};

private:
    void clear();

private:
    void addIndex(Bean* bean, Json::Int value);
    void addIndex(Bean* bean, Json::UInt value);
    void addIndex(Bean* bean, Json::Int64 value);
    void addIndex(Bean* bean, Json::UInt64 value);
    void addIndex(Bean* bean, double value);
    void addIndex(Bean* bean, const char* value);
    void addIndex(Bean* bean, bool value);

    void removeIndex(Bean* bean, Json::Int value);
    void removeIndex(Bean* bean, Json::UInt value);
    void removeIndex(Bean* bean, Json::Int64 value);
    void removeIndex(Bean* bean, Json::UInt64 value);
    void removeIndex(Bean* bean,  double value);
    void removeIndex(Bean* bean, const char* value);
    void removeIndex(Bean* bean, bool value);
    void removeIndex(Bean* bean, const Json::Value& value);

    void findEqual(const Json::Value& value, std::list<Bean*>& beans) const; 
    void findLessEqual(const Json::Value& value, std::list<Bean*>& beans) const;
    void findGreaterEqual(const Json::Value& value, std::list<Bean*>& beans) const;
    void findLessThan(const Json::Value& value, std::list<Bean*>& beans) const;
    void findGreaterThan(const Json::Value& value, std::list<Bean*>& beans) const;
    void findCommon(int type, const Json::Value& value, std::list<Bean*>& beans) const; 

private:
    std::string m_name_;
    unsigned int m_refCount_ = 0;
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