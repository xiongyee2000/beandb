#pragma once

#include "./AbstractPage.h"
// #include "./Bean.h"

#include <functional>

#include "./SqliteBeanDB.h"

namespace org {
namespace jinsha {
namespace bean {

class SqliteBeanDB;

class SqlitePage: public AbstractPage<oidType> 
{
private:
    SqlitePage(unsigned int pageSize, uint_t pageIndex, SqliteBeanDB* db, void* context) 
    : AbstractPage<oidType>(pageSize, pageIndex)
    , m_db_(db)
    , m_context_(context) 
    , m_loadPageFunc_(nullptr)
    {
    };

    virtual ~SqlitePage();

private:
    virtual int loadPage(uint_t pageIndex, std::vector<oidType>& elements) override;

    std::function<int (unsigned int pageSize, uint_t pageIndex, std::vector<oidType>& ids)> m_loadPageFunc_;

private:
    SqliteBeanDB* m_db_;
    void* m_context_;

friend class SqliteBeanDB;
};

}
}
}