

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/BeanWorld.h"

#include "./common.h"

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
    const Property* property = nullptr;

    property = world.defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world.defineArrayProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world.defineRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world.defineArrayRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));
}

TEST(BeanWorld, getProperty)
{
    BeanWorld world;
    Value value;
    int pid = 0;
    const Property* property = nullptr;

    init_world(world);

    Bean *bean = world.createBean();
    property = world.getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("a");
    EXPECT_TRUE(property == nullptr);

    property = world.defineProperty("p1", Property::IntType);
    property = world.getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");

    property = ((const BeanWorld*)&world)->getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");
}

TEST(BeanWorld, getProperties)
{
    BeanWorld *world = new BeanWorld();
    Value value;
    Property *p1, *p2;

    // init_world(*world);

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    p1 = world->defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty("", Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty(string("").c_str(),Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(p1 == world->getProperty("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    p1 = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(p1 == world->getProperty("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    p2 = world->defineProperty("p2", Property::IntType);
    EXPECT_TRUE(p2 == world->getProperty("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean1->setProperty(p1, 1);
    bean2->setProperty(p1, 2);

    world->undefineProperty("p1");
    EXPECT_TRUE(world->getProperties().size() == 1);
    EXPECT_TRUE(bean1->hasProperty(p1) == false);
    world->undefineProperty("p2");
    EXPECT_TRUE(world->getProperties().size() == 0);
    EXPECT_TRUE(bean2->hasProperty(p1) == false);
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

void test_findHas_common(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<Bean*>& beans, bool needIndex = false)
{
    init_world(world, needIndex);

    world.findHas(p_int, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_uint, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_int64, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_uint64, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_double, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_str, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_bool_0, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(p_bool_1, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findHas(r1, beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.setProperty(p_double, 1.0);
    bean1.setProperty(p_str, "hello");
    bean1.setProperty(p_bool_0, false);
    bean1.setProperty(p_bool_1, true);
    bean1.setProperty(p_int, 1);
    bean1.setProperty(p_uint, 2U);
    bean1.setProperty(p_int64, 3);
    bean1.setProperty(p_uint64, 4U);
    bean1.setRelation(r1, &bean2);

    world.findHas(p_int, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_uint, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_int64, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_uint64, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_double, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_str, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_bool_0, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_bool_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(r1, beans);
    EXPECT_TRUE(beans.size() == 1);

    bean2.setProperty(p_double, 1.0);
    bean2.setProperty(p_str, "hello");
    bean2.setProperty(p_bool_0, false);
    bean2.setProperty(p_bool_1, true);
    bean2.setProperty(p_int, 1);
    bean2.setProperty(p_uint, 2U);
    bean2.setProperty(p_int64, 3);
    bean2.setProperty(p_uint64, 4U);
    bean2.setRelation(r1, &bean1);

    world.findHas(p_int, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_uint, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_int64, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_uint64, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_double, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_str, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_bool_0, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(p_bool_1, beans);
    EXPECT_TRUE(beans.size() == 2);
    world.findHas(r1, beans);
    EXPECT_TRUE(beans.size() == 2);

    bean2.removeProperty(p_double);
    bean2.removeProperty(p_str);
    bean2.removeProperty(p_bool_0);
    bean2.removeProperty(p_bool_1);
    bean2.removeProperty(p_int);
    bean2.removeProperty(p_uint);
    bean2.removeProperty(p_int64);
    bean2.removeProperty(p_uint64);
    bean2.removeRelation(r1);

    world.findHas(p_int, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_uint, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_int64, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_uint64, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_double, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_str, beans);
    auto s = beans.size();
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_bool_0, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(p_bool_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findHas(r1, beans);
    EXPECT_TRUE(beans.size() == 1);
}

void test_findHas_common_array(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<Bean*>& beans, bool needIndex)
{
    Property *pArray_1, *rArray_1;
    world.findHas(nullptr, beans);
    EXPECT_TRUE(beans.size() == 0);

   pArray_1 =  world.defineArrayProperty("pArray_1", Property::IntType, needIndex);

    world.findHas(pArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayProperty(pArray_1);
    world.findHas(pArray_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineProperty("pArray_1");
    world.findHas(pArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);

    pArray_1 = world.defineArrayProperty("pArray_1", Property::BoolType, needIndex);

    bean1.createArrayProperty(pArray_1);
    world.findHas(pArray_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(pArray_1);
    world.findHas(pArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);

    rArray_1 = world.defineArrayRelation("rArray_1");

    world.findHas(rArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    world.findHas(rArray_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeRelation(rArray_1);
    world.findHas(rArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    world.findHas(rArray_1, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineRelation("rArray_1");
    world.findHas(rArray_1, beans);
    EXPECT_TRUE(beans.size() == 0);

}

TEST(BeanWorld, findHas_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common(world, bean1, bean2, beans, false);
}

TEST(BeanWorld, findHas_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common(world, bean1, bean2, beans, true);
}

TEST(BeanWorld, findHas_array_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common_array(world, bean1, bean2, beans, false);
}

TEST(BeanWorld, findHas_array_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common_array(world, bean1, bean2, beans, true);
}

TEST(BeanWorld, findEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    bean1.setProperty(p_double, 1.0);
    bean1.setProperty(p_str, "hello");
    bean1.setProperty(p_bool_0, false);
    bean1.setProperty(p_bool_1, true);
    bean1.setProperty(p_int, 1);
    bean1.setProperty(p_uint, 2U);
    bean1.setProperty(p_int64, 3);
    bean1.setProperty(p_uint64, 4U);

    bean2.setProperty(p_double, 1.0);
    bean2.setProperty(p_str, "hello");
    bean2.setProperty(p_bool_0, false);
    bean2.setProperty(p_bool_1, true);
    bean2.setProperty(p_int, 1);
    bean2.setProperty(p_uint, 2U);
    bean2.setProperty(p_int64, 3);
    bean2.setProperty(p_uint64, 4U);

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
}

TEST(BeanWorld, findEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);
    
    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_bool_0, false);
    bean1->setProperty(p_bool_1, true);
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 2U);
    bean1->setProperty(p_int64, 3);
    bean1->setProperty(p_uint64, 4U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 1.0);
    bean2->setProperty(p_str, "hello");
    bean2->setProperty(p_bool_0, false);
    bean2->setProperty(p_bool_1, true);
    bean2->setProperty(p_int, 1);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 3);
    bean2->setProperty(p_uint64, 4U);

    //reset property
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_double, 1.0);

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

}

TEST(BeanWorld, findLessEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);

    world.findLessEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 1.0);
    }
    world.findLessEqual(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 2.0);
    }
    world.findLessEqual(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 3.0);
    }

    world.findLessEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") <= 0);
    }
    world.findLessEqual(p_str, "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") <= 0);
    }
    world.findLessEqual(p_str, "world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") <= 0);
    }

   world.findLessEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 1);
    }
   world.findLessEqual(p_int, (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 2);
    }
   world.findLessEqual(p_int, (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 3);
    }

    world.findLessEqual(p_uint, (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)1);
    }
    world.findLessEqual(p_uint, (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)2);
    }
    world.findLessEqual(p_uint, (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)3);
    }
}

