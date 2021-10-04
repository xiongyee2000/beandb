#pragma once

#include <vector>
#include "./common.h"

namespace org {
namespace jinsha {
namespace bean {

template<typename T>
class AbstractPage
{
public:
    AbstractPage(unsigned int pageSize, uint_t pageIndex) 
    : m_pageSize_(pageSize)
    , m_pageIndex_(pageIndex) 
    {
        m_elements_.reserve(pageSize);
    };

    virtual ~AbstractPage() {};

    uint_t getPageIndex() const {return m_pageIndex_;};
    int getPageSize() const {return m_pageSize_;};

    const std::vector<T>& getElements() {return m_elements_;};

    int next() 
    {
        int err = loadPage(m_pageIndex_ + 1, m_elements_);
        if (0 == err) {
            m_pageIndex_++;
            return 0;
        } else {
            return -1;
        }
    };

    int prev() 
    {
        if (m_pageIndex_ == 0) return -2;
        int err = loadPage(m_pageIndex_ - 1, m_elements_);
        if (0 == err) {
            m_pageIndex_--;
            return 0;
        } else {
            return -1;
        }
    }

protected:
    virtual int loadPage(uint_t pageIndex, std::vector<T>& elements) = 0;

private:
    unsigned int m_pageSize_;
    uint_t m_pageIndex_;
    std::vector<T> m_elements_;
};

}
}
}