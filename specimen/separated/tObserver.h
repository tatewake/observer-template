/*
 Observer Template (C++) by TJ Grant (tjgrant@tatewake.com)

 This implementation of the "observer pattern" implements a C++ template-based observer.
 Thread-safe as long as notify and update methods are wrapped in thread-aware code.

 //--

 Copyright (c) 2011-02-01 TJ Grant (tjgrant@tatewake.com)

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
 is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#pragma once

#include <list>
#include <algorithm>
#include <cassert>

template<class T> class tSubject;
template<class T> class tObserver;

template<class T>
class tObserver
{
private:
    typedef tSubject<T>             SubjectType;
    typedef std::list<SubjectType*> ListType;

private:
    ListType    mSubjects;

private:
    void InformallyAttachSubject(SubjectType* newSub);
    void InformallyDetachSubject(SubjectType* newSub);
    void InformallyDetachAllSubjects();
    void FormallyAttachAllSubjects(ListType& newSub);

public:
    tObserver();
    tObserver(const tObserver& other);
#if __cplusplus >= 201103L
    tObserver(tObserver&& other);
#endif
    virtual ~tObserver();

public:
    tObserver& operator=(const tObserver& other);
#if __cplusplus >= 201103L
    tObserver& operator=(tObserver&& other);
#endif

public:
    virtual void update(T msg) = 0;

    friend class tSubject<T>;
};