TEST(BeanWorld, findLessEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);
    
    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();

    world.findLessEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 1.0);
    }
    world.findLessEqual(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 2.0);
    }
    world.findLessEqual(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 3.0);
    }

    world.findLessEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") <= 0);
    }
    world.findLessEqual(p_str, "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") <= 0);
    }
    world.findLessEqual(p_str, "world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") <= 0);
    }

   world.findLessEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 1);
    }
   world.findLessEqual(p_int, (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 2);
    }
   world.findLessEqual(p_int, (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 3);
    }

    world.findLessEqual(p_uint, (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)1);
    }
    world.findLessEqual(p_uint, (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)2);
    }
    world.findLessEqual(p_uint, (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)3);
    }
}

TEST(BeanWorld, findGreaterEqual_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);

    world.findGreaterEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 1.0);
    }
    world.findGreaterEqual(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 2.0);
    }
    world.findGreaterEqual(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 3.0);
    }
}

TEST(BeanWorld, findGreaterEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();

    world.findGreaterEqual(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 1.0);
    }
    world.findGreaterEqual(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 2.0);
    }
    world.findGreaterEqual(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 3.0);
    }

    world.findGreaterEqual(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") >= 0);
    }
    world.findGreaterEqual(p_str, "my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") >= 0);
    }
    world.findGreaterEqual(p_str, "world", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") >= 0);
    }

   world.findGreaterEqual(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 1);
    }
   world.findGreaterEqual(p_int, (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 2);
    }
   world.findGreaterEqual(p_int, (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 3);
    }

    world.findGreaterEqual(p_uint, (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)1);
    }
    world.findGreaterEqual(p_uint, (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)2);
    }
    world.findGreaterEqual(p_uint, (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)3);
    }
}

