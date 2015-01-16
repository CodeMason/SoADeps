///
/// PtrRecycler.h
/// Vorb Engine
///
/// Created by Cristian Zaloj on 8 Dec 2014
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
///
///

#pragma once

#ifndef PtrRecycler_h__
#define PtrRecycler_h__

/// Creates and caches larges numbers of pointers
/// @tparam: Data type pointers are made of
template<typename T>
class PtrRecycler {
public:
    ~PtrRecycler() {
        freeAll();
    }

    T* create() {
        T* data;
        if (m_recycled.size() > 0) {
            // Get a recycled data
            data = m_recycled[m_recycled.size() - 1];

            // Update lists
            m_recycled.pop_back();
            *(m_recycledChecks[(ptrdiff_t)data]) = 0;
        } else {
            // Create a new data segment
            PtrBind* bind = new PtrBind();
            data = &bind->data;
            bind->recycleCheck = 0;

            // Add The Data Checks
            m_allocated.push_back(bind);
            m_recycledChecks[(ptrdiff_t)data] = &bind->recycleCheck;
        }
        return data;
    }
    void recycle(T* data) {
        i32* recycleCheck = m_recycledChecks[(ptrdiff_t)data];

        // Make sure it hasn't already been recycled
        if (*recycleCheck == 0) {
            m_recycled.push_back(data);
            *recycleCheck = 1;
        }
    }

    void freeAll() {
        if (m_allocated.size() > 0) {
            // Free all allocated memory
            for (size_t i = m_allocated.size(); i > 0;) {
                i--;
                delete m_allocated[i];
            }

            // Empty out the lists
            std::vector<PtrBind*>().swap(m_allocated);
            std::map<ptrdiff_t, i32*>().swap(m_recycledChecks);
            std::vector<T*>().swap(m_recycled);
        }
    }
private:
    struct PtrBind {
    public:
        PtrBind() :
            data(T()) {
            // Empty
        }

        T data;
        i32 recycleCheck = 0;
    };

    std::vector<PtrBind*> m_allocated;
    std::map<ptrdiff_t, i32*> m_recycledChecks;
    std::vector<T*> m_recycled;
};

#endif // PtrRecycler_h__