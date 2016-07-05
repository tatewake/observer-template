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

#include <stdio.h>
#include <vector>

#include "tObserver.h"

#if __cplusplus >= 201103L
#include <utility>
#endif

static std::vector<size_t> tSTNotifications;

class tSubjectTestClass
: public tSubject<const size_t&>
{
public:
    tSubjectTestClass() { }
    tSubjectTestClass(const tSubjectTestClass& other) : tSubject<const size_t&>(other) { }
    virtual ~tSubjectTestClass() { }

    tSubjectTestClass& operator=(const tSubjectTestClass& other) { return static_cast<tSubjectTestClass&>(tSubject<const size_t&>::operator=(other)); }

#if __cplusplus >= 201103L
    tSubjectTestClass(tSubjectTestClass&& other) : tSubject<const size_t&>(std::move(other)) { }
    tSubjectTestClass& operator=(tSubjectTestClass&& other) { return static_cast<tSubjectTestClass&>(tSubject<const size_t&>::operator=(std::move(other))); }
#endif
};

class tObserverTestClass
: public tObserver<const size_t&>
{
protected:
    size_t mBase;

public:
    tObserverTestClass(size_t base) : mBase(base) { }
    virtual ~tObserverTestClass() { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg + mBase);
    }
};

class tDetachObserverOnUpdateTestClass
: public tObserver<const size_t&>
{
protected:
    tSubject<const size_t&>*  mSubject;
    tObserver<const size_t&>* mObserver;
    bool                        mDidRun;

public:
    tDetachObserverOnUpdateTestClass(tSubject<const size_t&>* sub, tObserver<const size_t&>* obs) : mSubject(sub), mObserver(obs), mDidRun(false) { }
    virtual ~tDetachObserverOnUpdateTestClass() { }

    void reset()
    {
        mDidRun = false;
    }

    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        if (!mDidRun)
        {
            mSubject->detach(mObserver);
            mDidRun = true;
        }
    }
};

class tDetachAllOnUpdateTestClass
: public tObserver<const size_t&>
{
protected:
    tSubject<const size_t&>*  mSubject;
    bool                        mDidRun;

public:
    tDetachAllOnUpdateTestClass(tSubject<const size_t&>* sub) : mSubject(sub), mDidRun(false) { }
    virtual ~tDetachAllOnUpdateTestClass() { }

    void reset()
    {
        mDidRun = false;
    }

    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        if (!mDidRun)
        {
            mSubject->detachAll();
            mDidRun = true;
        }
    }
};

class tDeleteSubjectOnUpdateTestClass
: public tObserver<const size_t&>
{
protected:
    tSubject<const size_t&>*   mSubject;

public:
    tDeleteSubjectOnUpdateTestClass(tSubject<const size_t&>* sub = NULL) : mSubject(sub) { }
    virtual ~tDeleteSubjectOnUpdateTestClass() { }

    void reset(tSubject<const size_t&>* sub)
    {
        mSubject = sub;
    }

    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        assert(mSubject);
        delete mSubject;
    }
};

class AttachNewDuringNotify
: public tObserver<const size_t&>
{
public:
    tObserverTestClass mSecond;
    tSubject<const size_t&>* mSubject;

public:
    AttachNewDuringNotify(tSubject<const size_t&>* newSubject) : mSecond(10), mSubject(newSubject) { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg);
        mSubject->attach(&mSecond);
    }
};

class DetachExistingDuringNotify
: public tObserver<const size_t&>
{
public:
    tSubject<const size_t&>* mSubject;
    tObserver<const size_t&>* mObserver;
    bool mPaused;

public:
    DetachExistingDuringNotify(tSubject<const size_t&>* newSubject, tObserver<const size_t&>* newObserver) : mSubject(newSubject), mObserver(newObserver), mPaused(false) { }

    void reset()
    {
        mPaused = false;
    }

    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        if (!mPaused)
        {
            mSubject->detach(mObserver);
            mPaused = true;
        }
    }
};

class DetachAllAndAttachExistingDuringNotify
: public tObserver<const size_t&>
{
public:
    tSubject<const size_t&>* mSubject;
    tObserver<const size_t&>* mObserver;
    bool mPaused;

public:
    DetachAllAndAttachExistingDuringNotify(tSubject<const size_t&>* newSubject, tObserver<const size_t&>* newObserver) : mSubject(newSubject), mObserver(newObserver), mPaused(false) { }

    void reset()
    {
        mPaused = false;
    }

    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        if (!mPaused)
        {
            mSubject->detachAll();
            mSubject->attach(mObserver);
            mPaused = true;
        }
    }
};

