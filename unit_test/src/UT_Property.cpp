

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


void test_findHas_common(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<Bean*>& beans, bool needIndex = false)
{
    init_world(world, needIndex);

    p_int->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_uint->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_int64->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_uint64->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_double->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_str->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_bool_0->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    p_bool_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    r1->findHas(beans);
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

    p_int->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_uint->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_int64->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_uint64->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_double->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_str->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_bool_0->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_bool_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    r1->findHas(beans);
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

    p_int->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_uint->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_int64->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_uint64->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_double->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_str->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_bool_0->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    p_bool_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 2);
    r1->findHas(beans);
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

    p_int->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_uint->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_int64->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_uint64->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_double->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_str->findHas(beans);
    auto s = beans.size();
    EXPECT_TRUE(beans.size() == 1);
    p_bool_0->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    p_bool_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    r1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
}

void test_findHas_common_array(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<Bean*>& beans, bool needIndex)
{
    Property *pArray_1, *rArray_1;
   pArray_1 =  world.defineArrayProperty("pArray_1", Property::IntType, needIndex);

    pArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayProperty(pArray_1);
    pArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineProperty("pArray_1");
    pArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);

    pArray_1 = world.defineArrayProperty("pArray_1", Property::BoolType, needIndex);

    bean1.createArrayProperty(pArray_1);
    pArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(pArray_1);
    pArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);

    rArray_1 = world.defineArrayRelation("rArray_1");

    rArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    rArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeRelation(rArray_1);
    rArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    rArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineRelation("rArray_1");
    rArray_1->findHas(beans);
    EXPECT_TRUE(beans.size() == 0);

}

TEST(Property, findHas_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common(world, bean1, bean2, beans, false);
}

TEST(Property, findHas_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common(world, bean1, bean2, beans, true);
}

TEST(Property, findHas_array_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common_array(world, bean1, bean2, beans, false);
}

TEST(Property, findHas_array_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_findHas_common_array(world, bean1, bean2, beans, true);
}

void test_find_init(BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_bool_0, false);
    bean1->setProperty(p_bool_1, true);
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 2U);
    bean1->setProperty(p_int64, 3);
    bean1->setProperty(p_uint64, 4U);

    bean2->setProperty(p_double, 1.0);
    bean2->setProperty(p_str, "hello");
    bean2->setProperty(p_bool_0, false);
    bean2->setProperty(p_bool_1, true);
    bean2->setProperty(p_int, 1);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 3);
    bean2->setProperty(p_uint64, 4U);
}

void test_findEqual_common(BeanWorld& world, Bean* bean1, Bean* bean2)
{
    std::list<Bean*> beans;

    test_find_init(world, bean1, bean2, nullptr);

    p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) == 2U);
    }
}

TEST(Property, findEqual_without_index)
{
    BeanWorld world;
    init_world(world);
    Bean *bean1 = world.createBean();
    Bean *bean2 = world.createBean();
    test_findEqual_common(world, bean1, bean2);
}

TEST(Property, findEqual_with_index)
{
    BeanWorld world;
    init_world(world);
    Bean *bean1 = world.createBean();
    Bean *bean2 = world.createBean();

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
     test_findEqual_common(world, bean1, bean2);

    p_double->removeIndex();
    p_str->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();
    p_int64->removeIndex();
    p_uint64->removeIndex();
     test_findEqual_common(world, bean1, bean2);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
     test_findEqual_common(world, bean1, bean2);
}

void test_findLessEqual_common(BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<Bean*> beans;
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);


    p_double->findLessEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 1.0);
    }
    p_double->findLessEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 2.0);
    }
    p_double->findLessEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) <= 3.0);
    }

    p_str->findLessEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") <= 0);
    }
    p_str->findLessEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") <= 0);
    }
    p_str->findLessEqual("world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") <= 0);
    }

   p_int->findLessEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 1);
    }
   p_int->findLessEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 2);
    }
   p_int->findLessEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) <= 3);
    }

    p_uint->findLessEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)1);
    }
    p_uint->findLessEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)2);
    }
    p_uint->findLessEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) <= (uint_t)3);
    }
}

