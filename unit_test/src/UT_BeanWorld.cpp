

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/BeanWorld.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

void basic()
{
    BeanWorld *world = new BeanWorld();
    EXPECT_TRUE(nullptr != world);
    EXPECT_TRUE(world->getNumOfBeans() == 0);
    EXPECT_TRUE(world->getProperties().size() == 0);

    EXPECT_TRUE(world->getNumOfBeans() == 0);

    Bean *bean1 = world->createBean();
    EXPECT_TRUE(nullptr != bean1);
    EXPECT_TRUE(world->getNumOfBeans() == 1);

    Bean *bean2 = world->createBean();
    EXPECT_TRUE(nullptr != bean2);
    EXPECT_TRUE(world->getNumOfBeans() == 2);

    Bean *bean3 = world->createBean();
    EXPECT_TRUE(nullptr != bean3);
    EXPECT_TRUE(world->getNumOfBeans() == 3);

    world->removeBean(bean1->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 2);

    world->removeBean(bean2->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 1);

    world->removeBean(bean3->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 0);

    delete world;
}

TEST(BeanWorld, Constructor)
{
    basic();
}

TEST(BeanWorld, createBean_removeBean)
{
    basic();
}

TEST(BeanWorld, getNumOfBeans)
{
    basic();
}

TEST(BeanWorld, getBeans)
{
    BeanWorld *world = new BeanWorld();

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    auto& beans  = world->getBeans();
    EXPECT_TRUE(beans.size() == 3);

    delete world;
}

TEST(BeanWorld, getBean)
{
    BeanWorld *world = new BeanWorld();
    Bean* bean_ = nullptr;

    std::vector<int>::const_iterator it;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    bean_ = world->getBean(bean1->getId());
    EXPECT_TRUE(bean_ == bean1);
    bean_ = world->getBean(bean2->getId());
    EXPECT_TRUE(bean_ == bean2);
    bean_ = world->getBean(bean3->getId());
    EXPECT_TRUE(bean_ == bean3);

    bean_ = world->getBean(555);
    EXPECT_TRUE(bean_ == nullptr);

    delete world;
}

TEST(BeanWorld, clear)
{
    BeanWorld *world = new BeanWorld();

    world->createBean();
    world->createBean();
    world->createBean();
    EXPECT_TRUE(world->getNumOfBeans() == 3);

    world->clear();
    EXPECT_TRUE(world->getNumOfBeans() == 0);

    delete world;
}

TEST(BeanWorld, defineProperty_undefineProperty)
{
    BeanWorld world;
    Value value;
    pidType pid = 0, pid_2 = 0;
    const Property* property = nullptr;

    pid = world.defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(pid = -1);
    pid = world.defineProperty("", Property::IntType);
    EXPECT_TRUE(pid = -1);

    world.undefineProperty("p1");
    pid_2 = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(pid_2 >= 0);
    pid = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(pid = -2);
    world.undefineProperty("p1");
    pid = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(pid >= 0 && pid == world.getPropertyId("p1"));

    world.undefineProperty("p1");
    pid_2 = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(pid_2 >= 0 && pid_2 == world.getPropertyId("p1"));
    pid = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(pid = -2);
     world.undefineProperty("p1");
    pid = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(pid >= 0 && pid == world.getPropertyId("p1"));

    world.undefineProperty("p1");
    pid_2 = world.defineRelation("p1");
    EXPECT_TRUE(pid_2 >= 0 && pid_2 == world.getPropertyId("p1"));
    pid = world.defineRelation("p1");
    EXPECT_TRUE(pid = -2);
    world.undefineProperty("p1");
    pid = world.defineRelation("p1");
    EXPECT_TRUE(pid >= 0 && pid == world.getPropertyId("p1"));

    world.undefineProperty("p1");
    pid_2 = world.defineArrayRelation("p1");
    EXPECT_TRUE(pid_2 >= 0 && pid_2 == world.getPropertyId("p1"));
    pid = world.defineArrayRelation("p1");
    EXPECT_TRUE(pid = -2);
     world.undefineProperty("p1");
    pid = world.defineArrayRelation("p1");
    EXPECT_TRUE(pid >= 0 && pid == world.getPropertyId("p1"));
}

TEST(BeanWorld, getProperty)
{
    BeanWorld world;
    Value value;
    int pid = 0;
    const Property* property = nullptr;

    Bean *bean = world.createBean();
    property = world.getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("a");
    EXPECT_TRUE(property == nullptr);

    pid = world.defineProperty("p1", Property::IntType);
    property = world.getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");
}

