CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(beandb CXX)

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -O2")
set(BEANDB_DEP_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include)
set(3RDPARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include/thirdParty/)
set(BEANDB_DEP_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/libs)
set(BEANDB_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#FIND_PACKAGE(jsoncpp REQUIRED)
#FIND_PACKAGE(sqlite3 REQUIRED)

file(GLOB_RECURSE BEANDB_SRC_FILES "*.cpp")

set (BEANDB_SRC_FILES 
    ${BEANDB_SRC_PATH}/org/jinsha/beandb/AbstractBeanDB.cpp
    ${BEANDB_SRC_PATH}/org/jinsha/beandb/Bean.cpp
    ${BEANDB_SRC_PATH}/org/jinsha/beandb/BeanWorld.cpp
    ${BEANDB_SRC_PATH}/org/jinsha/beandb/Property.cpp
    ${BEANDB_SRC_PATH}/org/jinsha/Util.cpp
)

set (BEANDB_SQLITE_SRC_FILES 
    ${BEANDB_SRC_PATH}/org/jinsha/beandb/SqliteBeanDB.cpp
)

INCLUDE_DIRECTORIES(${BEANDB_DEP_INC_PATH})
INCLUDE_DIRECTORIES(${3RDPARTY_PATH})
INCLUDE_DIRECTORIES(${BEANDB_SRC_PATH})

LINK_DIRECTORIES(${BEANDB_DEP_LIB_PATH}/x64)

ADD_LIBRARY(beandb SHARED ${BEANDB_SRC_FILES})
set (BEANDB_LINK_LIBRARY
    jsoncpp
)
TARGET_LINK_LIBRARIES(beandb ${BEANDB_LINK_LIBRARY})

#Comment out below lines if you don't intend to use sqlite backend.
ADD_LIBRARY(beandb-sqlite3 SHARED ${BEANDB_SQLITE_SRC_FILES})
set (BEANDB_SQLITE_LINK_LIBRARY
    jsoncpp
    sqlite3
)
TARGET_LINK_LIBRARIES(beandb-sqlite3  ${BEANDB_SQLITE_LINK_LIBRARY})

