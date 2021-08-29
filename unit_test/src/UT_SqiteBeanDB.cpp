

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
static void evaluate_testdb_empty_property(SqliteBeanDB& testdb);

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

    testdb.reInit();
    testdb.connect();

    property  = testdb.defineProperty("p1", Property::IntType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineProperty("p2", Property::UIntType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineProperty("p3", Property::RealType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineProperty("p4", Property::StringType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineProperty("p5", Property::BoolType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayProperty("ap1", Property::IntType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayProperty("ap2", Property::UIntType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayProperty("ap3", Property::RealType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayProperty("ap4", Property::StringType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayProperty("ap5", Property::BoolType, true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineRelation("r1",  true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineRelation("r2",  true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineRelation("r3",  true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineRelation("r4",  true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineRelation("r5",  true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayRelation("ar1", true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayRelation("ar2", true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayRelation("ar3", true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayRelation("ar4", true);
    EXPECT_TRUE(property != nullptr);
    property = testdb.defineArrayRelation("ar5", true);
    EXPECT_TRUE(property != nullptr);

    validate_testdb_1(testdb);

    err = testdb.undefineProperty("p1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("p2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("p3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("p4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("p5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ap1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ap2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ap3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ap4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ap5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("r1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("r2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("r3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("r4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("r5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ar1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ar2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ar3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ar4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("ar5");
    EXPECT_TRUE(err == 0);

    evaluate_testdb_empty_property(testdb);

    err = testdb.undefineProperty(nullptr);
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty("");
    EXPECT_TRUE(err == 0);

    testdb.reInit();
    testdb.disconnect();

}


TEST(SqliteBeanDB, getProperty)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world((AbstractBeanDB&)testdb);
    testdb.connect();


    EXPECT_TRUE(testdb.getProperty("p1")->getName() == "p1" && 
        testdb.getProperty("p1")->getType() == Property::PrimaryType && 
        testdb.getProperty("p1")->getValueType() == Property::IntType);
    EXPECT_TRUE(testdb.getProperty("p2")->getName() == "p2" && 
        testdb.getProperty("p2")->getType() == Property::PrimaryType && 
        testdb.getProperty("p2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(testdb.getProperty("p3")->getName() == "p3" && 
        testdb.getProperty("p3")->getType() == Property::PrimaryType && 
        testdb.getProperty("p3")->getValueType() == Property::RealType);
    EXPECT_TRUE(testdb.getProperty("p4")->getName() == "p4" && 
        testdb.getProperty("p4")->getType() == Property::PrimaryType && 
        testdb.getProperty("p4")->getValueType() == Property::StringType);
    EXPECT_TRUE(testdb.getProperty("p5")->getName() == "p5" && 
        testdb.getProperty("p5")->getType() == Property::PrimaryType && 
        testdb.getProperty("p5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(testdb.getProperty("ap1")->getName() == "ap1" && 
        testdb.getProperty("ap1")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("ap1")->getValueType() == Property::IntType);
    EXPECT_TRUE(testdb.getProperty("ap2")->getName() == "ap2" && 
        testdb.getProperty("ap2")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("ap2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(testdb.getProperty("ap3")->getName() == "ap3" && 
        testdb.getProperty("ap3")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("ap3")->getValueType() == Property::RealType);
    EXPECT_TRUE(testdb.getProperty("ap4")->getName() == "ap4" && 
        testdb.getProperty("ap4")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("ap4")->getValueType() == Property::StringType);
    EXPECT_TRUE(testdb.getProperty("ap5")->getName() == "ap5" && 
        testdb.getProperty("ap5")->getType() == Property::ArrayPrimaryType && 
        testdb.getProperty("ap5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(testdb.getProperty("r1")->getName() == "r1" && 
        testdb.getProperty("r1")->getType() == Property::RelationType);
    EXPECT_TRUE(testdb.getProperty("r2")->getName() == "r2" && 
        testdb.getProperty("r2")->getType() == Property::RelationType);
    EXPECT_TRUE(testdb.getProperty("r3")->getName() == "r3" && 
        testdb.getProperty("r3")->getType() == Property::RelationType);
    EXPECT_TRUE(testdb.getProperty("r4")->getName() == "r4" && 
        testdb.getProperty("r4")->getType() == Property::RelationType);
    EXPECT_TRUE(testdb.getProperty("r5")->getName() == "r5" && 
        testdb.getProperty("r5")->getType() == Property::RelationType);

    EXPECT_TRUE(testdb.getProperty("ar1")->getName() == "ar1" && 
        testdb.getProperty("ar1")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(testdb.getProperty("ar2")->getName() == "ar2" && 
        testdb.getProperty("ar2")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(testdb.getProperty("ar3")->getName() == "ar3" && 
        testdb.getProperty("ar3")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(testdb.getProperty("ar4")->getName() == "ar4" && 
        testdb.getProperty("ar4")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(testdb.getProperty("ar5")->getName() == "ar5" && 
        testdb.getProperty("ar5")->getType() == Property::ArrayRelationType);

   testdb.disconnect();
}

TEST(SqliteBeanDB, loadProperties_)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    testdb.connect();

    err = testdb.m_world_->reloadProperties();
    EXPECT_TRUE(err == 0);
    validate_testdb_1(testdb);
    
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


TEST(SqliteBeanDB, saveBean)
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);

    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world((AbstractBeanDB&)testdb);

    testdb.connect();
    // testdb.loadProperties_();

    Property* p1 = testdb.getProperty("p1");
    Property* p2 = testdb.getProperty("p2");
    Property* p3 = testdb.getProperty("p3");
    Property* p4 = testdb.getProperty("p4");
    Property* p5 = testdb.getProperty("p5");
    Property* ap1 = testdb.getProperty("ap1");
    Property* ap2 = testdb.getProperty("ap2");
    Property* ap3 = testdb.getProperty("ap3");
    Property* ap4 = testdb.getProperty("ap4");
    Property* ap5 = testdb.getProperty("ap5");
    Property* r1 = testdb.getProperty("r1");
    Property* r2 = testdb.getProperty("r2");
    Property* r3 = testdb.getProperty("r3");
    Property* r4 = testdb.getProperty("r4");
    Property* r5 = testdb.getProperty("r5");
    Property* ar1 = testdb.getProperty("ar1");
    Property* ar2= testdb.getProperty("ar2");
    Property* ar3 = testdb.getProperty("ar3");
    Property* ar4 = testdb.getProperty("ar4");
    Property* ar5 = testdb.getProperty("ar5");

    // bean = testdb.getBean(1);
    // EXPECTE_TRUE(bean->getProperty(p1) == 1);
    // bean->setProperty(p1, 101);
    // testdb.saveBean(bean, p1);
    // world.clear();
    // bean = testdb.getBean(1);
    //  EXPECTE_TRUE(bean->getProperty(p1) == 1);

    testdb.reInit();
    testdb.disconnect();
}


static void validate_testdb_1(SqliteBeanDB& testdb)
{
    Property *property = nullptr;
    std::unordered_map<std::string, Property*> propertyMap;
    testdb.loadProperties_(propertyMap);

    property = propertyMap.at("p1");
    EXPECT_TRUE(property->getName() == "p1" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("p2");
    EXPECT_TRUE(property->getName() == "p2" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::UIntType);
    
    property = propertyMap.at("p3");
    EXPECT_TRUE(property->getName() == "p3" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::RealType);
    
    property = propertyMap.at("p4");
    EXPECT_TRUE(property->getName() == "p4" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::StringType);

    property = propertyMap.at("p5");
    EXPECT_TRUE(property->getName() == "p5" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::BoolType);

    property = propertyMap.at("ap1");
    EXPECT_TRUE(property->getName() == "ap1" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::IntType);

    property = propertyMap.at("ap2");
    EXPECT_TRUE(property->getName() == "ap2" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::UIntType);

    property = propertyMap.at("ap3");
    EXPECT_TRUE(property->getName() == "ap3" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::RealType);

    property = propertyMap.at("ap4");
    EXPECT_TRUE(property->getName() == "ap4" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::StringType);

    property = propertyMap.at("ap5");
    EXPECT_TRUE(property->getName() == "ap5" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::BoolType);

    property = propertyMap.at("r1");
    EXPECT_TRUE(property->getName() == "r1" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("r2");
    EXPECT_TRUE(property->getName() == "r2" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("r3");
    EXPECT_TRUE(property->getName() == "r3" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("r4");
    EXPECT_TRUE(property->getName() == "r4" && 
        property->getType() == Property::RelationType);
 
    property = propertyMap.at("r5");
    EXPECT_TRUE(property->getName() == "r5" && 
        property->getType() == Property::RelationType);

    property = propertyMap.at("ar1");
    EXPECT_TRUE(property->getName() == "ar1" && 
        property->getType() == Property::ArrayRelationType);

    property = propertyMap.at("ar2");
    EXPECT_TRUE(property->getName() == "ar2" && 
        property->getType() == Property::ArrayRelationType);

    property = propertyMap.at("ar3");
    EXPECT_TRUE(property->getName() == "ar3" && 
        property->getType() == Property::ArrayRelationType);

    property = propertyMap.at("ar4");
    EXPECT_TRUE(property->getName() == "ar4" && 
        property->getType() == Property::ArrayRelationType);

    property = propertyMap.at("ar5");
    EXPECT_TRUE(property->getName() == "ar5" && 
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