class AttachExistingDuringNotify
: public tObserver<const size_t&>
{
public:
    tSubject<const size_t&>* mSubject;
    tObserver<const size_t&>* mObserver;
    bool mPaused;

public:
    AttachExistingDuringNotify(tSubject<const size_t&>* newSubject, tObserver<const size_t&>* newObserver) : mSubject(newSubject), mObserver(newObserver), mPaused(false) { }

    void reset()
    {
        mPaused = false;
    }
    virtual void update(const size_t& msg)
    {
#pragma unused(msg)
        if (!mPaused)
        {
            mSubject->attach(mObserver);
            mPaused = true;
        }
    }
};


class tObserverCopyCtorXDuringNotifyXTestClass
: public tObserver<const size_t&>
{
protected:
    tSubjectTestClass*  mSource1;
    tSubjectTestClass** mSource2;
    size_t mBase;
    size_t mTrigger;

public:
    tObserverCopyCtorXDuringNotifyXTestClass(size_t base, tSubjectTestClass* newSource1, tSubjectTestClass** newSource2, size_t newTrigger)
    : mSource1(newSource1), mSource2(newSource2), mBase(base), mTrigger(newTrigger) { }
    virtual ~tObserverCopyCtorXDuringNotifyXTestClass() { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg + mBase);

        if (msg == mTrigger)
        {
            *mSource2 = new tSubjectTestClass(*mSource1);
        }
    }
};

class tObserverCopyAssignXDuringNotifyXTestClass
: public tObserver<const size_t&>
{
protected:
    tSubjectTestClass* mSource1;
    tSubjectTestClass* mSource2;
    size_t mBase;
    size_t mTrigger;

public:
    tObserverCopyAssignXDuringNotifyXTestClass(size_t base, tSubjectTestClass* newSource1, tSubjectTestClass* newSource2, size_t newTrigger)
    : mSource1(newSource1), mSource2(newSource2), mBase(base), mTrigger(newTrigger) { }
    virtual ~tObserverCopyAssignXDuringNotifyXTestClass() { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg + mBase);

        if (msg == mTrigger)
        {
            *mSource2 = *mSource1;
        }
    }
};

#if __cplusplus >= 201103L
class tObserverMoveCtorXDuringNotifyXTestClass
: public tObserver<const size_t&>
{
protected:
    tSubjectTestClass*  mSource1;
    tSubjectTestClass** mSource2;
    size_t mBase;
    size_t mTrigger;

public:
    tObserverMoveCtorXDuringNotifyXTestClass(size_t base, tSubjectTestClass* newSource1, tSubjectTestClass** newSource2, size_t newTrigger)
    : mSource1(newSource1), mSource2(newSource2), mBase(base), mTrigger(newTrigger) { }
    virtual ~tObserverMoveCtorXDuringNotifyXTestClass() { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg + mBase);

        if (msg == mTrigger)
        {
            *mSource2 = new tSubjectTestClass(std::move(*mSource1));
        }
    }
};

class tObserverMoveAssignXDuringNotifyXTestClass
: public tObserver<const size_t&>
{
protected:
    tSubjectTestClass* mSource1;
    tSubjectTestClass* mSource2;
    size_t mBase;
    size_t mTrigger;

public:
    tObserverMoveAssignXDuringNotifyXTestClass(size_t base, tSubjectTestClass* newSource1, tSubjectTestClass* newSource2, size_t newTrigger)
    : mSource1(newSource1), mSource2(newSource2), mBase(base), mTrigger(newTrigger) { }
    virtual ~tObserverMoveAssignXDuringNotifyXTestClass() { }

    virtual void update(const size_t& msg)
    {
        tSTNotifications.push_back(msg + mBase);

        if (msg == mTrigger)
        {
            *mSource2 = std::move(*mSource1);
        }
    }
};
#endif

class tSubjectTests
{
public:
	tSubjectTests()
	{
		testAttachDetachAndNotify();
        testDetachAll();
		testDetachDuringNotify();
        testDetachAllDuringNotify();
		testDeleteSubjectDuringNotify();
        testAttachSubjectDuringNotify();

        testAttachDetachAttachDuringNotify();
        testAttachDetachAllAttachDuringNotify();

        testCopyCtor();
        testCopyAssign();

#if __cplusplus >= 201103L
        testMoveCtor();
        testMoveAssign();
#endif

        testCopyCtor2DuringNotify1();
        testCopyAssign2DuringNotify1();

#if __cplusplus >= 201103L
        testMoveCtor2DuringNotify1();
        testMoveAssign2DuringNotify1();
#endif
    }

