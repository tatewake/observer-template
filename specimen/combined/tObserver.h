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
class tSubject
{
private:
    typedef tObserver<T>                ObserverType;
    typedef std::list<ObserverType*>    ListType;

private:
    ListType    mObservers;
    ListType    mNewObservers;
    bool        mCurrentlyNotifying;
    bool*       mSubjectDeletedPtr;

private:
    void InformallyAttachObserver(ObserverType* newOb);
    void InformallyDetachObserver(ObserverType* newOb);
    void FormallyAttachIfNotNull(const ListType& observers);
    void FormallyAttach(const ListType& observers);

public:
    tSubject();
    tSubject(const tSubject& other);
#if __cplusplus >= 201103L
    tSubject(tSubject&& other);
#endif
    virtual ~tSubject();

public:
    tSubject& operator=(const tSubject& other);
#if __cplusplus >= 201103L
    tSubject& operator=(tSubject&& other);
#endif

public:
    void attach(ObserverType* newOb);
    void detach(ObserverType* newOb);
    void detachAll();
    void notify(T msg);

    friend class tObserver<T>;
};

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

template<class T>
void tSubject<T>::InformallyAttachObserver(ObserverType* newOb)
{
    if (!mCurrentlyNotifying)
    {
        mObservers.push_back(newOb);
    }
    else
    {
        mNewObservers.push_back(newOb);
    }
}

template<class T>
void tSubject<T>::InformallyDetachObserver(ObserverType* newOb)
{
    if (!mCurrentlyNotifying)
    {
        mObservers.erase(find(mObservers.begin(), mObservers.end(), newOb));
    }
    else
    {
        typename ListType::iterator iter = find(mNewObservers.begin(), mNewObservers.end(), newOb);

        if (iter != mNewObservers.end())
        {
            mNewObservers.erase(iter);
        }
        else
        {
            iter = find(mObservers.begin(), mObservers.end(), newOb);

            if (iter != mObservers.end())
            {
                (*iter) = NULL;
            }
        }
    }
}

template<class T>
void tSubject<T>::FormallyAttachIfNotNull(const ListType& observers)
{
    for(typename ListType::const_iterator iter = observers.begin() ; iter != observers.end(); iter++)
    {
        if (*iter)
        {
            attach(*iter);
        }
    }
}

template<class T>
void tSubject<T>::FormallyAttach(const ListType& observers)
{
    for(typename ListType::const_iterator iter = observers.begin() ; iter != observers.end(); iter++)
    {
        attach(*iter);
    }
}

template<class T>
tSubject<T>::tSubject()
:   mCurrentlyNotifying(false),
mSubjectDeletedPtr(NULL)
{
}

template<class T>
tSubject<T>::tSubject(const tSubject& other)
:   mCurrentlyNotifying(false),
mSubjectDeletedPtr(NULL)
{
    if (this != &other)
    {
        FormallyAttachIfNotNull(other.mObservers);
        FormallyAttach(other.mNewObservers);
    }
}

#if __cplusplus >= 201103L
template<class T>
tSubject<T>::tSubject(tSubject&& other)
:   mCurrentlyNotifying(false),
mSubjectDeletedPtr(NULL)
{
    if (this != &other)
    {
        FormallyAttachIfNotNull(other.mObservers);
        FormallyAttach(other.mNewObservers);
        other.detachAll();
    }
}
#endif

template<class T>
tSubject<T>::~tSubject()
{
    if (mSubjectDeletedPtr)
    {
        *mSubjectDeletedPtr = true;
    }

    for(typename ListType::iterator iter = mObservers.begin(); iter != mObservers.end(); iter++)
    {
        if (*iter)
        {
            (*iter)->InformallyDetachSubject(this);
        }
    }
}

template<class T>
tSubject<T>& tSubject<T>::operator=(const tSubject<T>& other)
{
    if (this != &other)
    {
        detachAll();

        FormallyAttachIfNotNull(other.mObservers);
        FormallyAttach(other.mNewObservers);
    }

    return *this;
}

#if __cplusplus >= 201103L
template<class T>
tSubject<T>& tSubject<T>::operator=(tSubject&& other)
{
    if (this != &other)
    {
        detachAll();

        FormallyAttachIfNotNull(other.mObservers);
        FormallyAttach(other.mNewObservers);

        other.detachAll();
    }

    return *this;
}
#endif