TEST(Property, findLessEqual_without_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    test_findLessEqual_common(world, bean1, bean2, bean3);
}

TEST(Property, findLessEqual_with_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    // bean1->setProperty(p_double, 1.0);
    // bean1->setProperty(p_str, "hello");
    // bean1->setProperty(p_int, 1);
    // bean1->setProperty(p_uint, 1U);
    // bean1->setProperty(p_int64, 101);
    // bean1->setProperty(p_uint64, 101U);

    Bean* bean2 = world.createBean();
    // bean2->setProperty(p_double, 2.0);
    // bean2->setProperty(p_str, "my");
    // bean2->setProperty(p_int, 2);
    // bean2->setProperty(p_uint, 2U);
    // bean2->setProperty(p_int64, 102);
    // bean2->setProperty(p_uint64, 102U);

    Bean* bean3 = world.createBean();
    // bean3->setProperty(p_double, 3.0);
    // bean3->setProperty(p_str, "world");
    // bean3->setProperty(p_int, 3);
    // bean3->setProperty(p_uint, 3U);
    // bean3->setProperty(p_int64, 103);
    // bean3->setProperty(p_uint64, 103U);
    
    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findLessEqual_common(world, bean1, bean2, bean3);
    
    p_double->removeIndex();
    p_str->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();
    p_int64->removeIndex();
    p_uint64->removeIndex();
    test_findLessEqual_common(world, bean1, bean2, bean3);
    
    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findLessEqual_common(world, bean1, bean2, bean3);
}

void test_FindGreaterEqual_common(BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<Bean*> beans;
    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);

    p_double->findGreaterEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 1.0);
    }
    p_double->findGreaterEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 2.0);
    }
    p_double->findGreaterEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) >= 3.0);
    }

    p_str->findGreaterEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") >= 0);
    }
    p_str->findGreaterEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") >= 0);
    }
    p_str->findGreaterEqual("world", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") >= 0);
    }

   p_int->findGreaterEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 1);
    }
   p_int->findGreaterEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 2);
    }
   p_int->findGreaterEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) >= 3);
    }

    p_uint->findGreaterEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)1);
    }
    p_uint->findGreaterEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)2);
    }
    p_uint->findGreaterEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) >= (uint_t)3);
    }
}

TEST(Property, findGreaterEqual_without_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    test_FindGreaterEqual_common(world, bean1, bean2, bean3);
}

TEST(Property, findGreaterEqual_with_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_FindGreaterEqual_common(world, bean1, bean2, bean3);

    p_double->removeIndex();
    p_str->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();
    p_int64->removeIndex();
    p_uint64->removeIndex();
    test_FindGreaterEqual_common(world, bean1, bean2, bean3);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_FindGreaterEqual_common(world, bean1, bean2, bean3);
}

void test_findLessThan_common(BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<Bean*> beans;

    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);


    p_double->findLessThan(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    p_double->findLessThan(2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 2.0);
    }
    p_double->findLessThan(3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 3.0);
    }
    p_double->findLessThan(4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) < 4.0);
    }

    p_str->findLessThan("hello", beans);
   EXPECT_TRUE(beans.size() == 0);
    p_str->findLessThan("my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") < 0);
    }
    p_str->findLessThan("world", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") < 0);
    }
    p_str->findLessThan("zzz", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("zzz") < 0);
    }

   p_int->findLessThan((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 0);
   p_int->findLessThan((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 2);
    }
   p_int->findLessThan((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 3);
    }
   p_int->findLessThan(4, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) < 4);
    }

    p_uint->findLessThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 0);
    p_uint->findLessThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)2);
    }
    p_uint->findLessThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)3);
    }
    p_uint->findLessThan((uint_t)4, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) < (uint_t)4);
    }
}

TEST(Property, findLessThan_without_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();
    
    test_findLessThan_common(world, bean1, bean2, bean3);
}

