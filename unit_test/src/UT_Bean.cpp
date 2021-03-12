

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

#include "./common.h"

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

    init_world(world);
    
    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(nullptr));

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(!bean1->isMember(p1));
    bean1->setProperty(p1, 1);
    EXPECT_TRUE(bean1->isMember(p1));
    EXPECT_TRUE(bean1->getMemberNames().size() ==1);

    EXPECT_TRUE(!bean1->isMember(p2));
    bean1->setProperty(p2, 2);
    EXPECT_TRUE(bean1->isMember(p2));
    EXPECT_TRUE(bean1->getMemberNames().size() == 2);

    EXPECT_TRUE(!bean1->isMember(pArray_1));
    bean1->createArrayProperty(pArray_1);
    EXPECT_TRUE(bean1->isMember(pArray_1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 3);

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(r1));
    bean1->setRelation(r1, nullptr);
    EXPECT_TRUE(!bean1->isMember(r1));
    bean1->setRelation(r1, bean2);
    EXPECT_TRUE(bean1->isMember(r1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 4);

    EXPECT_TRUE(!bean1->isMember(rArray_1));
    bean1->createArrayRelation(rArray_1);
    EXPECT_TRUE(bean1->isMember(rArray_1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 5);
}

TEST(Bean, create_has_remove)
{
    BeanWorld world;
    Value value;
    int errCode = 0;
    
    init_world(world);

    Bean* bean1 = world.createBean();

    errCode = bean1->createArrayProperty(nullptr);
    EXPECT_TRUE(errCode == -2);

    EXPECT_TRUE(!bean1->hasProperty(nullptr));
    EXPECT_TRUE(!bean1->hasArrayProperty(nullptr));
    EXPECT_TRUE(!bean1->hasRelation(nullptr));
    EXPECT_TRUE(!bean1->hasArrayRelation(nullptr));

    EXPECT_TRUE(!bean1->hasProperty(p1));
    bean1->setProperty(p1, 1);
    EXPECT_TRUE(bean1->hasProperty(p1));
    EXPECT_TRUE(!bean1->hasArrayProperty(p1));
    EXPECT_TRUE(!bean1->hasRelation(p1));
    EXPECT_TRUE(!bean1->hasArrayRelation(p1));

    EXPECT_TRUE(!bean1->hasArrayProperty(pArray_1));
    errCode = bean1->createArrayProperty(pArray_1);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(!bean1->hasProperty(pArray_1));
    EXPECT_TRUE(bean1->hasArrayProperty(pArray_1));
    EXPECT_TRUE(!bean1->hasRelation(pArray_1));
    EXPECT_TRUE(!bean1->hasArrayRelation(pArray_1));

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->hasRelation(r1));
    bean1->setRelation(r1, nullptr);
    EXPECT_TRUE(!bean1->hasRelation(r1));
    bean1->setRelation(r1, bean2);
    EXPECT_TRUE(!bean1->hasProperty(r1));
    EXPECT_TRUE(!bean1->hasArrayProperty(r1));
    EXPECT_TRUE(bean1->hasRelation(r1));
    EXPECT_TRUE(!bean1->hasArrayRelation(r1));

    EXPECT_TRUE(!bean1->hasArrayRelation(rArray_1));
    bean1->createArrayRelation(rArray_1);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(!bean1->hasProperty(rArray_1));
    EXPECT_TRUE(!bean1->hasArrayProperty(rArray_1));
    EXPECT_TRUE(!bean1->hasRelation(rArray_1));
    EXPECT_TRUE(bean1->hasArrayRelation(rArray_1));

    value = bean1->removeProperty(nullptr);
    EXPECT_TRUE(value.isNull());

    bean1->setProperty(p1, 1);
    value = bean1->removeProperty(p1);
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean1->hasProperty(p1) == false);

    bean1->createArrayProperty(pArray_1);
    bean1->appendProperty(pArray_1, 1);
    value = bean1->removeProperty(pArray_1);
    EXPECT_TRUE(value[0] == 1);
    EXPECT_TRUE(bean1->hasArrayProperty(p1) == false);

    auto memberSize = bean1->getMemberNames().size();
    bean1->removeRelation(nullptr);
    EXPECT_TRUE(memberSize == bean1->getMemberNames().size());

    bean1->setRelation(r1, bean2);
    EXPECT_TRUE(bean1->hasRelation(r1) == true);
    bean1->removeRelation(r1);
    EXPECT_TRUE(bean1->hasRelation(r1) == false);

    bean1->createArrayRelation(rArray_1);
    bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(bean1->hasArrayRelation(rArray_1) == true);
    bean1->removeRelation(rArray_1);
    EXPECT_TRUE(bean1->hasArrayRelation(rArray_1) == false);
}

