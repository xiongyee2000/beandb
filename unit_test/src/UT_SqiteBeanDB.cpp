

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
static void evaluate_testdb_empty_property(SqliteBeanDB& testdb);
void initTestHelper(TestHelper& testHelper, AbstractBeanDB& db, bool needIndex);
void setBean(TestHelper& testHelper, Bean* bean);
void validateBean(TestHelper& testHelper, Bean* bean);

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
    AbstractBeanDB* testdbPtr = &testdb;
    BeanWorld world(*testdbPtr);
    int err = 0;
    Property* property = nullptr;
    TestHelper testHelper;

    testdb.reInit();
    testdb.connect();

    initTestHelper(testHelper, testdb, false);
    validate_testdb_1(testdb);

    err = testdb.undefineProperty("p1");
    EXPECT_TRUE(err == 0);
    property = testdb.getProperty("p1");
    EXPECT_TRUE(property == nullptr);

    err = testdb.undefineProperty(nullptr);
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("");
    EXPECT_TRUE(err == 0);

    testdb.disconnect();

}


TEST(SqliteBeanDB, getProperty)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);

    testdb.connect();

   EXPECT_TRUE(testdb.getProperty("p1")->getName() == "p1" && 
        testdb.getProperty("p1")->getType() == Property::PrimaryType && 
        testdb.getProperty("p1")->getValueType() == Property::IntType);
    EXPECT_TRUE(testdb.getProperty("p2")->getName() == "p2" && 
        testdb.getProperty("p2")->getType() == Property::PrimaryType && 
        testdb.getProperty("p2")->getValueType() == Property::IntType);

   EXPECT_TRUE(testdb.getProperty("p_int")->getName() == "p_int" && 
        testdb.getProperty("p_int")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_int")->getValueType() == Property::IntType);
   EXPECT_TRUE(testdb.getProperty("p_uint")->getName() == "p_uint" && 
        testdb.getProperty("p_uint")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_uint")->getValueType() == Property::UIntType);
    EXPECT_TRUE(testdb.getProperty("p_real")->getName() == "p_real" && 
        testdb.getProperty("p_real")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_real")->getValueType() == Property::RealType);
    EXPECT_TRUE(testdb.getProperty("p_str")->getName() == "p_str" && 
        testdb.getProperty("p_str")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_str")->getValueType() == Property::StringType);
    EXPECT_TRUE(testdb.getProperty("p_bool_0")->getName() == "p_bool_0" && 
        testdb.getProperty("p_bool_0")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_bool_0")->getValueType() == Property::BoolType);
    EXPECT_TRUE(testdb.getProperty("p_bool_1")->getName() == "p_bool_1" && 
        testdb.getProperty("p_bool_1")->getType() == Property::PrimaryType && 
        testdb.getProperty("p_bool_1")->getValueType() == Property::BoolType);

    EXPECT_TRUE(testdb.getProperty("p_array_int")->getName() == "p_array_int" && 
        testdb.getProperty("p_array_int")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("p_array_int")->getValueType() == Property::IntType);
    EXPECT_TRUE(testdb.getProperty("p_array_uint")->getName() == "p_array_uint" && 
        testdb.getProperty("p_array_uint")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("p_array_uint")->getValueType() == Property::UIntType);
    EXPECT_TRUE(testdb.getProperty("p_array_real")->getName() == "p_array_real" && 
        testdb.getProperty("p_array_real")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("p_array_real")->getValueType() == Property::RealType);
    EXPECT_TRUE(testdb.getProperty("p_array_str")->getName() == "p_array_str" && 
        testdb.getProperty("p_array_str")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("p_array_str")->getValueType() == Property::StringType);
    EXPECT_TRUE(testdb.getProperty("p_array_bool")->getName() == "p_array_bool" && 
        testdb.getProperty("p_array_bool")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("p_array_bool")->getValueType() == Property::BoolType);

    EXPECT_TRUE(testdb.getProperty("r1")->getName() == "r1" && 
        testdb.getProperty("r1")->getType() == Property::RelationType);
    EXPECT_TRUE(testdb.getProperty("r2")->getName() == "r2" && 
        testdb.getProperty("r2")->getType() == Property::RelationType);

    EXPECT_TRUE(testdb.getProperty("r_array_1")->getName() == "r_array_1" && 
        testdb.getProperty("r_array_1")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(testdb.getProperty("r_array_2")->getName() == "r_array_2" && 
        testdb.getProperty("r_array_2")->getType() == Property::ArrayRelationType);

   testdb.disconnect();
}

TEST(SqliteBeanDB, getProperties)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    testdb.connect();

    const std::unordered_map<std::string, Property*>& pmap = testdb.getProperties();
    testdb.m_world_->m_propertyMap_ = pmap;
    validate_properties_testdb_1(testdb.m_world_->m_propertyMap_);
    
    testdb.disconnect();

}

TEST(SqliteBeanDB, createBean_deleteBean)
{
    Bean *bean = nullptr;
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world((AbstractBeanDB&)testdb);
    int err = 0;

    testdb.reInit();
    testdb.connect();

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    err = testdb.deleteBean(nullptr);
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
    BeanWorld world((AbstractBeanDB&)testdb);
    Bean* bean = nullptr;

    testdb.connect();

    TestHelper testHelper;
    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.getBean(1);
    bean2 = testdb.getBean(1);
    bean3 = testdb.getBean(1);
    validateBean(testHelper, bean1);
    validateBean(testHelper, bean2);
    validateBean(testHelper, bean3);
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
    BeanWorld world((AbstractBeanDB&)testdb);
    TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.reInit();
    testdb.connect();

    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.createBean();
    beanId_1 = bean1->getId();
    setBean(testHelper, bean1);

    bean2 = testdb.createBean();
    beanId_2 = bean2->getId();
    setBean(testHelper, bean2);

    bean3 = testdb.createBean();
    beanId_3 = bean3->getId();
    setBean(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->createArrayRelation(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = testdb.saveBean(bean1);
    EXPECT_TRUE(err == 0);
    err = testdb.saveBean(bean2);
    EXPECT_TRUE(err == 0);
    err = testdb.saveBean(bean3);
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.getBean(beanId_1);
    bean2 = testdb.getBean(beanId_2);
    bean3 = testdb.getBean(beanId_3);

    validateBean(testHelper, bean1);
    validateBean(testHelper, bean2);

    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r1) ==beanId_1);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r2) ==beanId_2);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r_array_1, 0) ==beanId_1);
    EXPECT_TRUE(bean3->getRelationBeanId(testHelper.r_array_1, 1) ==beanId_2);

    testdb.reInit();
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


void evaluate_testdb_empty_property(SqliteBeanDB& testdb)
{
    testdb.connect();
    std::unordered_map<std::string, Property*> propertyMap;
    testdb.loadProperties_(propertyMap);
    EXPECT_TRUE(propertyMap.size() == 0);
    testdb.disconnect();
}

void initTestHelper(TestHelper& testHelper, AbstractBeanDB& db, bool needIndex)
{
    testHelper.p_int = db.defineProperty("p_int", Property::IntType);
    testHelper.p_uint = db.defineProperty("p_uint", Property::UIntType);
    testHelper.p_int64 = db.defineProperty("p_int64", Property::IntType);
    testHelper.p_uint64 = db.defineProperty("p_uint64", Property::UIntType);
    testHelper.p_real = db.defineProperty("p_real", Property::RealType);
    testHelper.p_str = db.defineProperty("p_str", Property::StringType);
    testHelper.p_bool_0 = db.defineProperty("p_bool_0", Property::BoolType);
    testHelper.p_bool_1 = db.defineProperty("p_bool_1", Property::BoolType);

    testHelper.p1 = db.defineProperty("p1", Property::IntType);
    testHelper.p2 = db.defineProperty("p2", Property::IntType);
    testHelper.p_array_int = db.defineArrayProperty("p_array_int", Property::IntType);
    testHelper.p_array_uint = db.defineArrayProperty("p_array_uint", Property::UIntType);
    testHelper.p_array_real = db.defineArrayProperty("p_array_real", Property::RealType);
    testHelper.p_array_bool = db.defineArrayProperty("p_array_bool", Property::BoolType);
    testHelper.p_array_str = db.defineArrayProperty("p_array_str", Property::StringType);
    testHelper.r1 = db.defineRelation("r1");
    testHelper.r2 = db.defineRelation("r2");
    testHelper.r_array_1 =  db.defineArrayRelation("r_array_1");
    testHelper.rArray_2 =  db.defineArrayRelation("r_array_2");

    if (needIndex)
    {
        testHelper.p_real->createIndex();
        testHelper.p_str->createIndex();
        testHelper.p_int->createIndex();
        testHelper.p_uint->createIndex();
        testHelper.p_int64->createIndex();
        testHelper.p_uint64->createIndex();
        testHelper.p_bool_0->createIndex();
        testHelper.p_bool_1->createIndex();
        testHelper.p1->createIndex();
        testHelper.p2->createIndex();
        testHelper.p_array_int->createIndex();
        testHelper.r1->createIndex();
        testHelper.r2->createIndex();
        testHelper.r_array_1->createIndex();
        testHelper.rArray_2->createIndex();
    }
}

void setBean(TestHelper& testHelper, Bean* bean)
{
    // bean->setProperty(testHelper.p_int, Json::Value::minInt);
    bean->setProperty(testHelper.p_int, -1);
    bean->setProperty(testHelper.p_uint, (Json::UInt)1);
    bean->setProperty(testHelper.p_int64, -1);
    bean->setProperty(testHelper.p_uint64, (Json::UInt64)1);
    bean->setProperty(testHelper.p_real, 1.0);
    bean->setProperty(testHelper.p_bool_0, false);
    bean->setProperty(testHelper.p_bool_1, true);
    bean->setProperty(testHelper.p_str, "foo");

    bean->createArrayProperty(testHelper.p_array_int);
    bean->appendProperty(testHelper.p_array_int, 101);
    bean->appendProperty(testHelper.p_array_int, 102);
}

void validateBean(TestHelper& testHelper, Bean* bean)
{
    EXPECT_TRUE(bean->getProperty(testHelper.p_int).asInt() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint).asUInt() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_int64).asInt64() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint64).asUInt64() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_real).asDouble() == 1.0);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_0).asBool() == false);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_1).asBool() == true);
    EXPECT_TRUE(bean->getProperty(testHelper.p_str).asString() == "foo");

    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 0).asInt() == 101);
    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 1).asInt() == 102);
}