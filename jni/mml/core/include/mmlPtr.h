#ifndef MML_AUTO_PTR
#define MML_AUTO_PTR

#include "mmlIObject.h"


template < class T >
class mmlPtrReferrer
{
public:

    mmlPtrReferrer ( T ** ptr)
    :mRawPtr(mmlNULL), mPtr(ptr)
    {
    }
    
    ~mmlPtrReferrer()
    {
        if ( mRawPtr != mmlNULL )
        {
             mRawPtr->QueryInterface(MML_OBJECT_UUID(T), (void**) mPtr);
             MML_RELEASE(mRawPtr);
        }
    }
    
    
    operator T ** ()
    {
        return mPtr;
    }
    
    operator mmlIObject ** ()
    {
        return &mRawPtr;
    }
    
    operator void ** ()
    {
        return static_cast<void**>(&mRawPtr);
    }

   
private:
    
    mmlIObject * mRawPtr;
    T ** mPtr;
        
};

template < class T >
class mmlPtrReferrerNoCast
{
public:

	mmlPtrReferrerNoCast ( T ** ptr)
		:mPtr(ptr), mRawPtr(mmlNULL)
	{
	}

	~mmlPtrReferrerNoCast()
	{
	}


	operator T ** ()
	{
		return mPtr;
	}

private:

	mmlIObject * mRawPtr;
	T ** mPtr;

};

template < class T >
class mmlAutoPtr
{
private:
	mutable T * ptr;
public:

	mmlAutoPtr ( T * refPtr = mmlNULL)
		:ptr(refPtr)
	{
		MML_ADDREF(ptr);
	}

	~mmlAutoPtr ()
	{
		MML_RELEASE(ptr);
	}

	mmlInt32 operator < (mmlAutoPtr & that_ptr)
	{
		if ( ptr != mmlNULL )
		{
			mmlBool result;
			if ( ptr->Compare(MML_LESS , that_ptr.ptr , result ) == mmlTrue &&
				result == mmlTrue )
			{
				return mmlTrue;
			}
		}
		return mmlFalse;
	}

	T * operator -> () const
	{
		return ptr;
	}

	T & operator *() const
	{
		return *ptr;
	}

	operator T*() const
	{
		return ptr;
	}

	T** getRef ()
	{
		MML_RELEASE(ptr);
		return &ptr;
	}

	mmlAutoPtr ( const mmlAutoPtr & that_ptr )
		:ptr(that_ptr.ptr)
	{
		MML_ADDREF(ptr);
	}

	mmlAutoPtr & operator = (const mmlAutoPtr & that_ptr)
	{
		if ( &that_ptr != this)
		{
			MML_RELEASE(ptr);
			ptr = that_ptr.ptr;
			MML_ADDREF(ptr);
		}
		return *this;
	}

	mmlAutoPtr & operator = (T * that_ptr)
	{
		MML_RELEASE(ptr);
		ptr = that_ptr;
		MML_ADDREF(ptr);
		return * this;
	}

	mmlInt32 operator == (T * that_ptr)
	{
		if ( ptr == that_ptr )
		{
			return 1;
		}
		return 0;
	}

	mmlPtrReferrer < T > getterAddRef()
	{
		MML_RELEASE(ptr);
		return mmlPtrReferrer < T > (&ptr);
	}

	mmlPtrReferrerNoCast < T > getterAddRefNoCast()
	{
		MML_RELEASE(ptr);
		return mmlPtrReferrerNoCast < T > (&ptr);
	}
};

template < class T1 , class T2 >
class mmlRelativePtrAddRef
{
private:

	mmlAutoPtr < T1 > & mSmartPtr;
	T2 * mPtr;

public:

	mmlRelativePtrAddRef ( mmlAutoPtr < T1 > & smart_ptr )
		:mSmartPtr(smart_ptr), mPtr(0)
	{
		smart_ptr = mmlNULL;
	}

	~mmlRelativePtrAddRef()
	{
		if ( mPtr != mmlNULL )
		{
			T1 * ptr = mmlNULL;
			mPtr->QueryInterface(MML_OBJECT_UUID(T1) , (void**)&ptr);
			mSmartPtr = ptr;
			MML_RELEASE(ptr);
			MML_RELEASE(mPtr);
		}
	}

	operator T2 ** ()
	{
		return &mPtr;
	}

};


template < class T >
class mmlSharedPtr
{
private:

        T* ptr;

        void increment()
        {
            MML_ADDREF(ptr);
        }

