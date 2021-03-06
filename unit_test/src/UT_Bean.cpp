

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/BeanWorld.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

TEST(Bean, Constructor)
{
    BeanWorld world;

    Bean *bean = new Bean(&world);
    EXPECT_TRUE(nullptr != bean);
}

TEST(Bean, getId)
{
    BeanWorld world;

    Bean *bean1 = world.createBean();
    Bean *bean2 = world.createBean();
    EXPECT_TRUE(bean1->getId() >= 0);
    EXPECT_TRUE(bean2->getId() >= 0);
    EXPECT_TRUE(bean1->getId() != bean2->getId());
}

TEST(Bean, isMember)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;
    
    world.defineProperty("p1", Property::IntType);
    world.defineProperty("p2", Property::IntType);
    world.defineArrayProperty("array_1", Property::IntType);
    world.defineRelation("relation_1");
     world.defineArrayRelation("relationArray_1");
    
    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(nullptr));
    EXPECT_TRUE(!bean1->isMember(""));

    EXPECT_TRUE(!bean1->isMember("p1"));
    // EXPECT_TRUE(!bean1->isMember(string("p1")));
    bean1->setProperty("p1", 1);
    EXPECT_TRUE(bean1->isMember("p1"));
    // EXPECT_TRUE(bean1->isMember(string("p1")));

    EXPECT_TRUE(!bean1->isMember("p2"));
    bean1->setProperty("p2", 2);
    EXPECT_TRUE(bean1->isMember("p2"));

    EXPECT_TRUE(!bean1->isMember("array_1"));
    bean1->createArrayProperty("array_1");
    EXPECT_TRUE(bean1->isMember("array_1"));

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->isMember("relation_1"));
    bean1->setRelation("relation_1", nullptr);
    EXPECT_TRUE(!bean1->isMember("relation_1"));
    bean1->setRelation("relation_1", bean2);
    EXPECT_TRUE(bean1->isMember("relation_1"));

    EXPECT_TRUE(!bean1->isMember("relationArray_1"));
    bean1->createArrayRelation("relationArray_1");
    EXPECT_TRUE(bean1->isMember("relationArray_1"));

}

TEST(Bean, hasProperty_hasRelation)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;
    
    world.defineProperty("p1", Property::IntType);
    world.defineProperty("p2", Property::IntType);
    world.defineArrayProperty("array_1", Property::IntType);
    world.defineRelation("relation_1");
     world.defineArrayRelation("relationArray_1");

    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->hasProperty(nullptr));
    EXPECT_TRUE(!bean1->hasProperty(""));
    EXPECT_TRUE(!bean1->hasArrayProperty(nullptr));
    EXPECT_TRUE(!bean1->hasArrayProperty(""));
    EXPECT_TRUE(!bean1->hasRelation(nullptr));
    EXPECT_TRUE(!bean1->hasRelation(""));
    EXPECT_TRUE(!bean1->hasArrayRelation(nullptr));
    EXPECT_TRUE(!bean1->hasArrayRelation(""));

    EXPECT_TRUE(!bean1->hasProperty("p1"));
    bean1->setProperty("p1", 1);
    EXPECT_TRUE(bean1->hasProperty("p1"));
    EXPECT_TRUE(!bean1->hasArrayProperty("p1"));
    EXPECT_TRUE(!bean1->hasRelation("p1"));
    EXPECT_TRUE(!bean1->hasArrayRelation("p1"));

    EXPECT_TRUE(!bean1->hasArrayProperty("array_1"));
    bean1->createArrayProperty("array_1");
    EXPECT_TRUE(!bean1->hasProperty("array_1"));
    EXPECT_TRUE(bean1->hasArrayProperty("array_1"));
    EXPECT_TRUE(!bean1->hasRelation("array_1"));
    EXPECT_TRUE(!bean1->hasArrayRelation("array_1"));

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->hasRelation("relation_1"));
    bean1->setRelation("relation_1", nullptr);
    EXPECT_TRUE(!bean1->hasRelation("relation_1"));
    bean1->setRelation("relation_1", bean2);
    EXPECT_TRUE(!bean1->hasProperty("relation_1"));
    EXPECT_TRUE(!bean1->hasArrayProperty("relation_1"));
    EXPECT_TRUE(bean1->hasRelation("relation_1"));
    EXPECT_TRUE(!bean1->hasArrayRelation("relation_1"));

    EXPECT_TRUE(!bean1->hasArrayRelation("relationArray_1"));
    bean1->createArrayRelation("relationArray_1");
    EXPECT_TRUE(!bean1->hasProperty("relationArray_1"));
    EXPECT_TRUE(!bean1->hasArrayProperty("relationArray_1"));
    EXPECT_TRUE(!bean1->hasRelation("relationArray_1"));
    EXPECT_TRUE(bean1->hasArrayRelation("relationArray_1"));

}

