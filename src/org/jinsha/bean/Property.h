#pragma once

#include <string>

namespace org {
namespace jinsha {
namespace bean {

class BeanWorld;

class Property
{
public:
    Property(const std::string& name) : name_(name) {};
    Property(const char* name) : name_(name) {};
    virtual ~Property() {};

    const std::string& getName() const {return name_;};
    bool indexed() {return indexed_;};

private:
    std::string name_;
    bool indexed_ = false;

friend class BeanWorld;
};

}
}
}