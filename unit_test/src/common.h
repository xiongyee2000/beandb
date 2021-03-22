#pragma once

#include "org/jinsha/bean/common.h"
#include "org/jinsha/bean/BeanWorld.h"

using namespace org::jinsha::bean;

////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
extern Property* p_double;
extern Property* p_str;
extern Property* p_int;
extern Property* p_uint;
extern Property* p_int64;
extern Property* p_uint64;
extern Property* p_bool_0;
extern Property* p_bool_1;
extern Property* p1;
extern Property* p2;
extern Property* pArray_1;
extern Property* r1;
extern Property* r2;
extern Property* rArray_1;
extern Property* rArray_2;


void init_world(BeanWorld& world, bool needIndex = false);