    void testAttachDetachAndNotify()
    {
        size_t expectedResult[] = { 2,3,4 };

        tSubjectTestClass source;
        tObserverTestClass listener(0);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);
        source.notify(4);
        source.detach(&listener);
        source.notify(5);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testAttachDetachAndNotify passed\n");
    }

    void testDetachAll()
    {
        size_t expectedResult[] = { 2,3,13,4,14,24 };

        tSubjectTestClass source;
        tObserverTestClass listener1(0);
        tObserverTestClass listener2(10);
        tObserverTestClass listener3(20);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener1);
        source.notify(2);
        source.attach(&listener2);
        source.notify(3);
        source.attach(&listener3);
        source.notify(4);
        source.detachAll();
        source.notify(5);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testDetachAll passed\n");
    }

    void testDetachDuringNotify()
    {
        size_t expectedResult[] =
        {
            1,11,21,
              12,22,
            3,13,23,
            4,   24,
            5,15,
            7
        };

        tSubjectTestClass source;
        tObserverTestClass listenerA(0);
        tObserverTestClass listenerB(10);
        tObserverTestClass listenerC(20);
        tDetachObserverOnUpdateTestClass    detachA(&source, &listenerA);
        tDetachObserverOnUpdateTestClass    detachB(&source, &listenerB);
        tDetachObserverOnUpdateTestClass    detachC(&source, &listenerC);

        tSTNotifications.clear();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(1);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&detachA);
        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(2);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&listenerA);
        source.attach(&detachA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(3);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&listenerA);
        source.attach(&detachB);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(4);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&listenerA);
        source.attach(&detachC);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(5);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&detachA);
        source.attach(&detachB);
        source.attach(&detachC);
        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(6);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        source.attach(&listenerA);
        source.attach(&detachA);
        source.attach(&detachB);
        source.attach(&detachC);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(7);
        source.detachAll(); detachA.reset(); detachB.reset(); detachC.reset();

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testDetachDuringNotify passed\n");
    }

    void testDetachAllDuringNotify()
    {
        size_t expectedResult[] =
        {
            1,11,21,
            3,
            4,14,
            5,15,25,
        };

        tSubjectTestClass source;
        tObserverTestClass listenerA(0);
        tObserverTestClass listenerB(10);
        tObserverTestClass listenerC(20);
        tDetachAllOnUpdateTestClass    detachAll(&source);

        tSTNotifications.clear();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(1);
        source.detachAll(); detachAll.reset();

        source.attach(&detachAll);
        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(2);
        source.detachAll(); detachAll.reset();

        source.attach(&listenerA);
        source.attach(&detachAll);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(3);
        source.detachAll(); detachAll.reset();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&detachAll);
        source.attach(&listenerC);
        source.notify(4);
        source.detachAll(); detachAll.reset();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.attach(&detachAll);
        source.notify(5);
        source.detachAll(); detachAll.reset();

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testDetachAllDuringNotify passed\n");
    }

    void testDeleteSubjectDuringNotify()
    {
        size_t expectedResult[] =
        {
            1,11,21,
            2,12,
            3,
        };

        tSubjectTestClass* source = NULL;
        tObserverTestClass listenerA(0);
        tObserverTestClass listenerB(10);
        tObserverTestClass listenerC(20);
        tDeleteSubjectOnUpdateTestClass    deleteSubject;

        tSTNotifications.clear();

        source = new tSubjectTestClass; deleteSubject.reset(source);

        source->attach(&listenerA);
        source->attach(&listenerB);
        source->attach(&listenerC);
        source->attach(&deleteSubject);
        source->notify(1);

        source = new tSubjectTestClass; deleteSubject.reset(source);

        source->attach(&listenerA);
        source->attach(&listenerB);
        source->attach(&deleteSubject);
        source->attach(&listenerC);
        source->notify(2);

        source = new tSubjectTestClass; deleteSubject.reset(source);

        source->attach(&listenerA);
        source->attach(&deleteSubject);
        source->attach(&listenerB);
        source->attach(&listenerC);
        source->notify(3);

        source = new tSubjectTestClass; deleteSubject.reset(source);

        source->attach(&deleteSubject);
        source->attach(&listenerA);
        source->attach(&listenerB);
        source->attach(&listenerC);
        source->notify(4);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testDeleteSubjectDuringNotify passed\n");
    }

    void testAttachSubjectDuringNotify()
    {
        size_t expectedResult[] =
        {
            4,
        };

        tSubjectTestClass source;
        AttachNewDuringNotify listener(&source);

        tSTNotifications.clear();

        source.attach(&listener);
        source.notify(4);
        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testAttachSubjectDuringNotify passed\n");
    }

    void testAttachDetachAttachDuringNotify()
    {
        size_t expectedResult[] =
        {
            1,11,21,2,12,22,
              13,23,  14,24,4,
              15,25,  16,26,6,
              17,27,  18,28,8,
        };

        tSubjectTestClass source;
        tObserverTestClass listenerA(0);
        tObserverTestClass listenerB(10);
        tObserverTestClass listenerC(20);
        AttachExistingDuringNotify    attachA(&source, &listenerA);
        DetachExistingDuringNotify    detachA(&source, &listenerA);
        AttachExistingDuringNotify    attachAAgain(&source, &listenerA);

        tSTNotifications.clear();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(1);
        source.notify(2);
        source.detachAll(); attachA.reset(); detachA.reset(); attachAAgain.reset();

        source.attach(&attachA);
        source.attach(&detachA);
        source.attach(&attachAAgain);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(3);
        source.notify(4);
        source.detachAll(); attachA.reset(); detachA.reset(); attachAAgain.reset();

        source.attach(&listenerB);
        source.attach(&attachA);
        source.attach(&detachA);
        source.attach(&attachAAgain);
        source.attach(&listenerC);
        source.notify(5);
        source.notify(6);
        source.detachAll(); attachA.reset(); detachA.reset(); attachAAgain.reset();

        source.attach(&listenerB);
        source.attach(&listenerC);
        source.attach(&attachA);
        source.attach(&detachA);
        source.attach(&attachAAgain);
        source.notify(7);
        source.notify(8);
        source.detachAll(); attachA.reset(); detachA.reset(); attachAAgain.reset();

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testAttachDetachAttachDuringNotify passed\n");
    }

    void testAttachDetachAllAttachDuringNotify()
    {
        size_t expectedResult[] =
        {
            1,11,21,2,12,22,
                    4,
              15,   6,
              17,27,8
        };

        tSubjectTestClass source;
        tObserverTestClass listenerA(0);
        tObserverTestClass listenerB(10);
        tObserverTestClass listenerC(20);
        AttachExistingDuringNotify              attachA(&source, &listenerA);
        DetachAllAndAttachExistingDuringNotify  detachAllAndAttachA(&source, &listenerA);

        tSTNotifications.clear();

        source.attach(&listenerA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(1);
        source.notify(2);
        source.detachAll(); attachA.reset(); detachAllAndAttachA.reset();

        source.attach(&attachA);
        source.attach(&detachAllAndAttachA);
        source.attach(&listenerB);
        source.attach(&listenerC);
        source.notify(3);
        source.notify(4);
        source.detachAll(); attachA.reset(); detachAllAndAttachA.reset();

        source.attach(&listenerB);
        source.attach(&attachA);
        source.attach(&detachAllAndAttachA);
        source.attach(&listenerC);
        source.notify(5);
        source.notify(6);
        source.detachAll(); attachA.reset(); detachAllAndAttachA.reset();

        source.attach(&listenerB);
        source.attach(&listenerC);
        source.attach(&attachA);
        source.attach(&detachAllAndAttachA);
        source.notify(7);
        source.notify(8);
        source.detachAll(); attachA.reset(); detachAllAndAttachA.reset();

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testAttachDetachAllAttachDuringNotify passed\n");
    }

    void testCopyCtor()
    {
        size_t expectedResult[] = { 2,3,4,5,7 };

        tSubjectTestClass source;
        tObserverTestClass listener(0);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        tSubjectTestClass source2 = source;

        source.notify(4);
        source2.notify(5);
        source.detach(&listener);
        source.notify(6);
        source2.notify(7);
        source2.detach(&listener);
        source.notify(8);
        source2.notify(9);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyCtor passed\n");
    }

    void testCopyAssign()
    {
        size_t expectedResult[]  = { 3,54,5,56,7,8,10 };

        tSubjectTestClass source;
        tSubjectTestClass source2;
        tObserverTestClass listener(0);
        tObserverTestClass listener2(50);

        tSTNotifications.clear();

        source.notify(1);
        source2.notify(2);
        source.attach(&listener);
        source2.attach(&listener2);
        source.notify(3);
        source2.notify(4);
        source.notify(5);
        source2.notify(6);

        source2 = source;

        source.notify(7);
        source2.notify(8);
        source.detach(&listener);
        source.notify(9);
        source2.notify(10);
        source2.detach(&listener);
        source.notify(11);
        source2.notify(12);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyAssign passed\n");
    }

#if __cplusplus >= 201103L
    void testMoveCtor()
    {
        size_t expectedResult[] = { 2,3,5 };

        tSubjectTestClass source;
        tObserverTestClass listener(0);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        tSubjectTestClass source2 = std::move(source);

        source.notify(4);
        source2.notify(5);
        source2.detach(&listener);
        source.notify(6);
        source2.notify(7);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveCtor passed\n");
    }

    void testMoveAssign()
    {
        size_t expectedResult[]  = { 3,54,5,56,8 };

        tSubjectTestClass source;
        tSubjectTestClass source2;
        tObserverTestClass listener(0);
        tObserverTestClass listener2(50);

        tSTNotifications.clear();

        source.notify(1);
        source2.notify(2);
        source.attach(&listener);
        source2.attach(&listener2);
        source.notify(3);
        source2.notify(4);
        source.notify(5);
        source2.notify(6);

        source2 = std::move(source);

        source.notify(7);
        source2.notify(8);
        source2.detach(&listener);
        source.notify(9);
        source2.notify(10);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveAssign passed\n");
    }
#endif

    void testCopyCtor2DuringNotify1()
    {
        size_t expectedResult[] = { 2,3,4,5,7 };

        tSubjectTestClass source;
        tSubjectTestClass* source2;

        tObserverCopyCtorXDuringNotifyXTestClass listener(0, &source, &source2, 3);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        source.notify(4);
        source2->notify(5);
        source.detach(&listener);
        source.notify(6);
        source2->notify(7);
        source2->detach(&listener);
        source.notify(8);
        source2->notify(9);

        delete source2;

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyCtor2DuringNotify1 passed\n");
    }

    void testCopyAssign2DuringNotify1()
    {
        size_t expectedResult[] = { 2,3,4,5,7 };

        tSubjectTestClass source;
        tSubjectTestClass source2;

        tObserverCopyAssignXDuringNotifyXTestClass listener(0, &source, &source2, 3);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        source.notify(4);
        source2.notify(5);
        source.detach(&listener);
        source.notify(6);
        source2.notify(7);
        source2.detach(&listener);
        source.notify(8);
        source2.notify(9);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testCopyAssign2DuringNotify1 passed\n");
    }

#if __cplusplus >= 201103L
    void testMoveCtor2DuringNotify1()
    {
        size_t expectedResult[] = { 2,3,5 };

        tSubjectTestClass source;
        tSubjectTestClass* source2;

        tObserverMoveCtorXDuringNotifyXTestClass listener(0, &source, &source2, 3);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        source.notify(4);
        source2->notify(5);
        source2->detach(&listener);
        source.notify(6);
        source2->notify(7);

        delete source2;

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveCtor2DuringNotify1 passed\n");
    }

    void testMoveAssign2DuringNotify1()
    {
        size_t expectedResult[] = { 2,3,5 };

        tSubjectTestClass source;
        tSubjectTestClass source2;

        tObserverMoveAssignXDuringNotifyXTestClass listener(0, &source, &source2, 3);

        tSTNotifications.clear();

        source.notify(1);
        source.attach(&listener);
        source.notify(2);
        source.notify(3);

        source.notify(4);
        source2.notify(5);
        source2.detach(&listener);
        source.notify(6);
        source2.notify(7);

        assert(tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t));

        if (tSTNotifications.size() == sizeof(expectedResult) / sizeof(size_t))
        {
            for (size_t i = 0; i < sizeof(expectedResult) / sizeof(size_t); i++)
            {
                assert(tSTNotifications[i] == expectedResult[i]);
            }
        }

        printf("*** ::testMoveAssign2DuringNotify1 passed\n");
    }
#endif
};

void RunSubjectTests()
{
    printf("*** Running tSubjectTests...\n");
    tSubjectTests();
}

