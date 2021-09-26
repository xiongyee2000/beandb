

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <unordered_map>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public

#include "org/jinsha/bean/SqliteBeanDB.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./unit_test/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./unit_test/data/sqlite_db_1";

static void validate_testdb_1(SqliteBeanDB& testdb);
static void validate_properties_testdb_1(std::unordered_map<std::string, Property*>& propertyMap);


TEST(SqliteBeanDB, constuctor_destructor)
{
    const char* testdbDir = nullptr;
    SqliteBeanDB* testdb;
    
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = "";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = "invalid";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = g_tmpDBDir;
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}

TEST(SqliteBeanDB, connect_disconnect)
{
    int err = 0;
    const char* testdbDir = "invalid";
    SqliteBeanDB* testdb = new SqliteBeanDB(testdbDir);
    err = testdb->connect();
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(!testdb->connected());
    delete testdb;

    testdbDir = g_sqlite_db_1;
    testdb = new SqliteBeanDB(testdbDir);
    err = testdb->connect();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb->connected());
    err = testdb->disconnect();
    EXPECT_TRUE(!testdb->connected());
    EXPECT_TRUE(err == 0);
    delete testdb;
}

TEST(SqliteBeanDB, transaction)
{
    int err = 0;
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);

    err = testdb.beginTransaction();
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(!testdb.inTransaction());
    err = testdb.commitTransaction();
    EXPECT_TRUE(err != 0);
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err != 0);

    testdb.connect();

    err = testdb.commitTransaction();
    EXPECT_TRUE(err != 0);
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err != 0);

    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.commitTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());

    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());

    err = testdb.beginTransaction();
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());

    err = testdb.beginTransaction();
    err = testdb.commitTransaction();
    err = testdb.commitTransaction();
    EXPECT_TRUE(err != 0);

    err = testdb.beginTransaction();
    err = testdb.rollbackTransaction();
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err != 0);

    testdb.disconnect();
}


TEST(SqliteBeanDB, defineProperty_undefineProperty)
{
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    int err = 0;
    Property* property = nullptr;
    TestHelper testHelper;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);
    validate_testdb_1(testdb);

    err = world->undefineProperty("p1");
    EXPECT_TRUE(err == 0);
    property = world->getProperty("p1");
    EXPECT_TRUE(property == nullptr);

    err = world->undefineProperty(nullptr);
    EXPECT_TRUE(err == 0);
    err = world->undefineProperty("");
    EXPECT_TRUE(err == 0);

    testdb.disconnect();

}


TEST(SqliteBeanDB, getProperty)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;

    testdb.connect();
    world = testdb.getWorld();

   EXPECT_TRUE(world->getProperty("p1")->getName() == "p1" && 
        world->getProperty("p1")->getType() == Property::PrimaryType && 
        world->getProperty("p1")->getValueType() == Property::IntType);
    EXPECT_TRUE(world->getProperty("p2")->getName() == "p2" && 
        world->getProperty("p2")->getType() == Property::PrimaryType && 
        world->getProperty("p2")->getValueType() == Property::IntType);

   EXPECT_TRUE(world->getProperty("p_int")->getName() == "p_int" && 
        world->getProperty("p_int")->getType() == Property::PrimaryType && 
        world->getProperty("p_int")->getValueType() == Property::IntType);
   EXPECT_TRUE(world->getProperty("p_uint")->getName() == "p_uint" && 
        world->getProperty("p_uint")->getType() == Property::PrimaryType && 
        world->getProperty("p_uint")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world->getProperty("p_real")->getName() == "p_real" && 
        world->getProperty("p_real")->getType() == Property::PrimaryType && 
        world->getProperty("p_real")->getValueType() == Property::RealType);
    EXPECT_TRUE(world->getProperty("p_str")->getName() == "p_str" && 
        world->getProperty("p_str")->getType() == Property::PrimaryType && 
        world->getProperty("p_str")->getValueType() == Property::StringType);
    EXPECT_TRUE(world->getProperty("p_bool_0")->getName() == "p_bool_0" && 
        world->getProperty("p_bool_0")->getType() == Property::PrimaryType && 
        world->getProperty("p_bool_0")->getValueType() == Property::BoolType);
    EXPECT_TRUE(world->getProperty("p_bool_1")->getName() == "p_bool_1" && 
        world->getProperty("p_bool_1")->getType() == Property::PrimaryType && 
        world->getProperty("p_bool_1")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world->getProperty("p_array_int")->getName() == "p_array_int" && 
        world->getProperty("p_array_int")->getType() == Property::ArrayPrimaryType && 
        world->getProperty("p_array_int")->getValueType() == Property::IntType);
    EXPECT_TRUE(world->getProperty("p_array_uint")->getName() == "p_array_uint" && 
        world->getProperty("p_array_uint")->getType() == Property::ArrayPrimaryType && 
        world->getProperty("p_array_uint")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world->getProperty("p_array_real")->getName() == "p_array_real" && 
        world->getProperty("p_array_real")->getType() == Property::ArrayPrimaryType && 
        world->getProperty("p_array_real")->getValueType() == Property::RealType);
    EXPECT_TRUE(world->getProperty("p_array_str")->getName() == "p_array_str" && 
        world->getProperty("p_array_str")->getType() == Property::ArrayPrimaryType && 
        world->getProperty("p_array_str")->getValueType() == Property::StringType);
    EXPECT_TRUE(world->getProperty("p_array_bool")->getName() == "p_array_bool" && 
        world->getProperty("p_array_bool")->getType() == Property::ArrayPrimaryType && 
        world->getProperty("p_array_bool")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world->getProperty("r1")->getName() == "r1" && 
        world->getProperty("r1")->getType() == Property::RelationType);
    EXPECT_TRUE(world->getProperty("r2")->getName() == "r2" && 
        world->getProperty("r2")->getType() == Property::RelationType);

    EXPECT_TRUE(world->getProperty("r_array_1")->getName() == "r_array_1" && 
        world->getProperty("r_array_1")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world->getProperty("r_array_2")->getName() == "r_array_2" && 
        world->getProperty("r_array_2")->getType() == Property::ArrayRelationType);

   testdb.disconnect();
}