TEST(Property, findLessThan_with_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findLessThan_common(world, bean1, bean2, bean3);

    p_double->removeIndex();
    p_str->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();
    p_int64->removeIndex();
    p_uint64->removeIndex();
    test_findLessThan_common(world, bean1, bean2, bean3);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findLessThan_common(world, bean1, bean2, bean3);
}

void test_findGreaterThan_common(BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<Bean*> beans;

    bean1->setProperty(p_double, 1.0);
    bean1->setProperty(p_str, "hello");
    bean1->setProperty(p_int, 1);
    bean1->setProperty(p_uint, 1U);
    bean1->setProperty(p_int64, 101);
    bean1->setProperty(p_uint64, 101U);

    bean2->setProperty(p_double, 2.0);
    bean2->setProperty(p_str, "my");
    bean2->setProperty(p_int, 2);
    bean2->setProperty(p_uint, 2U);
    bean2->setProperty(p_int64, 102);
    bean2->setProperty(p_uint64, 102U);

    bean3->setProperty(p_double, 3.0);
    bean3->setProperty(p_str, "world");
    bean3->setProperty(p_int, 3);
    bean3->setProperty(p_uint, 3U);
    bean3->setProperty(p_int64, 103);
    bean3->setProperty(p_uint64, 103U);


    p_double->findGreaterThan(0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 0.0);
    }
    p_double->findGreaterThan(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 1.0);
    }
    p_double->findGreaterThan(2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) > 2.0);
    }
    p_double->findGreaterThan(3.0, beans);
   EXPECT_TRUE(beans.size() == 0);

    p_str->findGreaterThan("a", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("a") > 0);
    }
    p_str->findGreaterThan("hello", beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("hello") > 0);
    }
    p_str->findGreaterThan("my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare("my") > 0);
    }
    p_str->findGreaterThan("world", beans);
   EXPECT_TRUE(beans.size() == 0);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str).compare( "world") > 0);
    }

    p_int->findGreaterThan(0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 0);
    }

   p_int->findGreaterThan(1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 1);
    }
   p_int->findGreaterThan(2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) > 2);
    }
   p_int->findGreaterThan(3, beans);
   EXPECT_TRUE(beans.size() == 0);

    p_uint->findGreaterThan((uint_t)0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)0);
    }
    p_uint->findGreaterThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)1);
    }
    p_uint->findGreaterThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_uint) > (uint_t)2);
    }
    p_uint->findGreaterThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 0);
}

TEST(Property, findGreaterThan_without_index)
{
    BeanWorld world;
    std::list<Bean*> beans;

    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();
    test_findGreaterThan_common(world, bean1, bean2 ,bean3);
}

TEST(Property, findGreaterThan_with_index)
{
    BeanWorld world;
    init_world(world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findGreaterThan_common(world, bean1, bean2 ,bean3);

    p_double->removeIndex();
    p_str->removeIndex();
    p_int->removeIndex();
    p_uint->removeIndex();
    p_int64->removeIndex();
    p_uint64->removeIndex();
    test_findGreaterThan_common(world, bean1, bean2 ,bean3);

    p_double->createIndex();
    p_str->createIndex();
    p_int->createIndex();
    p_uint->createIndex();
    p_int64->createIndex();
    p_uint64->createIndex();
    test_findGreaterThan_common(world, bean1, bean2 ,bean3);
}

TEST(Property, index_more_complicated)
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

    p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    p_uint->findEqual((uint_t)2U, beans);
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

    p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    p_uint->findEqual((uint_t)2U, beans);
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

   p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_0) == false);
    }

    p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_bool_1) == true);
    }

    p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_str) == "hello");
    }

   p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_int) == 1);
    }

    p_uint->findEqual((uint_t)2U, beans);
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
    p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& bean : beans)
    {
        EXPECT_TRUE(bean->getProperty(p_double) == 1.0);
    }

    ///////////////////////////////////////////////////////
    //remove bean after createIndex()
    ///////////////////////////////////////////////////////
    world.removeBean(bean2->getId());
    p_double->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);

}