

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

#include "org/jinsha/beandb/SqliteBeanDB.h"
#include "org/jinsha/beandb/Bean.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::beandb;

static const char* g_tmpDBDir = "./unit_test/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./unit_test/data/sqlite_db_1";
static const char* g_invalid_sqlite_db = "./unit_test/data/sqlite_invalid_db";

static void validate_testdb_1(SqliteBeanDB& testdb);
static void validate_properties_testdb_1(std::unordered_map<std::string, Property*>& propertyMap);


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

void find_beanInit(TestHelper& testHelper, Bean* bean1, Bean* bean2, Bean* bean3)
{
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 1U);
    bean1->set(testHelper.p_int64, 101);
    bean1->set(testHelper.p_uint64, 101U);

    bean2->set(testHelper.p_real, 2.0);
    bean2->set(testHelper.p_str, "my");
    bean2->set(testHelper.p_int, 2);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 102);
    bean2->set(testHelper.p_uint64, 102U);

    bean3->set(testHelper.p_real, 3.0);
    bean3->set(testHelper.p_str, "world");
    bean3->set(testHelper.p_int, 3);
    bean3->set(testHelper.p_uint, 3U);
    bean3->set(testHelper.p_int64, 103);
    bean3->set(testHelper.p_uint64, 103U);
}


TEST(SqliteBeanDB, constuctor_destructor)
{
    const char* testdbDir = nullptr;
    SqliteBeanDB* testdb = nullptr;
    char buff[128] = {0};
    char* cmd = &buff[0];

    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == ".");
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;

    testdbDir = "";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == ".");
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;

    testdbDir = "invalid";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;


    testdbDir = g_invalid_sqlite_db;
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
    sprintf(buff, "rm -rf %s/*", g_invalid_sqlite_db);
    system(cmd);
    
    testdbDir = g_tmpDBDir;
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}



TEST(SqliteBeanDB, clear)
{
    SqliteBeanDB* testdb = nullptr;
    BeanWorld *world = nullptr;
    int err = 0;

    testdb = new SqliteBeanDB("###");
    err = testdb->clear();
    EXPECT_TRUE(err == -1);
    delete testdb;

    testdb = new SqliteBeanDB("/var/log");
    err = testdb->clear();
    EXPECT_TRUE(err == -2);
    delete testdb;

    testdb = new SqliteBeanDB(g_tmpDBDir);
    err = testdb->clear();
    EXPECT_TRUE(err == 0);

    testdb->connect();
    err = testdb->clear();
    EXPECT_TRUE(err == -1);

    testdb->disconnect();
    err = testdb->clear();
    EXPECT_TRUE(err == 0);
    delete testdb;
}


TEST(SqliteBeanDB, reinit)
{
    SqliteBeanDB* testdb = nullptr;
    BeanWorld *world = nullptr;
    int err = 0;
    // char buff[128] = {0};
    // char* cmd = &buff[0];

    testdb = new SqliteBeanDB("###");
    err = testdb->reInit();
    EXPECT_TRUE(err == -1);
    delete testdb;

    testdb = new SqliteBeanDB("/var/log");
    err = testdb->reInit();
    EXPECT_TRUE(err == -2);
    delete testdb;

    testdb = new SqliteBeanDB(g_tmpDBDir);
    err = testdb->reInit();
    EXPECT_TRUE(err == 0);

    testdb->connect();
    err = testdb->reInit();
    EXPECT_TRUE(err == -1);

    testdb->disconnect();
    err = testdb->reInit();
    EXPECT_TRUE(err == 0);
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
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());
    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    err = testdb.commitTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.commitTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());

    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.commitTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());
    err = testdb.commitTransaction();
    EXPECT_TRUE(err != 0);

    err = testdb.beginTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(testdb.inTransaction());
    err = testdb.rollbackTransaction();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!testdb.inTransaction());
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
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;
    Json::Value value, nativeData;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();   
    initTestHelper(testHelper, *world, false);
    validate_testdb_1(testdb);
 
    err = world->undefineProperty(testHelper.p1);
    EXPECT_TRUE(err == 0);
    property = world->getProperty("p1");
    EXPECT_TRUE(property == nullptr);

    err = world->undefineProperty(nullptr);
    EXPECT_TRUE(err == 0);

    bean1 = world->newBean();
    bean2 = world->newBean();
    bean3 = world->newBean();
    beanId_1 = bean1->getId();
    beanId_2 = bean1->getId();
    beanId_3 = bean1->getId();

    bean1->set(testHelper.p_int, 1);
    bean1->addArray(testHelper.p_array_int);
    bean1->append(testHelper.p_array_int, 0);
    bean1->append(testHelper.p_array_int, 1);

    bean2->set(testHelper.p_int, 1);
    bean2->addArray(testHelper.p_array_int);
    bean2->append(testHelper.p_array_int, 0);
    bean2->append(testHelper.p_array_int, 1);

    bean3->setRelation(testHelper.r1, beanId_1);
    bean3->addArray(testHelper.r_array_1);
    bean3->append(testHelper.r_array_1, beanId_1);
    bean3->append(testHelper.r_array_1, beanId_2);

    err = world->undefineProperty(testHelper.p_int);
    EXPECT_TRUE(err == 0);
    err = world->undefineProperty(testHelper.p_array_int);
    EXPECT_TRUE(err == 0);
    err = world->undefineRelation(testHelper.r1);
    EXPECT_TRUE(err == 0);
    err = world->undefineProperty(testHelper.r_array_1);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();   
    initTestHelper(testHelper, *world, false);
    
    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);
    bean3 = world->getBean(beanId_3);

    EXPECT_TRUE(bean1->get(testHelper.p_int).isNull());
    EXPECT_TRUE(bean1->get(testHelper.p_array_int).isNull());

    EXPECT_TRUE(bean2->get(testHelper.p_int).isNull());
    EXPECT_TRUE(bean2->get(testHelper.p_array_int).isNull());

    EXPECT_TRUE(bean3->get(testHelper.r1).isNull());
    EXPECT_TRUE(bean3->get(testHelper.r_array_1).isNull());

    testdb.loadBeanBase(beanId_1, value);
    EXPECT_TRUE(!value.isMember(testHelper.p_int->getName()));
    EXPECT_TRUE(!value.isMember(testHelper.p_array_int->getName()));

    testdb.loadBeanBase(beanId_2, value);
    EXPECT_TRUE(!value.isMember(testHelper.p_int->getName()));
    EXPECT_TRUE(!value.isMember(testHelper.p_array_int->getName()));

    testdb.loadBeanBase(beanId_3, value);
    EXPECT_TRUE(!value.isMember(testHelper.r1->getName()));
    EXPECT_TRUE(!value.isMember(testHelper.r_array_1->getName()));

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


