#pragma once

#include "org/jinsha/bean/common.h"
#include "org/jinsha/bean/BeanWorld.h"
#include "./DummyBeanDB.h"

using namespace org::jinsha::bean;

////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
// extern Property* p_double;
// extern Property* p_str;
// extern Property* p_int;
// extern Property* p_uint;
// extern Property* p_int64;
// extern Property* p_uint64;
// extern Property* p_bool_0;
// extern Property* p_bool_1;
// extern Property* p1;
// extern Property* p2;
// extern Property* pArray_1;
// extern Property* r1;
// extern Property* r2;
// extern Property* rArray_1;
// extern Property* rArray_2;

typedef struct TestHelper
{
    Property* p_real;
    Property* p_str;
    Property* p_int;
    Property* p_uint;
    Property* p_int64;
    Property* p_uint64;
    Property* p_bool_0;
    Property* p_bool_1;
    Property* p1;
    Property* p2;
    Property* p_array_int;
    Property* p_array_uint;
    Property* p_array_real;
    Property* p_array_bool;
    Property* p_array_str;
    Property* r1;
    Property* r2;
    Property* r_array_1;
    Property* rArray_2;
} TestHelper;



void initTestHelper(TestHelper& testHelper, BeanWorld& world, bool needIndex = false);
// void initTestHelper(TestHelper& testHelper, AbstractBeanDB& db, bool needIndex = false);


void setBeanProperties(TestHelper& testHelper, Bean* bean);
void setBeanNativeData(TestHelper& testHelper, Bean* bean);
void validateBeanProperties(TestHelper& testHelper, Bean* bean);
void validateBeanNativeData(TestHelper& testHelper, Bean* bean);
void validateBean(TestHelper& testHelper, Bean* bean);
static void evaluate_testdb_empty_property(AbstractBeanDB& testdb);