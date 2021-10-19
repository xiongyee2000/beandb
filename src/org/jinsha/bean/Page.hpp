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
    /**
     * The prototype of the method to load a specific page.
     * 
     * The database backend must implement this method/function for find() methods of BeanWorld.
     * 
     * @param pageSize the page size
     * @param pageIndex the index of page to be loaded, starting from 0
     * @param elements the vector containing result elements of the find() method
     * @return 0 for success, or an error code:
     *                   -1001: no element found at specified page
     *                   others: error occurred
     */
    typedef std::function<int (Page<T>* page, unsigned int pageSize, unsigned long pageIndex, std::vector<T>& elements)> LoadPageFuncType;

    /**
     * Constructor
     * 
     * @param pageSize the page size
     * @param func the method/function to do load page
     */
    Page(unsigned int pageSize, LoadPageFuncType func) 
    : m_pageSize_(pageSize)
    , m_loadPageFunc_(func)
    {
        m_elements_.reserve(pageSize);
        gotoPage(0);
    };

    /**
     * Destructor
     */
    virtual ~Page() {};

    /**
     * Get page index of the current page.
     * 
     * @return the page index
     */
    uint_t getPageIndex() const {return m_pageIndex_;};

    /**
     * Get page size of the current page.
     * 
     * @return the page size
     */
    int getPageSize() const {return m_pageSize_;};

    /**
     * Get the size of elements that this page holds.
     * 
     * @return the size of elements that this page holds.
     */
    size_t size() const {return m_elements_.size();};

    /**
     * Get the element at index in this page.
     * 
     * @param index the index
     * @return the reference of the element at index
     * 
     * Notes:
     * - If the index exceeds the size of this page (as got from size()),
     *   an exception will be triggered by system.
     */
    const T& at(size_t index) const {return m_elements_.at(index);};

    /**
     * Go to next page.
     * 
     * @return 0 for success, or an error code:
     *                   -1: no callable loadPage method
     *                   -1001: no element in next page (i.e. this is the last page)
     *                   others: other errors
     */
    int next() 
    {
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(this, m_pageSize_, m_pageIndex_ + 1, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_++;
        }
        return err;
    };

    /**
     * Go to previous page.
     * 
     * @return 0 for success, or an error code:
     *                   -1: no callable loadPage method
     *                   -2: no previous page (i.e. this is the first page (pageIndex == 0))
     *                   others: other errors
     */
    int prev() 
    {
        if (m_pageIndex_ == 0) return -2;
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(this, m_pageSize_, m_pageIndex_ - 1, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_--;
        }
        return err;
    }

    /**
     * Go to page at given pageIndex.
     * 
     * @param pageIndex the page index
     * @return 0 for success, or an error code:
     *                   -1: no callable loadPage method
     *                   -1001: no element in the given page
     *                   others: other errors
     */
    int gotoPage(uint_t pageIndex)
    {
        // if (pageIndex == m_pageIndex_) return 0;
        int err = 0;
        if (m_loadPageFunc_) {
            err = m_loadPageFunc_(this, m_pageSize_, pageIndex, m_elements_);
        } else {
            err = -1;
        }
        if (!err) {
            m_pageIndex_ = pageIndex;
        }
        return err;
    }

protected:
    const std::vector<T>& getElements() {return m_elements_;};

private:
    LoadPageFuncType m_loadPageFunc_;
    unsigned int m_pageSize_ = 0;
    uint_t m_pageIndex_ = 0;
    std::vector<T> m_elements_;
};

using BeanIdPage = Page<oidType>;

}
}
}