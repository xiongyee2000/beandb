
#include <cstring>

#include "./BeanObject.h"

namespace org {
namespace jinsha {
namespace Bean {

int BeanObject::defineProperty(const std::string& name, BeanObject::PTYPE type)
{
    if (name.empty()) return -1;
    return defineProperty(name.c_str(), type);
}

int BeanObject::defineProperty(const char* name, BeanObject::PTYPE type)
{
    if (name == nullptr) return -1;
    if (type == BeanObject::INVALID) return -1;

    for (auto& iter  : propertyNames_)
    {
        if (iter.compare(name) == 0) return -2;
    }

    propertyNames_.push_back(name);
    propertyTypes_.push_back(type);
    propertyValues_.push_back(nullptr);

    return propertyNames_.size()  - 1;
}

void BeanObject::undefineProperty(const std::string& name)
{
    if (name.empty()) return;
    undefineProperty(name.c_str());
}

void BeanObject::undefineProperty(const char* name)
{
    if (name == nullptr) return;

    for (size_t i = 0; i < propertyNames_.size(); i++)
    {
        if (propertyNames_[i].compare(name) == 0) 
        {
            propertyNames_[i]  = "";
            propertyTypes_[i] =  BeanObject::INVALID;
            delete propertyValues_[i];
            propertyValues_[i] = nullptr;
        }
    }
}

int BeanObject::getPropertyId(const std::string& name)
{
    if (name.empty()) return -1;
    return getPropertyId(name.c_str());
}

int BeanObject::getPropertyId(const char* name)
{
    if (name == nullptr) return -1;

    for (size_t i = 0; i < propertyNames_.size(); i++)
    {
        if (propertyNames_[i].compare(name) == 0)
        {
            return i;
        }
    }
    return -1;
}

const char* BeanObject::getPropertyName(unsigned int pid)
{
    if (!propertyExists(pid)) return nullptr;
    return propertyNames_[pid].c_str();
}

int BeanObject::getPropertyType(unsigned int pid)
{
    if (!propertyExists(pid)) return -1;
    return propertyTypes_[pid];
}

int BeanObject::getBoolProperty(unsigned int pid, bool &value)
{
    return getProperty(pid, value, BeanObject::BOOL);
}

int BeanObject::getCharProperty(unsigned int pid, char &value)
{
    return getProperty(pid, value, BeanObject::CHAR);
}

int BeanObject::getUCharProperty(unsigned int pid, unsigned char &value)
{
    return getProperty(pid, value, BeanObject::UCHAR);
}

int BeanObject::getIntProperty(unsigned int pid, int &value)
{
    return getProperty(pid, value, BeanObject::INT);
}

int BeanObject::getUintProperty(unsigned int pid, unsigned int &value)
{
    return getProperty(pid, value, BeanObject::UINT);
}

int BeanObject::getLongProperty(unsigned int pid, long &value)
{
    return getProperty(pid, value, BeanObject::LONG);
}

int BeanObject::getULongProperty(unsigned int pid, unsigned long &value)
{
    return getProperty(pid, value, BeanObject::ULONG);
}

int BeanObject::getFloatProperty(unsigned int pid, float &value)
{
    return getProperty(pid, value, BeanObject::FLOAT);
}

int BeanObject::getDoubleProperty(unsigned int pid, double &value)
{
    return getProperty(pid, value, BeanObject::DOUBLE);
}

int BeanObject::getStrProperty(unsigned int pid, std::string** value)
{
    return getProperty(pid, value, BeanObject::STR);
}

int BeanObject::getBeanProperty(unsigned int pid, BeanObject** value)
{
    return getProperty(pid, value, BeanObject::BEAN);
}

int BeanObject::setBoolProperty(unsigned int pid, bool value)
{
    return setProperty(pid, value, BeanObject::BOOL);
}

int BeanObject::setCharProperty(unsigned int pid, char value)
{
    return setProperty(pid, value, BeanObject::CHAR);
}

int BeanObject::setUCharProperty(unsigned int pid, unsigned char value)
{
    return setProperty(pid, value, BeanObject::UCHAR);
}

int BeanObject::setIntProperty(unsigned int pid, int value)
{
    return setProperty(pid, value, BeanObject::INT);
}

int BeanObject::setUintProperty(unsigned int pid, unsigned int value)
{
    return setProperty(pid, value, BeanObject::UINT);
}

int BeanObject::setLongProperty(unsigned int pid, long value)
{
    return setProperty(pid, value, BeanObject::LONG);
}

int BeanObject::setULongProperty(unsigned int pid, unsigned long value)
{
    return setProperty(pid, value, BeanObject::ULONG);
}

int BeanObject::setFloatProperty(unsigned int pid, float value)
{
    return setProperty(pid, value, BeanObject::FLOAT);
}

int BeanObject::setDoubleProperty(unsigned int pid, double value)
{
    return setProperty(pid, value, BeanObject::DOUBLE);
}

int BeanObject::setStringProperty(unsigned int pid, const std::string& value)
{
    return setProperty(pid, value, BeanObject::STR);
}

int BeanObject::setStrProperty(unsigned int pid, const char* value)
{
    std::string str(value);
    return setProperty(pid, str, BeanObject::STR);
}

int BeanObject::setBeanProperty(unsigned int pid, const BeanObject& value)
{
    return setProperty(pid, value, BeanObject::BEAN);
}

template<typename T>
int  BeanObject::getProperty(unsigned int pid, T& value,  PTYPE type)
{
    if (!propertyExists(pid)) return -1;
    if ( propertyTypes_[pid] != type) return -2;

    value = *((T*)propertyValues_[pid]);
    return 0;
}

template<typename T>
int  BeanObject::getProperty(unsigned int pid, T** value,  PTYPE type)
{
    if (pid >= propertyNames_.size()) return -1;
    if ( propertyTypes_[pid] != type) return -2;

    *value = (T*)(propertyValues_[pid]);
    return 0;
}

template<typename T>
int BeanObject::setProperty(unsigned int pid,  const T& value, PTYPE type)
{
    if (pid >= propertyNames_.size()) return -1;
    if ( propertyTypes_[pid] != type) return -2;

    T* v = new T;
    *v = value;
    propertyValues_[pid] = (void*)v;
    return 0;
}

bool BeanObject::propertyExists(unsigned int pid)
{
    if (pid >= propertyNames_.size()) return false;
    if ( propertyTypes_[pid] == BeanObject::INVALID) return false; //removed property
    return true;
}

// template<typename T>
// bool BeanObject::isValidPType(T& value)
// {
// }

}
}
}