template<class T>
void tSubject<T>::attach(ObserverType* newOb)
{
    assert(newOb);
    assert(find(mObservers.begin(), mObservers.end(), newOb) == mObservers.end() && find(mNewObservers.begin(), mNewObservers.end(), newOb) == mNewObservers.end());

    if (newOb)
    {
        newOb->InformallyAttachSubject(this);
        InformallyAttachObserver(newOb);
    }
}

template<class T>
void tSubject<T>::detach(ObserverType* newOb)
{
    assert(newOb);
    assert(find(mObservers.begin(), mObservers.end(), newOb) != mObservers.end() || find(mNewObservers.begin(), mNewObservers.end(), newOb) != mNewObservers.end());

    if (newOb)
    {
        newOb->InformallyDetachSubject(this);
        InformallyDetachObserver(newOb);
    }
}

template<class T>
void tSubject<T>::detachAll()
{
    ListType observersCopy = mObservers;

    for(typename ListType::iterator iter = observersCopy.begin(); iter != observersCopy.end(); iter++)
    {
        detach(*iter);
    }

    for(typename ListType::iterator iter = mNewObservers.begin(); iter != mNewObservers.end(); iter++)
    {
        (*iter)->InformallyDetachSubject(this);
    }

    mNewObservers.clear();
}

template<class T>
void tSubject<T>::notify(const T msg)
{
    bool subjectDeleted = false;

    mCurrentlyNotifying = true;
    mSubjectDeletedPtr = &subjectDeleted;

    for(typename ListType::iterator iter = mObservers.begin(); iter != mObservers.end(); iter++)
    {
        if (*iter)
        {
            (*iter)->update(msg);
        }

        if (subjectDeleted)
        {
            break;
        }
    }

    if (!subjectDeleted)
    {
        mSubjectDeletedPtr = NULL;
        mCurrentlyNotifying = false;

        mObservers.remove(NULL);
        mObservers.insert(mObservers.end(), mNewObservers.begin(), mNewObservers.end());
        mNewObservers.clear();
    }
}

template<class T>
void tObserver<T>::InformallyAttachSubject(SubjectType* newSub)
{
    mSubjects.push_back(newSub);
}

template<class T>
void tObserver<T>::InformallyDetachSubject(SubjectType* newSub)
{
    mSubjects.erase(find(mSubjects.begin(), mSubjects.end(), newSub));
}

template<class T>
void tObserver<T>::InformallyDetachAllSubjects()
{
    for(typename ListType::iterator iter = mSubjects.begin(); iter != mSubjects.end(); iter++)
    {
        (*iter)->InformallyDetachObserver(this);
    }

    mSubjects.clear();
}

template<class T>
void tObserver<T>::FormallyAttachAllSubjects(ListType& newSub)
{
    for(typename ListType::iterator iter = newSub.begin(); iter != newSub.end(); iter++)
    {
        (*iter)->attach(this);
    }
}

template<class T>
tObserver<T>::tObserver()
{
}

template<class T>
tObserver<T>::tObserver(const tObserver& other)
{
    if (this != &other)
    {
        ListType subjects(other.mSubjects);
        FormallyAttachAllSubjects(subjects);
    }
}

#if __cplusplus >= 201103L
template<class T>
tObserver<T>::tObserver(tObserver&& other)
{
    if (this != &other)
    {
        FormallyAttachAllSubjects(other.mSubjects);
        other.InformallyDetachAllSubjects();
    }
}
#endif

template<class T>
tObserver<T>::~tObserver()
{
    InformallyDetachAllSubjects();
}

template<class T>
tObserver<T>& tObserver<T>::operator=(const tObserver<T>& other)
{
    if (this != &other)
    {
        InformallyDetachAllSubjects();
        ListType subjects(other.mSubjects);
        FormallyAttachAllSubjects(subjects);
    }

    return *this;
}

#if __cplusplus >= 201103L
template<class T>
tObserver<T>& tObserver<T>::operator=(tObserver&& other)
{
    if (this != &other)
    {
        InformallyDetachAllSubjects();
        FormallyAttachAllSubjects(other.mSubjects);
        other.InformallyDetachAllSubjects();
    }

    return *this;
}
#endif