TEST(SqliteBeanDB, world_createBean_deleteBean)
{
    Bean *bean = nullptr;
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    int err = 0;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();

    bean = world->newBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = world->newBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = world->newBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = world->deleteBean(bean);
    EXPECT_TRUE(err == 0);

    err = world->deleteBean(nullptr);
    EXPECT_TRUE(err == 0);

    testdb.reInit();
    testdb.disconnect();
}


TEST(SqliteBeanDB, loadBeanBase)
{
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld *world = nullptr;
    int err = 0;
    Property* property = nullptr;
    TestHelper testHelper;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;
    Json::Value value, nativeData;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();   
    initTestHelper(testHelper, *world, false);
    validate_testdb_1(testdb);
 
    bean1 = world->newBean();
    bean2 = world->newBean();
    bean3 = world->newBean();
    beanId_1 = bean1->getId();
    beanId_2 = bean2->getId();
    beanId_3 = bean3->getId();

    bean1->set(testHelper.p_int, 1);
    bean1->addArray(testHelper.p_array_int);
    bean1->append(testHelper.p_array_int, 0);
    bean1->append(testHelper.p_array_int, 1);

    nativeData["test"] = 1;
    bean1->setNativeData(nativeData);

    bean2->set(testHelper.p_int, 1);
    bean2->addArray(testHelper.p_array_int);
    bean2->append(testHelper.p_array_int, 0);
    bean2->append(testHelper.p_array_int, 1);

    bean3->setRelation(testHelper.r1, beanId_1);
    bean3->addArray(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, beanId_1);
    bean3->appendRelation(testHelper.r_array_1, beanId_2);

    world->saveAll();
    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();   
    initTestHelper(testHelper, *world, false);
    
    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);
    bean3 = world->getBean(beanId_3);

    testdb.loadBeanBase(beanId_1, value);
    EXPECT_TRUE(value[testHelper.p_int->getName()] == 1);
    EXPECT_TRUE(value.isMember(testHelper.p_array_int->getName()));
    EXPECT_TRUE(value[testHelper.p_array_int->getName()].isNull());

    testdb.loadBeanBase(beanId_2, value);
    EXPECT_TRUE(value[testHelper.p_int->getName()] == 1);
    EXPECT_TRUE(value.isMember(testHelper.p_array_int->getName()));
    EXPECT_TRUE(value[testHelper.p_array_int->getName()].isNull());

    testdb.loadBeanBase(beanId_3, value);
    EXPECT_TRUE(value[testHelper.r1->getName()] == 1u);
    EXPECT_TRUE(value[testHelper.r_array_1->getName()][0] == 1u);
    EXPECT_TRUE(value[testHelper.r_array_1->getName()][1] == 2u);


    testdb.loadBeanBase(beanId_1, value, &nativeData);
    EXPECT_TRUE(nativeData["test"] == 1);

    testdb.loadBeanBase(beanId_2, value, &nativeData);
    EXPECT_TRUE(nativeData.isNull());

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

    bean1 = world->getBean(555);
    EXPECT_TRUE(bean1 == nullptr);

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

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    bean1->set(testHelper.p_int, 1, false);
    bean1->set(testHelper.p_bool_1, true, false);
    bean1->set(testHelper.p_str, "str", false);

    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);
    bean1 = world->getBean(beanId_1);
    EXPECT_TRUE(bean1->get(testHelper.p_int).isNull());
    EXPECT_TRUE(bean1->get(testHelper.p_bool_1).isNull());
    EXPECT_TRUE(bean1->get(testHelper.p_str).isNull());

    bean1->set(testHelper.p_int, 1, false);
    bean1->set(testHelper.p_bool_1, true, false);
    bean1->set(testHelper.p_str, "str", false);
    bean1->save();
    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);
    bean1 = world->getBean(beanId_1);
    EXPECT_TRUE(!bean1->get(testHelper.p_int).isNull());
    EXPECT_TRUE(!bean1->get(testHelper.p_bool_1).isNull());
    EXPECT_TRUE(!bean1->get(testHelper.p_str).isNull());
    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);
    setBeanNativeData(testHelper, bean1);

    bean2 = world->newBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);
    setBeanNativeData(testHelper, bean2);

    bean3 = world->newBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);
    setBeanNativeData(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->addArray(testHelper.r_array_1);
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

    EXPECT_TRUE(bean3->getObjectId(testHelper.r1) ==beanId_1);
    EXPECT_TRUE(bean3->getObjectId(testHelper.r2) ==beanId_2);
    EXPECT_TRUE(bean3->getObjectId(testHelper.r_array_1, 0) ==beanId_1);
    EXPECT_TRUE(bean3->getObjectId(testHelper.r_array_1, 1) ==beanId_2);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(beanId_1);
    bean3 = world->getBean(beanId_3);

    err = bean1->setAt(testHelper.p_array_int, 0, 99, false);    
    EXPECT_TRUE(err == 0);
    err = bean3->setRelationAt(testHelper.r_array_1, 0, bean3->getId());
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

    value = bean1->getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    beanId_3 = bean3->getObjectId(testHelper.r_array_1, 0);
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
    size = bean3->size(testHelper.r_array_1);
    EXPECT_TRUE(size == 2);
    size = bean3->size(testHelper.p_array_str);
    EXPECT_TRUE(size == 2);

    bean3->addArray(testHelper.p_array_str);
    bean3->append(testHelper.p_array_str, "str3");
    bean3->append(testHelper.p_array_str, "str4");

    testdb.disconnect();

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    beanId_1 = bean1->getId();
    value = bean1->get(testHelper.p_str);
    EXPECT_TRUE(value == "foo");
    value = bean1->getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 101);

    bean3 = world->getBean(3);
    beanId_1 = bean3->getObjectId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId_1 == 1);

    size = bean3->size(testHelper.p_array_str);
    EXPECT_TRUE(size == 4);

    testdb.disconnect();

    testdb.connect();
   world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(1);
    bean2 = world->getBean(2);
    err = bean1->setAt(testHelper.p_array_int, 0, 99);
    EXPECT_TRUE(err == 0);
    bean3 = world->getBean(3);
    err = bean3->setRelationAt(testHelper.r_array_1, 0, bean2->getId());
    EXPECT_TRUE(err == 0);

    testdb.disconnect();

    testdb.connect();
   world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);
    bean1 = world->getBean(1);
    err = bean1->append(testHelper.p_array_int, 100);
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
    value = bean1->getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    value = bean1->getAt(testHelper.p_array_int, bean1->size(testHelper.p_array_int) - 1);
    EXPECT_TRUE(value == 100);

    bean3 = world->getBean(3);
    beanId_2 = bean3->getObjectId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId_2 == 2);
    beanId_3 = bean3->getObjectId(testHelper.r_array_1, bean3->size(testHelper.r_array_1) - 1);
    EXPECT_TRUE(beanId_3 == 3);

    testdb.disconnect();
}

