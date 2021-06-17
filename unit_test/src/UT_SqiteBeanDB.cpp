

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public

#include "org/jinsha/bean/SqliteBeanDB.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

static const char* g_testdbDir = "./unit_test/data/sqlite_db_dir";

TEST(SqliteBeanDB, constuctor)
{
    BeanWorld world;
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

    testdbDir = g_testdbDir;
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}


TEST(SqliteBeanDB, saveProperty_loadProperty)
{
    Property *property;
    const char* testdbDir = g_testdbDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);

    testdb.reInit();

    testdb.connect();

    property = world.defineProperty("p1", Property::IntType);
    testdb.saveProperty(property);

    property = world.defineProperty("p2", Property::UIntType);
    testdb.saveProperty(property);

    property = world.defineProperty("p3", Property::RealType);
    testdb.saveProperty(property);

    property = world.defineProperty("p4", Property::StringType);
    testdb.saveProperty(property);

    property = world.defineProperty("p5", Property::BoolType);
    testdb.saveProperty(property);

    property = world.defineArrayProperty("ap1", Property::IntType);
    testdb.saveProperty(property);

    property = world.defineArrayProperty("ap2", Property::UIntType);
    testdb.saveProperty(property);

    property = world.defineArrayProperty("ap3", Property::RealType);
    testdb.saveProperty(property);

    property = world.defineArrayProperty("ap4", Property::StringType);
    testdb.saveProperty(property);

    property = world.defineArrayProperty("ap5", Property::BoolType);
    testdb.saveProperty(property);

    property = world.defineRelation("r1");
    testdb.saveProperty(property);

    property = world.defineRelation("r2");
    testdb.saveProperty(property);

    property = world.defineRelation("r3");
    testdb.saveProperty(property);

    property = world.defineRelation("r4");
    testdb.saveProperty(property);

    property = world.defineRelation("r5");
    testdb.saveProperty(property);

    property = world.defineArrayRelation("ar1");
    testdb.saveProperty(property);

    property = world.defineArrayRelation("ar2");
    testdb.saveProperty(property);

    property = world.defineArrayRelation("ar3");
    testdb.saveProperty(property);

    property = world.defineArrayRelation("ar4");
    testdb.saveProperty(property);

    property = world.defineArrayRelation("ar5");
    testdb.saveProperty(property);

    world.clear();
    // EXPECT_TRUE(world.getProperty("p1") == nullptr);

    testdb.loadProperties();
    EXPECT_TRUE(world.getProperty("p1")->getName() == "p1" && 
        world.getProperty("p1")->getType() == Property::PrimaryType && 
        world.getProperty("p1")->getValueType() == Property::IntType);
    EXPECT_TRUE(world.getProperty("p2")->getName() == "p2" && 
        world.getProperty("p2")->getType() == Property::PrimaryType && 
        world.getProperty("p2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world.getProperty("p3")->getName() == "p3" && 
        world.getProperty("p3")->getType() == Property::PrimaryType && 
        world.getProperty("p3")->getValueType() == Property::RealType);
    EXPECT_TRUE(world.getProperty("p4")->getName() == "p4" && 
        world.getProperty("p4")->getType() == Property::PrimaryType && 
        world.getProperty("p4")->getValueType() == Property::StringType);
    EXPECT_TRUE(world.getProperty("p5")->getName() == "p5" && 
        world.getProperty("p5")->getType() == Property::PrimaryType && 
        world.getProperty("p5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world.getProperty("ap1")->getName() == "ap1" && 
        world.getProperty("ap1")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap1")->getValueType() == Property::IntType);
    EXPECT_TRUE(world.getProperty("ap2")->getName() == "ap2" && 
        world.getProperty("ap2")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world.getProperty("ap3")->getName() == "ap3" && 
        world.getProperty("ap3")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap3")->getValueType() == Property::RealType);
    EXPECT_TRUE(world.getProperty("ap4")->getName() == "ap4" && 
        world.getProperty("ap4")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap4")->getValueType() == Property::StringType);
    EXPECT_TRUE(world.getProperty("ap5")->getName() == "ap5" && 
        world.getProperty("ap5")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world.getProperty("r1")->getName() == "r1" && 
        world.getProperty("r1")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r2")->getName() == "r2" && 
        world.getProperty("r2")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r3")->getName() == "r3" && 
        world.getProperty("r3")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r4")->getName() == "r4" && 
        world.getProperty("r4")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r5")->getName() == "r5" && 
        world.getProperty("r5")->getType() == Property::RelationType);

    EXPECT_TRUE(world.getProperty("ar1")->getName() == "ar1" && 
        world.getProperty("ar1")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar2")->getName() == "ar2" && 
        world.getProperty("ar2")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar3")->getName() == "ar3" && 
        world.getProperty("ar3")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar4")->getName() == "ar4" && 
        world.getProperty("ar4")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar5")->getName() == "ar5" && 
        world.getProperty("ar5")->getType() == Property::ArrayRelationType);
    testdb.disconnect();

}