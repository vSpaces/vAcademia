/*
 * GMEM.H - Macros for windows 3.0 memory management in protected mode
 *
 * because windows 3.0 runs in pmode GlobalLock and GlobalUnlock are
 * unnessary.  The "Selector" to a memory object will always be the
 * same for the life of the memory object.
 *
 * these macros take advantage of the following win3 memory "facts"
 *
 *      a SELECTOR (to a global object) is a HANDLE
 *      a HANDLE is *not* a SELECTOR!!!!!!!!
 *
 *      GlobalLock() and GlobalUnlock() do *not* keep lock counts
 *
 *      GlobalLock() is the only way to convert a HANDLE to a SELECTOR
 *
 * functions:
 *
 *      GHandle(sel)                convert a SELECTOR to a HANDLE
 *      GSelector(h)                convert a HANDLE to a SELECTOR
 *
 *      GAlloc(ulBytes)          allocate a SELECTOR ulBytes in size
 *      GAllocPtr(ulBytes)          allocate a POINTER ulBytes in size
 *
 *      GReAlloc(sel,ulBytes)    re-alloc a SELECTOR
 *      GReAllocPtr(lp,ulBytes)     re-alloc a POINTER
 *
 *      GSize(sel)               return the size in bytes of a SELECTOR
 *
 *      GLock(sel)               convert a SELECTOR into a POINTER
 *      GUnlock(sel)             does nothing
 *
 *      GFree(sel)               free a SELECTOR
 *      GFreePtr(lp)                free a POINTER
 *
 */
/*
extern HANDLE __H;

#define MAKEP(sel,off)              ((LPVOID)MAKELONG(off,sel))
#define SelPtr(sel)                 MAKEP(sel,0)

#define GHandle(sel)                ((HANDLE)(sel))
#define GSelector(h)                (HIWORD((DWORD)GlobalLock(h)))

#define GAllocF(f,ulBytes)          ((__H=GlobalAlloc(f,(LONG)(ulBytes))) ? GSelector(__H) : NULL )
#define GAllocPtrF(f,ulBytes)       MAKEP(GAllocF(f,ulBytes),0)

#define GAllocPtr(ulBytes)          GAllocPtrF(GMEM_MOVEABLE,ulBytes)
#define GAlloc(ulBytes)             GAllocF(GMEM_MOVEABLE,ulBytes)

#define GReAlloc(sel,ulBytes)       ((__H=GlobalReAlloc((HANDLE)(sel),(LONG)(ulBytes),0)) ? GSelector(__H) : NULL )
#define GReAllocPtr(lp,ulBytes)     MAKEP(GReAlloc(HIWORD((DWORD)(lp)),ulBytes),0)

#define GSize(sel)                  GlobalSize((HANDLE)(sel))

#define GLock(sel)                  MAKEP(sel,0)
#define GUnlock(sel)

#define GFree(sel)                  (GlobalUnlock(GHandle(sel)),GlobalFree(GHandle(sel)))
#define GFreePtr(lp)                GFree(HIWORD((DWORD)(lp)))
*/

#define     GlobalPtrHandle(lp)         \
                ((HGLOBAL)GlobalHandle(lp))

#define     GlobalLockPtr(lp)                \
                ((BOOL)GlobalLock(GlobalPtrHandle(lp)))
#define     GlobalUnlockPtr(lp)      \
                GlobalUnlock(GlobalPtrHandle(lp))

#define     GlobalAllocPtr(flags, cb)        \
                (GlobalLock(GlobalAlloc((flags), (cb))))
#define     GlobalReAllocPtr(lp, cbNew, flags)       \
                (GlobalUnlockPtr(lp), GlobalLock(GlobalReAlloc(GlobalPtrHandle(lp) , (cbNew), (flags))))
#define     GlobalFreePtr(lp)                \
                (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))

#define GAlloc(ulBytes) GlobalAlloc(GMEM_MOVEABLE,(DWORD) ulBytes)
#define GAllocF(f,ulBytes) GlobalAlloc(f,(DWORD) ulBytes)
#define GLock(sel)      GlobalLock(sel)
#define GUnlock(sel)    GlobalUnlock(sel)
#define GFree(sel)      (GlobalUnlock(sel),GlobalFree(sel))
#define GReAlloc(sel,ulBytes)\
                        (GlobalReAlloc(sel , ulBytes, GMEM_MOVEABLE))

#define GAllocPtr(ulBytes)          GlobalLock(GlobalAlloc(GMEM_MOVEABLE,(DWORD) ulBytes))
#define GFreePtr(lp)                (GlobalFreePtr(lp))

//#define GAllocPtr(ulBytes)          new BYTE[ulBytes]
//#define GFreePtr(lp)                delete [] lp
