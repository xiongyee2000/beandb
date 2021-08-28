

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
#include "./DummyBeanDB.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./unit_test/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./unit_test/data/sqlite_db_1";

static void validate_testdb_1(SqliteBeanDB& testdb);
static void evaluate_testdb_empty_property(SqliteBeanDB& testdb);

TEST(SqliteBeanDB, constuctor)
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
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    AbstractBeanDB* testdbPtr = &testdb;
    int err = 0;

    err = testdb.connect();
    EXPECT_TRUE(err == 0);
    err = testdb.disconnect();
    EXPECT_TRUE(err == 0);
}

TEST(SqliteBeanDB, defineProperty_undefineProperty)
{
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    AbstractBeanDB* testdbPtr = &testdb;
    BeanWorld world(*testdbPtr);
    int err = 0;
    int pid = 0;

    testdb.reInit();
    testdb.connect();

    pid  = testdb.defineProperty_("p1", Property::PrimaryType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("p2", Property::PrimaryType, Property::UIntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("p3", Property::PrimaryType, Property::RealType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("p4", Property::PrimaryType, Property::StringType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("p5", Property::PrimaryType, Property::BoolType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ap1", Property::ArrayPrimaryType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ap2", Property::ArrayPrimaryType, Property::UIntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ap3", Property::ArrayPrimaryType, Property::RealType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ap4", Property::ArrayPrimaryType, Property::StringType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ap5", Property::ArrayPrimaryType, Property::BoolType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("r1", Property::RelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("r2", Property::RelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("r3", Property::RelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("r4", Property::RelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("r5", Property::RelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ar1", Property::ArrayRelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ar2", Property::ArrayRelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ar3", Property::ArrayRelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ar4", Property::ArrayRelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);
    pid = testdb.defineProperty_("ar5", Property::ArrayRelationType, Property::IntType, true);
    EXPECT_TRUE(pid >= 0);

    validate_testdb_1(testdb);

    err = testdb.undefineProperty_("p1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("p2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("p3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("p4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("p5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ap1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ap2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ap3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ap4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ap5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("r1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("r2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("r3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("r4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("r5");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ar1");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ar2");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ar3");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ar4");
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("ar5");
    EXPECT_TRUE(err == 0);

    evaluate_testdb_empty_property(testdb);

    err = testdb.undefineProperty_(nullptr);
    EXPECT_TRUE(err == 0);
    err = testdb.undefineProperty_("");
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
   validate_testdb_1(testdb);
   testdb.disconnect();
}

// TEST(SqliteBeanDB, loadProperties_)
// {
//     // Property *property;
//     const char* testdbDir = g_sqlite_db_1;
//     SqliteBeanDB testdb(testdbDir);
//     BeanWorld world((AbstractBeanDB&)testdb);
//     int err = 0;

//     world.clear();

//     testdb.connect();

//     err = testdb.loadProperties_();
//     EXPECT_TRUE(err == 0);

//     EXPECT_TRUE(world.getProperty("p1")->getName() == "p1" && 
//         world.getProperty("p1")->getType() == Property::PrimaryType && 
//         world.getProperty("p1")->getValueType() == Property::IntType);
//     EXPECT_TRUE(world.getProperty("p2")->getName() == "p2" && 
//         world.getProperty("p2")->getType() == Property::PrimaryType && 
//         world.getProperty("p2")->getValueType() == Property::UIntType);
//     EXPECT_TRUE(world.getProperty("p3")->getName() == "p3" && 
//         world.getProperty("p3")->getType() == Property::PrimaryType && 
//         world.getProperty("p3")->getValueType() == Property::RealType);
//     EXPECT_TRUE(world.getProperty("p4")->getName() == "p4" && 
//         world.getProperty("p4")->getType() == Property::PrimaryType && 
//         world.getProperty("p4")->getValueType() == Property::StringType);
//     EXPECT_TRUE(world.getProperty("p5")->getName() == "p5" && 
//         world.getProperty("p5")->getType() == Property::PrimaryType && 
//         world.getProperty("p5")->getValueType() == Property::BoolType);

//     EXPECT_TRUE(world.getProperty("ap1")->getName() == "ap1" && 
//         world.getProperty("ap1")->getType() == Property::ArrayPrimaryType && 
//         world.getProperty("ap1")->getValueType() == Property::IntType);
//     EXPECT_TRUE(world.getProperty("ap2")->getName() == "ap2" && 
//         world.getProperty("ap2")->getType() == Property::ArrayPrimaryType && 
//         world.getProperty("ap2")->getValueType() == Property::UIntType);
//     EXPECT_TRUE(world.getProperty("ap3")->getName() == "ap3" && 
//         world.getProperty("ap3")->getType() == Property::ArrayPrimaryType && 
//         world.getProperty("ap3")->getValueType() == Property::RealType);
//     EXPECT_TRUE(world.getProperty("ap4")->getName() == "ap4" && 
//         world.getProperty("ap4")->getType() == Property::ArrayPrimaryType && 
//         world.getProperty("ap4")->getValueType() == Property::StringType);
//     EXPECT_TRUE(world.getProperty("ap5")->getName() == "ap5" && 
//         world.getProperty("ap5")->getType() == Property::ArrayPrimaryType && 
//         world.getProperty("ap5")->getValueType() == Property::BoolType);

//     EXPECT_TRUE(world.getProperty("r1")->getName() == "r1" && 
//         world.getProperty("r1")->getType() == Property::RelationType);
//     EXPECT_TRUE(world.getProperty("r2")->getName() == "r2" && 
//         world.getProperty("r2")->getType() == Property::RelationType);
//     EXPECT_TRUE(world.getProperty("r3")->getName() == "r3" && 
//         world.getProperty("r3")->getType() == Property::RelationType);
//     EXPECT_TRUE(world.getProperty("r4")->getName() == "r4" && 
//         world.getProperty("r4")->getType() == Property::RelationType);
//     EXPECT_TRUE(world.getProperty("r5")->getName() == "r5" && 
//         world.getProperty("r5")->getType() == Property::RelationType);

//     EXPECT_TRUE(world.getProperty("ar1")->getName() == "ar1" && 
//         world.getProperty("ar1")->getType() == Property::ArrayRelationType);
//     EXPECT_TRUE(world.getProperty("ar2")->getName() == "ar2" && 
//         world.getProperty("ar2")->getType() == Property::ArrayRelationType);
//     EXPECT_TRUE(world.getProperty("ar3")->getName() == "ar3" && 
//         world.getProperty("ar3")->getType() == Property::ArrayRelationType);
//     EXPECT_TRUE(world.getProperty("ar4")->getName() == "ar4" && 
//         world.getProperty("ar4")->getType() == Property::ArrayRelationType);
//     EXPECT_TRUE(world.getProperty("ar5")->getName() == "ar5" && 
//         world.getProperty("ar5")->getType() == Property::ArrayRelationType);
    
//     testdb.disconnect();

// }

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

    Property* p1 = world.getProperty("p1");
    Property* p2 = world.getProperty("p2");
    Property* p3 = world.getProperty("p3");
    Property* p4 = world.getProperty("p4");
    Property* p5 = world.getProperty("p5");
    Property* ap1 = world.getProperty("ap1");
    Property* ap2 = world.getProperty("ap2");
    Property* ap3 = world.getProperty("ap3");
    Property* ap4 = world.getProperty("ap4");
    Property* ap5 = world.getProperty("ap5");
    Property* r1 = world.getProperty("r1");
    Property* r2 = world.getProperty("r2");
    Property* r3 = world.getProperty("r3");
    Property* r4 = world.getProperty("r4");
    Property* r5 = world.getProperty("r5");
    Property* ar1 = world.getProperty("ar1");
    Property* ar2= world.getProperty("ar2");
    Property* ar3 = world.getProperty("ar3");
    Property* ar4 = world.getProperty("ar4");
    Property* ar5 = world.getProperty("ar5");

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