TEST(Bean, setProperty_removeProperty_pid)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;

    world.defineProperty("p1", Property::IntType);

    Bean &bean = *world.createBean();

    pid = bean.setProperty(-1, 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);

    pid = bean.setProperty(999, 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);

    pid = bean.setProperty("p1", Json::Value::minInt);
    pid = bean.setProperty(pid, 1);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == 1);
}

TEST(Bean, setProperty_removeProperty)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;

    world.defineProperty("p1", Property::IntType);

    Bean &bean = *world.createBean();

    pid = bean.setProperty(nullptr, 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);

    pid = bean.setProperty("", 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);

    // pid = bean.setProperty(string(""), 1);
    // EXPECT_TRUE(pid == -1);
    // EXPECT_TRUE(bean.getMemberNames().size() == 0);

    pid = bean.setProperty("p1", Json::Value::minInt);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == Json::Value::minInt);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::minInt);

    pid = bean.setProperty("p1", Json::Value::minInt64);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == Json::Value::minInt64);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::minInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::UIntType);
    pid = bean.setProperty("p1", Json::Value::maxUInt);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == Json::Value::maxUInt);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::maxUInt);

    pid = bean.setProperty("p1", Json::Value::maxUInt64);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == Json::Value::maxUInt64);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::maxUInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::RealType);
    pid = bean.setProperty("p1", 8.8f);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == 8.8f);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == 8.8f);

    pid = bean.setProperty("p1", 8.8);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == 8.8);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == 8.8);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::StringType);

    bean.setProperty("p1", "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == "v1");
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == "v1");

    bean.setProperty("p1", "v1");
    world.defineProperty("p2", Property::StringType);
    pid = bean.setProperty("p2", "v2");
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 2);
    EXPECT_TRUE(bean.getProperty("p2") == "v2");

    value = bean.removeProperty("p1");
    EXPECT_TRUE(value == "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1").isNull());

    value = bean.removeProperty("p2");
    EXPECT_TRUE(value == "v2");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.getProperty("p2").isNull());

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::BoolType);
    pid = bean.setProperty("p1", true);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == true);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == true);

    pid = bean.setProperty("p1", false);
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty("p1") == false);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == false);
    
    value = bean.removeProperty("null");
    EXPECT_TRUE(value.isNull());
}

TEST(Bean, removeProperty)
{
    BeanWorld world;
    Value value;
    // pidType pid = 0;
    Bean& bean = *world.createBean();

    value = bean.removeProperty(nullptr);
    EXPECT_TRUE(value.isNull());

    value = bean.removeProperty("");
    EXPECT_TRUE(value.isNull());

    value = bean.removeProperty("p1");
    EXPECT_TRUE(value.isNull());
    EXPECT_TRUE(bean.isMember("p1") == false);

    world.defineProperty("p1", Property::IntType);
    
    bean.setProperty("p1", 1);
    value = bean.removeProperty("p1");
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean.isMember("p1") == false);
}

