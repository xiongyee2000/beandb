
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

Bean::Bean(BeanWorld* world) : 
jsonValue_(Json::ValueType::objectValue), 
m_world_(world)
{

}


Bean::~Bean()
{
    clear();
}


Json::Value Bean::removeMember (const char *key)
{
    return m_world_->removeProperty(this, key);
}


Json::Value Bean::removeMember (const std::string &key)
{
    return removeMember(key.c_str());
}


void Bean::clear()
{
    m_world_->removeBean(this);
}

// int Bean::setProperty(const char* name,  const Json::Value& value)
// {
//     return m_world_->setProperty(this, name, value);
// }


int Bean::setProperty(const char* name, Json::Int value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, Json::UInt value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, Json::Int64 value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, Json::UInt64 value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, bool value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, double value)
{
    return m_world_->setProperty(this, name, value);
}

int Bean::setProperty(const char* name, const char* value)
{
    return m_world_->setProperty(this, name, value);
}

}
}
}