TEST(BeanWorld, getProperties)
{
    BeanWorld *world = new BeanWorld();
    Value value;
    int pid = 0;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    pid = world->defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = world->defineProperty("", Property::IntType);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = world->defineProperty(string("").c_str(),Property::IntType);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(pid == world->getPropertyId("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    pid = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(pid == -2);
    EXPECT_TRUE(world->getProperties().size() == 1);

    pid = world->defineProperty("p2", Property::IntType);
    EXPECT_TRUE(pid == world->getPropertyId("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean1->setProperty("p1", 1);
    bean2->setProperty("p1", 2);

    world->undefineProperty("p1");
    EXPECT_TRUE(world->getProperties().size() == 1);
    EXPECT_TRUE(bean1->hasProperty("p1") == false);
    world->undefineProperty("p2");
    EXPECT_TRUE(world->getProperties().size() == 0);
    EXPECT_TRUE(bean2->hasProperty("p1") == false);
    delete world;
}

TEST(BeanWorld, getPropertyId)
{
    BeanWorld *world = new BeanWorld();
    Value value;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    world->defineProperty("p2", Property::IntType);
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean2->setProperty("p1", "v1");
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean2->setProperty("p2", "v2");
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean1->removeProperty("p1");
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean2->removeProperty("p1");
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean1->removeProperty("p2");
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean2->removeProperty("p2");
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    delete world;
}

TEST(BeanWorld, removeBean)
{
    BeanWorld world;
    Value value;
    oidType oid = 0;
    Bean* bean = world.createBean();
    oid = bean->getId();
    world.removeBean(bean->getId());
    EXPECT_TRUE(world.getBean(oid) == nullptr);
}


TEST(BeanWorld, findEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean &bean1 = *world.createBean();
    bean1.setProperty("double_p", 1.0);
    bean1.setProperty("str_p", "hello");
    bean1.setProperty("bool_p0", false);
    bean1.setProperty("bool_p1", true);
    bean1.setProperty("int_p", 1);
    bean1.setProperty("uint_p", 2U);
    bean1.setProperty("int64_p", 3);
    bean1.setProperty("uint64_p", 4U);

    Bean& bean2 = *world.createBean();
    bean2.setProperty("double_p", 1.0);
    bean2.setProperty("str_p", "hello");
    bean2.setProperty("bool_p0", false);
    bean2.setProperty("bool_p1", true);
    bean2.setProperty("int_p", 1);
    bean2.setProperty("uint_p", 2U);
    bean2.setProperty("int64_p", 3);
    bean2.setProperty("uint64_p", 4U);

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
}

TEST(BeanWorld, findEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("bool_p0", false);
    bean1->setProperty("bool_p1", true);
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 2U);
    bean1->setProperty("int64_p", 3);
    bean1->setProperty("uint64_p", 4U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 1.0);
    bean2->setProperty("str_p", "hello");
    bean2->setProperty("bool_p0", false);
    bean2->setProperty("bool_p1", true);
    bean2->setProperty("int_p", 1);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 3);
    bean2->setProperty("uint64_p", 4U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* bool_p0_property = world.getProperty("bool_p0");
    Property* bool_p1_property = world.getProperty("bool_p1");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");
    Property* int64_p_property = world.getProperty("int64_p");
    Property* uint64_p_property = world.getProperty("uint64_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    bool_p0_property->createIndex();
    bool_p1_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    int64_p_property->createIndex();
    uint64_p_property->createIndex();

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

}

TEST(BeanWorld, findLessEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 1U);
    bean1->setProperty("int64_p", 101);
    bean1->setProperty("uint64_p", 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);
    bean2->setProperty("str_p", "my");
    bean2->setProperty("int_p", 2);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 102);
    bean2->setProperty("uint64_p", 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);
    bean3->setProperty("str_p", "world");
    bean3->setProperty("int_p", 3);
    bean3->setProperty("uint_p", 3U);
    bean3->setProperty("int64_p", 103);
    bean3->setProperty("uint64_p", 103U);

    world.findLessEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 1.0);
    }
    world.findLessEqual("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 2.0);
    }
    world.findLessEqual("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 3.0);
    }

    world.findLessEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("hello") <= 0);
    }
    world.findLessEqual("str_p", "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("my") <= 0);
    }
    world.findLessEqual("str_p", "world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare( "world") <= 0);
    }

   world.findLessEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 1);
    }
   world.findLessEqual("int_p", (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 2);
    }
   world.findLessEqual("int_p", (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 3);
    }

    world.findLessEqual("uint_p", (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)1);
    }
    world.findLessEqual("uint_p", (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)2);
    }
    world.findLessEqual("uint_p", (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)3);
    }
}

