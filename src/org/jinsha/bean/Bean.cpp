
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

Bean::Bean(BeanWorld* world) : 
m_jsonValue_(Json::ValueType::objectValue), 
m_world_(world)
{

}


Bean::~Bean()
{
    clear();
}


Json::Value Bean::removeMember (const char *key)
{
    return removeProperty(key);
}


Json::Value Bean::removeMember (const std::string &key)
{
    return removeMember(key.c_str());
}


void Bean::clear()
{
    for (auto& memberName : m_jsonValue_.getMemberNames())
    {
        removeMember(memberName);
    }
}

// int Bean::setProperty(const char* name,  const Json::Value& value)
// {
//     return m_world_->setProperty(this, name, value);
// }


pidType Bean::setProperty(const char* name, Json::Int value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, Json::UInt value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, Json::Int64 value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, Json::UInt64 value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, bool value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, double value)
{
    return m_world_->setProperty(this, name, value);
}

pidType Bean::setProperty(const char* name, const char* value)
{
    return m_world_->setProperty(this, name, value);
}

Json::Value Bean::removeProperty( const char* name)
{
    return m_world_->removeProperty(this, name);
}

// class Iterator
// {
// public:
//     Iterator& begin();
//     Iterator& end();
//     Bean& operator*();
//     Bean* operator&();
//     Iterator& operator++();
//     Iterator& operator--();

// private:
//     Iterator(const Iterator& other) = delete;
//     Iterator& operator=(const Iterator& other) = delete;
//     static const Iterator end_;
//     std::list<Bean*> items_;
// }

}
}
}