TEST(Bean, removeProperty_pid)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;
    Bean& bean = *world.createBean();

    value = bean.removeProperty(-1);
    EXPECT_TRUE(value.isNull());

    value = bean.removeProperty(999);
    EXPECT_TRUE(value.isNull());

    world.defineProperty("p1", Property::IntType);
    
    pid = bean.setProperty("p1", 1);
    value = bean.removeProperty(pid);
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean.isMember("p1") == false);

    value = bean.removeProperty(pid);
    EXPECT_TRUE(value.isNull());

    value = bean.removeProperty(world.getPropertyId("p1"));
    EXPECT_TRUE(value.isNull());
}


TEST(Bean, array_property)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;
    int result = 0;

    Bean &bean = *world.createBean();
    EXPECT_TRUE(bean.hasArrayProperty("array_1") == false);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 0);
    value = bean.getArrayProperty("array_1", 0);
    EXPECT_TRUE(value.isNull());

    pid = world.defineArrayProperty("array_1", Property::IntType);
    EXPECT_TRUE(pid >= 0);
    pid = bean.setArrayProperty("array_1", 0, 1);
    EXPECT_TRUE(pid == -5);

    //create empty array
    pid = bean.createArrayProperty("array_1");
    EXPECT_TRUE(pid == 0);
    EXPECT_TRUE(bean.hasArrayProperty("array_1") == true);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 0);
    value = bean.getArrayProperty("array_1", 0);
    EXPECT_TRUE(value.isNull());

    //append
    result = bean.appendProperty(nullptr, 0);
    EXPECT_TRUE(result == -1);
    result = bean.appendProperty("", 0);
    EXPECT_TRUE(result == -1);
    result = bean.appendProperty("a", 0);
    EXPECT_TRUE(result == -1);
    result = bean.appendProperty("array_1", "0");
    EXPECT_TRUE(result == -3);
    result = bean.appendProperty("array_1", Json::Value::null);
    EXPECT_TRUE(result == -2);
    result = bean.appendProperty("array_1", 0);
    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 1);
    value = bean.getArrayProperty("array_1", 0);
    EXPECT_TRUE(value == 0);
    value = bean.getArrayProperty("array_1", 1);
    EXPECT_TRUE(value.isNull());
    result = bean.appendProperty("array_1", 1);
    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 2);
    value = bean.getArrayProperty("array_1", 1);
    EXPECT_TRUE(value == 1);

    //setProperty
    result = bean.setArrayProperty(nullptr, 0, 99);
    EXPECT_TRUE(result == -1);
    result = bean.setArrayProperty("", 0, 99);
    EXPECT_TRUE(result == -1);
    result = bean.setArrayProperty("array", 0, 99);
    EXPECT_TRUE(result == -1);
    result = bean.setArrayProperty("array_1", 0, Json::Value::null);
    EXPECT_TRUE(result == -2);
    result = bean.setArrayProperty("array_1", 0, "99");
    EXPECT_TRUE(result == -3);
    result = bean.setArrayProperty("array_1", 2, 99);
    EXPECT_TRUE(result == -4);
    result = bean.setArrayProperty("array_1", 0, 99);
    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 2);
    value = bean.getArrayProperty("array_1", 0);
    EXPECT_TRUE(value == 99);
    result = bean.setArrayProperty("array_1", 1, 999);
    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 2);
    value = bean.getArrayProperty("array_1", 1);
    EXPECT_TRUE(value == 999);

    // //resize
    // result = bean.resizeProperty(nullptr, 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty("", 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty("a", 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty("array_1", 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 1);
    // result = bean.resizeProperty("array_1", 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 1);
    // value = bean.getArrayProperty("array_1", 0);
    // EXPECT_TRUE(value == 99);
    // result = bean.resizeProperty("array_1", 2);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 2);
    // value = bean.getArrayProperty("array_1", 0);
    // EXPECT_TRUE(value == 99);
    // value = bean.getArrayProperty("array_1", 1);
    // EXPECT_TRUE(value.isNull());
    // result = bean.resizeProperty("array_1", 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize("array_1") == 1);
    // value = bean.getArrayProperty("array_1", 0);
    // EXPECT_TRUE(value == 99);

}