TEST(SqliteBeanDB, loadProperties)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    testdb.connect();

    std::unordered_map<std::string, Property*> propertyMap;
    err = testdb.loadProperties(propertyMap);
    testdb.m_world_->m_propertyMap_ = propertyMap;
    validate_properties_testdb_1(testdb.m_world_->m_propertyMap_);
    
    testdb.disconnect();

}


TEST(SqliteBeanDB, loadBeanProperty)
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
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asInt() == -1);

    err = testdb.loadBeanProperty(beanId_1, testHelper.p_uint, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt() == 1);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_int64, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asInt64() == -1);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_uint64, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt64() == 1);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_bool_0, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == false);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_bool_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == true);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_real, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asDouble() == 1.0);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_str, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value == "foo");

    err = testdb.loadBeanProperty(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value[0] == 101);
    EXPECT_TRUE(value[1] == 102);

    err = testdb.loadBeanProperty(beanId_3, testHelper.r1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.asUInt64() == 1);
    err = testdb.loadBeanProperty(beanId_3, testHelper.r2, value);
    EXPECT_TRUE(err  == 0);
    EXPECT_TRUE(value.asUInt64()  == 2);
    err = testdb.loadBeanProperty(beanId_3, testHelper.r_array_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value[0].asUInt64()  == 1);
    EXPECT_TRUE(value[1].asUInt64()  == 2);

    testdb.disconnect();

}


