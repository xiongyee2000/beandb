

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/Property.h"
#include "org/jinsha/bean/Bean.h"
#include "org/jinsha/bean/BeanWorld.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

TEST(Property, getName)
{
    BeanWorld world;
    Bean* bean = world.createBean();
    world.defineProperty("p1", Property::IntType);
    Property* property = world.getProperty("p1");
    EXPECT_TRUE(property != nullptr);
    EXPECT_TRUE(property->getName() == "p1");
}

TEST(Property, indexed)
{
    BeanWorld world;

    Bean *bean = world.createBean();
    world.defineProperty("p1", Property::IntType);
    Property* property = world.getProperty("p1");
    EXPECT_TRUE(property->indexed() == false);
    property->createIndex();
    EXPECT_TRUE(property->indexed() == true);
}

TEST(Property, create_index)
{
    BeanWorld world;

    Bean *bean = world.createBean();
    world.defineProperty("p1", Property::IntType);
    Property* property = world.getProperty("p1");
    EXPECT_TRUE(property->indexed() == false);
    property->createIndex();
    EXPECT_TRUE(property->indexed() == true);
}

TEST(Property, remove_index)
{
    BeanWorld world;

    Bean *bean = world.createBean();
    world.defineProperty("p1", Property::IntType);
    Property* property = world.getProperty("p1");
    EXPECT_TRUE(property->indexed() == false);
    property->createIndex();
    EXPECT_TRUE(property->indexed() == true);
    property->removeIndex();
    EXPECT_TRUE(property->indexed() == false);
}

TEST(BeanWorld, index_more_complicated)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    ///////////////////////////////////////////////////////
    //createIndex()
    ///////////////////////////////////////////////////////
    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_bool_0, false);
    bean1->setProperty(p_bool_1, true);
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 2U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 1.0);
    bean2->setProperty(p_str, "hello");
    bean2->setProperty(p_bool_0, false);
    bean2->setProperty(p_bool_1, true);
    bean2->setProperty(p_int, 1);
    bean2->setProperty(p_uint, 2U);

    p_double->createIndex();
    p_str->createIndex();
    p_bool_0->createIndex();
    p_bool_1->createIndex();
    p_int->createIndex();
    p_uint->createIndex();

    world.findEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    world.findEqual(p_bool_0, false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    world.findEqual(p_bool_1, true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    world.findEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   world.findEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    world.findEqual(p_uint, (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //removeIndex()
    ///////////////////////////////////////////////////////
    p_double->removeIndex();
    p_str->removeIndex();
    p_bool_0->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();

    world.findEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    world.findEqual(p_bool_0, false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    world.findEqual(p_bool_1, true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    world.findEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   world.findEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    world.findEqual(p_uint, (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //reset property after createIndex()
    ///////////////////////////////////////////////////////
    p_double->createIndex();
    p_str->createIndex();
    p_bool_0->createIndex();
    p_bool_1->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_bool_0, false);
    bean1->setProperty(p_bool_1, true);
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 2U);

   world.findEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    world.findEqual(p_bool_0, false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    world.findEqual(p_bool_1, true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    world.findEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   world.findEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    world.findEqual(p_uint, (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //remove property after createIndex()
    ///////////////////////////////////////////////////////
    p_double->createIndex();
    p_str->createIndex();
    p_bool_0->createIndex();
    p_int->createIndex();
    p_uint->createIndex();

    bean1->removeProperty(p_double);
    world.findEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    ///////////////////////////////////////////////////////
    //remove bean after createIndex()
    ///////////////////////////////////////////////////////
    world.removeBean(bean2->getId());
    world.findEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);

}