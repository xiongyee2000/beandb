

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
    int err = 0;
    
    init_world(world);

    Bean* bean1 = world.createBean();

    err = bean1->createArrayProperty(nullptr);
    EXPECT_TRUE(err == -2);

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
    err = bean1->createArrayProperty(pArray_1);
    EXPECT_TRUE(err == 0);
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
    EXPECT_TRUE(err == 0);
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
    bean1->removeProperty(nullptr);
    EXPECT_TRUE(memberSize == bean1->getMemberNames().size());

    bean1->setRelation(r1, bean2);
    EXPECT_TRUE(bean1->hasRelation(r1) == true);
    bean1->removeProperty(r1);
    EXPECT_TRUE(bean1->hasRelation(r1) == false);

    bean1->createArrayRelation(rArray_1);
    bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(bean1->hasArrayRelation(rArray_1) == true);
    bean1->removeProperty(rArray_1);
    EXPECT_TRUE(bean1->hasArrayRelation(rArray_1) == false);
}

TEST(Bean, property)
{
    BeanWorld world;
    Value value;
    int err = 0;

    init_world(world);

    world.defineProperty("p1", Property::IntType);

    Bean &bean = *world.createBean();

    err = bean.setProperty(p1, Json::Value());
    EXPECT_TRUE(err == -1);
    err = bean.setProperty(nullptr, 1);
    EXPECT_TRUE(err == -2);
    err = bean.setProperty(p1, 1.0);
    EXPECT_TRUE(err == -3);

    err = bean.setProperty(p1, Json::Value::minInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::minInt);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt);

    err = bean.setProperty(p1, Json::Value::minInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::minInt64);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::UIntType);
    err = bean.setProperty(p1, Json::Value::maxUInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::maxUInt);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt);

    err = bean.setProperty(p1, Json::Value::maxUInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(p1) == Json::Value::maxUInt64);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt64);

    world.undefineProperty("p1");
    world.defineProperty("p1", Property::RealType);
    err = bean.setProperty(p1, 8.8f);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(p1) == 8.8f);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == 8.8f);

    err = bean.setProperty(p1, 8.8);
    EXPECT_TRUE(err == 0);
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
    err = bean.setProperty(p2, "v2");
    EXPECT_TRUE(err == 0);
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
    err = bean.setProperty(p1, true);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(p1) == true);
    value = bean.removeProperty(p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasProperty(p1) == false);
    EXPECT_TRUE(value == true);

    err = bean.setProperty(p1, false);
    EXPECT_TRUE(err == 0);
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
    int err = 0;

    init_world(world);

    Bean &bean = *world.createBean();
    EXPECT_TRUE(bean.hasArrayProperty(pArray_1) == false);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 0);
    Value value2 = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value2.isNull());

    err = bean.appendProperty(pArray_1, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.appendProperty(nullptr, 0);
    EXPECT_TRUE(err == -2);

    err = bean.setProperty(pArray_1, 0, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.setProperty(nullptr, 0, 1);
    EXPECT_TRUE(err == -2);

    property = world.defineArrayProperty("pArray_1", Property::IntType);
    EXPECT_TRUE(property == world.getProperty("pArray_1"));
    err = bean.appendProperty(pArray_1, 1);
    EXPECT_TRUE(err == -4);
    err = bean.setProperty(pArray_1, 0, 1);
    EXPECT_TRUE(err == -4);

    //create empty array
    err = bean.createArrayProperty(pArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.hasArrayProperty(pArray_1) == true);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 0);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value.isNull());

    //append
    err = bean.appendProperty(pArray_1, "0");
    EXPECT_TRUE(err == -3);
    err = bean.appendProperty(pArray_1, 0);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 1);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value == 0);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value.isNull());
    err = bean.appendProperty(pArray_1, 1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value == 1);

    //setProperty()
    err = bean.setProperty(pArray_1, 99, 99);
    EXPECT_TRUE(err == -5);
    err = bean.setProperty(pArray_1, 0, 99);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 0);
    EXPECT_TRUE(value == 99);
    err = bean.setProperty(pArray_1, 1, 999);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 2);
    value = bean.getArrayProperty(pArray_1, 1);
    EXPECT_TRUE(value == 999);

    //removeProperty()
    value = bean.removeProperty(nullptr, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(p1, 0);
    EXPECT_TRUE(value.isNull());
    bean.setProperty(p1, 1);
    value = bean.removeProperty(p1, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(pArray_1, 3);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(pArray_1, 0);
    EXPECT_TRUE(value == 99);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 1);
    value = bean.removeProperty(pArray_1, 0);
    EXPECT_TRUE(value == 999);
    EXPECT_TRUE(bean.getArraySize(pArray_1) == 0);
}

TEST(Bean, relation)
{
    BeanWorld world;
    Value value;
    int err = 0;

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

    err = bean1->setRelation(r1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(p1, bean2);
    EXPECT_TRUE(err == -3);

    EXPECT_TRUE(!bean1->getRelationBeanId(r1));
    err = bean1->setRelation(r1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(r1) == bean2->getId());

}

TEST(Bean, array_relation)
{
    BeanWorld world;
    Value value;
    int err = 0;
    
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    err = bean1->appendRelation(rArray_1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->appendRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(p1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(err == -4);

    err = bean1->setRelation(rArray_1, 0, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, 0, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(p1, 0, bean2);
    EXPECT_TRUE(err == -3);
    err = bean1->setRelation(rArray_1, 0, bean2);
    EXPECT_TRUE(err == -4);

    EXPECT_TRUE(!bean1->getRelationBeanId(rArray_1, 0));
    err = bean1->createArrayRelation(nullptr);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(p1);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(rArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(rArray_1) == 0);

    err = bean1->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(rArray_1) == 1);

    err = bean1->setRelation(rArray_1, 1, bean2);
    EXPECT_TRUE(err == -5);
    err = bean1->setRelation(rArray_1, 0, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(rArray_1, 0) == bean2->getId());
    err = bean1->setRelation(rArray_1, 0, bean3);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(rArray_1, 0) == bean3->getId());
    bean1->setProperty(p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(p1, 0));
    EXPECT_TRUE(!bean1->getRelationBeanId(rArray_1, 1));

    //getArraySize()
    EXPECT_TRUE(bean1->getArraySize(nullptr) == 0);
    EXPECT_TRUE(bean1->getArraySize(p1) == 0);

    //removeProperty()
    value = bean3->removeProperty(nullptr, 0);
    EXPECT_TRUE(value.isNull());
    value = bean3->removeProperty(p1, 0);
    EXPECT_TRUE(value.isNull());
    bean3->setProperty(p1, 1);
    value = bean3->removeProperty(p1, 0);
    EXPECT_TRUE(value.isNull());
    value = bean3->removeProperty(pArray_1, 3);
    EXPECT_TRUE(value.isNull());

    err = bean3->createArrayRelation(rArray_1);
    err = bean3->appendRelation(rArray_1, bean1);
    err = bean3->appendRelation(rArray_1, bean2);
    EXPECT_TRUE(bean3->getArraySize(rArray_1) == 2);

    value = bean3->removeProperty(rArray_1, 0);
    EXPECT_TRUE(value == bean1->getId());
    EXPECT_TRUE(bean3->getArraySize(rArray_1) == 1);
    value = bean3->removeProperty(rArray_1, 0);
    EXPECT_TRUE(value == bean2->getId());
    EXPECT_TRUE(bean3->getArraySize(rArray_1) == 0);
}


