
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

Bean::Bean(BeanWorld* world) : Value(Json::ValueType::objectValue), m_world_(world)
{

}

Bean::~Bean()
{

}

int Bean::setProperty(const char* name,  const Json::Value& value)
{
    return m_world_->setProperty(this, name, value);
}

void Bean::removeProperty(const char* name)
{
    return m_world_->removeProperty(this, name);
}

}
}
}