TEST(Bean, property)
{
    BeanWorld world;
    Value value;
    int errCode = 0;

    init_world(world);

    world.defineProperty("p1", Property::IntType);

    Bean &bean = *world.createBean();

    errCode = bean.setProperty(p1, Json::Value());
    EXPECT_TRUE(errCode == -1);
    errCode = bean.setProperty(nullptr, 1);
    EXPECT_TRUE(errCode == -2);
    errCode = bean.setProperty(p1, 1.0);
    EXPECT_TRUE(errCode == -3);

    errCode = bean.setProperty(p1, Json::Value::minInt);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::minInt);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt);

    errCode = bean.setProperty(p1, Json::Value::minInt64);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::minInt64);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::UIntType);
    errCode = bean.setProperty(p1, Json::Value::maxUInt);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::maxUInt);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt);

    errCode = bean.setProperty(p1, Json::Value::maxUInt64);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::maxUInt64);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::RealType);
    errCode = bean.setProperty(p1, 8.8f);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == 8.8f);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == 8.8f);

    errCode = bean.setProperty(p1, 8.8);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getProperty(p1) == 8.8);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == 8.8);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::StringType);

    bean.setProperty(p1, "v1");
    EXPECT_TRUE(bean.getProperty(p1) == "v1");
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == "v1");

    bean.setProperty(p1, "v1");
    world.undefineProperty("p2");
    world.defineProperty("p2", Property::StringType);
    errCode = bean.setProperty(p2, "v2");
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 2);
    EXPECT_TRUE(bean.getProperty(p2) == "v2");

    value = bean.removeProperty(p1);
    EXPECT_TRUE(value == "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(p1).isNull());

    value = bean.removeProperty(p2);
    EXPECT_TRUE(value == "v2");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.getProperty(p2).isNull());

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::BoolType);
    errCode = bean.setProperty(p1, true);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(p1) == true);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == true);

    errCode = bean.setProperty(p1, false);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(p1) == false);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == false);

}


TEST(Bean, array_property)
{
    BeanWorld world;
    Json::Value value;
    Property* property;
    int errCode = 0;

    init_world(world);

    Bean &bean = *world.createBean();
    EXPECT_TRUE(bean.hasArrayProperty(pArray_1) == false);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 0);
    Value value2 = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value2.isNull());

    errCode = bean.appendProperty(pArray_1, Value::null);
    EXPECT_TRUE(errCode == -1);
    errCode = bean.appendProperty(nullptr, 0);
    EXPECT_TRUE(errCode == -2);

    errCode = bean.setProperty(pArray_1, 0, Value::null);
    EXPECT_TRUE(errCode == -1);
    errCode = bean.setProperty(nullptr, 0, 1);
    EXPECT_TRUE(errCode == -2);

    property = world.defineArrayProperty("pArray_1", Property::IntType);
    EXPECT_TRUE(property == world.getProperty("pArray_1"));
    errCode = bean.appendProperty(pArray_1, 1);
    EXPECT_TRUE(errCode == -4);
    errCode = bean.setProperty(pArray_1, 0, 1);
    EXPECT_TRUE(errCode == -4);

    //create empty array
    errCode = bean.createArrayProperty(pArray_1);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.hasArrayProperty(pArray_1) == true);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 0);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value.isNull());

    //append
    errCode = bean.appendProperty(pArray_1, "0");
    EXPECT_TRUE(errCode == -3);
    errCode = bean.appendProperty(pArray_1, 0);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 1);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value == 0);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value.isNull());
    errCode = bean.appendProperty(pArray_1, 1);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value == 1);

    //setArrayProperty
    errCode = bean.setProperty(pArray_1, 99, 99);
    EXPECT_TRUE(errCode == -5);
    errCode = bean.setProperty(pArray_1, 0, 99);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value == 99);
    errCode = bean.setProperty(pArray_1, 1, 999);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value == 999);

    // //resize
    // result = bean.resizeProperty(nullptr, 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty("", 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty("a", 1);
    // EXPECT_TRUE(result == -1);
    // result = bean.resizeProperty(pArray_1, 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 1);
    // result = bean.resizeProperty(pArray_1, 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 1);
    // value = bean.getArrayProperty(pArray_1, 0);
    // EXPECT_TRUE(value == 99);
    // result = bean.resizeProperty(pArray_1, 2);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 2);
    // value = bean.getArrayProperty(pArray_1, 0);
    // EXPECT_TRUE(value == 99);
    // value = bean.getArrayProperty(pArray_1, 1);
    // EXPECT_TRUE(value.isNull());
    // result = bean.resizeProperty(pArray_1, 1);
    // EXPECT_TRUE(result == 0);
    // EXPECT_TRUE(bean.getArrayPropertySize(pArray_1) == 1);
    // value = bean.getArrayProperty(pArray_1, 0);
    // EXPECT_TRUE(value == 99);

}

