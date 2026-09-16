#include <cstdlib>
#include <cassert>
#include "SharedPtr.h"

int main() {

   //testing with a single shared pointer
   SharedPtr<int> ptr1(new int(5));
   assert(*ptr1 == 5);
   assert(*ptr1.get() == 5);
   assert(ptr1.getControl() != nullptr);
   assert(*static_cast<int*>(ptr1.getControl()->managedAddress()) == 5);
   assert(ptr1.getControl()->refCount() == 1);
   assert(ptr1.useCount() == ptr1.getControl()->refCount());

   //testing with multiple shared pointers
   const SharedPtr<int> ptr2(ptr1);
   SharedPtr<int> ptr3(ptr2);
   assert(ptr2.getControl() == ptr3.getControl());
   assert(ptr2.getControl()->refCount() == 3);
   ptr1 = std::move(ptr3);
   assert(ptr3.getControl() == nullptr);
   assert(ptr1.getControl()->refCount() == 2);
   ptr3 = ptr2;
   assert(ptr1.getControl()->refCount() == 3);
   assert(ptr2.getControl() != nullptr);
   assert(ptr1 == ptr2);
   ptr3.reset();
   assert(!ptr3);
   assert(ptr3.getControl() == nullptr);
   assert(ptr1.useCount() == 2);

   //test swap
   SharedPtr<int> ptr4(new int(8));
   ptr4.swap(ptr1);
   assert(ptr4.useCount() == 2);
   assert(ptr1.useCount() == 1);
   assert(*ptr4 == 5);
   assert(*ptr1 == 8);
   assert(ptr4 == ptr2);

   //test default constructor
   SharedPtr<int> ptr5;
   assert(!ptr5);
   assert(ptr5.getControl() == nullptr);

   ptr1.reset();
   ptr4.reset();
   ptr5.reset();


   return EXIT_SUCCESS;
}