TEST(SqliteBeanDB, insertBeanProperty)
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

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();

    err = testdb.insertBeanProperty(9999, testHelper.p_int, 1);
    EXPECT_TRUE(err == -1001);

    bean1->addArray(testHelper.p_array_int);
    err = testdb.insertBeanProperty(beanId_1, testHelper.p_int, 1);
    EXPECT_TRUE(err == 0);

    err = testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    err = testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    testdb.loadBeanProperty(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(value == 1);
    testdb.loadBeanProperty(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(value[0] == 0);
    EXPECT_TRUE(value[1] == 1);

    testdb.disconnect();

}

TEST(SqliteBeanDB, updateBeanProperty)
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

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();

    Bean* bean3 = world->getBean(3);
    oidType beanId_3 = bean3->getId();

    err = testdb.updateBeanProperty(9999, testHelper.p_str, "foo");
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_int, 9999, 1);
    EXPECT_TRUE(err == 0);

    bean1->set(testHelper.p_str, "xxx");
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_str, "foo");
    EXPECT_TRUE(err == 0);

    err = testdb.updateBeanProperty(beanId_3, testHelper.r1, 3);
    EXPECT_TRUE(err == 0);

    bean1->addArray(testHelper.p_array_int);
    bean1->addArray(testHelper.p_array_bool);
    bean1->addArray(testHelper.p_array_real);

    testdb.insertBeanProperty(beanId_1, testHelper.p_int, 1);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 1);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_bool, false);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_bool, false);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_real, 1.0);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_real, 1.1);

    err = testdb.updateBeanProperty(beanId_1, testHelper.p_int, 99);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_int, 0, 10);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_int, 1, 11);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_bool, 0, true);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_bool, 1, true);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_real, 0, 555.555);
    EXPECT_TRUE(err == 0);
    err = testdb.updateBeanProperty(beanId_1, testHelper.p_array_real, 1, 888.888);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    testdb.loadBeanProperty(beanId_1, testHelper.p_str, value);
    EXPECT_TRUE(value == "foo");
    testdb.loadBeanProperty(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(value == 99);
    testdb.loadBeanProperty(beanId_3, testHelper.r1, value);
    EXPECT_TRUE(value == 3u);
    testdb.loadBeanProperty(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(value[0] == 10);
    EXPECT_TRUE(value[1] == 11);
    testdb.loadBeanProperty(beanId_1, testHelper.p_array_bool, value);
    EXPECT_TRUE(value[0] == true);
    EXPECT_TRUE(value[1] == true);
    testdb.loadBeanProperty(beanId_1, testHelper.p_array_real, value);
    EXPECT_TRUE(value[0] == 555.555);
    EXPECT_TRUE(value[1] == 888.888);

    testdb.disconnect();

}

TEST(SqliteBeanDB, deleteBeanProperty)
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

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();
    
    bean1->addArray(testHelper.p_array_int);

    testdb.insertBeanProperty(beanId_1, testHelper.p_int, 1);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 1);

    err = testdb.deleteBeanProperty(9999, testHelper.p_int);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty(9999, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty(beanId_1, testHelper.p_array_int, 9999);
    EXPECT_TRUE(err == 0);

    err = testdb.deleteBeanProperty(beanId_1, testHelper.p_int);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty(beanId_1, testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBeanProperty(beanId_1, testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    err = testdb.loadBeanProperty(beanId_1, testHelper.p_int, value);
    EXPECT_TRUE(err != 0);
    err = testdb.loadBeanProperty(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isArray() && value.size() == 0);

    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 0);
    testdb.insertBeanProperty(beanId_1, testHelper.p_array_int, 1);
    err = testdb.deleteBeanProperty(beanId_1, testHelper.p_array_int);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);
    bean1 = world->getBean(beanId_1);

    err = testdb.loadBeanProperty(beanId_1, testHelper.p_array_int, value);
    EXPECT_TRUE(err != 0);

    testdb.disconnect();

}


TEST(SqliteBeanDB, deleteBean)
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
    
    testdb.loadBeanProperty(beanId_3, testHelper.r1, value);
    EXPECT_TRUE(value.asUInt64() == 1);
    testdb.loadBeanProperty(beanId_3, testHelper.r2, value);
    EXPECT_TRUE(value.asUInt64()  == 2);
    testdb.loadBeanProperty(beanId_3, testHelper.r_array_1, value);
    EXPECT_TRUE(value[0].asUInt64()  == 1);
    EXPECT_TRUE(value[1].asUInt64()  == 2);

    err = testdb.deleteBean(999);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBean(1);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteBean(2);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    err = testdb.loadBeanProperty(beanId_3, testHelper.r1, value);
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(value.isNull());
    err = testdb.loadBeanProperty(beanId_3, testHelper.r2, value);
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(value.isNull());
    err = testdb.loadBeanProperty(beanId_3, testHelper.r_array_1, value);
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(value.isNull());

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

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();

    err = testdb.loadBeanNativeData(beanId_1, value);
    EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(value.getMemberNames().size() == 0);
    EXPECT_TRUE(value.isNull());

    err = testdb.updateBeanNativeData(beanId_1, Json::Value::nullRef);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    err = testdb.loadBeanNativeData(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isNull());

    testdb.disconnect();
    testdb.connect();
    value["root"][0] = 0;
    value["root"][1] = 1;
    err = testdb.updateBeanNativeData(beanId_1, value);
    EXPECT_TRUE(err == 0);
    
    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();

    bean1 = world->getBean(beanId_1);
    value = bean1->getNativeData();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value["root"][0] == 0);
    EXPECT_TRUE(value["root"][1] == 1);

    err = testdb.loadBeanNativeData(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value["root"][0] == 0);
    EXPECT_TRUE(value["root"][1] == 1);

    err = testdb.deleteBeanNativeData(beanId_1);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    err = testdb.loadBeanNativeData(beanId_1, value);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(value.isNull());

    testdb.disconnect();

}


