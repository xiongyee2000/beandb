#include "./SqlitePage.h"

namespace org {
namespace jinsha {
namespace bean {


SqlitePage::~SqlitePage()
{
}


int SqlitePage::loadPage(uint_t pageIndex, std::vector<oidType>& elements)
{
    if (pageIndex == getPageIndex()) return 0;
    if (!m_db_->connected()) return -1;
    if (m_loadPageFunc_) {
        return m_loadPageFunc_(getPageSize(), pageIndex, elements);
    } else {
        return -1;
    }
}

}
}
}