TEST(BeanWorld, findLessEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 1U);
    bean1->setProperty("int64_p", 101);
    bean1->setProperty("uint64_p", 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);
    bean2->setProperty("str_p", "my");
    bean2->setProperty("int_p", 2);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 102);
    bean2->setProperty("uint64_p", 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);
    bean3->setProperty("str_p", "world");
    bean3->setProperty("int_p", 3);
    bean3->setProperty("uint_p", 3U);
    bean3->setProperty("int64_p", 103);
    bean3->setProperty("uint64_p", 103U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");
    Property* int64_p_property = world.getProperty("int64_p");
    Property* uint64_p_property = world.getProperty("uint64_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    int64_p_property->createIndex();
    uint64_p_property->createIndex();

    world.findLessEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 1.0);
    }
    world.findLessEqual("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 2.0);
    }
    world.findLessEqual("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") <= 3.0);
    }

    world.findLessEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("hello") <= 0);
    }
    world.findLessEqual("str_p", "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("my") <= 0);
    }
    world.findLessEqual("str_p", "world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare( "world") <= 0);
    }

   world.findLessEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 1);
    }
   world.findLessEqual("int_p", (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 2);
    }
   world.findLessEqual("int_p", (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") <= 3);
    }

    world.findLessEqual("uint_p", (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)1);
    }
    world.findLessEqual("uint_p", (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)2);
    }
    world.findLessEqual("uint_p", (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") <= (uint_t)3);
    }
}

TEST(BeanWorld, findGreaterEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);

    world.findGreaterEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 1.0);
    }
    world.findGreaterEqual("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 2.0);
    }
    world.findGreaterEqual("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 3.0);
    }
}