TEST(SqliteBeanDB, findEqual)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();

    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_bool_0, false);
    bean1->set(testHelper.p_bool_1, true);
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 2U);
    bean1->set(testHelper.p_int64, 3);
    bean1->set(testHelper.p_uint64, 4U);

    bean2->set(testHelper.p_real, 1.0);
    bean2->set(testHelper.p_str, "hello");
    bean2->set(testHelper.p_bool_0, false);
    bean2->set(testHelper.p_bool_1, true);
    bean2->set(testHelper.p_int, 1);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 3);
    bean2->set(testHelper.p_uint64, 4U);

    bean1->save();
    bean2->save();

    page = testdb.findBeans(op_le, testHelper.p_bool_0, false);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_lt, testHelper.p_bool_0, false);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_ge, testHelper.p_bool_1, true);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_gt, testHelper.p_bool_1, true);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.p_bool_1, true);
    EXPECT_TRUE(page == nullptr);

    page = world->findEqual(testHelper.p_real, 1.0);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) == 1.0);
    }

    page = world->findEqual(testHelper.p_bool_0, false);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_bool_0) == false);
    }

    page = world->findEqual(testHelper.p_bool_1, true);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_bool_1) == true);
    }

    page = world->findEqual(testHelper.p_str, "hello");
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str) == "hello");
    }

    page = world->findEqual(testHelper.p_int, (int_t)1);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) == 1);
    }

    page = world->findEqual(testHelper.p_uint, (uint_t)2U);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) == 2U);
    }

    testdb.disconnect();
}


TEST(SqliteBeanDB, findLessThan)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    find_beanInit(testHelper, bean1, bean2, bean3);

    page = world->findLessThan(testHelper.p_real, 1.0);
    EXPECT_TRUE(page->size() == 0);
    page = testdb.findBeans(op_lt, testHelper.p_real, 2.0);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) < 2.0);
    }
    page = world->findLessThan(testHelper.p_real, 3.0);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) < 3.0);
    }
    page = world->findLessThan(testHelper.p_real, 4.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) < 4.0);
    }

    page = world->findLessThan(testHelper.p_str, "hello");
    EXPECT_TRUE(page->size() == 0);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("hello") < 0);
    }
    page = world->findLessThan(testHelper.p_str, "my");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("my") < 0);
    }
    page = world->findLessThan(testHelper.p_str, "world");
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("world") < 0);
    }
    page = world->findLessThan(testHelper.p_str, "z");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("z") < 0);
    }

    page = world->findLessThan(testHelper.p_int, (int_t)1);
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessThan(testHelper.p_int, (int_t)2);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) < 2);
    }
    page = world->findLessThan(testHelper.p_int, (int_t)3);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) < 3);
    }
    page = world->findLessThan(testHelper.p_int, (int_t)4);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) < 4);
    }

    page = world->findLessThan(testHelper.p_uint, (uint_t)1);
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessThan(testHelper.p_uint, (uint_t)2);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) < (uint_t)2);
    }
    page = world->findLessThan(testHelper.p_uint, (uint_t)3);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) < (uint_t)3);
    }
    page = world->findLessThan(testHelper.p_uint, (uint_t)4);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) < (uint_t)4);
    }

    testdb.disconnect();
}


TEST(SqliteBeanDB, findLessEqual)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    find_beanInit(testHelper, bean1, bean2, bean3);

    page = world->findLessEqual(testHelper.p_real, 0.0);
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessEqual(testHelper.p_real, 1.0);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) <= 1.0);
    }
    page = world->findLessEqual(testHelper.p_real, 2.0);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) <= 2.0);
    }
    page = world->findLessEqual(testHelper.p_real, 3.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) <= 3.0);
    }
    page = world->findLessEqual(testHelper.p_real, 4.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) <= 4.0);
    }

    page = world->findLessEqual(testHelper.p_str, "a");
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessEqual(testHelper.p_str, "hello");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("hello") <= 0);
    }
    page = world->findLessEqual(testHelper.p_str, "my");
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("my") <= 0);
    }
    page = world->findLessEqual(testHelper.p_str, "world");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("world") <= 0);
    }
    page = world->findLessEqual(testHelper.p_str, "z");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("z") <= 0);
    }

    page = world->findLessEqual(testHelper.p_int, (int_t)0);
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessEqual(testHelper.p_int, (int_t)1);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) <= 1);
    }
    page = world->findLessEqual(testHelper.p_int, (int_t)2);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) <= 2);
    }
    page = world->findLessEqual(testHelper.p_int, (int_t)3);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) <= 3);
    }
    page = world->findLessEqual(testHelper.p_int, (int_t)4);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) <= 4);
    }

    page = world->findLessEqual(testHelper.p_uint, (uint_t)0);
    EXPECT_TRUE(page->size() == 0);
    page = world->findLessEqual(testHelper.p_uint, (uint_t)1);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) <= (uint_t)1);
    }
    page = world->findLessEqual(testHelper.p_uint, (uint_t)2);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) <= (uint_t)2);
    }
    page = world->findLessEqual(testHelper.p_uint, (uint_t)3);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) <= (uint_t)3);
    }
    page = world->findLessEqual(testHelper.p_uint, (uint_t)4);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) <= (uint_t)4);
    }

    testdb.disconnect();
}


