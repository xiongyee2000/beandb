CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(beandb_ut CXX)

add_definitions(-g -std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall --coverage")
set(BEANDB_DEP_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/include)
set(3RDPARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/include/thirdParty)
set(BEANDB_DEP_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/libs)
set(BEANDB_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
#set(GTEST_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include)
#set(GTEST_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/libs/x64)
set(GTEST_LIB_PATH /usr/local/lib)

file(GLOB_RECURSE UT_SRC "*.cpp")

file(GLOB_RECURSE BEANDB_SRC "${BEANDB_SRC_PATH}/org/jinsha/*.cpp" "${BEANDB_SRC_PATH}/org/jinsha/*.cc")

FIND_LIBRARY(GTEST name libgtest.a HINTS ${GTEST_LIB_PATH}/)
FIND_LIBRARY(GMOCK name libgmock.a HINTS ${GTEST_LIB_PATH}/)

FIND_PACKAGE(jsoncpp REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

INCLUDE_DIRECTORIES(${BEANDB_DEP_INC_PATH}/)
INCLUDE_DIRECTORIES(${3RDPARTY_PATH})
INCLUDE_DIRECTORIES(${BEANDB_SRC_PATH}/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_PATH}/)

LINK_DIRECTORIES(${BEANDB_DEP_LIB_PATH}/x64)
LINK_DIRECTORIES(${GTEST_LIB_PATH})

ADD_EXECUTABLE(beandb_ut ${UT_SRC} ${BEANDB_SRC})

#link libraries
set (MY_LINK_LIBRARY
pthread
jsoncpp
sqlite3
)

TARGET_LINK_LIBRARIES(beandb_ut ${GTEST} ${GMOCK} ${CMAKE_THREAD_LIBS_INIT} ${MY_LINK_LIBRARY} )
