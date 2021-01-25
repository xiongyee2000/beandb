#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace org {
namespace jinsha {
namespace Bean {

class BeanObject
{
public:
    enum PTYPE
    {
        INVALID = 0,
        BOOL = 10,
        CHAR,
        UCHAR,
        INT,
        UINT,
        LONG,
        ULONG,
        STR,
        FLOAT,
        DOUBLE,
        LIST = 100,
        BEAN = 200
    };

    long getClassId() {return classId_;};
    std::string getClassName() {return className_;};
    std::string getClassUri() {return classUri_;};

    unsigned long getId() {return id_;};
    std::string &getName() {return name_;};
    std::string &getUri() {return uri_;};

    void setClassId(long id) {classId_ = id;};
    void setClassName(const std::string& name) {className_ = name;};
    void setClassUri(const std::string& uri) {classUri_ = uri;};

    void setId(long id) {id_ = id;};
    void setName(const std::string& name) {name_ = name;};
    void setUri(const std::string& uri) {uri_ = uri;};

    /**
     * Define a property for this bean. 
     * 
     * @param name name of the property 
     * @param type type of the property
     * @return The  id of the property, or a negative number indicating 
     * error code:
     *      -1 if name is nullptr or empty, or type is invalid
     *      -2 if the property already exists
     */
    int defineProperty(const std::string& name, BeanObject::PTYPE type);
    int defineProperty(const char* name, BeanObject::PTYPE type);

    /**
     * Undefine a property for this bean. 
     * 
     * Note the property value will be removed once the property is 
     * undefined.
     * 
     * @param name name of the property 
     */
    void undefineProperty(const std::string& name); 
    void undefineProperty(const char* name); 

    /**
     * Get property id by name. 
     * 
     * @param name name of the property 
     * @return The  id of the property, or -1 if the property does not exist.
     */
    int getPropertyId(const std::string& name);
    int getPropertyId(const char* name);

    /**
     * Get property name by id. 
     * 
     * @param pid id  of the property 
     * @return The name of the property, or nullptr if the property
     * doesn't exist.
     */
    const char* getPropertyName(unsigned int pid);

    /**
     * Get property type. 
     * 
     * @param pid id  of the property
     * @return the type of the property, or INVALID if the property does not exist.
     */
    int getPropertyType(unsigned int pid);

    /**
     * Get property value.
     * 
     * @param pid the id of the property
     * @param value the value of the property
     * @return error code: 0 for success, others for errors:
     * error code:
     * -1: property does not exist
     * -2: inconsistent property type
     */
    int getBoolProperty(unsigned int pid, bool &value);
    int getCharProperty(unsigned int pid, char &value);
    int getUCharProperty(unsigned int pid, unsigned char &value);
    int getIntProperty(unsigned int pid, int &value);
    int getUintProperty(unsigned int pid, unsigned int &value);
    int getLongProperty(unsigned int pid, long &value);
    int getULongProperty(unsigned int pid, unsigned long &value);
    int getFloatProperty(unsigned int pid, float &value);
    int getDoubleProperty(unsigned int pid, double &value);

    /**
     * Same as primative getXXXProperty() but note value is managed
     * by the bean obect so don't delete it!
     */
    int getStrProperty(unsigned int pid, std::string** value);

    /**
     * Same as primative getXXXProperty() but note value is managed 
     * by the bean obect so don't delete it!
     */
    int getBeanProperty(unsigned int pid, BeanObject** value);

    /**
     * Set property value.
     * 
     * @param pid the id of the property
     * @param value the value of the property
     * @return error code: 0 for success, others for errors
     */
    int setBoolProperty(unsigned int pid, bool value);
    int setCharProperty(unsigned int pid, char value);
    int setUCharProperty(unsigned int pid, unsigned char value);
    int setIntProperty(unsigned int pid, int value);
    int setUintProperty(unsigned int pid, unsigned int value);
    int setLongProperty(unsigned int pid, long value);
    int setULongProperty(unsigned int pid, unsigned long value);
    int setFloatProperty(unsigned int pid, float value);
    int setDoubleProperty(unsigned int pid, double value);
    int setStringProperty(unsigned int pid, const std::string& value);
    int setStrProperty(unsigned int pid, const char* value);
    int setBeanProperty(unsigned int pid, const BeanObject& value);
    // int setStrProperty(unsigned int pid, const char* value);

private:
    // template<typename T>
    // static bool isValidPType(T& value);

    bool propertyExists(unsigned int pid);

template<typename T>
int  getProperty(unsigned int pid, T& value, PTYPE type);
template<typename T>
int  getProperty(unsigned int pid, T** value, PTYPE type);

template<typename T>
int  setProperty(unsigned int pid, const T& value, PTYPE type);

    long classId_ = 0;
    std::string className_;
    std::string classUri_;

    unsigned long id_ = 0;
    std::string name_;
    std::string uri_;

    std::vector<std::string> propertyNames_;
    std::vector<int> propertyTypes_;
    std::vector<void*> propertyValues_;

    std::unordered_map<int, long> longPropertyMap_;
};

}
}
}