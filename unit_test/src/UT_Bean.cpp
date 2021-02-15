

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

TEST(Bean, setProperty_removeProperty)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;

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
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == Json::Value::minInt);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::minInt);

    pid = bean.setProperty("p1", Json::Value::maxUInt);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == Json::Value::maxUInt);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::maxUInt);

    pid = bean.setProperty("p1", Json::Value::minInt64);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == Json::Value::minInt64);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::minInt64);

    pid = bean.setProperty("p1", Json::Value::maxUInt64);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == Json::Value::maxUInt64);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == Json::Value::maxUInt64);

    pid = bean.setProperty("p1", 8.8f);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == 8.8f);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == 8.8f);

    pid = bean.setProperty("p1", 8.8);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == 8.8);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == 8.8);

    pid = bean.setProperty("p1", 'a');
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == 'a');
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == 'a');

    pid = bean.setProperty("p1", true);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == true);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == true);

    pid = bean.setProperty("p1", false);
    EXPECT_TRUE(pid == world.getPropertyId("p1"));
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == false);
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == false);

    bean.setProperty("p1", "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean["p1"] == "v1");
    value = bean.removeMember("p1");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.isMember("p1") == false);
    EXPECT_TRUE(value == "v1");

    bean.setProperty("p1", "v1");
    pid = bean.setProperty("p2", "v2");
    EXPECT_TRUE(pid == world.getPropertyId("p2"));
    EXPECT_TRUE(bean.getMemberNames().size() == 2);
    EXPECT_TRUE(bean["p2"] == "v2");

    value = bean.removeMember("p1");
    EXPECT_TRUE(value == "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.get("p1", Json::Value::null).isNull());

    value = bean.removeMember("p2");
    EXPECT_TRUE(value == "v2");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.get("p2",  Json::Value::null).isNull());
    
    value = bean.removeMember("null");
    EXPECT_TRUE(value.isNull());
}


TEST(Bean, removeMember)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;

    Bean& bean = *world.createBean();
    bean["p1"] = 1;
    value = bean.removeMember("p1");
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean.isMember("p1") == false);
}

TEST(Bean, isMember)
{
    BeanWorld world;
    Value value;
    pidType pid = 0;

    Bean& bean = *world.createBean();
    bean["p1"] = 1;
    bean.setProperty("p2", 2);
    
    EXPECT_TRUE(bean.isMember("p1"));
    EXPECT_TRUE(bean.isMember("p2"));
    EXPECT_TRUE(!bean.isMember("xxx"));
}