TEST(SqliteBeanDB, getProperties)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    testdb.connect();
    BeanWorld *world = testdb.getWorld();

    const std::unordered_map<std::string, Property*>& pmap = world->getProperties();
    world->m_propertyMap_ = pmap;
    validate_properties_testdb_1(testdb.m_world_->m_propertyMap_);
    
    testdb.disconnect();

}

TEST(SqliteBeanDB, createBean_deleteBean)
{
    Bean *bean = nullptr;
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    int err = 0;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();

    bean = world->createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = world->createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = world->createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    err = world->deleteBean(nullptr);
    EXPECT_TRUE(err == 0);

    testdb.reInit();
    testdb.disconnect();
}

TEST(SqliteBeanDB, getBean)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld* world = nullptr;

    testdb.connect();
    world = testdb.getWorld();

    TestHelper testHelper;
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    bean2 = world->getBean(1);
    bean3 = world->getBean(1);
    validateBeanProperties(testHelper, bean1);
    validateBeanProperties(testHelper, bean2);
    validateBeanProperties(testHelper, bean3);
}

TEST(SqliteBeanDB, saveBean)
{
    int err = 0;
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;
    Json::Value value;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->createBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);
    setBeanNativeData(testHelper, bean1);

    bean2 = world->createBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);
    setBeanNativeData(testHelper, bean2);

    bean3 = world->createBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);
    setBeanNativeData(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->createArrayRelation(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = bean1->save();
    EXPECT_TRUE(err == 0);
    err = bean2->save();
    EXPECT_TRUE(err == 0);
    err = bean3->save();
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);
    bean3 = world->getBean(beanId_3);

    validateBean(testHelper, bean1);
    validateBean(testHelper, bean2);

    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r1) ==beanId_1);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r2) ==beanId_2);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r_array_1, 0) ==beanId_1);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r_array_1, 1) ==beanId_2);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(beanId_1);
    bean3 = world->getBean(beanId_3);

    err = bean1->setArrayProperty(testHelper.p_array_int, 0, 99, false);    
    EXPECT_TRUE(err == 0);
    err = bean3->setArrayRelation(testHelper.r_array_1, 0, bean3->getId());
    EXPECT_TRUE(err == 0);
    err = bean1->save();
    EXPECT_TRUE(err == 0);
    err = bean3->save();
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);
    bean3 = world->getBean(beanId_3);

    value = bean1->getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    beanId_3 = bean3->getRelationBeanId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId_3 == bean3->getId());

    testdb.disconnect();
}