TEST(SqliteBeanDB, findGreaterThan)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    find_beanInit(testHelper, bean1, bean2, bean3);

    bean1->save();
    bean2->save();
    bean3->save();

    page = world->findGreaterThan(testHelper.p_real, 0.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) > 0.0);
    }
    page = world->findGreaterThan(testHelper.p_real, 1.0);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) > 1.0);
    }
    page = world->findGreaterThan(testHelper.p_real, 2.0);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) > 2.0);
    }
    page = world->findGreaterThan(testHelper.p_real, 3.0);
    EXPECT_TRUE(page->size() == 0);

    page = world->findGreaterThan(testHelper.p_str, "a");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("a") > 0);
    }
    page = world->findGreaterThan(testHelper.p_str, "hello");
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("hello") > 0);
    }
    page = world->findGreaterThan(testHelper.p_str, "my");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("my") > 0);
    }
    page = world->findGreaterThan(testHelper.p_str, "world");
    EXPECT_TRUE(page->size() == 0);

    page = world->findGreaterThan(testHelper.p_int, 0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) > 0);
    }
    page = world->findGreaterThan(testHelper.p_int, 1);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) > 1);
    }
    page = world->findGreaterThan(testHelper.p_int, 2);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) > 2);
    }
    page = world->findGreaterThan(testHelper.p_int, 3);
    EXPECT_TRUE(page->size() == 0);

    page = world->findGreaterThan(testHelper.p_uint, (uint_t)0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) > (uint_t)0);
    }
    page = world->findGreaterThan(testHelper.p_uint, (uint_t)1);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) > (uint_t)1);
    }
    page = world->findGreaterThan(testHelper.p_uint, (uint_t)2);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) > (uint_t)2);
    }
    page = world->findGreaterThan(testHelper.p_uint, (uint_t)3);
    EXPECT_TRUE(page->size() == 0);

    testdb.disconnect();
}


TEST(SqliteBeanDB, findGreaterEqual)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    find_beanInit(testHelper, bean1, bean2, bean3);

    bean1->save();
    bean2->save();
    bean3->save();

    page = world->findGreaterEqual(testHelper.p_real, 0.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) >= 0.0);
    }
    page = world->findGreaterEqual(testHelper.p_real, 1.0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) >= 1.0);
    }
    page = world->findGreaterEqual(testHelper.p_real, 2.0);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_real) >= 2.0);
    }
    page = world->findGreaterEqual(testHelper.p_real, 3.0);
    EXPECT_TRUE(page->size() == 1);

    page = world->findGreaterEqual(testHelper.p_str, "a");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("a") >= 0);
    }
    page = world->findGreaterEqual(testHelper.p_str, "hello");
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("hello") >= 0);
    }
    page = world->findGreaterEqual(testHelper.p_str, "my");
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("my") >= 0);
    }
    page = world->findGreaterEqual(testHelper.p_str, "world");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).compare("world") >= 0);
    }
    page = world->findGreaterEqual(testHelper.p_str, "z");
    EXPECT_TRUE(page->size() == 0);

    page = world->findGreaterEqual(testHelper.p_int, 0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) >= 0);
    }
    page = world->findGreaterEqual(testHelper.p_int, 1);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) >= 1);
    }
    page = world->findGreaterEqual(testHelper.p_int, 2);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) >= 2);
    }
    page = world->findGreaterEqual(testHelper.p_int, 3);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_int) >= 3);
    }
    page = world->findGreaterEqual(testHelper.p_int, 4);
    EXPECT_TRUE(page->size() == 0);

    page = world->findGreaterEqual(testHelper.p_uint, (uint_t)0);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) >= (uint_t)0);
    }
    page = world->findGreaterEqual(testHelper.p_uint, (uint_t)1);
    EXPECT_TRUE(page->size() == 3);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) >= (uint_t)1);
    }
    page = world->findGreaterEqual(testHelper.p_uint, (uint_t)2);
    EXPECT_TRUE(page->size() == 2);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) >= (uint_t)2);
    }
    page = world->findGreaterEqual(testHelper.p_uint, (uint_t)3);
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_uint) >= (uint_t)3);
    }
    page = world->findGreaterEqual(testHelper.p_uint, (uint_t)4);
    EXPECT_TRUE(page->size() == 0);

    testdb.disconnect();
}