TEST(BeanWorld, findGreaterEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 1U);
    bean1->setProperty("int64_p", 101);
    bean1->setProperty("uint64_p", 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);
    bean2->setProperty("str_p", "my");
    bean2->setProperty("int_p", 2);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 102);
    bean2->setProperty("uint64_p", 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);
    bean3->setProperty("str_p", "world");
    bean3->setProperty("int_p", 3);
    bean3->setProperty("uint_p", 3U);
    bean3->setProperty("int64_p", 103);
    bean3->setProperty("uint64_p", 103U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");
    Property* int64_p_property = world.getProperty("int64_p");
    Property* uint64_p_property = world.getProperty("uint64_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    int64_p_property->createIndex();
    uint64_p_property->createIndex();

    world.findGreaterEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 1.0);
    }
    world.findGreaterEqual("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 2.0);
    }
    world.findGreaterEqual("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") >= 3.0);
    }

    world.findGreaterEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("hello") >= 0);
    }
    world.findGreaterEqual("str_p", "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("my") >= 0);
    }
    world.findGreaterEqual("str_p", "world", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare( "world") >= 0);
    }

   world.findGreaterEqual("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") >= 1);
    }
   world.findGreaterEqual("int_p", (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") >= 2);
    }
   world.findGreaterEqual("int_p", (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") >= 3);
    }

    world.findGreaterEqual("uint_p", (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") >= (uint_t)1);
    }
    world.findGreaterEqual("uint_p", (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") >= (uint_t)2);
    }
    world.findGreaterEqual("uint_p", (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") >= (uint_t)3);
    }
}

TEST(BeanWorld, findLessThan_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p",  2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);

    world.findLessThan("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 2.0);
    }
    world.findLessThan("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 3.0);
    }
    world.findLessThan("double_p", 4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 4.0);
    }
}

TEST(BeanWorld, findLessThan_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 1U);
    bean1->setProperty("int64_p", 101);
    bean1->setProperty("uint64_p", 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);
    bean2->setProperty("str_p", "my");
    bean2->setProperty("int_p", 2);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 102);
    bean2->setProperty("uint64_p", 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);
    bean3->setProperty("str_p", "world");
    bean3->setProperty("int_p", 3);
    bean3->setProperty("uint_p", 3U);
    bean3->setProperty("int64_p", 103);
    bean3->setProperty("uint64_p", 103U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");
    Property* int64_p_property = world.getProperty("int64_p");
    Property* uint64_p_property = world.getProperty("uint64_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    int64_p_property->createIndex();
    uint64_p_property->createIndex();

    world.findLessThan("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 2.0);
    }
    world.findLessThan("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 3.0);
    }
    world.findLessThan("double_p", 4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") < 4.0);
    }

    world.findLessThan("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan("str_p", "my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("my") < 0);
    }
    world.findLessThan("str_p", "world", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare( "world") < 0);
    }
    world.findLessThan("str_p", "zzz", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("zzz") < 0);
    }

   world.findLessThan("int_p", (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 0);
   world.findLessThan("int_p", (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") < 2);
    }
   world.findLessThan("int_p", (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") < 3);
    }
   world.findLessThan("int_p", 4, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") < 4);
    }

    world.findLessThan("uint_p", (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findLessThan("uint_p", (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") < (uint_t)2);
    }
    world.findLessThan("uint_p", (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") < (uint_t)3);
    }
    world.findLessThan("uint_p", (uint_t)4, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") < (uint_t)4);
    }
}

TEST(BeanWorld, findGreaterThan_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);

    world.findGreaterThan("double_p", 0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 0.0);
    }
    world.findGreaterThan("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 1.0);
    }
    world.findGreaterThan("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 2.0);
    }
    world.findGreaterThan("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 0);
}

TEST(BeanWorld, findGreaterThan_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    world.defineProperty("int_p", Property::IntType);
    world.defineProperty("uint_p", Property::UIntType);
    world.defineProperty("int64_p", Property::IntType);
    world.defineProperty("uint64_p", Property::UIntType);
    world.defineProperty("double_p", Property::RealType);
    world.defineProperty("str_p", Property::StringType);
    world.defineProperty("bool_p0", Property::BoolType);
    world.defineProperty("bool_p1", Property::BoolType);

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 1U);
    bean1->setProperty("int64_p", 101);
    bean1->setProperty("uint64_p", 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 2.0);
    bean2->setProperty("str_p", "my");
    bean2->setProperty("int_p", 2);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 102);
    bean2->setProperty("uint64_p", 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty("double_p", 3.0);
    bean3->setProperty("str_p", "world");
    bean3->setProperty("int_p", 3);
    bean3->setProperty("uint_p", 3U);
    bean3->setProperty("int64_p", 103);
    bean3->setProperty("uint64_p", 103U);

    Property* double_p_property = world.getProperty("double_p");
    Property* str_p_property = world.getProperty("str_p");
    Property* int_p_property = world.getProperty("int_p");
    Property* uint_p_property = world.getProperty("uint_p");
    Property* int64_p_property = world.getProperty("int64_p");
    Property* uint64_p_property = world.getProperty("uint64_p");

    double_p_property->createIndex();
    str_p_property->createIndex();
    int_p_property->createIndex();
    uint_p_property->createIndex();
    int64_p_property->createIndex();
    uint64_p_property->createIndex();

    world.findGreaterThan("double_p", 0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 0.0);
    }
    world.findGreaterThan("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 1.0);
    }
    world.findGreaterThan("double_p", 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("double_p") > 2.0);
    }
    world.findGreaterThan("double_p", 3.0, beans);
   EXPECT_TRUE(beans.size() == 0);

    world.findGreaterThan("str_p", "a", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("a") > 0);
    }
    world.findGreaterThan("str_p", "hello", beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("hello") > 0);
    }
    world.findGreaterThan("str_p", "my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare("my") > 0);
    }
    world.findGreaterThan("str_p", "world", beans);
   EXPECT_TRUE(beans.size() == 0);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("str_p").compare( "world") > 0);
    }

    world.findGreaterThan("int_p", 0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") > 0);
    }

   world.findGreaterThan("int_p", 1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") > 1);
    }
   world.findGreaterThan("int_p", 2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("int_p") > 2);
    }
   world.findGreaterThan("int_p", 3, beans);
   EXPECT_TRUE(beans.size() == 0);

    world.findGreaterThan("uint_p", (uint_t)0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") > (uint_t)0);
    }
    world.findGreaterThan("uint_p", (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") > (uint_t)1);
    }
    world.findGreaterThan("uint_p", (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty("uint_p") > (uint_t)2);
    }
    world.findGreaterThan("uint_p", (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 0);

    // Json::Value& v = (Json::Value&)Json::Value::null;
    // Json::Value& v = (Json::Value&)Json::Value::nullSingleton();
    // v["aaa"] = 1;
    // int a;
}
