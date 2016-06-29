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

#include <cassert>
#include <stdio.h>
#include <vector>

#include "tSubject.h"
#include "tObserver.h"
#include "tSubjectImpl.h"
#include "tObserverImpl.h"

#if __cplusplus >= 201103L
#include <utility>
#endif

static std::vector<size_t> tOTNotifications;

class tOTSubject
: public tSubject<const uint32_t&>
{
public:
    tOTSubject() { }
    tOTSubject(const tOTSubject& other) : tSubject<const uint32_t &>(other) { }
    virtual ~tOTSubject() { }

    tOTSubject& operator=(const tOTSubject& other) { return static_cast<tOTSubject&>(tSubject<const uint32_t &>::operator=(other)); }

#if __cplusplus >= 201103L
    tOTSubject(tOTSubject&& other) : tSubject<const uint32_t &>(std::move(other)) { }
    tOTSubject& operator=(tOTSubject&& other) { return static_cast<tOTSubject&>(tSubject<const uint32_t &>::operator=(std::move(other))); }
#endif
};

class tOTObserver
: public tObserver<const uint32_t&>
{
protected:
    uint32_t mBase;

public:
    tOTObserver(uint32_t base) : mBase(base) { }
    tOTObserver(const tOTObserver& other) : tObserver<const uint32_t &>(other)
    {
        mBase = other.mBase;
    }

    virtual ~tOTObserver() { }

    tOTObserver& operator=(const tOTObserver& other)
    {
        mBase = other.mBase;
        return static_cast<tOTObserver&>(tObserver<const uint32_t &>::operator=(other));
    }

#if __cplusplus >= 201103L
    tOTObserver(tOTObserver&& other) : tObserver<const uint32_t &>(std::move(other))
    {
        mBase = other.mBase;
        other.mBase = 0;
    }
    tOTObserver& operator=(tOTObserver&& other)
    {
        mBase = other.mBase;
        other.mBase = 0;
        return static_cast<tOTObserver&>(tObserver<const uint32_t &>::operator=(std::move(other)));
    }
#endif

    void setBase(uint32_t base)
    {
        mBase = base;
    }

    virtual void update(const uint32_t& msg)
    {
        tOTNotifications.push_back(msg + mBase);
    }
};

class tObserverTests
{
public:
	tObserverTests()
	{
        testUpdate();

        testCopyCtor();
		testCopyAssign();

#if __cplusplus >= 201103L
        testMoveCtor();
        testMoveAssign();
#endif
    }

    void testUpdate()
	{
        size_t expectedResult[] = { 12, };

        tOTSubject source;
        tOTObserver listener(10);

        tOTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.detach(&listener);
        source.notify(3);

        assert(tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (uint32_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tOTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testUpdate passed\n");
    }

    void testCopyCtor()
    {
        size_t expectedResult[] =
        {
            12,
            13, 13,
            14, 24,
                25,
        };

        tOTSubject source;
        tOTObserver listener(10);

        tOTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        tOTObserver listener2 = listener;
        source.notify(3);
        listener2.setBase(20);
        source.notify(4);
        source.detach(&listener);
        source.notify(5);

        assert(tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (uint32_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tOTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyCtor passed\n");
    }

    void testCopyAssign()
    {
        size_t expectedResult[] =
        {
            12, 32,
            13, 13,
            14, 24,
                25,
        };

        tOTSubject source;
        tOTObserver listener(10);
        tOTObserver listener2(30);

        tOTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.attach(&listener2);
        source.notify(2);
        listener2 = listener;
        source.notify(3);
        listener2.setBase(20);
        source.notify(4);
        source.detach(&listener);
        source.notify(5);

        assert(tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (uint32_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tOTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyAssign passed\n");
    }

#if __cplusplus >= 201103L
    void testMoveCtor()
    {
        size_t expectedResult[] =
        {
            12,
                13,
                24,
                25,
        };

        tOTSubject source;
        tOTObserver listener(10);

        tOTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        tOTObserver listener2 = std::move(listener);
        source.notify(3);
        listener2.setBase(20);
        source.notify(4);
        source.notify(5);

        assert(tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (uint32_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tOTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveCtor passed\n");
    }

    void testMoveAssign()
    {
        size_t expectedResult[] =
        {
            12, 32,
                13,
                24,
                25,
        };

        tOTSubject source;
        tOTObserver listener(10);
        tOTObserver listener2(30);

        tOTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.attach(&listener2);
        source.notify(2);
        listener2 = std::move(listener);
        source.notify(3);
        listener2.setBase(20);
        source.notify(4);
        source.notify(5);

        assert(tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tOTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (uint32_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tOTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveAssign passed\n");
    }
#endif
};

void RunSubjectTests();

void RunObserverTests()
{
    printf("*** Running tObserverTests...\n");
    tObserverTests();
}

class tSubjectTests;
int main (int argc, char *argv[])
{
    RunObserverTests();
    RunSubjectTests();

    printf("*** All tests passed!\n");

    return 0;
}

