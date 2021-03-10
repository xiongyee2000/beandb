#pragma once

#include "org/jinsha/bean/common.h"
#include "org/jinsha/bean/BeanWorld.h"

using namespace org::jinsha::bean;

////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
extern pidType pid_int_p ;
extern pidType pid_uint_p ;
extern pidType pid_int64_p ;
extern pidType pid_uint64_p ;
extern pidType pid_double_p ;
extern pidType pid_str_p ;
extern pidType pid_bool_p0 ;
extern pidType pid_bool_p1 ;
extern pidType pid_p1 ;
extern pidType pid_p2 ;
extern pidType pid_pArray_1 ;
extern pidType pid_r1 ;
extern pidType pid_rArray_1;

extern Property* double_p_property;
extern Property* str_p_property;
extern Property* int_p_property;
extern Property* uint_p_property;
extern Property* int64_p_property;
extern Property* uint64_p_property;
extern Property* bool_p0_property;
extern Property* bool_p1_property;
extern Property* p1_property;
extern Property* p2_property;
extern Property* pArray_1;
extern Property* r1;
extern Property* rArray_1;


void init_world(BeanWorld& world, bool needIndex = false);