TEST(SqliteBeanDB, find_like)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();
    Bean* bean2 = world->newBean();
    oidType beanId_2= bean2->getId();
    Bean* bean3 = world->newBean();
    oidType beanId_3 = bean3->getId();

    bean1->set(testHelper.p_str, "beandb");
    bean2->set(testHelper.p_str, "is");
    bean3->set(testHelper.p_str, "amazing!");

    bean1->addArray(testHelper.p_array_str);
    bean2->addArray(testHelper.p_array_str);
    bean3->addArray(testHelper.p_array_str);

    bean1->append(testHelper.p_array_str, "bean1 #1");
    bean1->append(testHelper.p_array_str, "bean1 #2");
    bean2->append(testHelper.p_array_str, "bean2 #1");
    bean2->append(testHelper.p_array_str, "bean2 #2");
    bean3->append(testHelper.p_array_str, "bean3 #1");
    bean3->append(testHelper.p_array_str, "bean3 #2");

    world->saveAll();

    page = testdb.findBeans(op_like, testHelper.p_int, 1);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.p_uint, 1u);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.p_real, 1.0);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.p_bool_0, false);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.p_array_int, 1);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.r1, 1);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.r_array_1, 1);
    EXPECT_TRUE(page == nullptr);

    page = testdb.findBeans(op_like, testHelper.p_str, "");
    EXPECT_TRUE(page->size() == 0);

    page = testdb.findBeans(op_like, testHelper.p_str, "%a%");
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean3->getId());
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).asString().find("a") != std::string::npos);
    }

    page = testdb.findBeans(op_like, testHelper.p_str, "%i%");
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean2->getId());
    EXPECT_TRUE(page->at(1) == bean3->getId());
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->get(testHelper.p_str).asString().find("i") != std::string::npos);
    }

    page = world->findLike(testHelper.p_array_str, "bean1%");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->getAt(testHelper.p_array_str, 0).asString().find("bean1") == 0);
    }
    page = testdb.findBeans(op_like, testHelper.p_array_str, "bean2%");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->getAt(testHelper.p_array_str, 0).asString().find("bean2") == 0);
    }
    page = testdb.findBeans(op_like, testHelper.p_array_str, "bean3%");
    EXPECT_TRUE(page->size() == 1);
    for (size_t i = 0; i < page->size(); i++)
    {
        EXPECT_TRUE(world->getBean(page->at(i))->getAt(testHelper.p_array_str, 0).asString().find("bean3") == 0);
    }
}


TEST(SqliteBeanDB, findEqual_relation)
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
    BeanIdPage* page = nullptr;
    // vector<oidType>* beanIds;

    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    oidType beanId_1 = bean1->getId();
    oidType beanId_2 = bean2->getId();
    oidType beanId_3 = bean3->getId();

    bean1->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    bean3->setRelation(testHelper.r1, bean3);
    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    bean3->addArray(testHelper.r_array_1);
    bean1->appendRelation(testHelper.r_array_1, bean1);
    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean1->appendRelation(testHelper.r_array_1, bean3);
    bean2->appendRelation(testHelper.r_array_1, bean1);
    bean2->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean3);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);
    bean3->appendRelation(testHelper.r_array_1, bean3);

    page = testdb.findBeans(op_le, testHelper.r1, 1u);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_lt, testHelper.r1, 1u);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_ge, testHelper.r_array_1, 1u);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_gt, testHelper.r_array_1, 1u);
    EXPECT_TRUE(page == nullptr);
    page = testdb.findBeans(op_like, testHelper.r_array_1, 1u);
    EXPECT_TRUE(page == nullptr);

    page = world->findEqual(nullptr, 1);
    EXPECT_TRUE(page == nullptr);
    page = world->findEqual(testHelper.r1, 1, 0);
    EXPECT_TRUE(page == nullptr);
    page = world->findEqual(testHelper.r1, Json::Value::nullRef);
    EXPECT_TRUE(page == nullptr);
    value = Json::Value(Json::arrayValue);
    page = world->findEqual(testHelper.r1, value);
    EXPECT_TRUE(page == nullptr);
    value = Json::Value(Json::objectValue);
    page = world->findEqual(testHelper.r1, value);
    EXPECT_TRUE(page == nullptr);

    page = world->findEqual(testHelper.r1, bean3->getId(), 3);
    EXPECT_TRUE(page != nullptr);
    EXPECT_TRUE(page->getPageSize() == 3);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 3);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    EXPECT_TRUE(page->at(2) == bean3->getId());
    delete page;
 
    page = world->findEqual(testHelper.r1, bean3->getId(), 1);
    EXPECT_TRUE(page != nullptr);
    EXPECT_TRUE(page->getPageSize() == 1);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    err = page->next();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean2->getId());
    err = page->next();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean3->getId());
    err = page->next();
    EXPECT_TRUE(err == -1001);
    delete page;

    page = world->findEqual(testHelper.r1, bean3->getId(), 2);
    EXPECT_TRUE(page != nullptr);
    EXPECT_TRUE(page->getPageSize() == 2);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    err = page->next();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean3->getId());
    err = page->next();
    EXPECT_TRUE(page->getPageIndex() == 1);
    EXPECT_TRUE(err == -1001);
    err = page->prev();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    err = page->prev();
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(err == -2);
    err = page->gotoPage(0);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 0);
    err = page->gotoPage(1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 1);
    err = page->gotoPage(2);
    EXPECT_TRUE(err != 0);
    EXPECT_TRUE(page->getPageIndex() == 1);
    
    delete page;

    page = world->findEqual(testHelper.r_array_1, bean1->getId(), 3);
    EXPECT_TRUE(page != nullptr);
    EXPECT_TRUE(page->size() == 3);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    EXPECT_TRUE(page->at(2) == bean3->getId());
    delete page;

    page = world->findEqual(testHelper.r_array_1, bean1->getId(), 2);
    EXPECT_TRUE(page != nullptr);
    EXPECT_TRUE(page->getPageSize() == 2);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    err = page->next();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean3->getId());
    err = page->prev();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->getPageIndex() == 0);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    delete page;

    testdb.disconnect();

    page = testdb.findBeans(op_eq, testHelper.r1, bean1->getId());
    EXPECT_TRUE(page == nullptr);
    delete page;
}


