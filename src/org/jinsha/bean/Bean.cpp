
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

using namespace std;

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


bool Bean::isMember (const char* key) const
{
    if (key == nullptr) return false;
    const Json::Value& v = m_jsonValue_.get(key, Json::Value::null);
    return (v != Json::Value::null);
}


bool Bean::isMember (const string& key) const
{
    return isMember(key.c_str());
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


int Bean::setProperty(const char* name,  const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (value.isNull()) return -1;
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