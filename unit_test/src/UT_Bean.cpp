

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

    Bean *bean = new Bean(&world);
    EXPECT_TRUE(bean->getId() == 0);
}

TEST(Bean, setProperty_removeProperty)
{
    BeanWorld world;
    Value value;

    Bean *bean = new Bean(&world);

    bean->setProperty(nullptr, 1);
    EXPECT_TRUE(bean->getMemberNames().size() == 0);

    bean->setProperty("", 1);
    EXPECT_TRUE(bean->getMemberNames().size() == 0);

    bean->setProperty(string("").c_str(), 1);
    EXPECT_TRUE(bean->getMemberNames().size() == 0);

    bean->setProperty("p1", 8);
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    EXPECT_TRUE(bean->get("p1", value) == 8);
    bean->removeMember("p1");
    EXPECT_TRUE(bean->getMemberNames().size() == 0);
    EXPECT_TRUE(bean->get("p1", value).isNull());

    bean->setProperty("p1", (float)8.8);
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    // EXPECT_TRUE(bean->get("p1", value) == 8.8f);

    bean->setProperty("p1", (double)8.8);
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    // EXPECT_TRUE(bean->get("p1", value) == 8.8f);

    bean->setProperty("p1", 'a');
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    EXPECT_TRUE(bean->get("p1", value) == 'a');

    bean->setProperty("p1", true);
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    EXPECT_TRUE(bean->get("p1", value) == true);

    bean->setProperty("p1", "v1");
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    EXPECT_TRUE(bean->get("p1", value) == "v1");

    bean->setProperty("p2", "v2");
    EXPECT_TRUE(bean->getMemberNames().size() == 2);
    EXPECT_TRUE(bean->get("p2", value) == "v2");
    bean->removeMember("p1");
    EXPECT_TRUE(bean->getMemberNames().size() == 1);
    EXPECT_TRUE(bean->get("p1", value).isNull());
    bean->removeMember("p2");
    EXPECT_TRUE(bean->getMemberNames().size() == 0);
    EXPECT_TRUE(bean->get("p2", value).isNull());
}
