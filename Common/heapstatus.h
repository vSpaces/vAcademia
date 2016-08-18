//
// heapstatus.h - Heap Iteration class
// Must be included before crtdbg.h
//

#include <malloc.h>
#include <stdexcept>

#ifndef _HEAPSTATUS_H_INCLUDED_
#define _HEAPSTATUS_H_INCLUDED_



struct CountSizePolicy
{
   // Note that in _DEBUG mode, the size of each allocated block is larger 
   // than the size that is specified in the allocation request (either 
   // malloc() or new).
   //
   // This is due to additional trailing/leading bytes which are wrapped around 
   // each allocated block. 
   // These bytes hold some magic numbers, that are used for testing the 
   // block's integrity.
   //
   // See crtdbg.h for more details
   //
   unsigned int usedBytes_; 
   unsigned int freeBytes_;
   
   CountSizePolicy() { reset(); }
   
   void reset() 
   {
      usedBytes_ = 0;
      freeBytes_ = 0;
   }

   void onUsedBlock(void *p, unsigned int size)
   {
      usedBytes_ += size;
   }
   
   void onFreeBlock(void *p, unsigned int size)
   {
      freeBytes_ += size;
   }   
};

template<typename BlockVisitPolicy = CountSizePolicy>
struct HeapStatus : BlockVisitPolicy
{
public:
	HeapStatus() { refresh(); }
   
   void refresh()
   {
      reset();
      
      _HEAPINFO hinfo;
      hinfo._pentry = NULL;
            
      int heapstatus;
      while(true)
      {
         heapstatus = _heapwalk(&hinfo);
         if(heapstatus == _HEAPEND || heapstatus == _HEAPEMPTY)
            return;

         if(hinfo._useflag == _USEDENTRY)
            onUsedBlock(hinfo._pentry, hinfo._size);
         else
            onFreeBlock(hinfo._pentry, hinfo._size);         
      }
      
      // If we got here there is a problem.
      switch(heapstatus)
      {
         case _HEAPBADPTR:
            throw std::runtime_error("Heap Status error: bad pointer to heap");
            break;
         case _HEAPBADBEGIN:
            throw std::runtime_error("Heap Status error: bad start of heap");
            break;
         case _HEAPBADNODE:
            throw std::runtime_error("Heap Status error: bad node in heap");
            break;
      }      
   }   
};


#endif // _HEAPSTATUS_H_INCLUDED_