
#include <cstring>

#include "./Bean.h"
#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

// int Bean::setProperty(const std::string& name, Json::ValueType type)
// {
//     if (name.empty()) return -1;
//     return setProperty(name.c_str(), type);
// }

int Bean::setProperty(const char* name,  Json::Value::Int64 value)
{
    if (name == nullptr) return -1;
    if (*name == 0) return -1;

    //set value for json object first
     (*this)[name] = Json::Value(value);

    if (m_world_->hasProperty(name)) //bean world has such property
    {
        //todo: determine type
    }
    else
    {
        //add this property to the bean world
        m_world_->addProperty(name);
    }

    return 0;
}

// void Bean::removeProperty(const std::string& name)
// {
//     if (name.empty()) return;
//     removeProperty(name.c_str());
// }

void Bean::removeProperty(const char* name)
{
    if (name == nullptr) return;
    if (*name == 0) return;

    //remove member of json object first
    (*this).removeMember(name);

    //todo: remove property from bean world
}

}
}
}