TEST(SqliteBeanDB, delayLoad)
{
    int err = 0;
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;
    Json::Value value;
    Json::ArrayIndex size = 0;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean3= world->getBean(3);
    size = bean3->getArraySize(testHelper.r_array_1);
    EXPECT_TRUE(size == 4);

    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    beanId_1 = bean1->getId();
    value = bean1->getProperty(testHelper.p_str);
    EXPECT_TRUE(value == "foo");
    value = bean1->getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 101);

    bean3 = world->getBean(3);
    beanId_1 = bean3->getRelationBeanId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId_1 == 1);

    testdb.disconnect();

    testdb.connect();
   world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    bean2 = world->getBean(2);
    err = bean1->setArrayProperty(testHelper.p_array_int, 0, 99);
    EXPECT_TRUE(err == 0);
    bean3 = world->getBean(3);
    err = bean3->setArrayRelation(testHelper.r_array_1, 0, bean2->getId());
    EXPECT_TRUE(err == 0);

    testdb.disconnect();

    testdb.connect();
   world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);
    bean1 = world->getBean(1);
    err = bean1->appendProperty(testHelper.p_array_int, 100);
    EXPECT_TRUE(err == 0);
    bean3 = world->getBean(3);
    err = bean3->appendRelation(testHelper.r_array_1, bean3->getId());
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    
    testdb.connect();
   world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    beanId_1 = bean1->getId();
    value = bean1->getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    value = bean1->getArrayProperty(testHelper.p_array_int, bean1->getArraySize(testHelper.p_array_int) - 1);
    EXPECT_TRUE(value == 100);

    bean3 = world->getBean(3);
    beanId_2 = bean3->getRelationBeanId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId_2 == 2);
    beanId_3 = bean3->getRelationBeanId(testHelper.r_array_1, bean3->getArraySize(testHelper.r_array_1) - 1);
    EXPECT_TRUE(beanId_3 == 3);

    testdb.disconnect();
}

TEST(SqliteBeanDB, loadProperties_)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    testdb.connect();

    std::unordered_map<std::string, Property*> propertyMap;
    err = testdb.loadProperties_(propertyMap);
    testdb.m_world_->m_propertyMap_ = propertyMap;
    validate_properties_testdb_1(testdb.m_world_->m_propertyMap_);
    
    testdb.disconnect();

}


TEST(SqliteBeanDB, loadBeanProperty_)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->getBean(1);
    Bean* bean2 = world->getBean(2);
    Bean* bean3 = world->getBean(3);
    oidType beanId_1 = bean1->getId();
    oidType beanId_2 = bean2->getId();
    oidType beanId_3 = bean3->getId();
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asInt() == -1);

    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_uint, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt() == 1);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_int64, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asInt64() == -1);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_uint64, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt64() == 1);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_bool_0, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == false);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_bool_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == true);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_real, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asDouble() == 1.0);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_str, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == "foo");

    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value[0] == 101);
    EXPECT_TRUE(value[1] == 102);

    err = testdb.loadBeanProperty_(beanId_3, testHelper.r1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt64() == 1);
    err = testdb.loadBeanProperty_(beanId_3, testHelper.r2, value);
    EXPECT_TRUE(err  == 0);
    EXPECT_TRUE(value.asUInt64()  == 2);
    err = testdb.loadBeanProperty_(beanId_3, testHelper.r_array_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value[0].asUInt64()  == 1);
    EXPECT_TRUE(value[1].asUInt64()  == 2);

    testdb.disconnect();

}