TEST(SqliteBeanDB, findSubjects)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    page = world->findSubjects(testHelper.p_int);
    EXPECT_TRUE(page == nullptr);
    page = world->findSubjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 0);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    bean1->setRelation(testHelper.r1, bean3);
    page = world->findSubjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    bean2->setRelation(testHelper.r1, bean3);
    page = world->findSubjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());

    bean1->remove(testHelper.r1);
    page = world->findSubjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean2->getId());

    bean2->remove(testHelper.r1);
    page = world->findSubjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 0);

    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    bean1->appendRelation(testHelper.r_array_1, bean1);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    bean1->appendRelation(testHelper.r_array_1, bean2);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    bean2->appendRelation(testHelper.r_array_1, bean1);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    bean2->appendRelation(testHelper.r_array_1, bean2);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());

    bean1->remove(testHelper.r_array_1);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean2->getId());
    bean2->remove(testHelper.r_array_1);
    page = world->findSubjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    testdb.disconnect();
}


TEST(SqliteBeanDB, findOjects)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    page = world->findObjects(testHelper.p_int);
    EXPECT_TRUE(page == nullptr);
    page = world->findObjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 0);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    bean1->setRelation(testHelper.r1, bean1);
    page = world->findObjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    bean2->setRelation(testHelper.r1, bean2);
    page = world->findObjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());

    bean1->remove(testHelper.r1);
    page = world->findObjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean2->getId());

    bean2->remove(testHelper.r1);
    page = world->findObjects(testHelper.r1);
    EXPECT_TRUE(page->size() == 0);

    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    bean1->appendRelation(testHelper.r_array_1, bean1);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    bean1->appendRelation(testHelper.r_array_1, bean2);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    bean2->appendRelation(testHelper.r_array_1, bean2);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    bean2->appendRelation(testHelper.r_array_1, bean3);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 3);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    EXPECT_TRUE(page->at(2) == bean3->getId());

    bean1->remove(testHelper.r_array_1);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean2->getId());
    EXPECT_TRUE(page->at(1) == bean3->getId());
    bean2->remove(testHelper.r_array_1);
    page = world->findObjects(testHelper.r_array_1);
    EXPECT_TRUE(page->size() == 0);

    testdb.disconnect();
}


TEST(SqliteBeanDB, getAllBeans)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    page = world->getAllBeans(2);
    EXPECT_TRUE(page->size() == 0);
    delete page;

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    page = world->getAllBeans(2);
    EXPECT_TRUE(page->size() == 2);
    EXPECT_TRUE(page->at(0) == bean1->getId());
    EXPECT_TRUE(page->at(1) == bean2->getId());
    err = page->next();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(page->size() == 1);
    EXPECT_TRUE(page->at(0) == bean3->getId());

    testdb.disconnect();
}


TEST(SqliteBeanDB, BeanWorld_reloadAll)
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;
    Bean* bean = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world);

    err = world->loadAll();
    EXPECT_TRUE(err == 0);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    world->unloadBean(bean1);
    world->unloadBean(bean2);
    world->unloadBean(bean3);
    bean = world->getBean(bean1->getId(), false);
    EXPECT_TRUE(bean == nullptr);
    bean = world->getBean(bean2->getId(), false);
    EXPECT_TRUE(bean == nullptr);
    bean = world->getBean(bean3->getId(), false);

    err = world->loadAll();
    EXPECT_TRUE(err == 0);

    bean = world->getBean(bean1->getId(), false);
    EXPECT_TRUE(bean->getId() == bean1->getId());
    bean = world->getBean(bean2->getId(), false);
    EXPECT_TRUE(bean->getId() == bean2->getId());
    bean = world->getBean(bean3->getId(), false);
    EXPECT_TRUE(bean->getId() == bean3->getId());

    testdb.disconnect();
}