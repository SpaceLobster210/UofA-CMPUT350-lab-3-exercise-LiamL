#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

#include <utility>

class ControlBlockBase {
public:
    ControlBlockBase() { refcount = 1; };

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() {};

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        refcount += 1;
        return refcount;
    }

    long decrement()
    {
        refcount -= 1;
        return refcount;
    }

    long refCount() const
    {
        return refcount;
    }

private:
    long refcount;
};


template <typename T>
class ControlBlock: public ControlBlockBase {
    public:
        ControlBlock(T* myObj = nullptr) : managedaddress{myObj} { refcount = 1; };

        ~ControlBlock() override { delete managedaddress; };

        T* managedAddress() override { return managedaddress; };

    private:
        T* managedaddress;
};



template <typename T>
class SharedPtr {
    public:
        SharedPtr() {
            stored = nullptr;
            control = nullptr;
        };

        SharedPtr(T* myObj = nullptr) : stored{myObj} {
            control = &(new ControlBlock<T>(myObj));
        };

        ~SharedPtr() {
            decrementControl();
        };

        void decrementControl() {
            if (control != nullptr) {
                control->decrement();
                if (control->refCount() <= 0) {
                    delete control;
                }
            }
        }

        void incrementControl() {
            if (control != nullptr) {
                control->increment();
            }
        }

        //Copy Constructor
        SharedPtr(const SharedPtr& other) {
            decrementControl();
            stored = other.get();
            control = other.getControl();
            incrementControl();
        };

        //Copy Assignment Operator
        SharedPtr& operator=(const SharedPtr& other) {
            decrementControl();
            stored = other.get();
            control = other.getControl();
            incrementControl();
            return *this;
        }

        //Move Constructor
        SharedPtr(SharedPtr&& other) {
            if (other.get() != stored) {
                reset();
                stored = other.get();
            }
            decrementControl();
            control = other.getControl();
        };

        //Move Assignment Operator
        SharedPtr& operator=(SharedPtr&& other) {
            if (other.get() != stored) {
                reset();
                stored = other.get();
            }
            decrementControl();
            control = other.getControl();
            return *this;
        }

        T& operator*() const { return *stored; }

        T* operator->() { return stored; }

        T* get() const { return stored; }

        ControlBlockBase* getControl() const { return control; }

        bool operator==(const SharedPtr<T>& other) const { return (stored == other.get()); }

        operator bool() const { return (stored != nullptr); }

        void swap(SharedPtr<T>& other) {
            ControlBlockBase* tempCtrl = control;
            incrementControl();

            stored = other.get();
            control = other.getControl();
            other.setPointer(tempCtrl);
        }

        void reset(T* newPtr = nullptr) {
            decrementControl();
            stored = newPtr;
            if (newPtr != nullptr) {
                control = &(new ControlBlock<T>(newPtr));
            }
        }

        long useCount() const { return control->refCount(); }

        void setPointer(ControlBlockBase* newCtrl) {
            if (newCtrl != nullptr) {
                stored = newCtrl->managedAddress();
            }

            decrementControl();
            control = newCtrl;
        }

    private:
        T* stored;
        ControlBlockBase* control;
};


template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif