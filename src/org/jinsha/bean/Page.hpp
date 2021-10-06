#pragma once

#include <vector>
#include <functional>
#include "./common.h"

namespace org {
namespace jinsha {
namespace bean {

template<typename T>
class Page
{
public:
    typedef std::function<int (unsigned int pageSize, unsigned long pageIndex, std::vector<T>& elements)> LoadPageFuncType;

    Page(unsigned int pageSize, LoadPageFuncType func) 
    : m_pageSize_(pageSize)
    , m_loadPageFunc_(func)
    {
        m_elements_.reserve(pageSize);
        gotoPage(0);
    };

    virtual ~Page() {};

    virtual uint_t getPageIndex() const {return m_pageIndex_;};
    virtual int getPageSize() const {return m_pageSize_;};
    virtual size_t size() const {return m_elements_.size();};
    virtual const T& at(size_t index) const {return m_elements_.at(index);};

    // virtual  const std::vector<T>& getElements() {return m_elements_;};

    virtual int next() 
    {
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(m_pageSize_, m_pageIndex_ + 1, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_++;
        }
        return err;
    };

    virtual int prev() 
    {
        if (m_pageIndex_ == 0) return -2;
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(m_pageSize_, m_pageIndex_ - 1, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_--;
        }
        return err;
    }


    virtual int gotoPage(uint_t pageIndex)
    {
        // if (pageIndex == m_pageIndex_) return 0;
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(m_pageSize_, pageIndex, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_ = pageIndex;
        }
        return err;
    }

protected:
    LoadPageFuncType m_loadPageFunc_;

private:
    unsigned int m_pageSize_ = 0;
    uint_t m_pageIndex_ = 0;
    std::vector<T> m_elements_;
};

using BeanIdPage = Page<oidType>;

}
}
}