TEST(BeanWorld, findLessThan_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double,  2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);

    world.findLessThan(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 2.0);
    }
    world.findLessThan(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 3.0);
    }
    world.findLessThan(p_double, 4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 4.0);
    }
}

TEST(BeanWorld, findLessThan_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();

    world.findLessThan(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 2.0);
    }
    world.findLessThan(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 3.0);
    }
    world.findLessThan(p_double, 4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 4.0);
    }

    world.findLessThan(p_str, "hello", beans);
   EXPECT_TRUE(beans.size() == 0);
    world.findLessThan(p_str, "my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") < 0);
    }
    world.findLessThan(p_str, "world", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") < 0);
    }
    world.findLessThan(p_str, "zzz", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("zzz") < 0);
    }

   world.findLessThan(p_int, (int_t)1, beans);
   EXPECT_TRUE(beans.size() == 0);
   world.findLessThan(p_int, (int_t)2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 2);
    }
   world.findLessThan(p_int, (int_t)3, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 3);
    }
   world.findLessThan(p_int, 4, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 4);
    }

    world.findLessThan(p_uint, (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 0);
    world.findLessThan(p_uint, (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)2);
    }
    world.findLessThan(p_uint, (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)3);
    }
    world.findLessThan(p_uint, (uint_t)4, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)4);
    }
}

TEST(BeanWorld, findGreaterThan_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);

    world.findGreaterThan(p_double, 0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 0.0);
    }
    world.findGreaterThan(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 1.0);
    }
    world.findGreaterThan(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 2.0);
    }
    world.findGreaterThan(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 0);
}

TEST(BeanWorld, findGreaterThan_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();

    world.findGreaterThan(p_double, 0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 0.0);
    }
    world.findGreaterThan(p_double, 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 1.0);
    }
    world.findGreaterThan(p_double, 2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 2.0);
    }
    world.findGreaterThan(p_double, 3.0, beans);
   EXPECT_TRUE(beans.size() == 0);

    world.findGreaterThan(p_str, "a", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("a") > 0);
    }
    world.findGreaterThan(p_str, "hello", beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") > 0);
    }
    world.findGreaterThan(p_str, "my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") > 0);
    }
    world.findGreaterThan(p_str, "world", beans);
   EXPECT_TRUE(beans.size() == 0);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") > 0);
    }

    world.findGreaterThan(p_int, 0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 0);
    }

   world.findGreaterThan(p_int, 1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 1);
    }
   world.findGreaterThan(p_int, 2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 2);
    }
   world.findGreaterThan(p_int, 3, beans);
   EXPECT_TRUE(beans.size() == 0);

    world.findGreaterThan(p_uint, (uint_t)0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)0);
    }
    world.findGreaterThan(p_uint, (uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)1);
    }
    world.findGreaterThan(p_uint, (uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)2);
    }
    world.findGreaterThan(p_uint, (uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 0);

    // Json::Value& v = (Json::Value&)Json::Value::null;
    // Json::Value& v = (Json::Value&)Json::Value::nullSingleton();
    // v["aaa"] = 1;
    // int a;
}