        void decrement()
        {
            T * tmp = (T*)mmlAtomicSet((void**) &ptr , 0);
            MML_RELEASE(tmp);
        }

    public:


        template < class T2 >
        operator mmlSharedPtr<T2 > () 
        {
            return mmlSharedPtr < T2 > (ptr);
        }

        mmlSharedPtr() : ptr(0)
        {}
        ;
        
        mmlSharedPtr(T* original)
        : ptr(original)
        {
            increment();
        };

        mmlSharedPtr(mmlBool original)
            :ptr(mmlNULL)
        {
        };

        mmlSharedPtr(mmlIObject* original)
            : ptr(mmlNULL)
        {
            if ( original != mmlNULL )
            {
				MML_ADDREF(original);
                original->QueryInterface(MML_OBJECT_UUID(T), (void**)&ptr);
				MML_RELEASE(original);
            }
        };

        mmlSharedPtr(const mmlSharedPtr& x)
        : ptr(x.ptr)
        {
            increment();
        };

        ~mmlSharedPtr()
        {
            decrement();
        }

        T& operator*() const
        {
            return *ptr;
        }
        
        operator  T *()
        {
            return ptr;
        }
        
        mmlPtrReferrer < T > getterAddRef()
        {
            decrement();
            return mmlPtrReferrer < T > (&ptr);
        }

        mmlBool operator!() const
        {
            if (ptr)
            {
                return (ptr == 0) ? mmlTrue : mmlFalse;
            }
            return mmlTrue;
        }

        T* operator->() const
        {
            return ptr;
        }

        template < class T2 > mmlSharedPtr& dynamicCast(const mmlSharedPtr < T2 > & x)
        {
            if (ptr == x.getPtr()) return *this;
            decrement();
            if ( x != mmlNULL )
            {
                (x.getPtr())->QueryInterface(MML_OBJECT_UUID(T), (void**) &ptr);
            }
            return *this;
        } 

        template < class T2 >
        mmlSharedPtr& operator=(const mmlSharedPtr < T2 > & x)
        {
            if (ptr == x.getPtr()) return * this;
            decrement();
            if ( x != mmlNULL )
            {
                (x.getPtr())->QueryInterface(MML_OBJECT_UUID(T), (void**) &ptr);
            }
            return *this;
        }


        mmlSharedPtr& operator=(T* original)
        {
            if (ptr == original) return * this;
            decrement();
            ptr = original;
            increment();
            return *this;
        };

        mmlSharedPtr& operator=(mmlIObject * original)
        {
            decrement();
            if ( original != mmlNULL )
            {
				MML_ADDREF(original);
                original->QueryInterface(MML_OBJECT_UUID(T), (void**) &ptr);
				MML_RELEASE(original);
            }
            return *this;
        };
        
        mmlSharedPtr& operator=(mmlInt32 not_used)
        {
            decrement();
            return *this;
        };

        mmlSharedPtr& operator=(const mmlSharedPtr& x)
        {
            if (ptr == x.getPtr()) return * this;
            decrement();
            ptr = x.ptr;
            increment();
            return *this;
        }

        friend mmlBool operator!=(const void* y, const mmlSharedPtr& x)
        {
            if (x.ptr != y)
                return mmlTrue;
            else
                return mmlFalse;
        }

        friend mmlBool operator!=(const mmlSharedPtr& x, const void* y)
        {
            if (x.ptr != y)
                return mmlTrue;
            else
                return mmlFalse;
        }

        friend mmlBool operator==(const void* y, const mmlSharedPtr& x)
        {
            if (x.ptr == y)
                return mmlTrue;
            else
                return mmlFalse;
        }

        friend mmlBool operator==(const mmlSharedPtr& x, const void* y)
        {
            if (x.ptr == y)
                return mmlTrue;
            else
                return mmlFalse;
        }

        mmlBool operator==(const mmlSharedPtr& x) const
        {
            if (x.ptr == ptr)
                return mmlTrue;
            else
                return mmlFalse;
        }

        mmlBool operator!=(const mmlSharedPtr& x) const
        {
            if (x.ptr != ptr)
                return mmlTrue;
            else
                return mmlFalse;
        }

        mmlBool operator < (const mmlSharedPtr& x) const
        {
            if (x.ptr < ptr)
                return mmlTrue;
            else
                return mmlFalse;
        }

        T* getPtr() const
        {
            return ptr;
        }

};


#endif //MML_AUTO_PTR