TEST(SqliteBeanDB, insertBeanProperty_)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->createBean();
    oidType beanId_1 = bean1->getId();

    err = testdb.insertBeanProperty_(9999, testHelper.p_int, 1);
    EXPECT_TRUE(err == 0);

    bean1->createArrayProperty(testHelper.p_array_int);
    err = testdb.insertBeanProperty_(beanId_1, testHelper.p_int, 1);
    EXPECT_TRUE(err == 0);

    err = testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    err = testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    testdb.loadBeanProperty_(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(value == 1);
    testdb.loadBeanProperty_(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(value[0] == 0);
    EXPECT_TRUE(value[1] == 1);

    testdb.disconnect();

}

TEST(SqliteBeanDB, updateBeanProperty_)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->createBean();
    oidType beanId_1 = bean1->getId();

    err = testdb.updateBeanProperty_(9999, testHelper.p_str, "foo");
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_int, 9999, 1);
    EXPECT_TRUE(err == -1001);

    bean1->setProperty(testHelper.p_str, "xxx");
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_str, "foo");
    EXPECT_TRUE(err == 0);

    bean1->createArrayProperty(testHelper.p_array_int);
    bean1->createArrayProperty(testHelper.p_array_bool);
    bean1->createArrayProperty(testHelper.p_array_real);

    testdb.insertBeanProperty_(beanId_1, testHelper.p_int, 1);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 1);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_bool, false);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_bool, false);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_real, 1.0);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_real, 1.1);

    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_int, 99);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_int, 0, 10);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_int, 1, 11);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_bool, 0, true);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_bool, 1, true);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_real, 0, 555.555);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty_(beanId_1, testHelper.p_array_real, 1, 888.888);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    testdb.loadBeanProperty_(beanId_1, testHelper.p_str, value);
    EXPECT_TRUE(value == "foo");
    testdb.loadBeanProperty_(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(value == 99);
    testdb.loadBeanProperty_(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(value[0] == 10);
    EXPECT_TRUE(value[1] == 11);
    testdb.loadBeanProperty_(beanId_1, testHelper.p_array_bool, value);
    EXPECT_TRUE(value[0] == true);
    EXPECT_TRUE(value[1] == true);
    testdb.loadBeanProperty_(beanId_1, testHelper.p_array_real, value);
    EXPECT_TRUE(value[0] == 555.555);
    EXPECT_TRUE(value[1] == 888.888);

    testdb.disconnect();

}

TEST(SqliteBeanDB, deleteBeanProperty_)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->createBean();
    oidType beanId_1 = bean1->getId();
    
    bean1->createArrayProperty(testHelper.p_array_int);

    testdb.insertBeanProperty_(beanId_1, testHelper.p_int, 1);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 1);

    err = testdb.deleteBeanProperty_(9999, testHelper.p_int);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty_(9999, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty_(beanId_1, testHelper.p_array_int, 9999);
    EXPECT_TRUE(err == 0);

    err = testdb.deleteBeanProperty_(beanId_1, testHelper.p_int);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty_(beanId_1, testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty_(beanId_1, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(err != 0);
    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isArray() && value.size() == 0);

    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty_(beanId_1, testHelper.p_array_int, 1);
    err = testdb.deleteBeanProperty_(beanId_1, testHelper.p_array_int);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    err = testdb.loadBeanProperty_(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err != 0);

    testdb.disconnect();

}


TEST(SqliteBeanDB, nativeData)
{
    // char buff[128] = {0};
    // char* cmd = &buff[0];
    // sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    // system(cmd);

    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->createBean();
    oidType beanId_1 = bean1->getId();

    err = testdb.loadBeanNativeData_(beanId_1, value);
    EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(value.getMemberNames().size() == 0);
    EXPECT_TRUE(value.isNull());

    err = testdb.updateBeanNativeData_(beanId_1, Json::Value::nullRef);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    err = testdb.loadBeanNativeData_(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isNull());

    testdb.disconnect();
    testdb.connect();
    value["root"][0] = 0;
    value["root"][1] = 1;
    err = testdb.updateBeanNativeData_(beanId_1, value);
    EXPECT_TRUE(err == 0);
    
    testdb.disconnect();
    testdb.connect();
    err = testdb.loadBeanNativeData_(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value["root"][0] == 0);
    EXPECT_TRUE(value["root"][1] == 1);

    err = testdb.deleteBeanNativeData_(beanId_1);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    err = testdb.loadBeanNativeData_(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isNull());

    testdb.disconnect();

}


////////////////////////////////////////////////////////////////
// helper functions
////////////////////////////////////////////////////////////////

static void validate_testdb_1(SqliteBeanDB& testdb)
{
    validate_properties_testdb_1(testdb.m_world_->m_propertyMap_);
}

static void validate_properties_testdb_1(std::unordered_map<std::string, Property*>& propertyMap)
{
    Property *property = nullptr;

    property = propertyMap.at("p1");
    EXPECT_TRUE(property->getName() == "p1" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("p2");
    EXPECT_TRUE(property->getName() == "p2" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("p_int");
    EXPECT_TRUE(property->getName() == "p_int" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("p_uint");
    EXPECT_TRUE(property->getName() == "p_uint" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::UIntType);

    property = propertyMap.at("p_real");
    EXPECT_TRUE(property->getName() == "p_real" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::RealType);
    
    property = propertyMap.at("p_str");
    EXPECT_TRUE(property->getName() == "p_str" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::StringType);

    property = propertyMap.at("p_bool_0");
    EXPECT_TRUE(property->getName() == "p_bool_0" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::BoolType);

    property = propertyMap.at("p_bool_1");
    EXPECT_TRUE(property->getName() == "p_bool_1" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::BoolType);

    property = propertyMap.at("p_array_int");
    EXPECT_TRUE(property->getName() == "p_array_int" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("p_array_uint");
    EXPECT_TRUE(property->getName() == "p_array_uint" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::UIntType);

    property = propertyMap.at("p_array_real");
    EXPECT_TRUE(property->getName() == "p_array_real" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::RealType);

    property = propertyMap.at("p_array_str");
    EXPECT_TRUE(property->getName() == "p_array_str" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::StringType);

    property = propertyMap.at("p_array_bool");
    EXPECT_TRUE(property->getName() == "p_array_bool" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::BoolType);

    property = propertyMap.at("r1");
    EXPECT_TRUE(property->getName() == "r1" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("r2");
    EXPECT_TRUE(property->getName() == "r2" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("r_array_1");
    EXPECT_TRUE(property->getName() == "r_array_1" && 
        property->getType() == Property::ArrayRelationType);

    property = propertyMap.at("r_array_2");
    EXPECT_TRUE(property->getName() == "r_array_2" && 
        property->getType() == Property::ArrayRelationType);

}