TEST(Bean, relation)
{
    BeanWorld world;
    Value value;
    int errCode = 0;

    init_world(world);

    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->getRelationBeanId(nullptr));

    EXPECT_TRUE(!bean1->getRelationBeanId(p1));
    bean1->setProperty(p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(p1));

    EXPECT_TRUE(!bean1->getRelationBeanId(pArray_1));
    bean1->createArrayProperty(pArray_1);
    EXPECT_TRUE(!bean1->getRelationBeanId(pArray_1));

    Bean* bean2 = world.createBean();

    errCode = bean1->setRelation(r1, nullptr);
    EXPECT_TRUE(errCode == -1);
    errCode = bean1->setRelation(nullptr, bean2);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->setRelation(p1, bean2);
    EXPECT_TRUE(errCode == -2);

    EXPECT_TRUE(!bean1->getRelationBeanId(r1));
    errCode = bean1->setRelation(r1, bean2);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(r1) == bean2->getId());

}

TEST(Bean, array_relation)
{
    BeanWorld world;
    Value value;
    int errCode = 0;
    
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    errCode = bean1->appendRelation(rArray_1, nullptr);
    EXPECT_TRUE(errCode == -1);
    errCode = bean1->appendRelation(nullptr, bean2);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->appendRelation(p1, bean2);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(errCode == -4);

    errCode = bean1->setRelation(rArray_1, 0, nullptr);
    EXPECT_TRUE(errCode == -1);
    errCode = bean1->setRelation(nullptr, 0, bean2);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->setRelation(p1, 0, bean2);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->setRelation(rArray_1, 0, bean2);
    EXPECT_TRUE(errCode == -4);

    EXPECT_TRUE(!bean1->getRelationBeanId(rArray_1, 0));
    errCode = bean1->createArrayRelation(nullptr);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->createArrayRelation(p1);
    EXPECT_TRUE(errCode == -2);
    errCode = bean1->createArrayRelation(rArray_1);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean1->getArrayRelationSize(rArray_1) == 0);

    errCode = bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean1->getArrayRelationSize(rArray_1) == 1);

    errCode = bean1->setRelation(rArray_1, 1, bean2);
    EXPECT_TRUE(errCode == -5);
    errCode = bean1->setRelation(rArray_1, 0, bean2);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(rArray_1, 0) == bean2->getId());
    errCode = bean1->setRelation(rArray_1, 0, bean3);
    EXPECT_TRUE(errCode == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(rArray_1, 0) == bean3->getId());

    bean1->setProperty(p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(p1, 0));
    EXPECT_TRUE(!bean1->getRelationBeanId(rArray_1, 1));

    //getArrayRelationSize()
    EXPECT_TRUE(bean1->getArrayRelationSize(nullptr) == 0);
    EXPECT_TRUE(bean1->getArrayRelationSize(p1) == 0);
}


