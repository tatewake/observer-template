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
