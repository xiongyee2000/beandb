
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


void Bean::clear()
{
    for (auto& memberName : m_jsonValue_.getMemberNames())
    {
        removeProperty(memberName.c_str());
    }
}


int Bean::setProperty(const char* name,  const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    if (value.isNull()) return -1;
    return m_world_->setProperty(this, name, value);
}


pidType Bean::setProperty(pidType pid,  const Json::Value& value)
{
    if (value.isNull()) return -1;
    Property* property = (Property*)m_world_->getProperty(pid);
    if (property == nullptr) return -1;
    m_world_->setProperty(this, pid, value);
    return pid;
}


Json::Value Bean::removeProperty( const char* name)
{
    return m_world_->removeProperty(this, name);
}


Json::Value Bean::removeProperty(pidType pid)
{
    if (pid < 0 || (size_t)pid >= m_world_->m_properties_.size()) 
        return Json::Value::null;
    else
        return m_world_->removeProperty(this, pid);
        
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