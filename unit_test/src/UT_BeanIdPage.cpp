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

#include "org/jinsha/beandb/Page.hpp"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::beandb;

TEST(BeanIdPage, getPageSize)
{
    BeanIdPage* beanIdPage = nullptr;

    BeanIdPage::LoadPageFuncType func = [] (BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& elements) -> int {

    };
    
    beanIdPage = new BeanIdPage(0, func);
    EXPECT_TRUE(beanIdPage->getPageSize() == 0);
    delete beanIdPage;
    
    beanIdPage = new BeanIdPage(1, func);
    EXPECT_TRUE(beanIdPage->getPageSize() == 1);
    delete beanIdPage;
    
    beanIdPage = new BeanIdPage(2, func);
    EXPECT_TRUE(beanIdPage->getPageSize() == 2);
    delete beanIdPage;

}


// TEST(BeanIdPage, next)
// {
//     std::vector<oidType> data;

//     for (oidType i = 0; i<5; i++) {
//         data.push_back(i+1);
//     }

//     BeanIdPage::LoadPageFuncType func = [&data] (unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& elements) -> int {
//         return 0;
//     };

//     BeanIdPage beanIdPage(3, func);

// }