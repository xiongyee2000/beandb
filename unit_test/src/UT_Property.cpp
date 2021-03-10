

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
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("bool_p0", false);
    bean1->setProperty("bool_p1", true);
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 2U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 1.0);
    bean2->setProperty("str_p", "hello");
    bean2->setProperty("bool_p0", false);
    bean2->setProperty("bool_p1", true);
    bean2->setProperty("int_p", 1);
    bean2->setProperty("uint_p", 2U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* bool_p0_property = world.getProperty("bool_p0");
    Property* bool_p1_property = world.getProperty("bool_p1");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    bool_p0_property->createIndex();
    bool_p1_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();

    world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") == 1.0);
    }

    world.findEqual("bool_p0", false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p0") == false);
    }

    world.findEqual("bool_p1", true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p1") == true);
    }

    world.findEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p") == "hello");
    }

   world.findEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") == 1);
    }

    world.findEqual("uint_p", (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") == 2U);
    }

    ///////////////////////////////////////////////////////
    //removeIndex()
    ///////////////////////////////////////////////////////
    double_p_property->removeIndex();
    str_p_property->removeIndex();
    bool_p0_property->removeIndex();
    int_p_property->removeIndex();
    uint_p_property->removeIndex();

    world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") == 1.0);
    }

    world.findEqual("bool_p0", false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p0") == false);
    }

    world.findEqual("bool_p1", true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p1") == true);
    }

    world.findEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p") == "hello");
    }

   world.findEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") == 1);
    }

    world.findEqual("uint_p", (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") == 2U);
    }

    ///////////////////////////////////////////////////////
    //reset property after createIndex()
    ///////////////////////////////////////////////////////
    double_p_property->createIndex();
    str_p_property->createIndex();
    bool_p0_property->createIndex();
    bool_p1_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("bool_p0", false);
    bean1->setProperty("bool_p1", true);
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 2U);

   world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") == 1.0);
    }

    world.findEqual("bool_p0", false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p0") == false);
    }

    world.findEqual("bool_p1", true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("bool_p1") == true);
    }

    world.findEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p") == "hello");
    }

   world.findEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") == 1);
    }

    world.findEqual("uint_p", (uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") == 2U);
    }

    ///////////////////////////////////////////////////////
    //remove property after createIndex()
    ///////////////////////////////////////////////////////
    double_p_property->createIndex();
    str_p_property->createIndex();
    bool_p0_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();

    bean1->removeProperty("double_p");
    world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") == 1.0);
    }

    ///////////////////////////////////////////////////////
    //remove bean after createIndex()
    ///////////////////////////////////////////////////////
    world.removeBean(bean2->getId());
    world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);

}