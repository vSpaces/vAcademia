// www    -> www.gorep.se
// e-mail -> per@gorep.se
// 
// Author -> Per Ghosh (e-mail: per@gorep.se)
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
//	This file is provided "as is" with no expressed or implied warranty.
//	The author accepts no liability for any damage/loss of business that
//	this product may cause.

/*! \file gd_listtmpl.h
    \brief collection classes

FAQ:
Q: How can i create a string class.<br>
A: 
\code
CBuffer<char,'0'> bufferText;
bufferText = "This is a text.";
bufferText += " Some more text";
int iPosition = bufferText.Find( "Some" );
\endcode

Q: How can I store some integer values but none of them has the value -1<br>
A: CBuffer<int,-1> bufferInt;

Q: How can I store some pointers<br>
A: CBuffer<void*,NULL> bufferPtr;

Q: I want to store som text strings<br>
A:
\code
CBuffers<wchar_t,wchar_t,_T('\0')> buffersText;
buffersText.Add( _T("A text");
\endcode

Q: I want to store some objects (CMyObject) in a array.<br>
A: 
\code
int iDummy;
CMyObject* pMyObject;
CArrayPtr<CMyObject> arrayToMyObject;
arrayToMyObject.Add( new CMyObject() );
iDummy = 10;
arrayToMyObject.Add( new CMyObject( iDummy ) );
iDummy = 20;
arrayToMyObject.InsertAt( 1, new CMyObject( iDummy ) );
pMyObject = arrayToMyObject.GetAt( 0 );
iDummy = arrayToMyObject.GetSize(); // gets the value 3
arrayToMyObject.DeleteItems();
\endcode

Q: I want to add a large number of items to CArrayPtr. How should I do
   this in a efficient way.<br>
A: If you know how many items the array is going to store set it in
   the SetGrowBy function. If you don't know the size but know the
   items in array is going to be large test different values by setting
   SetGrowBy !

Q: Why should I use the CSortedArrayPtr<br>
A: The CSortedArrayPtr is much faster when you need to find specified
   items in array. The drawback is that you need to have some sort of
   key-value for each item. And these keys can't be duplicated in array.
   The CSortedArrayPtr is using binary search to find a item in array. <br>
   TIP 1: When you add items to the array try to add the smallest first
            and larger later. It is faster to add items if they are added
            at the end of array.

Q: I want to add items as fast as possible to a sorted array, but when I 
   add items, they don't need to be sorted.<br>
A: You could add items last in the sorted array classes and when you need
   them just call sort before, and the array will be sorted.<br>

Q: Is there any special operators that need to be implemented by classes
   that is going to be stored in sorted arrays.<br>
A: There are two operators that needs to be implemented and should work on
   the key type. The operators are '>' and '<'.<br>

Q: I want to store CXxx in a hash<br>
A: 
\code
CMapPtr<ULONG,CXxx> mapptrToXxx;
mapptrToXxx.InitHashTable( 21 );
mapptrToXxx.SetAt( 10, new CXxx );
CXxx* pXxx;
if( mapptrToXxx.Lookup( 10, &pXxx ) == true ) { ... }
mapptrToXxx.Empty( true );
\endcode
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _GD_LISTTMPL_
#define _GD_LISTTMPL_

#include <assert.h>

#ifndef _INC_NEW
	#include <new.h>
#endif

#ifdef _DEBUG
   #ifdef DEBUG_NEW
      #define new DEBUG_NEW
   #endif
#endif

namespace gd
{

template<class ITEM>
void CreateItems( ITEM* pItems, UINT uCount ) {
	memset( (void*)pItems, 0, uCount * sizeof( ITEM ) );
   while( uCount != 0 )
   {
      uCount--;
      // Don't allocate, just construct
      ::new((void*)pItems) ITEM;
      pItems++;
   }
}

template<class ITEM>
void DestroyItems( ITEM* pItems, UINT uCount ) {
   while( uCount != 0 )
   {
      uCount--;
      pItems->~ITEM();
      pItems++;
   }
}

template<class ITEM>
void CopyItems(ITEM* pitemTo, const ITEM* pitemFrom, UINT uCount ) {
   while( uCount != 0 )
   {
      *pitemTo = *pitemFrom;
      pitemTo++;
      pitemFrom++;
   }
}

#pragma warning( disable : 4284 )

/*! \brief CPtr<POINTER> is a smartpointer used to manage deletions of pointers allocated on heap.

Use this to simplyfy code. The destructor of CPtr<POINTER> will make sure that the allocated
object is deleted.<br>
The overhead created by using this smartpointer class is non. All functions are __forceinline and it
only contains one member that is the pointer.
\code
void Sample()
{
   CPtr<CString> pString;
   pString = new CString("This object is deleted by the smartpointer object");
   pString->StringFunction();
   ...
   ...
}
\endcode
*/
template<class POINTER>
class CPtr
{
public:
   explicit CPtr( POINTER* p = NULL ) throw() {
      m_p = p; }

   ~CPtr() throw() {
       MP_DELETE( m_p); }

   //! cast operator for (POINTER*)
   operator POINTER*() const throw() { 
      return m_p; }

   //! assign pointer 
   CPtr<POINTER>& operator=( POINTER* p ) throw() { 
      m_p = p; return *this; }

   POINTER& operator*() const throw() {
      return *m_p; }

   //! & operator that pointer to pointer for internal object
   POINTER** operator&() throw() {
      return &m_p; }


   //! -> operator that returns pointer to object
   POINTER* operator->() const throw() {  
      return m_p; }

   //! == compare to pointer
   bool operator==( const POINTER* p ) {
      return (m_p == p); }

   //! != compare if not equal to pointer
   bool operator!=( const POINTER* p ) {
      return (m_p != p); }

   //! get pointer to object
   POINTER* Get() const throw() {   
      return m_p; }

   //! returns poninter and clear the internal pointer without deletion
   POINTER* Release() throw() {  
      POINTER* p = m_p;
      m_p = NULL;
      return p; }

   //!  delete object and reset the pointer to NULL
   void Delete() throw() { 
      MP_DELETE( m_p);
      m_p = NULL; }
// Attributes
public:
   POINTER* m_p;  //!< the internal pointer
};

#pragma warning( default : 4284 )


const unsigned int ptriref_nil[] = { 0, NULL };

/*! 
*/
template<class POINTER>
class CPtrIRef
{
public:
   CPtrIRef() {
      m_pD = (CPtrIRef<POINTER>::D*)ptriref_nil; }

   explicit CPtrIRef( POINTER* p ) throw() {
      MP_NEW_V( m_pD, D, p ); }

   CPtrIRef( const CPtrIRef<POINTER>& o ) throw() {
      m_pD = o.m_pD;
      if( m_pD != (CPtrIRef<POINTER>::D*)ptriref_nil ) m_pD->AddRef(); }

   //! assign  
   CPtrIRef<POINTER>& operator=( const CPtrIRef<POINTER>& o ) throw() { 
      if( m_pD != (CPtrIRef<POINTER>::D*)ptriref_nil ) m_pD->Release(); 
      m_pD = o.m_pD;
      if( m_pD != (CPtrIRef<POINTER>::D*)ptriref_nil ) m_pD->AddRef(); }

   ~CPtrIRef() throw() {
      if( m_pD != (CPtrIRef<POINTER>::D*)ptriref_nil ) m_pD->Release(); }

   //! cast operator for (POINTER*)
   operator POINTER*() const throw() { 
      return m_pD->Get(); }

   //! assign pointer 
   CPtrIRef<POINTER>& operator=( POINTER* p ) throw() { 
      if( m_pD != (CPtrIRef<POINTER>::D*)ptriref_nil ) m_pD->Release();
      MP_NEW_V( m_pD, D, p ); return *this; }

   POINTER& operator*() const throw() {
      return *m_pD->Get(); }

   //! & operator that pointer to pointer for internal object
   POINTER** operator&() throw() {
      return &m_pD->Get(); }

   //! -> operator that returns pointer to object
   POINTER* operator->() const throw() {  
      return m_pD->Get(); }

   //! == compare to CPtrIRef
   bool operator==( const CPtrIRef<POINTER>* p ) {
      return (m_pD->Get() == p->m_pD->Get()); }

   //! == compare to pointer
   bool operator==( const POINTER* p ) const throw() {
      return (m_pD->Get() == p); }

   //! != compare if not equal to CPtrIRef
   bool operator==( const CPtrIRef<POINTER>* p ) const throw() {
      return (m_pD->Get() != p->m_pD->Get()); }

   //! != compare if not equal to pointer
   bool operator!=( const POINTER* p ) const throw() {
      return (m_pD->Get() != p); }

   //! get pointer to object
   POINTER* Get() const throw() {   
      return m_pD->Get(); }

   //! returns poninter and clear the internal pointer without deletion
   POINTER* Release() throw() {  
      POINTER* p = m_pD->Get();
      m_pD->Release();
      m_pD = NULL;
      return p; }

   //!  delete object and reset the pointer to NULL
   void Delete() throw() { 
      MP_DELETE( m_p);
      m_p = NULL; }

   struct D
   {
      D( POINTER* p ) {
         m_uRefs = 1; m_p = p; }
      void AddRef() {
         m_uRefs++; }
      void Release() {
         m_uRefs--; 
         if( m_uRefs == 0 ) MP_DELETE( m_p); }
      POINTER* Get() const throw() {
         return m_p; }
      unsigned m_uRefs; //!< references this pointer has to it 
      POINTER* m_p;     //!< the internal pointer
   };

// Attributes
public:
   D* m_pD;
   //static D* m_pDNull;
};

/*
template<class POINTER>
CPtrIRef<POINTER>::D* CPtrIRef<POINTER>::m_pDNull = ptriref_nil;
*/


// ************************************************************************************************
template<class ITEM>
class CStackPtr  
{
public:
	CStackPtr( UINT uCount = 1 );
	virtual ~CStackPtr();

// Get/Set
public:
   // Get number of elements on stack
   UINT GetCount();
   // Set how many more openings that should be created if we add more then can fit
   void SetGrowBy( UINT uGrowBy );

// Operation
public:
   // Add pointer to stack
   void Push( ITEM* pITEM );
   // Remove pointer from stack
   ITEM* Pop();
   //  delete items in stack          
   void DeleteItems();

// Attributes
private:
   UINT m_uCount;    //!< Items in stack
   UINT m_uMaxCount; //!< max number of items before we need to realocate
   UINT m_uGrowBy;   //!< how many more valuew that should fit if we need to allocate more space for stack

   ITEM** m_ppITEM;  //!< Pointer to pointers in stack
};                   

template<class ITEM>
CStackPtr<ITEM>::CStackPtr( UINT uCount ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uCount;
   m_ppITEM    = NULL;
}

template<class ITEM>
__forceinline CStackPtr<ITEM>::~CStackPtr() {
	MP_DELETE_ARR( m_ppITEM);
}

template<class ITEM>
__forceinline UINT CStackPtr<ITEM>::GetCount() {
   return m_uCount; }

template<class ITEM>
__forceinline void CStackPtr<ITEM>::SetGrowBy( UINT uGrowBy ) {
   _ASSERT( (uGrowBy < 0xff000000) && (uGrowBy > 0) ); // realistic
   m_uGrowBy = uGrowBy;
}

template<class ITEM>
void CStackPtr<ITEM>::Push( ITEM* pITEM ) {
   ITEM** ppITEM;
   if( m_uCount >= m_uMaxCount )
   {
      m_uMaxCount += m_uGrowBy;
      ppITEM = MP_NEW_ARR( ITEM*, m_uMaxCount);
      memcpy( ppITEM, m_ppITEM, sizeof(ITEM*) * m_uCount );
      MP_DELETE_ARR( m_ppITEM);
      m_ppITEM = ppITEM;
   }

   m_ppITEM[m_uCount] = pITEM;
   m_uCount++;
}

template<class ITEM>
ITEM* CStackPtr<ITEM>::Pop() {
   m_uCount--;
   return m_ppITEM[m_uCount];
}

template<class ITEM>
void CStackPtr<ITEM>::DeleteItems() {
   while( m_uCount )
   {
      m_uCount--;
      MP_DELETE( m_ppITEM[m_uCount]);
   }
}

// ****************************************************************************
/*!CBits could be used to increase performance to find items at specific
   positions. One way to find items in arrays that is very fast is when you
   know the exact location. But often this is not the case. You need to 
   iterate and look for the item that is serached for. 
   The CBits has a kind of hash table that is is speedy to check if there
   is an item at one specific location. If there is one then you could start
   searching for it. Otherwise just skip it. <br>
   Use this if you need speed in order to check if there is one item at the
   requested position.
*/
class CBits
{
// Construction
public: 
   CBits();
   CBits( unsigned uItems );
   ~CBits();

// Operation
public:
   //! Check bit at specified position
   bool CheckBit( unsigned uPosition );
   //! Set allocation size
   void SetBitCount( unsigned uItems );

   // Attributes
protected:
   unsigned  m_uTableSize;
   unsigned* m_puBitTable;
};

__forceinline CBits::CBits() 
{
   m_uTableSize = 0;
   m_puBitTable = NULL;
}

__forceinline CBits::CBits( unsigned uItems ) 
{
#ifdef _DEBUG
   m_puBitTable = NULL;
#endif
   SetBitCount( uItems );
}

__forceinline CBits::~CBits() {
	MP_DELETE_ARR( m_puBitTable);
}

__forceinline bool CBits::CheckBit( unsigned uPosition ) {
   _ASSERT( uPosition < m_uTableSize );
   unsigned uShift = uPosition % (sizeof(unsigned) * 8);
   uPosition /= (sizeof(unsigned) * 8);
   return ( (m_puBitTable[uPosition] & ((unsigned)1 << uShift)) != 0);
}

__forceinline void CBits::SetBitCount( unsigned uItems ) 
{
   _ASSERT( m_puBitTable == NULL ); 
   unsigned uAllocSize = 1 + (uItems / (sizeof(unsigned) * 8));
   m_uTableSize = uItems;
   m_puBitTable = MP_NEW_ARR( unsigned, uAllocSize);
   memset( m_puBitTable, 0, (uAllocSize << (sizeof(unsigned) >> 1)) );
}

// ****************************************************************************
/*! \brief Add searchable key to object.

   CKeyDecoratePtr is a class that decorates another class with one key. This
   key is used to find one specific item. <br>
   Objects that don't have any specific item that makes them searchable could
   be wrapped whith this template. They can be stored in searchable arrays
   later. Or used in other type of areas where there is some kind of searhing
   needed.
\code
   CKeyDecoratePtr<CString,int>* pKDString;
   CArrayPtr<CKeyDecoratePtr<CString,int> > AForKDString;

   AForKDString.Add( new CKeyDecoratePtr<CString,int>(0, new CString("0")) );
   AForKDString.Add( new CKeyDecoratePtr<CString,int>(1, new CString("1")) );
   AForKDString.Add( new CKeyDecoratePtr<CString,int>(2, new CString("2")) );

   for( unsigned u = 0; u < AForKDString.GetSize(); u++ )
   {
      if( *AForKDString[u] == 2 )
      {
         pKDString = AForKDString[u];
         ....
      }
   }

   AForKDString.DeleteItems();
\endcode
*/
template<class OBJECT, class KEY>
class CKeyDecoratePtr
{
public:
   //! Default (empty)
   CKeyDecoratePtr() {                
      m_pO = NULL; }
   //! Construct with key
   CKeyDecoratePtr( KEY key ) {       
      m_KEY = key; m_pO = NULL; }
   //! construct with key and object (object will be deleted in templateclass)
   CKeyDecoratePtr( KEY key, OBJECT* pO ) {
      m_KEY = key; m_pO = pO; }
   //! Copyconstructor
   CKeyDecoratePtr( const CKeyDecoratePtr<OBJECT,KEY>& other ) {   
      m_KEY = other.m_KEY;
      MP_NEW_V( m_pO, Object, *other.m_pO ); }
   
   ~CKeyDecoratePtr() {
		MP_DELETE( m_pO); }

   OBJECT* operator->() {     //<! Pointer operator (returns pointer to object)
      return m_pO; }

   //! Assignemet
   CKeyDecoratePtr<OBJECT,KEY>& operator=(const CKeyDecoratePtr<OBJECT,KEY>& other) {
      MP_DELETE( m_pO);
      MP_NEW_V( m_pO, Object, other);
      m_KEY = other.m_KEY;
      return *this;
   }
   //! <
   bool operator<(const KEY& key) const {                    
      return (m_KEY < key); }
   //! <
   bool operator<(const CKeyDecoratePtr<OBJECT,KEY>& object) const { 
      return (m_KEY < object.m_KEY); }
   //! <=
   bool operator<=(const KEY& key) const {                   
      return (m_KEY <= key); }
   //! <=
   bool operator<=(const CKeyDecoratePtr<OBJECT,KEY>& object) const {
      return (m_KEY <= object.m_KEY); }
   //! >
   bool operator>(const KEY& key) const {                    
      return (m_KEY > key); }
   //! >
   bool operator>(const CKeyDecoratePtr<OBJECT,KEY>& object) const { 
      return (m_KEY > object.m_KEY); }
   //! >=
   bool operator>=(const KEY& key) const {                   
      return (m_KEY >= key); }
   //! >=
   bool operator>=(const CKeyDecoratePtr<OBJECT,KEY>& object) const {
      return (m_KEY >= object.m_KEY); }
   //! ==
   bool operator==(const KEY& key) const {                   
      return (m_KEY == key); }
   //! ==
   bool operator==(const CKeyDecoratePtr<OBJECT,KEY>& object) const {
      return (m_KEY == object.m_KEY); }
   //! !=
   bool operator!=(const KEY& key) const {                   
      return (m_KEY != key); }
   //! !=
   bool operator!=(const CKeyDecoratePtr<OBJECT,KEY>& object) const {
      return (m_KEY != object.m_KEY); }

// Attributes
protected:
   KEY m_KEY;     //!< KEY value used as index
   OBJECT* m_pO;  //!< pointer to object

public:
   //! Key value
   KEY& Key() {                                              
      return m_KEY; }
   //! object pointer
   OBJECT*& Object() {                                       
      return m_pO; }
};

// ****************************************************************************
/*! \brief stores pointers in array 

   CArrayPtr is a tiny wrapper for storing pointers in a array. It can also
   delete that objects that are stored but it does not  MP_DELETE( pointer's when
   array object is destroyed. The object pointers that are stored is not copied
   and that means that if you insert a pointer to array and you modify it later
   the object that is pointed to in array is also modified.
*/
template<class ITEM>
class CArrayPtr
{
// Construction
public: 
   CArrayPtr( UINT uCount = 1 );
   CArrayPtr( const CArrayPtr<ITEM>& other );
   ~CArrayPtr();
   CArrayPtr<ITEM>& operator=( const CArrayPtr<ITEM>& other );
	ITEM*& operator[](UINT uIndex) const;


// Get/Set
public:
   //! Get number of elements in list
   UINT GetSize() const;
   //! Get number of elements in list
   UINT GetCount() const;
   //! Set how many more openings that should be created if we add more then can fit
   void SetGrowBy( UINT uGrowBy );

// Operation
public:
   //! Add item to array
   void  Add( const ITEM* pITEM );
   //! Insert item at index
   void  InsertAt( UINT uIndex, const ITEM* pITEM );
   //! Copy array into this
   void  Copy( const CArrayPtr<ITEM>* pACopyFrom );
   //! Merge another array to this array
   void  Merge( CArrayPtr<ITEM>* pAMerge );
   //! Get item for index
   ITEM* GetAt( UINT uIndex ) const;
   //! Set item at index
   ITEM* SetAt( UINT uIndex, const ITEM* pITEM );
   //! Switch position for two items in array
   void  SwitchItems( UINT uIndex1, UINT uIndex2 );
   //! Replace item in array
   ITEM* ReplaceItem( UINT uIndex, const ITEM* pITEM );
   //! Remove item at index
   ITEM* RemoveAt( UINT uIndex );
   void  Remove( UINT uStart, UINT uCount, bool bDelete );
   //! Attach data from another array
   void  Attach( CArrayPtr<ITEM>* pAFrom ); 
   //! Detach all data and insert data to specified array
   void  Detach( CArrayPtr<ITEM>* pATo ); 
   //! Remove all items fron array but do not delete
   void  RemoveAll();
   //!  delete all items (make sure that all items is allocated with new !)
   void  DeleteItems();

   void CreateEmptySlots( unsigned uCount );

   // CHECK
   //! Check if a pointer exist in array more than once. It it does then return index for the 
   //! duplicate. -1 if no duplicate pointers.
   int CheckDuplicatePointer();

protected:
   void SafeSize( UINT uSlots );
                         
// Attributes
protected:
   UINT m_uCount;    //!< Items in array
   UINT m_uMaxCount; //!< max number of items before we need to realocate
   UINT m_uGrowBy;   //!< how many more valuew that should fit if we need to allocate more space for stack

   ITEM** m_ppITEM;  //!< Pointer to pointers in stack
};

template<class ITEM>
CArrayPtr<ITEM>::CArrayPtr( UINT uCount ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uCount;
   m_ppITEM    = NULL;
}

template<class ITEM>
CArrayPtr<ITEM>::CArrayPtr( const CArrayPtr<ITEM>& other ) {
   Copy( &other );
}

template<class ITEM>
__forceinline CArrayPtr<ITEM>::~CArrayPtr() {
    MP_DELETE_ARR( m_ppITEM);
}

template<class ITEM>
__forceinline CArrayPtr<ITEM>& CArrayPtr<ITEM>::operator=( const CArrayPtr<ITEM>& other ) {
   Copy( &other );
   return *this;
}

/*! index operator to get value from array 
\code
CArrayPtr<CString> AString;
AString.Add( new CString( "first" ) );
AString.Add( new CString( "second" ) );
printf( "%s", (const char*)*AString[0] );
\endcode
*/
template<class ITEM>
__forceinline ITEM*& CArrayPtr<ITEM>::operator[](UINT uIndex) const { 
   _ASSERT( uIndex < m_uCount );
   return m_ppITEM[uIndex]; 
}


template<class ITEM>
__forceinline UINT CArrayPtr<ITEM>::GetSize() const {
   return m_uCount; }

template<class ITEM>
__forceinline UINT CArrayPtr<ITEM>::GetCount() const {
   return m_uCount; }

template<class ITEM>
__forceinline void CArrayPtr<ITEM>::SetGrowBy( UINT uGrowBy ) {
   _ASSERT( (uGrowBy < 0xff000000) && (uGrowBy > 0) ); // realistic
   m_uGrowBy = uGrowBy;
}

template<class ITEM>
void CArrayPtr<ITEM>::Add( const ITEM* pITEM ) {
   SafeSize( m_uCount + 1 );
   m_ppITEM[m_uCount] = const_cast<ITEM*>(pITEM);
   m_uCount++;
}

/**
 * Insert item at specified index
 * \param uIndex  index item is inserted to
 * \param pITEM pointer to item object that is inserted
 */
template<class ITEM>
void CArrayPtr<ITEM>::InsertAt( UINT uIndex, const ITEM* pITEM ) {
   _ASSERT( uIndex <= m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_ppITEM[uIndex+1], &m_ppITEM[uIndex], sizeof(ITEM*) * (m_uCount - uIndex) );
   m_ppITEM[uIndex] = const_cast<ITEM*>(pITEM);
   m_uCount++;
}

template<class ITEM>
void CArrayPtr<ITEM>::Copy( const CArrayPtr<ITEM>* pACopyFrom ) {
   UINT uIndex;
   DeleteItems();
   if( pACopyFrom->m_uCount == 0 ) return;
   SafeSize( pACopyFrom->m_uCount );
   m_uCount = pACopyFrom->m_uCount;
   for( uIndex = 0; uIndex < m_uCount; uIndex++ ) {
      MP_NEW_V( m_ppITEM[uIndex], ITEM, *pACopyFrom->GetAt( uIndex ) );
   }
}

template<class ITEM>
void CArrayPtr<ITEM>::Merge( CArrayPtr<ITEM>* pAMerge ) {
   UINT uCount = pAMerge->m_uCount;
   SafeSize( m_uCount + uCount );
   memmove( &m_ppITEM[m_uCount], pAMerge->m_ppITEM, sizeof(ITEM*) * uCount );
   m_uCount += uCount;
}

template<class ITEM>
__forceinline ITEM* CArrayPtr<ITEM>::GetAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return m_ppITEM[uIndex];
}

template<class ITEM>
ITEM* CArrayPtr<ITEM>::SetAt( UINT uIndex, const ITEM* pITEM ) {
   _ASSERT( uIndex < m_uCount );
   ITEM* pIReturn;
   pIReturn = m_ppITEM[uIndex];
   m_ppITEM[uIndex] = const_cast<ITEM*>(pITEM);
   return pIReturn;
}

template<class ITEM>
void CArrayPtr<ITEM>::SwitchItems( UINT uIndex1, UINT uIndex2 ) {
   _ASSERT( uIndex1 < m_uCount );
   _ASSERT( uIndex2 < m_uCount );
   ITEM* pTemp;
   pTemp = m_ppITEM[uIndex1];
   m_ppITEM[uIndex1] = m_ppITEM[uIndex2];
   m_ppITEM[uIndex2] = pTemp;
}

template<class ITEM>
ITEM* CArrayPtr<ITEM>::ReplaceItem( UINT uIndex, const ITEM* pITEM ) {
   _ASSERT( uIndex < m_uCount );
   ITEM* pTemp;
   pTemp = m_ppITEM[uIndex];
   m_ppITEM[uIndex] = const_cast<ITEM*>(pITEM);
   return pTemp;
}


template<class ITEM>
ITEM* CArrayPtr<ITEM>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   ITEM* pIReturn;
   pIReturn = m_ppITEM[uIndex];
   m_uCount--;
   memmove( &m_ppITEM[uIndex], &m_ppITEM[uIndex+1], sizeof(ITEM*) * (m_uCount - uIndex) );
   return pIReturn;
}

template<class ITEM>
void CArrayPtr<ITEM>::Remove( UINT uStart, UINT uCount, bool bDelete ) {
   _ASSERT( uStart < m_uCount );
   UINT uCounter;
   UINT uEnd;
   uEnd = uStart + uCount;
   if( uEnd > m_uCount ) uEnd = m_uCount;
   if( bDelete == true )
   {
      for( uCounter = uStart; uCounter < uEnd; uCounter++ )
      {
          MP_DELETE( m_ppITEM[uCounter]);
      }
   }
   memmove( &m_ppITEM[uStart], &m_ppITEM[uEnd-1], sizeof(ITEM*) * (m_uCount - uEnd) );
   m_uCount -= (uEnd - uStart);
}

/*! Attach data from another array to this array. Data in this array are removed
\code
CArrayPtr<CObject> AObject;   
CArrayPtr<CObject> AToObject;
AObject.Add( new CObject );
AObject.Add( new CObject );
AObject.Add( new CObject );
AToObject.Attach( &AObject );
// AObject is now empty and AToObject has all objects
\endcode
*/
template<class ITEM>
void CArrayPtr<ITEM>::Attach( CArrayPtr<ITEM>* pAFrom /*!< array data are inserted from*/) {
   MP_DELETE_ARR( m_ppITEM);
   m_uCount = pAFrom->m_uCount;
   m_uMaxCount = pAFrom->m_uMaxCount;
   m_uGrowBy = pAFrom->m_uGrowBy;
   m_ppITEM = pAFrom->m_ppITEM;
   
   pAFrom->m_uCount    = 0;
   pAFrom->m_uMaxCount = 0;
   pAFrom->m_ppITEM    = NULL;
}

/*! Detach all allocated data and insert that to the sent array. Data in array that is
sent are removed before it gets data.
\code
CArrayPtr<CObject> AObject;   
CArrayPtr<CObject> AGetObject;
AObject.Add( new CObject );
AObject.Add( new CObject );
AObject.Add( new CObject );
AObject.Detach( &AGetObject );
// AObject is now empty and AGetObject has all objects
\endcode
*/
template<class ITEM>
void CArrayPtr<ITEM>::Detach( CArrayPtr<ITEM>* pATo /*!< array data are inserted to*/) {
   _ASSERT( pATo != NULL );
   MP_DELETE_ARR( pATo->m_ppITEM);
   pATo->m_uCount = m_uCount;
   pATo->m_uMaxCount = m_uMaxCount;
   pATo->m_uGrowBy = m_uGrowBy;
   pATo->m_ppITEM = m_ppITEM;

   m_uCount    = 0;
   m_uMaxCount = 0;
   m_ppITEM    = NULL;
}

template<class ITEM>
__forceinline void CArrayPtr<ITEM>::RemoveAll() {
   m_uCount = 0;
}

/*! delete all internal objects */
template<class ITEM>
void CArrayPtr<ITEM>::DeleteItems() {
   _ASSERT( m_uCount < 0x1fffffff ); // Fucked up if ASSERT !! (unrealistic value)
   while( m_uCount )
   {
      m_uCount--;
      MP_DELETE( m_ppITEM[m_uCount]);
   }
}

/*! Create slots and set them to NULL. 
\code
CArrayPtr<CObject> AObject;   
AObject.CreateEmptySlots( 10 );
for( int i = 9; i >= 0; i-- )
{
   AObject[i] = new CObject;
}
\endcode
*/
template<class ITEM>
void CArrayPtr<ITEM>::CreateEmptySlots( unsigned uCount /*!< number of empty slots that is created */) {
   SafeSize( m_uCount + uCount );
   m_uCount += uCount;
   ITEM* pITEM = m_ppITEM[m_uCount-1];
   while( uCount-- ) 
   {
      pITEM = NULL;
      pITEM--;
   }
}
                     
// This function checks to se if one pionter exist more than once in array.
// If same pointer is found then it returns index in array for the duplicate.
template<class ITEM>
int CArrayPtr<ITEM>::CheckDuplicatePointer() {
   ITEM* pCheck;
   for( unsigned u = 0; u < m_uCount; u++ )
   {
      pCheck = m_ppITEM[u];
      for( unsigned u1 = 0; u1 < m_uCount; u1++ )
      {
         if( u == u1 ) continue;
         if( pCheck == m_ppITEM[u1] ) return u;
      }
   }
   return -1;
}

template<class ITEM>
void CArrayPtr<ITEM>::SafeSize( UINT uSlots ) {
   ITEM** ppITEM;
   if( uSlots >= m_uMaxCount )
   {
      m_uMaxCount += m_uGrowBy + uSlots;
      ppITEM = MP_NEW_ARR( ITEM*, m_uMaxCount);
      memcpy( ppITEM, m_ppITEM, sizeof(ITEM*) * m_uCount );
      MP_DELETE_ARR( m_ppITEM);
      m_ppITEM = ppITEM;
   }
}

// ****************************************************************************
/*! \brief array that sorts value. very fast to find object based on key
   
   CSortedArrayPtr is a array that has it's items sorted. Items that array
   contains has to have some operators and a key that it is sorted on.
   Operators that is needed are > and < .  These operators has to be working
   on the key and the object.
*/
template<class ITEM, class KEY>
class CSortedArrayPtr : public CArrayPtr<ITEM>
{
// Construction
public: 
   CSortedArrayPtr( UINT uCount = 1 );
   ~CSortedArrayPtr();

// Operation
public:
   //! Add item to array
   bool  Add( const ITEM* pITEM );
   //! Add item last in array
   void  AddLast( const ITEM* pITEM );
   //! Sort array
   void  Sort();
   //! Find object in array
   ITEM* Find( KEY key ) const;
   //! find index to item in array
   int   FindIndex( KEY key ) const;

protected:
};

template<class ITEM, class KEY>
__forceinline CSortedArrayPtr<ITEM, KEY>::CSortedArrayPtr( UINT uCount ) : CArrayPtr<ITEM>( uCount ) {
}

template<class ITEM, class KEY>
__forceinline CSortedArrayPtr<ITEM, KEY>::~CSortedArrayPtr() {
}

/*! Add item to array, item is inserted in it's correct position */
template<class ITEM, class KEY>
bool CSortedArrayPtr<ITEM, KEY>::Add( const ITEM* pITEM ) {
   int iLow, iHigh;
   UINT uMiddle;
   ITEM** ppITEM;

   uMiddle = iLow  = 0;
   iHigh = m_uCount - 1;
   ppITEM = m_ppITEM;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( *ppITEM[uMiddle] > *pITEM )
         iHigh = uMiddle - 1;
      else if( *ppITEM[uMiddle] < *pITEM )
         iLow = uMiddle + 1;
      else
      {
         return false;
      }
   }

   InsertAt( iLow, pITEM );

   return true;
}

/*! add item to last position in array. this doesn't check for the right
    position for the inserted value. */
template<class ITEM, class KEY>
__forceinline void CSortedArrayPtr<ITEM, KEY>::AddLast( const ITEM* pITEM ) {
   CArrayPtr<ITEM>::Add( item );
}

//! sorts objects in array
template<class ITEM, class KEY>
void CSortedArrayPtr<ITEM, KEY>::Sort() {
   UINT uMax, uMin, uPosition, uIndex;
   ITEM** ppITEM;
   ITEM* pITEM;

   uMax = m_uCount;
   ppITEM = m_ppITEM;

   // Use selection-sort
   for( uPosition = 0; uPosition < uMax; uPosition++ )
   {
      uMin = uPosition;
      for( uIndex = uPosition + 1; uIndex < uMax; uIndex++ )
      {
         if( *ppITEM[uIndex] < *ppITEM[uMin] ) uMin = uIndex;
      }

      if( uMin != uPosition )
      {
         pITEM = pITEM[uPosition];
         ppITEM[uPosition] = ppITEM[uMin];
         ppITEM[uMin] = pITEM;
      }
   }
}


template<class ITEM, class KEY>
ITEM* CSortedArrayPtr<ITEM, KEY>::Find( KEY key ) const {
   int iIndex;

   iIndex = FindIndex( key );
   if( iIndex != -1 )
   {
      return m_ppITEM[(UINT)iIndex];
   }

   return NULL;
}

template<class ITEM, class KEY>
int CSortedArrayPtr<ITEM, KEY>::FindIndex( KEY key ) const {
   int  iLow, iHigh;
   UINT uMiddle;
   ITEM** ppITEM;

   iLow  = 0;
   iHigh = m_uCount - 1;
   ppITEM = m_ppITEM;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( *ppITEM[uMiddle] > key )
         iHigh = uMiddle - 1;
      else if( *ppITEM[uMiddle] < key )
         iLow = uMiddle + 1;
      else
         return uMiddle;
   }

   return -1;
}

// ****************************************************************************
/*! \brief array that sorts value. finds objects fast
   
   CMultiSortedArrayPtr is an array that has it's items sorted. Items that array
   contains has to have some operators and a key that it is sorted on.
   Operators that is needed are < .  These operators has to be working
   on the key and the object.
   %CMultiSortedArrayPtr can store multiple objects that has the same key value
*/
template<class ITEM, class KEY>
class CMultiSortedArrayPtr : public CArrayPtr<ITEM>
{
// Construction
public: 
   CMultiSortedArrayPtr( UINT uCount = 1 ) : CArrayPtr<ITEM>( uCount ) { }
   ~CMultiSortedArrayPtr() {}

// Operation
public:
   //! Add item to array
   void Add( const ITEM* pITEM ) {
      if( m_uCount == 0 ) CArrayPtr<ITEM>::Add( pITEM );
      else {
         unsigned uIndex = _Find( pITEM );
         if( *GetAt( uIndex ) < *pITEM ) uIndex++;
         InsertAt( uIndex, pITEM );
      }
   }

   //! find object based on another object of the same typ
   ITEM* Find( const ITEM* pITEM ) const {
      if( m_uCount == 0 ) return NULL;
      unsigned uIndex = _Find( pITEM );
      if( *GetAt( uIndex ) == *pITEM ) return GetAt( uIndex );
      return NULL;
   }

   //! find object based on keyvalue
   ITEM* Find( const KEY& key ) const {
      if( m_uCount == 0 ) return NULL;
      unsigned uIndex = _Find( key );
      if( *GetAt( uIndex ) == key ) return GetAt( uIndex );
      return NULL;
   }

   //! Add item last in array
   void  AddLast( const ITEM* pITEM ) {
      Add( pITEM );
   }

   //! Sort array
   void  Sort() {
      ITEM* pValue;
      ITEM* pPosition, *pPreserve;
      ITEM* pFirst, * pLast;

      pFirst = GetAt( 0 );
      pLast = GetAt( m_uCount - 1 );

      while( pFirst < pLast )
      {
         pValue = pFirst;
         pPosition = pFirst;
         pPosition++;
         while( pPosition < pLast )
         {
            if( *pValue < *pPosition )
            {
               pValue = pPosition;
               pPreserve = pPosition;
            }
            pPosition++;
         }

         if( !(*pFirst == *pValue) )
         {
            pPreserve = pFirst;
            pFirst = pValue;
         }
         pFirst++;
      }
   }

   unsigned _Find( const KEY& key ) const;
   unsigned _Find( const ITEM* pITEM ) const;

protected:
};

template<class ITEM, class KEY>
unsigned CMultiSortedArrayPtr<ITEM, KEY>::_Find( const KEY& key ) const {
   unsigned uSize;
   unsigned uSection;
   unsigned uPosition, uFirst, uLast;
   uFirst = 0;
   uLast = m_uCount - 1;

   uSize = uLast - uFirst;
   while( 0 < uSize )
   {
      uSection = uSize / 2;
      uPosition = uFirst;
      uPosition += uSection;
      if( *GetAt( uPosition ) < key )
      {
         uFirst = ++uPosition;
         uSize -= uSection + 1;
      }
      else
      {
         uSize = uSection;
      }
   }

   return uFirst;
}

template<class ITEM, class KEY>
unsigned CMultiSortedArrayPtr<ITEM, KEY>::_Find( const ITEM* pITEM ) const {
   unsigned uSize;
   unsigned uSection;
   unsigned uPosition, uFirst, uLast;
   uFirst = 0;
   uLast = m_uCount - 1;

   uSize = uLast - uFirst;
   while( 0 < uSize )
   {
      uSection = uSize / 2;
      uPosition = uFirst;
      uPosition += uSection;
      if( *GetAt( uPosition ) < *pITEM )
      {
         uFirst = ++uPosition;
         uSize -= uSection + 1;
      }
      else
      {
         uSize = uSection;
      }
   }

   return uFirst;
}


// ****************************************************************************
/*! \brief stores blocks of data in array format

When storing structs that don't have any internal objects that are pointers or
needs some special destruction or construction. This is effective. All objects
are stored in one big memoryblock and that minimizes allocations. In order to
optimize set the growby value to some number that is max number of block items
stored in object.
*/
template<class BLOCK>
class CBlocks
{
public:
   CBlocks( UINT uCount = 1 );
   CBlocks( const CBlocks<BLOCK>& other );
   ~CBlocks();

   CBlocks<BLOCK>& operator=(const CBlocks<BLOCK>& other);

   //! get reference to block item
	BLOCK& operator[](UINT uIndex) const;
   const CBlocks<BLOCK>& operator+=(const BLOCK& block); 

   //! Get number of elements on stack
   UINT   GetSize() const;
   //! Set size for valid blocks
   void   SetSize( UINT uSize );
   //! Add item to array
   void   Add( BLOCK pBLOCK );
   void   Add( const BLOCK* pBLOCK );
   //! Insert item at index
   void   InsertAt( UINT uIndex, const BLOCK* pBLOCK );
   //! Remove item at index
   void   RemoveAt( UINT uIndex );
   //! Get pointer to block for index
   BLOCK* GetAt( UINT uIndex ) const;
   //! Set block at index
   void   SetAt( UINT uIndex, const BLOCK* pBLOCK );
   //! Add to stack
   void   Push( const BLOCK* pBLOCK );
   //! Remove from stack
   BLOCK* Pop();
   //! return pointer to internal data
   BLOCK* Data();
   //! Clear items
   void   RemoveAll();

protected:
   void   Copy( const CBlocks<BLOCK>& other );
   void   SafeSize( UINT uSlots );

// Attributes
protected:
   UINT m_uCount;    //!< number of blocks
   UINT m_uMaxCount; //!< max number of blocks before we need to realocate
   UINT m_uGrowBy;   //!< how many more block that should fit if we need to allocate more space

   BLOCK* m_pBLOCK;  //!< Pointer to blocks
};

template<class BLOCK>
CBlocks<BLOCK>::CBlocks( UINT uCount ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uCount;
   m_pBLOCK    = NULL;
}

template<class BLOCK>
CBlocks<BLOCK>::CBlocks( const CBlocks<BLOCK>& other ) {
   MP_DELETE_ARR( m_pBLOCK);
   m_uMaxCount = m_uCount = other.m_uCount;
   m_uGrowBy   = other.m_uGrowBy;
   m_pBLOCK    = (BLOCK*)MP_NEW_ARR( BYTE, sizeof(BLOCK) * m_uMaxCount);
   memcpy( m_pBLOCK, other.m_pBLOCK, sizeof(BLOCK) * m_uCount );
}
   
template<class BLOCK>
CBlocks<BLOCK>& CBlocks<BLOCK>::operator=(const CBlocks<BLOCK>& other ) {
   MP_DELETE_ARR( m_pBLOCK);
   m_uMaxCount = m_uCount = other.m_uCount;
   m_uGrowBy   = other.m_uGrowBy;
   m_pBLOCK    = (BLOCK*)MP_NEW_ARR( BYTE, sizeof(BLOCK) * m_uMaxCount);
   memcpy( m_pBLOCK, other.m_pBLOCK, sizeof(BLOCK) * m_uCount );
   return *this;
}

template<class BLOCK>
__forceinline BLOCK& CBlocks<BLOCK>::operator[]( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return m_pBLOCK[uIndex];
}

template<class BLOCK>
__forceinline const CBlocks<BLOCK>& CBlocks<BLOCK>::operator+=(const BLOCK& block) {
   Add( &block );
   return *this;
}

template<class BLOCK>
CBlocks<BLOCK>::~CBlocks() {
    MP_DELETE_ARR( m_pBLOCK);
}

template<class BLOCK>
__forceinline UINT CBlocks<BLOCK>::GetSize() const {
   return m_uCount; }

template<class BLOCK>
__forceinline void CBlocks<BLOCK>::SetSize( UINT uSize ) {
   _ASSERT( uSize <= m_uMaxCount );
   m_uCount = uSize; 
}

template<class BLOCK>
__forceinline void CBlocks<BLOCK>::Add( BLOCK block ) {
   Add( &block );
}

template<class BLOCK>
void CBlocks<BLOCK>::Add( const BLOCK* pBLOCK ) {
   SafeSize( m_uCount + 1 );
   memcpy( &m_pBLOCK[m_uCount], pBLOCK, sizeof( BLOCK ) );
   m_uCount++;
}

template<class BLOCK>
void CBlocks<BLOCK>::InsertAt( UINT uIndex, const BLOCK* pBLOCK ) {
   _ASSERT( uIndex <= m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_pBLOCK[uIndex+1], &m_pBLOCK[uIndex], sizeof(BLOCK) * (m_uCount - uIndex) );
   memcpy( &m_pBLOCK[uIndex], pBLOCK, sizeof( BLOCK ) );
   m_uCount++;
}

template<class BLOCK>
void CBlocks<BLOCK>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   m_uCount--;
   memmove( &m_pBLOCK[uIndex], &m_pBLOCK[uIndex+1], sizeof(BLOCK) * (m_uCount - uIndex) );
}


template<class BLOCK>
__forceinline BLOCK* CBlocks<BLOCK>::GetAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return &m_pBLOCK[uIndex];
}

template<class BLOCK>
__forceinline void CBlocks<BLOCK>::SetAt( UINT uIndex, const BLOCK* pBLOCK ) {
   _ASSERT( uIndex < m_uCount );
   memcpy( &m_pBLOCK[uIndex], pBLOCK, sizeof( BLOCK ) );
}

template<class BLOCK>
__forceinline void CBlocks<BLOCK>::Push( const BLOCK* pBLOCK ) {
   Add( pBLOCK ); 
}

template<class BLOCK>
BLOCK* CBlocks<BLOCK>::Pop() {
   _ASSERT( m_uCount > 0 );
   m_uCount--;
   return &m_pBLOCK[m_uCount];
}

template<class BLOCK>
__forceinline BLOCK* CBlocks<BLOCK>::Data() {
   return m_pBLOCK; }

template<class ITEM>
__forceinline void CBlocks<ITEM>::RemoveAll() {
   m_uCount = 0;
}

template<class BLOCK>
void CBlocks<BLOCK>::Copy( const CBlocks<BLOCK>& other ) {
   SafeSize( other.m_uCount );
   m_uCount = other.m_uCount;
   m_uGrowBy = other.m_uGrowBy;
   if( m_uCount > m_uMaxCount )
   {
      m_uMaxCount = m_uCount;
      MP_DELETE_ARR( m_pBLOCK);
      m_pBLOCK = (BLOCK*) MP_NEW_ARR( BYTE, sizeof(BLOCK) * m_uMaxCount);
   }
   memcpy( m_pBLOCK, other.m_pBLOCK, sizeof(BLOCK) * m_uCount );
}

template<class BLOCK>
void CBlocks<BLOCK>::SafeSize( UINT uBlocks ) {
   BLOCK* pBLOCK;
   if( uBlocks > m_uMaxCount )
   {
      m_uMaxCount += m_uGrowBy + uBlocks;
      pBLOCK = (BLOCK*) MP_NEW_ARR( BYTE, sizeof(BLOCK) * m_uMaxCount);
      memcpy( pBLOCK, m_pBLOCK, sizeof(BLOCK) * m_uCount );
      MP_DELETE_ARR( m_pBLOCK);
      m_pBLOCK = pBLOCK;
   }
}




// ****************************************************************************
/*! \brief object for storing blocks in a sorted array

This works as CBlocks<BLOCK> except that objects are sorted. Is is very fast
to find objects based on key or other objects.
*/
template<class BLOCK, class KEY>
class CSortedBlocks : public CBlocks<BLOCK>
{
// Construction
public: 
   CSortedBlocks( UINT uCount = 1 );

// Operation
public:
   //! Add item to array
   bool  Add( const BLOCK* pBLOCK );
   //! Add item to array
   bool  Add( const BLOCK& BLOCK );
   //! Add item last in array
   void  AddLast( const BLOCK* pBLOCK );
   //! Sort array
   void  Sort();
   //! Find object in array
   BLOCK* Find( KEY key ) const;
   //! find index for object based on key
   int   FindIndex( KEY key ) const;

protected:
};

template<class BLOCK, class KEY>
__forceinline CSortedBlocks<BLOCK, KEY>::CSortedBlocks( UINT uCount ) : CBlocks<BLOCK>( uCount ) {
}

template<class BLOCK, class KEY>
__forceinline bool CSortedBlocks<BLOCK, KEY>::Add( const BLOCK& block ) {
   return Add( &block ); 
}


template<class BLOCK, class KEY>
bool CSortedBlocks<BLOCK, KEY>::Add( const BLOCK* pBAdd ) {
   int iLow, iHigh;
   UINT uMiddle;
   BLOCK* pBLOCK;

   uMiddle = iLow  = 0;
   iHigh = m_uCount - 1;
   pBLOCK = m_pBLOCK;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( pBLOCK[uMiddle] > *pBAdd )
         iHigh = uMiddle - 1;
      else if( pBLOCK[uMiddle] < *pBAdd )
         iLow = uMiddle + 1;
      else
      {
         return false;
      }
   }

   InsertAt( iLow, pBAdd );

   return true;
}

template<class BLOCK, class KEY>
__forceinline void CSortedBlocks<BLOCK, KEY>::AddLast( const BLOCK* pBLOCK ) {
   CBlocks<BLOCK>::Add( pBLOCK );
}

/*! If items in CSortedBlocks<BLOCK, KEY> isn't sorted then this function could be used
    to sort the blocks. It uses selection sort in order to get each value at the right
    position.
\code
CSortedBlocks<double,double> SBDouble;
SBDouble.AddLast( 0.2 );
SBDouble.AddLast( 3.0 );
SBDouble.AddLast( 4.4 );
SBDouble.AddLast( 1.2 );
SBDouble.Sort();
\endcode
*/
template<class BLOCK, class KEY>
void CSortedBlocks<BLOCK, KEY>::Sort() {
   UINT uMax, uMin, uPosition, uIndex;
   BLOCK* pBLOCK;
   BLOCK block;

   uMax = m_uCount;
   pBLOCK = m_pBLOCK;

   // Use selection-sort
   for( uPosition = 0; uPosition < uMax; uPosition++ )
   {
      uMin = uPosition;
      for( uIndex = uPosition + 1; uIndex < uMax; uIndex++ )
      {
         if( pBLOCK[uIndex] < pBLOCK[uMin] ) uMin = uIndex;
      }

      if( uMin != uPosition )
      {
         block = pBLOCK[uPosition];
         pBLOCK[uPosition] = pBLOCK[uMin];
         pBLOCK[uMin] = block;
      }
   }
}

//! find pointer to block based on key value
template<class BLOCK, class KEY>
BLOCK* CSortedBlocks<BLOCK, KEY>::Find( KEY key ) const {
   int iIndex;

   iIndex = FindIndex( key );
   if( iIndex != -1 )
   {
      return &m_pBLOCK[(UINT)iIndex];
   }

   return NULL;
}

//! find index for block based on key value
template<class BLOCK, class KEY>
int CSortedBlocks<BLOCK, KEY>::FindIndex( KEY key ) const {
   int  iLow, iHigh;
   UINT uMiddle;
   BLOCK* pBLOCK;

   iLow  = 0;
   iHigh = m_uCount - 1;
   pBLOCK = m_pBLOCK;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( pBLOCK[uMiddle] > key )
         iHigh = uMiddle - 1;
      else if( pBLOCK[uMiddle] < key )
         iLow = uMiddle + 1;
      else
         return uMiddle;
   }

   return -1;
}

// ****************************************************************************
/* \class CMultiSortedBlocks 
   \brief object for storing blocks in a sorted array

This works as CBlocks<BLOCK> except that objects are sorted. Is is very fast
to find objects based on key or other objects. It can store values that has
same keyvalue. less than (<) operator needs to be implemented
*/
template<class BLOCK, class KEY>
class CMultiSortedBlocks : public CBlocks<BLOCK>
{
// Construction
public: 
   CMultiSortedBlocks( UINT uCount = 1 ) throw() : CBlocks<BLOCK>( uCount ) { }
   ~CMultiSortedBlocks() {}

// Operation
public:
   //! Add item to blocks
   void Add( const BLOCK* pBLOCK ) throw() {
      if( m_uCount == 0 ) CBlocks<BLOCK>::Add( pBLOCK );
      else {
         unsigned uIndex = _Find( pBLOCK );
         if( *GetAt( uIndex ) < *pBLOCK ) uIndex++;
         InsertAt( uIndex, pBLOCK );
      }
   }

   BLOCK* Find( const BLOCK* pBLOCK ) const throw() {
      if( m_uCount == 0 ) return NULL;
      unsigned uIndex = _Find( pBLOCK );
      if( *GetAt( uIndex ) == *pBLOCK ) return GetAt( uIndex );
      return NULL;
   }

   BLOCK* Find( const KEY& key ) const throw() {
      if( m_uCount == 0 ) return NULL;
      unsigned uIndex = _Find( key );
      if( *GetAt( uIndex ) == key ) return GetAt( uIndex );
      return NULL;
   }

   //! Add BLOCK last in array
   void  AddLast( const BLOCK* pBLOCK ) throw() {
      Add( pBLOCK );
   }

   //! Sort blocks
   void  Sort();

   unsigned _Find( const KEY& key ) const;
   unsigned _Find( const BLOCK* pBLOCK ) const;

protected:
};

/*! If items in CSortedBlocks<BLOCK, KEY> isn't sorted then this function could be used
    to sort the blocks. It uses selection sort in order to get each value at the right
    position.
\code
CSortedBlocks<double,double> SBDouble;
SBDouble.AddLast( 0.2 );
SBDouble.AddLast( 3.0 );
SBDouble.AddLast( 4.4 );
SBDouble.AddLast( 1.2 );
SBDouble.Sort();
\endcode
*/
template<class BLOCK, class KEY>
void CMultiSortedBlocks<BLOCK, KEY>::Sort() {
   UINT uMax, uMin, uPosition, uIndex;
   BLOCK* pBLOCK;
   BLOCK block;

   uMax = m_uCount;
   pBLOCK = m_pBLOCK;

   // Use selection-sort
   for( uPosition = 0; uPosition < uMax; uPosition++ )
   {
      uMin = uPosition;
      for( uIndex = uPosition + 1; uIndex < uMax; uIndex++ )
      {
         if( pBLOCK[uIndex] < pBLOCK[uMin] ) uMin = uIndex;
      }

      if( uMin != uPosition )
      {
         block = pBLOCK[uPosition];
         pBLOCK[uPosition] = pBLOCK[uMin];
         pBLOCK[uMin] = block;
      }
   }
}

template<class BLOCK, class KEY>
unsigned CMultiSortedBlocks<BLOCK, KEY>::_Find( const KEY& key ) const {
   unsigned uSize;
   unsigned uSection;
   unsigned uPosition, uFirst, uLast;
   uFirst = 0;
   uLast = m_uCount - 1;

   uSize = uLast - uFirst;
   while( 0 < uSize )
   {
      uSection = uSize / 2;
      uPosition = uFirst;
      uPosition += uSection;
      if( *GetAt( uPosition ) < key )
      {
         uFirst = ++uPosition;
         uSize -= uSection + 1;
      }
      else
      {
         uSize = uSection;
      }
   }

   return uFirst;
}

template<class BLOCK, class KEY>
unsigned CMultiSortedBlocks<BLOCK, KEY>::_Find( const BLOCK* pBLOCK ) const {
   unsigned uSize;
   unsigned uSection;
   unsigned uPosition, uFirst, uLast;
   uFirst = 0;
   uLast = m_uCount - 1;

   uSize = uLast - uFirst;
   while( 0 < uSize )
   {
      uSection = uSize / 2;
      uPosition = uFirst;
      uPosition += uSection;
      if( *GetAt( uPosition ) < *pBLOCK )
      {
         uFirst = ++uPosition;
         uSize -= uSection + 1;
      }
      else
      {
         uSize = uSection;
      }
   }

   return uFirst;
}



// ****************************************************************************
/*
*/
template<class ITEM>
class CDListPtr
{
private:
   struct Node
   {
      Node* pPrevious;
      Node* pNext;
      ITEM* pITEM;
   };

public:
   CDListPtr();

public:
   //! add element at first position in list
   void* AddHead( ITEM* pITEM );
   //! add item last in list
   void* AddTail( ITEM* pITEM );
   //! insert element after specified position
   void* InsertAfter( void* pNode, ITEM* pITEM );
   // return first node in list
   void* GetFirst();
   // return last node in list
   void* GetLast();
   //! get next item in list and increase the node pointer to next node
   ITEM* GetNext( void*& pNode );
   //! get previous item in list and decrease the node pointer to previous node
   ITEM* GetPrevious( void*& pNode );
   //! return item for specified node value
   ITEM* GetItem( void* pNode );
   //! remove specified node and return item for the node
   ITEM* Remove( void* pNode );
   //
   void  RemoveAll();
   //
   void  DeleteAll();

private:
	Node* NewNode( Node* pPrevious, Node* pNext ){
		Node* pNode;
		pNode = MP_NEW( Node);
		pNode->pPrevious = pPrevious;
		pNode->pNext = pNext;
		return pNode;
	}

private:
   UINT  m_uCount;
   Node* m_pFirst;
   Node* m_pLast;
};

template<class ITEM>
CDListPtr<ITEM>::CDListPtr() {
   m_uCount = 0; m_pFirst = NULL; m_pLast = NULL; }

template<class ITEM>
void* CDListPtr<ITEM>::AddHead( ITEM* pITEM ) {
   Node* pNode;
   pNode = NewNode( NULL, m_pFirst );
   pNode->pITEM = pITEM;

   if( m_pFirst != NULL )
   {
      m_pFirst->pPrevious = pNode;
   }
   else
   {
      m_pLast = pNode;
   }
   m_pFirst = pNode;

   return (void*)pNode;
}

template<class ITEM>
void* CDListPtr<ITEM>::AddTail( ITEM* pITEM ) {
   Node* pNode;
   pNode = NewNode( m_pLast, NULL );
   pNode->pITEM = pITEM;

   if( m_pLast != NULL )
   {
      m_pLast->pNext = pNode;
   }
   else
   {
      m_pFirst = pNode;
   }
   m_pLast = pNode;

   return (void*)pNode;
}

/*! insert node after specified node*/
template<class ITEM>
void* CDListPtr<ITEM>::InsertAfter( void* pNode, ITEM* pITEM ) {
   _ASSERT( pNode != NULL );
   Node* pNNew;
   pNNew = NewNode( (CDListPtr<ITEM>::Node*)pNode, ((CDListPtr<ITEM>::Node*)pNode)->pNext );
   pNNew->pITEM = pITEM;
   if( ((CDListPtr<ITEM>::Node*)pNode)->pNext != NULL )
   {
      ((CDListPtr<ITEM>::Node*)pNode)->pNext->pPrevious = pNNew;
   }
   else
   {
      m_pLast = pNNew;
   }

   ((CDListPtr<ITEM>::Node*)pNode)->pNext = pNNew;

   return pNNew;
}

template<class ITEM>
__forceinline void* CDListPtr<ITEM>::GetFirst() {
   return m_pFirst; }

template<class ITEM>
__forceinline void* CDListPtr<ITEM>::GetLast() {
   return m_pLast; }

/*! get item for node en move to next node in list */
template<class ITEM>
__forceinline ITEM* CDListPtr<ITEM>::GetNext( void*& pNode ) {
   _ASSERT( pNode != NULL );
   Node* pN;
   pN = (CDListPtr<ITEM>::Node*)pNode;
   pNode = pN->pNext;
   return pN->pITEM; 
}

/*! get item for node en move to previus node in list */
template<class ITEM>
__forceinline ITEM* CDListPtr<ITEM>::GetPrevious( void*& pNode ) {
   _ASSERT( pNode != NULL );
   Node* pN;
   pN = (CDListPtr<ITEM>::Node*)pNode;
   pNode = pN->pPrevious;
   return pN->pITEM; 
}

/*! return item for specified node */
template<class ITEM>
__forceinline ITEM* CDListPtr<ITEM>::GetItem( void* pNode ) {
   _ASSERT( pNode != NULL );
   return ((CDListPtr<ITEM>::Node*)pNode)->pITEM;
}

/*! remove node in list and return the item removed node was holding */
template<class ITEM>
ITEM* CDListPtr<ITEM>::Remove( void* pNode )
{
   _ASSERT( pNode != NULL );

   ITEM* pITEM = ((CDListPtr<ITEM>::Node*)pNode)->pITEM;

   if( m_pFirst == (CDListPtr<ITEM>::Node*)pNode )
   {
      m_pFirst = ((CDListPtr<ITEM>::Node*)pNode)->pNext;
   }
   else
   {
      ((CDListPtr<ITEM>::Node*)pNode)->pPrevious->pNext = ((CDListPtr<ITEM>::Node*)pNode)->pNext;
   }

   if( m_pLast == (CDListPtr<ITEM>::Node*)pNode )
   {
      m_pLast = ((CDListPtr<ITEM>::Node*)pNode)->pPrevious;
   }
   else
   {
      ((CDListPtr<ITEM>::Node*)pNode)->pNext->pPrevious = ((CDListPtr<ITEM>::Node*)pNode)->pPrevious;
   }

   MP_DELETE( pNode);

   return pITEM;
}

template<class ITEM>
void CDListPtr<ITEM>::RemoveAll() {
   Node* pNode;
   pNode = m_pFirst;

   while( pNode != NULL )
   {
      MP_DELETE( pNode->pPrevious);
      pNode = pNode->pNext;
   }

   m_pLast = NULL;
   m_pFirst = NULL;
}

template<class ITEM>
void CDListPtr<ITEM>::DeleteAll() {
   Node* pNode;
   pNode = m_pFirst;

   while( pNode != NULL )
   {
      MP_DELETE( pNode->pPrevious);
      MP_DELETE( pNode->pITEM);
      pNode = pNode->pNext;
   }
   m_pLast = NULL;
   m_pFirst = NULL;
}

// ****************************************************************************
/* \class CBuffer
   \brief buffer for storing items in array that ends with some special.

   CBuffer is a template class that can be used to save a specified type in a
   buffer. It has functionality for working with the buffer. You can for 
   example use it for mangage text (char's or short's).
*/
template<class TYPE, TYPE end>
class CBuffer
{
public:
   CBuffer(); //!< Default Constructor
   CBuffer( UINT uGrowBy ); //!< Sets the number of items that buffer is going to grow when it needs to allocate more
   CBuffer( TYPE* pTYPE ); //!< Constructor that takes data and inserts it
   CBuffer( TYPE* pTYPE, UINT uMaxLength ); //!< Constructor that takes data and inserts it, but not more data than max length
   CBuffer( const CBuffer& other ); //!< Copy contructor
   ~CBuffer();

   // Operators
   //! assign internal buffer from another object of same type
   const CBuffer& operator=(const CBuffer& other); 
   //! assign internal buffer from another buffer
   const CBuffer& operator=(const TYPE* pTYPE); 
   operator const TYPE*();
   operator TYPE*();
	TYPE& operator[](UINT uIndex) const;
   //! add one item to internal buffer
   const CBuffer& operator+=(TYPE type); 
   //! add items in buffer to internal buffer
   const CBuffer& operator+=(TYPE* pTYPE); 
   const CBuffer& operator+=(const TYPE* pTYPE); 

   // Copy object
   void Copy( const CBuffer& other );

   //! Get size for buffer (how many items)
   UINT GetSize() const;
   //! Get size for the internal buffer (items that could fit before new allocation is done)
   UINT GetBufferSize();
   //! Set how much array should grow if reallocation is done
   void SetGrowBy( UINT uGrow );
   //! Sets the length for buffer (WARNING!!! Be sure that you know what you are doing)
   void SetLength( UINT uLength );
   //! If buffer isn't as long as size the buffer is increased
   void SafeBufferSize( UINT uSize );

   //! Add item to buffer (inserted last)
   void  Add( TYPE type );
   //! Add items to buffer (inserted last)
   void  Add( const TYPE* pTYPE );
   //! Add lenght number of items to internal buffer 
   void  Add( const TYPE* pTYPE, UINT uLength );
   //! Get item at specified index
   TYPE  GetAt( UINT uIndex ) const;
   //! Set type at specified position
   void  SetAt( UINT uIndex, TYPE type );
   //! Insert type at specified index
   void  InsertAt( UINT uIndex, TYPE type );
   //! Inject items at specified position
   void  Inject( UINT uIndex, const TYPE* pTYPE );
   //! Remove item for specified index
   void  RemoveAt( UINT uIndex );
   //! Return the last item in buffer
   TYPE  Peek();
   //! Remove and return the last item in buffer
   TYPE  Pop();
   //! Find item in buffer and return index to it
   int   Find( TYPE type ) const;
   int   Find( TYPE* pTYPE ) const;
   //! Return pointer to buffer that holds all data
   TYPE* Data();
   //! Makes a copy of the internal buffer
   TYPE* Copy();
   //! Recalculate the length for buffer and update it's internal state
   void  ReleaseBuffer();
   //! recalculates the length for the buffer from end of current length
   void  RecalculateLengthFromEnd();
   //! Grows the buffer from current length and returns last position
   TYPE* Grow( UINT uLength );
   //! Free memory
   void  FreeMemory();
   //! Clear buffer (same as setting length to 0)
   void  Empty();
   //! Attach data to buffer
   void  Attach( TYPE* pTYPE );
   //! Detaches buffer and clear all members (doesn't  MP_DELETE( data)
   TYPE* Detach();

protected:
   void SafeSize( UINT uLength );

// Attributes
private:
   UINT m_uCount;    //!< Items in array
   UINT m_uMaxCount; //!< max number of items before we need to realocate
   UINT m_uGrowBy;   //!< how many more valuew that should fit if we need to allocate more space for stack

   TYPE* m_pTYPE;    //!< Buffer

   static TYPE m_TEnd;
};

template<class TYPE, TYPE end>
TYPE CBuffer<TYPE,end>::m_TEnd = end;

template<class TYPE, TYPE end>
CBuffer<TYPE,end>::CBuffer() {
   memset( this, 0, sizeof(CBuffer) );
   m_pTYPE = &CBuffer<TYPE,end>::m_TEnd;
}

template<class TYPE, TYPE end>
CBuffer<TYPE,end>::CBuffer( UINT uGrowBy ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uGrowBy;
   m_pTYPE     = &CBuffer<TYPE,end>::m_TEnd;
}

template<class TYPE, TYPE end>
CBuffer<TYPE,end>::CBuffer( TYPE* pTYPE ) {
   UINT uLength = 0;
   while( pTYPE[uLength] != CBuffer<TYPE,end>::m_TEnd ) uLength++;
   m_pTYPE = MP_NEW_ARR( TYPE, uLength);
   memcpy( m_pTYPE, pTYPE, sizeof(TYPE) * uLength );
   m_uCount = m_uMaxCount = uLength;
   m_uGrowBy = 0;
}

template<class TYPE, TYPE end>
CBuffer<TYPE,end>::CBuffer( TYPE* pTYPE, UINT uMaxLength ) {
   UINT uCheck = 0;
   while( (uCheck < uMaxLength) && (pTYPE[uCheck] != CBuffer<TYPE,end>::m_TEnd) ) uCheck++;
   if( uCheck == 0 )
   {
      m_pTYPE = &CBuffer<TYPE,end>::m_TEnd;
   }
   else
   {
      m_pTYPE = MP_NEW_ARR( TYPE, uCheck + 1);
      memcpy( m_pTYPE, pTYPE, sizeof(TYPE) * uCheck );
      m_pTYPE[uCheck] = CBuffer<TYPE,end>::m_TEnd;
   }
   m_uCount = m_uMaxCount = uCheck;
   m_uGrowBy = 0;
}

template<class TYPE, TYPE end>
CBuffer<TYPE,end>::CBuffer( const CBuffer<TYPE,end>& other ) {
   m_pTYPE = NULL;
   Copy( other );
}

template<class TYPE, TYPE end>
const CBuffer<TYPE,end>& CBuffer<TYPE,end>::operator=(const CBuffer& other) {
   Copy( other ); return *this;
}

template<class TYPE, TYPE end>
const CBuffer<TYPE,end>& CBuffer<TYPE,end>::operator=(const TYPE* pTYPE) {
   _ASSERT( pTYPE != NULL );
   UINT uLength = 0;
   m_uCount = 0;
   while( pTYPE[uLength++] != CBuffer<TYPE,end>::m_TEnd);
   SafeSize( uLength );
   uLength--;
   memcpy( m_pTYPE, pTYPE, sizeof(TYPE) * uLength );
   m_pTYPE[uLength] = CBuffer<TYPE,end>::m_TEnd;
   m_uCount = uLength;
   return *this;
}


template<class TYPE, TYPE end>
__forceinline CBuffer<TYPE,end>::~CBuffer() {
   if( m_pTYPE != &CBuffer<TYPE,end>::m_TEnd ) MP_DELETE_ARR( m_pTYPE);
}

template<class TYPE, TYPE end>
__forceinline CBuffer<TYPE,end>::operator const TYPE*() {
   return m_pTYPE; }

template<class TYPE, TYPE end>
__forceinline CBuffer<TYPE,end>::operator TYPE*() {
   return m_pTYPE; }

template<class TYPE, TYPE end>
__forceinline TYPE& CBuffer<TYPE,end>::operator[](UINT uIndex) const	{ 
   _ASSERT( uIndex < m_uCount );
   return m_pTYPE[uIndex]; 
}

template<class TYPE, TYPE end>
__forceinline const CBuffer<TYPE,end>& CBuffer<TYPE,end>::operator+=(const TYPE type) {
   Add( type ); return *this;
}

template<class TYPE, TYPE end>
__forceinline const CBuffer<TYPE,end>& CBuffer<TYPE,end>::operator+=(TYPE* pTYPE) {
   Add( pTYPE ); return *this;
}

template<class TYPE, TYPE end>
__forceinline const CBuffer<TYPE,end>& CBuffer<TYPE,end>::operator+=(const TYPE* pTYPE) {
   Add( pTYPE ); return *this;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Copy( const CBuffer<TYPE,end>& other ) {
   if( m_pTYPE != &CBuffer<TYPE,end>::m_TEnd ) MP_DELETE_ARR( m_pTYPE);
   m_uCount = other.m_uCount;
   m_uMaxCount = m_uCount + 1;
   m_uGrowBy = other.m_uGrowBy;
   if( m_uCount > 0 )
   {
      m_pTYPE = MP_NEW_ARR( type, m_uMaxCount);
      memcpy( m_pTYPE, other.m_pTYPE, sizeof(TYPE) * (m_uCount + 1) );
   }
   else
   {
      m_uMaxCount = 0;
      m_pTYPE = &CBuffer<TYPE,end>::m_TEnd;
   }
}

template<class TYPE, TYPE end>
__forceinline UINT CBuffer<TYPE,end>::GetSize() const {
   return m_uCount; 
}

template<class TYPE, TYPE end>
__forceinline UINT CBuffer<TYPE,end>::GetBufferSize() {
   return m_uMaxCount; 
}

template<class TYPE, TYPE end>
__forceinline void CBuffer<TYPE,end>::SetGrowBy( UINT uGrow ) {
   m_uGrowBy = uGrow;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::SetLength( UINT uLength ) {
   _ASSERT( uLength == 0 || uLength < m_uMaxCount );
   m_pTYPE[uLength] = CBuffer<TYPE,end>::m_TEnd;
   m_uCount = uLength;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::SafeBufferSize( UINT uSize ) {
   if( m_uMaxCount <= uSize ) SafeSize( uSize + 1 );
}


template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Add( TYPE type ) {
   SafeSize( m_uCount + 2 );
   m_pTYPE[m_uCount] = type;
   m_uCount++;
   m_pTYPE[m_uCount] = CBuffer<TYPE,end>::m_TEnd;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Add( const TYPE* pTYPE ) {
   UINT uLength = 0;
   while( pTYPE[uLength++] != CBuffer<TYPE,end>::m_TEnd);
   SafeSize( uLength + m_uCount );
   memcpy( &m_pTYPE[m_uCount], pTYPE, sizeof(TYPE) * uLength );
   m_uCount += uLength - 1;
   m_pTYPE[m_uCount] = CBuffer<TYPE,end>::m_TEnd;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Add( const TYPE* pTYPE, UINT uMaxLength ) {
   UINT uLength = 0;
   while( (pTYPE[uLength] != CBuffer<TYPE,end>::m_TEnd) && (uLength <= uMaxLength) ) uLength++;
   SafeSize( uLength + m_uCount );
   memcpy( &m_pTYPE[m_uCount], pTYPE, sizeof(TYPE) * uLength );
   m_uCount += uLength - 1;
   m_pTYPE[m_uCount] = CBuffer<TYPE,end>::m_TEnd;
}

template<class TYPE, TYPE end>
__forceinline TYPE CBuffer<TYPE,end>::GetAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return m_pTYPE[uIndex]; 
}

template<class TYPE, TYPE end>
__forceinline void CBuffer<TYPE,end>::SetAt( UINT uIndex, TYPE type ) {
   _ASSERT( uIndex < m_uCount );
   m_pTYPE[uIndex] = type;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::InsertAt( UINT uIndex, TYPE type ) {
   _ASSERT( uIndex <= m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_pTYPE[uIndex+1], &m_pTYPE[uIndex], sizeof(TYPE) * (m_uCount - uIndex) );
   m_pTYPE[uIndex] = type;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Inject( UINT uIndex, const TYPE* pTYPE ) {
   _ASSERT( uIndex <= m_uCount );
   UINT uLength = 0;
   while( pTYPE[uLength] != CBuffer<TYPE,end>::m_TEnd) uLength++;
   SafeSize( uLength + m_uCount );
   memmove( &m_pTYPE[uIndex + uLength], &m_pTYPE[uIndex], m_uCount - uIndex + 1 );
   memcpy( &m_pTYPE[uIndex], pTYPE, uLength );
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   m_uCount--;
   memmove( &m_pTYPE[uIndex], &m_pTYPE[uIndex+1], sizeof(TYPE) * (m_uCount - uIndex) );
}

template<class TYPE, TYPE end>
__forceinline TYPE CBuffer<TYPE,end>::Peek() {
   _ASSERT( m_uCount > 0 );
   return m_pTYPE[m_uCount - 1];
}

template<class TYPE, TYPE end>
TYPE CBuffer<TYPE,end>::Pop() {
   _ASSERT( m_uCount > 0 );
   TYPE type;
   m_uCount--;
   type = m_pTYPE[m_uCount];
   m_pTYPE[m_uCount] = CBuffer<TYPE,end>::m_TEnd;
   return type;
}

template<class TYPE, TYPE end>
int CBuffer<TYPE,end>::Find( TYPE type ) const {
   UINT uCount;
   UINT uIndex;
   uCount = m_uCount;
   for( uIndex = 0; uIndex < uCount; uIndex++ )
   {
      if( m_pTYPE[uIndex] == type ) return uIndex;
   }
   return -1;
}

template<class TYPE, TYPE end>
int CBuffer<TYPE,end>::Find( TYPE* pTYPE ) const {
   UINT uIndex = 0;
   UINT uCount = m_uCount;
   UINT uCheck;
   TYPE* pCheck;
   for( uIndex = 0; uIndex < uCount; uIndex++ )
   {
      if( m_pTYPE[uIndex] == pTYPE[0] )
      {
         uCheck = 1;
         pCheck = &m_pTYPE[uIndex];
         while( (pTYPE[uCheck] != CBuffer<TYPE,end>::m_TEnd) && (pCheck[uCheck] == pTYPE[uCheck]) ) uCheck++;
         if( pTYPE[uCheck] == CBuffer<TYPE,end>::m_TEnd ) return uCheck;
      }
   }
   return -1;
}

template<class TYPE, TYPE end>
__forceinline TYPE* CBuffer<TYPE,end>::Data() {
   return m_pTYPE; 
}

template<class TYPE, TYPE end>
__forceinline TYPE* CBuffer<TYPE,end>::Copy() {
   long lIndex;
   TYPE* pTYPE,* pT;
   lIndex = m_uCount;
   pT = m_pTYPE;
   pTYPE = MP_NEW_ARR( TYPE, lIndex+1);
   while( lIndex >= 0 )
   {
      pTYPE[lIndex] = pT[lIndex];
      lIndex--;
   }

   return pTYPE;
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::ReleaseBuffer() {
   UINT uCount = 0;
   TYPE* pTYPE = m_pTYPE;
   while( pTYPE[uCount] != CBuffer<TYPE,end>::m_TEnd ) uCount++;
   m_uCount = uCount;
}

template<class TYPE, TYPE end>
__forceinline void CBuffer<TYPE,end>::RecalculateLengthFromEnd() {
   TYPE* pTYPE = m_pTYPE;
   while( pTYPE[m_uCount] != CBuffer<TYPE,end>::m_TEnd ) m_uCount++;
}
   

template<class TYPE, TYPE end>
__forceinline void CBuffer<TYPE,end>::Empty() {
   SetLength( 0 );
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::Attach( TYPE* pTYPE ) {
   UINT uCount = 0;
   while( pTYPE[uCount] != end ) uCount++;
   m_uMaxCount = m_uCount = uCount;
   if( m_pTYPE != &CBuffer<TYPE,end>::m_TEnd ) MP_DELETE_ARR( m_pTYPE);
   m_pTYPE = pTYPE; 
}

template<class TYPE, TYPE end>
TYPE* CBuffer<TYPE,end>::Detach() {
   TYPE* pTYPE;
   pTYPE = (m_pTYPE == &CBuffer<TYPE,end>::m_TEnd) ?  NULL : m_pTYPE;
   m_uMaxCount = m_uCount = 0;
   m_pTYPE = &CBuffer<TYPE,end>::m_TEnd;
   return pTYPE;
}

template<class TYPE, TYPE end>
TYPE* CBuffer<TYPE,end>::Grow( UINT uLength ) {
   SafeSize( m_uCount + uLength );
   return &m_pTYPE[m_uCount];
}

template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::FreeMemory() {
   if( m_pTYPE != &CBuffer<TYPE,end>::m_TEnd ) MP_DELETE_ARR( m_pTYPE);
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_pTYPE     = &CBuffer<TYPE,end>::m_TEnd;
}


template<class TYPE, TYPE end>
void CBuffer<TYPE,end>::SafeSize( UINT uLength ) {
   TYPE* pTYPE;
   if( uLength >= m_uMaxCount )
   {
      m_uMaxCount = m_uGrowBy + uLength;
      pTYPE = MP_NEW_ARR( TYPE, m_uMaxCount);
      memcpy( pTYPE, m_pTYPE, sizeof(TYPE) * m_uCount );
      if( m_pTYPE != &CBuffer<TYPE,end>::m_TEnd ) MP_DELETE_ARR( m_pTYPE);
      m_pTYPE = pTYPE;
   }
}


// ****************************************************************************
/* 
*/
template<class TYPE>
class CItems
{
public:
   CItems(); // Default Constructor
   CItems( UINT uGrowBy ); // Sets the number of items that buffer is going to grow when it needs to allocate more
   CItems( const CItems& other ); // Copy contructor
   ~CItems();

   // Operators
   const CItems& operator=(const CItems& other); 
   operator const TYPE*();
	TYPE* operator[](UINT uIndex) const;
   const CItems& operator+=(TYPE type); 

   // Copy object
   void Copy( const CItems& other );

   // Get size for buffer (how many items)
   UINT GetSize() const;
   // Get size for the internal buffer (items that could fit before new allocation is done)
   UINT GetBufferSize();
   // Set how much array should grow if reallocation is done
   void SetGrowBy( UINT uGrow );
   // Sets the length for buffer (WARNING!!! Be sure that you know what you are doing)
   void SetLength( UINT uLength );
   // If buffer isn't as long as size the buffer is increased
   void SafeBufferSize( UINT uSize );

   // Add item to buffer (inserted last)
   void  Add( TYPE type );
   // Get item at specified index
   TYPE* GetAt( UINT uIndex ) const;
   // Set type at specified position
   void  SetAt( UINT uIndex, TYPE type );
   // Insert type at specified index
   void  InsertAt( UINT uIndex, const TYPE& type );
   // Remove item for specified index
   void  RemoveAt( UINT uIndex );
   // Find item in buffer and return index to it
   int   Find( TYPE type );
   // Return pointer to buffer that holds all data
   TYPE* Data();
   // Grows the buffer from current length and returns last position
   TYPE* Grow( UINT uLength );
   // Clear buffer (same as setting length to 0)
   void  Empty();
   // Detaches buffer and clear all members (doesn't  MP_DELETE( data)
   TYPE* Detach();

protected:
   void SafeSize( UINT uLength );

// Attributes
private:
   UINT m_uCount;    // Items in array
   UINT m_uMaxCount; // max number of items before we need to realocate
   UINT m_uGrowBy;   // how many more valuew that should fit if we need to allocate more space for stack

   TYPE* m_pTYPE;    // Buffer
};


template<class TYPE>
CItems<TYPE>::CItems() {
   memset( this, 0, sizeof(CItems) );
}

template<class TYPE>
CItems<TYPE>::CItems( UINT uGrowBy ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uGrowBy;
   m_pTYPE     = NULL;
}

template<class TYPE>
CItems<TYPE>::CItems( const CItems<TYPE>& other ) {
   m_pTYPE = NULL;
   Copy( other );
}

template<class TYPE>
const CItems<TYPE>& CItems<TYPE>::operator=(const CItems& other) {
   Copy( other ); return *this;
}


template<class TYPE>
__forceinline CItems<TYPE>::~CItems() {
   MP_DELETE_ARR( m_pTYPE);
}

template<class TYPE>
__forceinline CItems<TYPE>::operator const TYPE*() {
   return m_pTYPE; }

template<class TYPE>
__forceinline TYPE* CItems<TYPE>::operator[](UINT uIndex) const	{ 
   return GetAt( uIndex ); 
}

template<class TYPE>
__forceinline const CItems<TYPE>& CItems<TYPE>::operator+=(TYPE type) {
   Add( type ); return *this;
}

template<class TYPE>
void CItems<TYPE>::Copy( const CItems<TYPE>& other ) {
   MP_DELETE_ARR( m_pTYPE);
   m_uCount = other.m_uCount;
   m_uMaxCount = m_uCount + 1;
   m_uGrowBy = other.m_uGrowBy;
   if( m_uCount > 0 )
   {
      m_pTYPE = MP_NEW_ARR( TYPE, m_uMaxCount);
      memcpy( m_pTYPE, other.m_pTYPE, sizeof(TYPE) * (m_uCount) );
   }
   else
   {
      m_uMaxCount = 0;
      m_pTYPE = NULL;
   }
}

template<class TYPE>
__forceinline UINT CItems<TYPE>::GetSize() const {
   return m_uCount; 
}

template<class TYPE>
__forceinline UINT CItems<TYPE>::GetBufferSize() {
   return m_uMaxCount; 
}

template<class TYPE>
__forceinline void CItems<TYPE>::SetGrowBy( UINT uGrow ) {
   m_uGrowBy = uGrow;
}

template<class TYPE>
void CItems<TYPE>::SetLength( UINT uLength ) {
   _ASSERT( (uLength == 0) || (uLength < m_uMaxCount) );
   m_uCount = uLength;
}

template<class TYPE>
void CItems<TYPE>::SafeBufferSize( UINT uSize ) {
   if( m_uMaxCount <= uSize ) SafeSize( uSize + 1 );
}


template<class TYPE>
void CItems<TYPE>::Add( TYPE type ) {
   SafeSize( m_uCount + 2 );
   m_pTYPE[m_uCount] = type;
   m_uCount++;
}

template<class TYPE>
__forceinline TYPE* CItems<TYPE>::GetAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return &m_pTYPE[uIndex]; 
}

template<class TYPE>
__forceinline void CItems<TYPE>::SetAt( UINT uIndex, TYPE type ) {
   _ASSERT( uIndex < m_uCount );
   m_pTYPE[uIndex] = type;
}

template<class TYPE>
void CItems<TYPE>::InsertAt( UINT uIndex, const TYPE& type ) {
   _ASSERT( uIndex <= m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_pTYPE[uIndex+1], &m_pTYPE[uIndex], sizeof(TYPE) * (m_uCount - uIndex) );
   m_pTYPE[uIndex] = type;
   m_uCount++;
}

template<class TYPE>
void CItems<TYPE>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   m_uCount--;
   memmove( &m_pTYPE[uIndex], &m_pTYPE[uIndex+1], sizeof(TYPE) * (m_uCount - uIndex) );
}

template<class TYPE>
int CItems<TYPE>::Find( TYPE type ) {
   UINT uCount;
   UINT uIndex;
   uCount = m_uCount;
   for( uIndex = 0; uIndex < uCount; uIndex++ )
   {
      if( m_pTYPE[uIndex] == type ) return uIndex;
   }
   return -1;
}


template<class TYPE>
__forceinline TYPE* CItems<TYPE>::Data() {
   return m_pTYPE; 
}

template<class TYPE>
__forceinline void CItems<TYPE>::Empty() {
   SetLength( 0 );
}


template<class TYPE>
TYPE* CItems<TYPE>::Detach() {
   TYPE* pTYPE;
   pTYPE = m_pTYPE;
   m_uMaxCount = m_uCount = 0;
   m_pTYPE = NULL;
   return pTYPE;
}

template<class TYPE>
TYPE* CItems<TYPE>::Grow( UINT uLength ) {
   SafeSize( m_uCount + uLength );
   return &m_pTYPE[m_uCount];
}

template<class TYPE>
void CItems<TYPE>::SafeSize( UINT uLength ) {
   TYPE* pTYPE;
   if( uLength >= m_uMaxCount )
   {
      m_uMaxCount = m_uGrowBy + uLength;
      pTYPE = (TYPE*)MP_NEW_ARR( BYTE, sizeof(TYPE) * m_uMaxCount);
      memcpy( pTYPE, m_pTYPE, sizeof(TYPE) * m_uCount );
      MP_DELETE_ARR( m_pTYPE);
      m_pTYPE = pTYPE;
   }
}

/*
template<class VALUE, class POSITION>
class CIndex
{
   void Add( const VALUE& value, const POSITION& position );
   VALUE* Find( const VALUE& value );


protected:
   UINT m_uCount;
   UINT m_uMaxCount;
   VALUE* m_pVALUE;
   POSITION* m_pPOSITION;
};

template<class VALUE, class POSITION>
void CIndex<VALUE,POSITION>::Add( const VALUE& value, const POSITION& position )
{
}

template<class VALUE, class POSITION>
VALUE* CIndex<VALUE,POSITION>::Find( const VALUE& value )
{
   _ASSERT( m_pVALUE!= NULL );

   VALUE* pPosition,* pFirst;
   int iSize, iSection;

   pFirst = m_pVALUE;
   iSize = &m_pVALUE[m_uCount] - &pFirst[0];
   while( 0 < iSize )
   {
      iSection = iSize / 2;
      pPosition = pFirst;
      pPosition += iSection;
      if( *pPosition < type )
      {
         pFirst = ++pPosition;;
         iSize -= iSection + 1;
      }
      else
      {
         iSize = iSection;
      }
   }
   return pFirst;
}
*/


/*
*/
template<class TYPE>
class CIndex
{
public:
   CIndex();
   ~CIndex();

   TYPE& operator[]( UINT uIndex );
   void operator+=( TYPE type );

   // how many items
   UINT GetSize() const;

   // add object
   void  Add( const TYPE& type );
   // insert object att specified position
   void  InsertAt( UINT uIndex, const TYPE& type );
   // remove item at index
   void  RemoveAt( UINT uIndex );
   // get object att specified position
   TYPE& GetAt( UINT uIndex );
   // find object
   bool  Find( const TYPE& type, UINT* puIndex );
   TYPE* Find( const TYPE& type );
   // finds the first item and return index for it and also counts how many with same index if requsted
   int   FindRange( const TYPE& type, UINT* puLast );
   //  delete objects in buffer
   void  DeleteItems();
   // make sure buffer can store as many objects as size
   void  SafeSize( UINT uSize );

protected:
   UINT  m_uCount;
   UINT  m_uMaxCount;
   TYPE* m_pTYPE;
};

template<class TYPE>
CIndex<TYPE>::CIndex() {
   m_uCount = 0; m_uMaxCount = 0; m_pTYPE = NULL;
}

template<class TYPE>
CIndex<TYPE>::~CIndex() {
    MP_DELETE_ARR( m_pTYPE); // (BYTE*)
}

template<class TYPE>
__forceinline UINT CIndex<TYPE>::GetSize() const {
   return m_uCount; }

template<class TYPE>
__forceinline TYPE& CIndex<TYPE>::operator[]( UINT uIndex ) {
   return GetAt( uIndex ); }

template<class TYPE>
__forceinline void CIndex<TYPE>::operator+=( TYPE type ) {
   Add( type ); 
}

template<class TYPE>
void CIndex<TYPE>::Add( const TYPE& type ) {
   TYPE* pFirst;

   pFirst = Find( type );

   if( &pFirst[0] == &m_pTYPE[m_uCount] )
   {
      SafeSize( m_uCount + 1 );
      m_pTYPE[m_uCount] = type;
      m_uCount++;
   }
   else
   {
      InsertAt( &pFirst[0] - &m_pTYPE[0], type );
   }
}

template<class TYPE>
void CIndex<TYPE>::InsertAt( UINT uIndex, const TYPE& type ) {
   _ASSERT( uIndex < m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_pTYPE[uIndex+1], &m_pTYPE[uIndex], sizeof(TYPE) * (m_uCount - uIndex) );
   m_pTYPE[uIndex] = type;
   m_uCount++;
}

template<class TYPE>
void CIndex<TYPE>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   m_pTYPE[uIndex].~TYPE();
   m_uCount--;
   memmove( &m_pTYPE[uIndex], &m_pTYPE[uIndex+1], sizeof(TYPE) * (m_uCount - uIndex) );
}

template<class TYPE>
__forceinline TYPE& CIndex<TYPE>::GetAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   return m_pTYPE[uIndex];
}

template<class TYPE>
bool CIndex<TYPE>::Find( const TYPE& type, UINT* puIndex ) {
   TYPE* pTYPE;
   pTYPE = Find( type );
   if( (pTYPE != NULL) && (*pTYPE == type) )
   {
      if( puIndex != NULL ) *puIndex = pTYPE - m_pTYPE;
      return true;
   }

   return false;
}

template<class TYPE>
TYPE* CIndex<TYPE>::Find( const TYPE& type )
{
   TYPE* pPosition,* pFirst;
   int iSize, iSection;

   pFirst = m_pTYPE;
   iSize = &((TYPE*)m_pTYPE)[m_uCount] - &((TYPE*)pFirst)[0];
   while( 0 < iSize )
   {
      iSection = iSize / 2;
      pPosition = pFirst;
      pPosition += iSection;
      if( *pPosition < type )
      {
         pFirst = ++pPosition;;
         iSize -= iSection + 1;
      }
      else
      {
         iSize = iSection;
      }
   }
   return pFirst;
}

template<class TYPE>
int CIndex<TYPE>::FindRange( const TYPE& type, UINT* puLast ) {
   UINT uIndex, uFirst;
   TYPE* pTYPE;
   pTYPE = Find( type );
   if( *pTYPE == type )
   {
      uFirst = pTYPE - m_pTYPE;
      if( puLast != NULL )
      {
         for( uIndex = uFirst + 1; uIndex < m_uCount; uIndex++ )
         {
            if( !(m_pTYPE[uIndex] == type) ) break;
         }
         *puLast = uIndex;
      }

      return uFirst;
   }

   return -1;
}

template<class TYPE>
void CIndex<TYPE>::DeleteItems() {
   TYPE* pTYPE;
   UINT uIndex = m_uCount;
   while( uIndex != 0 )
   {
      uIndex--;
      m_pITEM[uIndex].~CIndex<TYPE>();
   }
   m_uCount = 0;
}

template<class TYPE>
void CIndex<TYPE>::SafeSize( UINT uSize ) {
   TYPE* pTYPE;
   if( uSize > m_uMaxCount )
   {
      m_uMaxCount = uSize;
      pTYPE = (TYPE*)new BYTE[uSize * sizeof(TYPE)];
      memcpy( pTYPE, m_pTYPE, m_uCount * sizeof(TYPE) );
      MP_DELETE_ARR( m_pTYPE); //(BYTE*)
      m_pTYPE = pTYPE;
   }
}


// ****************************************************************************
/* CBuffers can hold a item that has a specified length in bytes. It could be
   a C type or a struct. But do avoid storing classes or structs that has
   pointers to other object if you don't know exactly how CBuffers work. 
   Each item can be stored in a array with a delitmiter between each array.
   For example this class i ideal to store multiple strings where each string
   ends with a null character or some other character. The delitimer between
   each array don't need to be of same type as items that are stored in array.
   But the size for the delimiter can't be larger that the size for Items that
   are stored.
*/
template<class ITEM, class SPLIT, SPLIT delimiter>
class CBuffers
{
public:
   CBuffers();
   CBuffers( UINT uGrowBy );
   ~CBuffers();

   ITEM* operator[](UINT uIndex);

   UINT  GetSize() const;
   UINT  GetLastOffset();

   void  SetGrowBy( UINT uGrow );

   UINT  Add( const ITEM* pITEM );

   ITEM* GetItem( UINT uOffset ) const;
   ITEM* GetNext( int* piIndex ) const;

   int   Find( const ITEM* pITEM );
   // Remove all buffers
   void  Empty();

protected:
   void SafeSize( UINT uiCount );

   UINT  m_uItemCount;
   UINT  m_uCount;
   UINT  m_uMaxCount;
   UINT  m_uGrowBy;
   ITEM* m_pITEM;
};

template<class ITEM, class SPLIT, SPLIT delimiter>
CBuffers<ITEM, SPLIT, delimiter>::CBuffers() {
   memset( this, 0, sizeof(CBuffers<ITEM, SPLIT, delimiter>) );
}

template<class ITEM, class SPLIT, SPLIT delimiter>
CBuffers<ITEM, SPLIT, delimiter>::CBuffers( UINT uGrowBy ) {
   m_uItemCount = 0; m_uCount = 0; m_uMaxCount = 0; m_pITEM = NULL; 
   m_uGrowBy = uGrowBy;
}


template<class ITEM, class SPLIT, SPLIT delimiter>
CBuffers<ITEM, SPLIT, delimiter>::~CBuffers() {
    MP_DELETE_ARR( m_pITEM);
}

template<class ITEM, class SPLIT, SPLIT delimiter>
ITEM* CBuffers<ITEM, SPLIT, delimiter>::operator[](UINT uIndex) {
   _ASSERT( uIndex < m_uItemCount );
   UINT uPos, uItems;
   uPos = 0;
   uItems = 0;
   while( uItems < uIndex )
   {
      while( m_pITEM[uPos] != delimiter ) uPos++;
      uPos++;
      uItems++;
   }
   return &m_pITEM[uPos];
}

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline UINT CBuffers<ITEM, SPLIT, delimiter>::GetSize() const {
   return m_uItemCount; 
} 


template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline UINT CBuffers<ITEM, SPLIT, delimiter>::GetLastOffset() {
   return m_uCount; }

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline void CBuffers<ITEM, SPLIT, delimiter>::SetGrowBy( UINT uGrow ) {
   m_uGrowBy = uGrow; 
}

template<class ITEM, class SPLIT, SPLIT delimiter>
UINT CBuffers<ITEM, SPLIT, delimiter>::Add( const ITEM* pITEM ) {
   UINT uReturn;
   UINT uCount;

   uCount = 0;
   while( (SPLIT*)pITEM[uCount] != delimiter ) uCount++;
   SafeSize( uCount + m_uCount );

   memcpy( &m_pITEM[m_uCount], pITEM, uCount * sizeof(ITEM) );

   uReturn  = m_uCount;
   m_uCount += uCount;
   (SPLIT)m_pITEM[m_uCount] = delimiter;
   m_uCount++;
   m_uItemCount++;
   return uReturn;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline ITEM* CBuffers<ITEM, SPLIT, delimiter>::GetItem( UINT uOffset ) const {
   _ASSERT( (uOffset == 0) || (SPLIT)m_pITEM[uOffset-1] == delimiter );
   return &m_pITEM[uOffset];
}

template<class ITEM, class SPLIT, SPLIT delimiter>
ITEM* CBuffers<ITEM, SPLIT, delimiter>::GetNext( int* piIndex ) const {
   UINT  uIndex;
   UINT  uMax;
   ITEM* pITEM;
   if( *piIndex == -1 )
   {
      uIndex = 0;
   }
   else
   {
      uIndex = (UINT)*piIndex;
   }

   uMax  = m_uCount;
   pITEM = &m_pITEM[uIndex];
#ifdef _DEBUG
   if( uIndex > 0 ) ASSERT( (SPLIT)pITEM[-1] == delimiter );
#endif
   // Find next item
   while( uIndex < uMax )
   {
      uIndex++;
      if( (SPLIT)pITEM[uIndex] == delimiter )
      {
         
         *piIndex = (int)uIndex + 1;
         return pITEM;
      }
   }
   return NULL;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
int CBuffers<ITEM, SPLIT, delimiter>::Find( const ITEM* pITEM ) {
   UINT uIndex;
   UINT uReturn;
   UINT uCompare;
   UINT uMax;
   uMax = m_uCount;
   uIndex = 0;
   while( uIndex < uMax )
   {
      uCompare = 0;
      uReturn  = uIndex;
      while( pITEM[uCompare] == m_pITEM[uIndex] )
      {
         if( (SPLIT)pITEM[uCompare] == delimiter ) return uReturn;
         uCompare++;
         uIndex++;
      }

      while( (SPLIT)m_pITEM[uIndex] != delimiter ) uIndex++;
      uIndex++;
   }
   return -1;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline void CBuffers<ITEM, SPLIT, delimiter>::Empty() {
   MP_DELETE_ARR( m_pITEM);
   m_uItemCount = 0;
   m_uCount     = 0;
   m_uMaxCount  = 0;
   m_pITEM = NULL;
}


template<class ITEM, class SPLIT, SPLIT delimiter>
void CBuffers<ITEM, SPLIT, delimiter>::SafeSize( UINT uiCount ) {
   _ASSERT( sizeof(ITEM) >= sizeof(SPLIT) );
   ITEM* pITEM;
   uiCount++;
   if( uiCount >= m_uMaxCount )
   {
      m_uMaxCount = uiCount + m_uGrowBy;
      pITEM = MP_NEW_ARR( ITEM, m_uMaxCount);
      memcpy( pITEM, m_pITEM, sizeof(ITEM) * m_uCount );
      MP_DELETE_ARR( m_pITEM);
      m_pITEM = pITEM;
   }
}

template<class ITEM, class SPLIT, SPLIT delimiter>
class CBuffersEx
{
public:
   CBuffersEx();
   ~CBuffersEx();

// get/set
public:
   UINT  GetCount();
   void  SetGrowBy( UINT uGrow );

   // Adds a item vector to object
   void  Add( const ITEM* pITEM );
   // Remove a vector from object
   void  RemoveAt( UINT uIndex );
   // Get vector in object
   ITEM* GetItem( UINT uIndex );
   // Find item vector in object and return index for it
   int   Find( const ITEM* pITEM );

protected:
   void SafeSize( UINT uiCount );

   UINT  m_uItemCount;
   UINT  m_uCount;
   UINT  m_uMaxCount;
   UINT  m_uGrowBy;
   ITEM* m_pITEM;
   CBuffer<UINT,0xffffffff> m_BIndex;
};

template<class ITEM, class SPLIT, SPLIT delimiter>
CBuffersEx<ITEM, SPLIT, delimiter>::CBuffersEx() {
   m_uItemCount = 0;
   m_uCount     = 0;
   m_uMaxCount  = 0;
   m_uGrowBy    = 0;
   m_pITEM      = 0;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
CBuffersEx<ITEM, SPLIT, delimiter>::~CBuffersEx() {
    MP_DELETE_ARR( m_pITEM);
}

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline UINT CBuffersEx<ITEM, SPLIT, delimiter>::GetCount() {
   return m_BIndex.GetSize();
}

template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline void CBuffersEx<ITEM, SPLIT, delimiter>::SetGrowBy( UINT uGrow ) {
   m_uGrowBy = uGrow; 
}

template<class ITEM, class SPLIT, SPLIT delimiter>
void CBuffersEx<ITEM, SPLIT, delimiter>::Add( const ITEM* pITEM ) {
   UINT uCount;

   while( (SPLIT*)pITEM[uCount] != delimiter ) uCount++;
   SafeSize( uCount + m_uCount );

   memcpy( &m_pITEM[m_uCount], pITEM, uCount * sizeof(ITEM) );

   m_BIndex += m_uCount;
   m_uCount += uCount;
   (SPLIT)m_pITEM[m_uCount] = delimiter;
   m_uCount++;
   m_uItemCount++;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
void CBuffersEx<ITEM, SPLIT, delimiter>::RemoveAt( UINT uIndex ) {
   _ASSERT( (uIndex == 0) || (SPLIT)m_pITEM[m_BIndex[uIndex]-1] == delimiter );
   UINT  uOffset;
   UINT  uLength;
   UINT  uCount;
   UINT* puIndex;
   ITEM* pITEM;

   uOffset = m_BIndex[uIndex];
   pITEM = &m_pITEM[uOffset];
   uLength = 0;
   while( pITEM[uLength] != delimiter ) uLength++;

   // move memory over the removed vector
   uLength++;
   m_uCount -= uLength;
   memmove( &m_pITEM[uOffset], &pITEM[uLength], m_uCount - uOffset );

   // recalcualte offset positions
   puIndex = m_BIndex.Data();
   uCount  = m_BIndex.GetSize();
   for( ;uIndex < ; uIndex++ )
   {
      puIndex[uIndex] -= uLength;
   }

   m_BIndex.SetLength( uCount - 1 );
}


template<class ITEM, class SPLIT, SPLIT delimiter>
__forceinline ITEM* CBuffersEx<ITEM, SPLIT, delimiter>::GetItem( UINT uIndex ) {
   _ASSERT( (uIndex == 0) || (SPLIT)m_pITEM[m_BIndex[uIndex]-1] == delimiter );
   return &m_pITEM[m_BIndex[uIndex]];
}


template<class ITEM, class SPLIT, SPLIT delimiter>
int CBuffersEx<ITEM, SPLIT, delimiter>::Find( const ITEM* pITEM ) {
   UINT uIndex;
   UINT uOffset;
   UINT uCompare;

   uIndex = m_BIndex.GetSize();
   while( uIndex != 0 ) 
   {
      uIndex--;
      uOffset  = m_BIndex[uIndex];
      uCompare = 0;
      while( pITEM[uCompare] == m_pITEM[uOffset] )
      {
         if( (SPLIT)pITEM[uCompare] == delimiter )
         {
            return uIndex;
         }
         uCompare++;
         uOffset++;
      }
   }

   return -1;
}

template<class ITEM, class SPLIT, SPLIT delimiter>
void CBuffersEx<ITEM, SPLIT, delimiter>::SafeSize( UINT uiCount ) {
   _ASSERT( sizeof(ITEM) >= sizeof(SPLIT) );
   ITEM* pITEM;
   uiCount++;
   if( uiCount >= m_uMaxCount )
   {
      m_uMaxCount = uiCount + m_uGrowBy;
      pITEM = MP_NEW_ARR( ITEM, m_uMaxCount);
      memcpy( pITEM, m_pITEM, sizeof(ITEM) * m_uCount );
      MP_DELETE_ARR( m_pITEM);
      m_pITEM = pITEM;
   }
}



// ****************************************************************************
/*! \class CMapPtr 
  
   This class stores pointer in a hash table. Use this when you need to find
   a object fast and there is a lot of objects. If you only have 10 or less
   objects it is better to use a array but otherwise this class will speed up
   things. It uses a litle more memory than a array but that can be accepted
   if the speed is increased. There is very litle extra memory that is used.
*/
template<class KEY, class ITEM>
class CMapPtr
{
public: struct ITERATOR;
private: struct Entry;
public:
   CMapPtr();
   ~CMapPtr();

// get/set
public:
   UINT GetCount() const;
   UINT GetSize() const;

// operations
public:
   //! Initializes the hashobject. This function has to be called before you can use it.
   void  InitHashTable( UINT uSize );
   //! Get value at specified key
   ITEM* GetAt( KEY key ) const;
   //! Set a value at specified key
   ITEM* SetAt( KEY key, ITEM* pITEM );
   //! Remove item for specified key and returns the pointer to item that was removed
   ITEM* RemoveKey( KEY key );
   //! Find item at specified key
   bool  Lookup( KEY key, ITEM** ppITEM ) const;
   //! Get first key in hash
   void  GetFirstPosition( ITERATOR& iterator ) const;
   //! Gets next item and updates key (returnes NULL if no more items)
   bool  GetNext( ITERATOR& iterator, ITEM** ppItem ) const;
   //! Remove items from hash table (if bDelete is true then the objects are deleted)
   void  Empty( bool bDelete );
   //! delete all items from hash table
   void DeleteItems();
   //! Calculate the hash key
   UINT  HashKey( KEY key ) const;

   //! item that exists in hash object. Use this when iterating through all items in hash table
   struct ITERATOR
   {
      UINT uBranch;
      UINT uIndex;
   };

protected:
	Entry* FindKey( KEY key, Entry*, UINT uSize ) const{
		Entry* pentryFind;
		while( uSize != 0 )
		{
			uSize--;
			pentryFind = &pEntry[uSize];
			if( pentryFind->m_KEY == key ) return pentryFind;
		}
		return NULL;
	}
   void  Add( UINT uEntry, KEY key, ITEM* pITEM );

private:
   struct Entry
   {
      KEY   m_KEY;
      ITEM* m_pITEM;
   };
// Attributes
private:
   UINT   m_uCount;
   UINT   m_uGrowBy;    //!< How much a branch should grow if items are added and there is no more room
   UINT   m_uTableSize; //!< Table size for entries.
   UINT*  m_puTable;    //!< Has number if items in each entry (branch) and also the amount that can 
                        //!< be inserted before reallocation is done.
   Entry** m_ppEntry;   //!< Pointer to pointer for Entry object's
};

template<class KEY, class ITEM>
CMapPtr<KEY,ITEM>::CMapPtr() {
   m_uCount = 0; m_uGrowBy = 1; m_uTableSize = 0; m_puTable = NULL; m_ppEntry = 0; 
}

template<class KEY, class ITEM>
CMapPtr<KEY,ITEM>::~CMapPtr() {
   Empty( false );
}

/*---------------------------------------------------------------------------------*/ /**
 * Return number of items in hash list
 * \return number of items */
template<class KEY, class ITEM>
__forceinline UINT CMapPtr<KEY,ITEM>::GetCount() const {
   return m_uCount; 
}

/*---------------------------------------------------------------------------------*/ /**
 * Return number of items in hash list
 * \return number of items */
template<class KEY, class ITEM>
__forceinline UINT CMapPtr<KEY,ITEM>::GetSize() const {
   return m_uCount; 
}

template<class KEY, class ITEM>
void CMapPtr<KEY,ITEM>::InitHashTable( UINT uSize ) {
   _ASSERT( m_puTable == NULL );
   m_puTable = MP_NEW_ARR( UINT, uSize*2);
   m_ppEntry  = MP_NEW_ARR( Entry*, uSize);
   memset( m_puTable, 0, sizeof(UINT) * uSize * 2);
   memset( m_ppEntry, 0, sizeof(Entry*) * uSize );
   m_uTableSize = uSize;
}

template<class KEY, class ITEM>
ITEM* CMapPtr<KEY,ITEM>::GetAt( KEY key ) const {
   UINT uIndex;
   Entry* pEntry;
   uIndex = HashKey( key );
   pEntry = FindKey( key, m_ppEntry[uIndex], m_puTable[uIndex] );
   if( pEntry != NULL )
   {
      return pEntry->m_pITEM;
   }
   return NULL;
}

template<class KEY, class ITEM>
ITEM* CMapPtr<KEY,ITEM>::SetAt( KEY key, ITEM* pITEM ) {
   _ASSERT( m_puTable != NULL );
   ITEM* pITEMOld = NULL;
   UINT uIndex;
   Entry* pEntry;
   uIndex = HashKey( key );
   pEntry = FindKey( key, m_ppEntry[uIndex], m_puTable[uIndex] );
   if( pEntry == NULL ) Add( uIndex, key, pITEM );
   else 
   {
      pITEMOld = pEntry->m_pITEM;
      pEntry->m_pITEM = pITEM;
   }

   return pITEMOld;
}

template<class KEY, class ITEM>
ITEM* CMapPtr<KEY,ITEM>::RemoveKey( KEY key ) {
   _ASSERT( m_puTable != NULL );
   UINT uIndex;
   UINT uSize;
   Entry* pentryFind;
   Entry* pEntry;
   ITEM* pITEM;
   uIndex = HashKey( key );
   uSize  = m_puTable[uIndex];
   pEntry = m_ppEntry[uIndex];
   while( uSize != 0 )
   {
      uSize--;
      pentryFind = &pEntry[uSize];
      if( pentryFind->m_KEY == key )
      {
         pITEM = pentryFind->m_pITEM;
         m_puTable[uIndex]--;
         memmove( &pEntry[uSize], &pEntry[uSize+1], sizeof(Entry) * m_puTable[uIndex] );
         m_uCount--;
         return pITEM;
      }
   }
   return NULL;
}

template<class KEY, class ITEM>
bool CMapPtr<KEY,ITEM>::Lookup( KEY key, ITEM** ppITEM ) const {
   _ASSERT( m_puTable != NULL );
   UINT uIndex;
   Entry* pEntry;
   uIndex = HashKey( key );
   pEntry = m_ppEntry[uIndex];
   pEntry = FindKey( key, m_ppEntry[uIndex], m_puTable[uIndex] );
   if( pEntry != NULL )
   {
      *ppITEM = pEntry->m_pITEM;
      return true;
   }
   return false;
}

template<class KEY, class ITEM>
void CMapPtr<KEY,ITEM>::GetFirstPosition( ITERATOR& iterator ) const {
   _ASSERT( m_puTable != NULL );
   for( UINT uBranch = 0; uBranch < m_uTableSize; uBranch++ )
   {
      if( m_puTable[uBranch] != 0 )
      {
         iterator.uBranch = uBranch;
         iterator.uIndex = 0;
         return;
      }
   }
   iterator.uBranch = m_uTableSize + 1;
}


template<class KEY, class ITEM>
bool CMapPtr<KEY,ITEM>::GetNext( ITERATOR& iterator, ITEM** ppItem ) const {
   _ASSERT( m_puTable != NULL );
   UINT uBranch;

   uBranch = iterator.uBranch;
   if( uBranch >= m_uTableSize ) return false;

   for( ;uBranch < m_uTableSize; uBranch++ )
   {
      if( iterator.uIndex < m_puTable[uBranch] )
      {
         *ppItem = m_ppEntry[uBranch][iterator.uIndex].m_pITEM;
         iterator.uBranch = uBranch;
         iterator.uIndex++;
         return true;
      }

      iterator.uIndex = 0;
   }

   return false;
}

template<class KEY, class ITEM>
void CMapPtr<KEY,ITEM>::Empty( bool bDelete ) {
   UINT uBranch;
   UINT uIndex;
   uBranch = m_uTableSize;
   while( uBranch != 0 )
   {
      uBranch--;
      if( bDelete )
      {
         uIndex = m_puTable[uBranch];
         while( uIndex != 0 )
         {
            uIndex--;
            MP_DELETE( m_ppEntry[uBranch][uIndex].m_pITEM);
         }
      }
       MP_DELETE_ARR( m_ppEntry[uBranch]);
   }
   MP_DELETE_ARR( m_puTable);
   m_puTable = NULL;
   MP_DELETE_ARR( m_ppEntry);
   m_ppEntry = NULL;
   m_uTableSize = 0;
   m_uCount = 0;
}

/*---------------------------------------------------------------------------------*/ /**
 *  delete all items in hash table */
template<class KEY, class ITEM>
__forceinline void CMapPtr<KEY,ITEM>::DeleteItems() {
   Empty( true );
}

/*---------------------------------------------------------------------------------*/ /**
 * Calculate the entry where item should be inserted
 * \param key key that entry is calculated from
 * \return index to entry in table where item is inserted */
template<class KEY, class ITEM>
__forceinline UINT CMapPtr<KEY,ITEM>::HashKey( KEY key ) const {
#ifdef _DEBUG
   UINT uEntry;
   uEntry = key % m_uTableSize;
   return uEntry;
#else
   return (key % m_uTableSize);
#endif
}

template<class KEY, class ITEM>
void CMapPtr<KEY,ITEM>::Add( UINT uEntry, KEY key, ITEM* pITEM ) {
   UINT uSize;
   UINT uMaxSize;
   Entry* pentryTemp;
   Entry* pEntry = m_ppEntry[uEntry];
   uMaxSize      = m_puTable[uEntry+m_uTableSize];
   if( m_puTable[uEntry] >= uMaxSize )
   {
      uMaxSize += m_uGrowBy;
      pentryTemp = MP_NEW_ARR( Entry, uMaxSize);
      memcpy( pentryTemp, pEntry, sizeof(Entry) * m_puTable[uEntry] );
      MP_DELETE_ARR( pEntry);
      pEntry = pentryTemp;
      m_puTable[uEntry+m_uTableSize] = uMaxSize;
      m_ppEntry[uEntry] = pEntry;
   }
   uSize = m_puTable[uEntry];
   pEntry[uSize].m_KEY = key;
   pEntry[uSize].m_pITEM = pITEM;
   uSize++;
   m_puTable[uEntry] = uSize;
   m_uCount++;
}

// ****************************************************************************
/*! \brief array for storing objects

    CArray is used to store objects. It isn't as fast as CArrayPtr. But more 
    flexible and stores any kind of object.
*/
template<class ITEM>
class CArray
{
// Construction
public: 
   CArray( UINT uCount = 1 );
   ~CArray();

   //! Index operator used to get item at specified position
	ITEM& operator[](UINT uIndex) const;

// Get/Set
public:
   //! Get number of elements on stack
   UINT GetSize() const;
   //! Set how many more openings that should be created if we add more then can fit
   void SetGrowBy( UINT uGrowBy );

// Operation
public:
   //! Add item to array
   void  Add( const ITEM& item );
   //! Get item for index
   ITEM& GetAt( UINT uIndex ) const;
   //! return pointer to object at specified position
   ITEM* GetPtrAt( UINT uIndex ) const;
   //! Set item at index
   void  SetAt( UINT uIndex, const ITEM& item );
   //! Insert item at index
   void  InsertAt( UINT uIndex, const ITEM& item );
   //! remove item at index
   void  RemoveAt( UINT uIndex );
   //! destroy items from end of array
   void  Remove( unsigned uSize );
   //! Search for item at specified index
   int   FindIndex(const ITEM& item );
   //! delete all items
   void  Empty();

protected:
   void SafeSize( UINT uSlots );

// Attributes
protected:
   UINT m_uCount;    //!< Items in array
   UINT m_uMaxCount; //!< max number of items before we need to realocate
   UINT m_uGrowBy;   //!< how many more valuew that should fit if we need to allocate more space for stack

   ITEM* m_pITEM;    //!< Pointer to objects in array
};

template<class ITEM>
CArray<ITEM>::CArray( UINT uCount ) {
   m_uCount    = 0;
   m_uMaxCount = 0;
   m_uGrowBy   = uCount;
   m_pITEM     = NULL;
}

template<class ITEM>
CArray<ITEM>::~CArray() {
   DestroyItems<ITEM>( m_pITEM, m_uMaxCount );
   MP_DELETE_ARR( (BYTE*)m_pITEM);
}

template<class ITEM>
__forceinline ITEM& CArray<ITEM>::operator[](UINT uIndex) const	{ 
   return GetAt( uIndex ); }

template<class ITEM>
__forceinline UINT CArray<ITEM>::GetSize() const {
   return m_uCount; 
}

template<class ITEM>
__forceinline void CArray<ITEM>::SetGrowBy( UINT uGrowBy ) {
   _ASSERT( (uGrowBy < 0xff000000) && (uGrowBy > 0) ); // realistic
   m_uGrowBy = uGrowBy;
}

template<class ITEM>
void CArray<ITEM>::Add( const ITEM& item ) {
   SafeSize( m_uCount + 1 );
   m_pITEM[m_uCount] = item;
   m_uCount++;
}

template<class ITEM>
__forceinline ITEM& CArray<ITEM>::GetAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return m_pITEM[uIndex];
}

/*! return pointer to object at specified position
\code
gd::CArray<CString> arrString;
arrString.Add( "1" );
arrString.Add( "2" );

CString s = arrString.GetAt( 0 );
CString* ps = arrString.GetPtrAt( 0 );  
\endcode
*/
template<class ITEM>
ITEM* CArray<ITEM>::GetPtrAt( UINT uIndex ) const {
   _ASSERT( uIndex < m_uCount );
   return &m_pITEM[uIndex];
}


template<class ITEM>
__forceinline void CArray<ITEM>::SetAt( UINT uIndex, const ITEM& item ) {
   _ASSERT( uIndex < m_uCount );
   m_pITEM[uIndex] = item;
}


template<class ITEM>
void CArray<ITEM>::InsertAt( UINT uIndex, const ITEM& item ) {
   _ASSERT( uIndex <= m_uCount );
   SafeSize( m_uCount + 1 );
   memmove( &m_pITEM[uIndex+1], &m_pITEM[uIndex], sizeof(ITEM) * (m_uCount - uIndex) );
   CreateItems<ITEM>( &m_pITEM[uIndex], 1 );
   m_pITEM[uIndex] = item;
   m_uCount++;
}

/*! Remove object at specified position<br>
\code
gd::CArray<CString> arrString;
arrString.Add( "1" );
arrString.Add( "2" );
arrString.Add( "3" );
arrString.RemoveAt( 0 );
printf("%s", (const char*)arrString[0] );
// "2" is printed
\endcode
*/
template<class ITEM>
void CArray<ITEM>::RemoveAt( UINT uIndex ) {
   _ASSERT( uIndex < m_uCount );
   DestroyItems<ITEM>( &m_pITEM[uIndex], 1 );
   m_uCount--;
   memmove( &m_pITEM[uIndex], &m_pITEM[uIndex+1], sizeof(ITEM) * (m_uCount - uIndex) );
   CreateItems<ITEM>( &m_pITEM[m_uCount], 1 );
}

/*! Find position (index in array) where item is. Index for found item is returned or if not found 
    -1 will be returned */
template<class ITEM>
int CArray<ITEM>::FindIndex(const ITEM& item )
{
   for( unsigned u = 0; u < m_uCount; u++ ) {
      if( m_pITEM[u] == item ) return u;
   }
   return -1;
}

/*! Remove items from end of array. Use this function it a big chunk will be removed. It does not
    call empty constructor for unused slots.
*/
template<class ITEM>
void CArray<ITEM>::Remove( unsigned uSize ) {
   if( uSize > m_uCount ) uSize = m_uCount;
   DestroyItems<ITEM>( &m_pITEM[m_uCount - uSize], uSize );
   m_uCount -= uSize;
   m_uMaxCount = m_uCount;
}

template<class ITEM>
void CArray<ITEM>::Empty() {
#ifdef _DEBUG
   if( m_uMaxCount == 0 ) assert( m_pITEM == NULL );
#endif
   DestroyItems<ITEM>( m_pITEM, m_uMaxCount );
   if( m_pITEM ) MP_DELETE_ARR( (BYTE*)m_pITEM);
   m_pITEM = 0;
   m_uMaxCount = 0;
   m_uCount = 0;
}

template<class ITEM>
void CArray<ITEM>::SafeSize( UINT uSlots ) {
   ITEM* pITEM;
   if( uSlots >= m_uMaxCount )
   {
      m_uMaxCount += m_uGrowBy + uSlots;
      pITEM = (ITEM*) MP_NEW_ARR( BYTE, sizeof(ITEM) * m_uMaxCount);
      memcpy( pITEM, m_pITEM, sizeof(ITEM) * m_uCount );
      CreateItems<ITEM>( &pITEM[m_uCount], m_uMaxCount - m_uCount );
      MP_DELETE_ARR( (BYTE*)m_pITEM);
      m_pITEM = pITEM;
   }
}

/*! \brief store objects in a sorted array
*/
template<class ITEM, class KEY>
class CSortedArray : public CArray<ITEM>
{
// Construction
public: 
   CSortedArray( UINT uCount = 1 );
   ~CSortedArray();

// Operation
public:
   //! Add item to array
   bool  Add( const ITEM& item );
   //! Add item last in list (you need to sort the list if this isn't the last item)
   void  AddLast( const ITEM& item );
   //!
   void  Sort();
   //! Find object in array and return pointer to it
   ITEM* Find( KEY key ) const;
   //! Find object in array for specified key. Return what index it is at or -1 if not found
   int   FindIndex( KEY key ) const;

protected:
};

template<class ITEM, class KEY>
__forceinline CSortedArray<ITEM, KEY>::CSortedArray( UINT uCount ) : CArray<ITEM>( uCount ) {
}

template<class ITEM, class KEY>
__forceinline CSortedArray<ITEM, KEY>::~CSortedArray() {
}

template<class ITEM, class KEY>
bool CSortedArray<ITEM, KEY>::Add( const ITEM& item ) {
   int iLow, iHigh;
   UINT uMiddle;
   ITEM* pITEM;

   uMiddle = iLow  = 0;
   iHigh = m_uCount - 1;
   pITEM = m_pITEM;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( pITEM[uMiddle] > item )
         iHigh = uMiddle - 1;
      else if( pITEM[uMiddle] < item )
         iLow = uMiddle + 1;
      else
      {
         _ASSERT(0); return false;
      }
   }

   InsertAt( iLow, item );

   return true;
}

template<class ITEM, class KEY>
__forceinline void CSortedArray<ITEM, KEY>::AddLast( const ITEM& item ) {
   CArray<ITEM>::Add( item );
}

template<class ITEM, class KEY>
void CSortedArray<ITEM, KEY>::Sort() {
   UINT uMax, uMin, uPosition, uIndex;
   ITEM* pITEM;
   ITEM item;

   uMax = m_uCount;
   pITEM = m_pITEM;

   // Use selection-sort
   for( uPosition = 0; uPosition < uMax; uPosition++ )
   {
      uMin = uPosition;
      for( uIndex = uPosition + 1; uIndex < uMax; uIndex++ )
      {
         if( pITEM[uIndex] < pITEM[uMin] ) uMin = uIndex;
      }

      if( uMin != uPosition )
      {
         item = pITEM[uPosition];
         pITEM[uPosition] = pITEM[uMin];
         pITEM[uMin] = item;
      }
   }
}


template<class ITEM, class KEY>
ITEM* CSortedArray<ITEM, KEY>::Find( KEY key ) const {
   int iIndex;

   iIndex = FindIndex( key );
   if( iIndex != -1 )
   {
      return &m_pITEM[(UINT)iIndex];
   }

   return NULL;
}

template<class ITEM, class KEY>
int CSortedArray<ITEM, KEY>::FindIndex( KEY key ) const {
   int  iLow, iHigh;
   UINT uMiddle;
   ITEM* pITEM;

   iLow  = 0;
   iHigh = m_uCount - 1;
   pITEM = m_pITEM;

   while( iLow <= iHigh )
   {
      uMiddle = (iLow + iHigh) / 2;
      if( pITEM[uMiddle] > key )
         iHigh = uMiddle - 1;
      else if( pITEM[uMiddle] < key )
         iLow = uMiddle + 1;
      else
         return uMiddle;
   }

   return -1;
}

/*! \brief stores two values as a pair.

  Class used for storing two values as a pair. this is often handy when it comes to
  simple lists and other objects that needs to have some special storing mechanism
  for internal values.
*/
template<class FIRST, class SECOND>
struct Pair 
{
   //! default constructor
   Pair() :
      m_FIRST( FIRST() ), m_SECOND( SECOND() ) {}
   //! construcor for setting both values
	Pair(const FIRST& f, const SECOND& s)
		: m_FIRST(f), m_SECOND(s) {}
   //! copy construcor
   Pair(const Pair<FIRST, SECOND> &p)
		: m_FIRST(p.m_FIRST), m_SECOND(p.m_SECOND) {}

   FIRST m_FIRST;    //!< first value in pair
   SECOND m_SECOND;  //!< second value in pair

   //! access first value
   FIRST& First() {
      return m_FIRST; }
   //! access second value
   SECOND& Second() {
      return m_SECOND; }
};

/*! Compare two pair objects if equal
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator==(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
   return (A.m_FIRST == B.m_FIRST && A.m_SECOND == B.m_SECOND); 
}

/*! Compare two pair objects if not equal
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator!=(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
	return (!(A == B)); 
}

/*! is pair object less than type of item for first object in pair object
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator<(const Pair<FIRST, SECOND>& A, const FIRST& first) {
   return (A.m_FIRST < first ); 
}

/*! is pair object greater than type of item for first object in pair object
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator>(const Pair<FIRST, SECOND>& A, const FIRST& first) {
   return (A.m_FIRST > first ); 
}

/*! is first pair object less than second
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator<(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
   return (A.m_FIRST < B.m_FIRST || ((A.m_FIRST == B.m_FIRST) && (A.m_SECOND < B.m_SECOND))); 
}

/*! is second pair object less than first
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator>(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
   return (B < A); 
}

/*! is first pair object less or equal to second
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator<=(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
   return !(B < A); 
}

/*! is second pair object less or equal to  first
*/
template<class FIRST, class SECOND> 
__forceinline bool __cdecl operator>=(const Pair<FIRST, SECOND>& A, const Pair<FIRST, SECOND>& B) {
   return !(A < B); 
}
/*
PURPOSE: 
   Template function for sorting a vector
INFO:
   '==' and '<' needs to work for the object that is sorted.
*/
template<class tINDEX>
void Sort( tINDEX* pFirst, tINDEX* pLast )
{
   tINDEX Value;
   tINDEX* pPosition, *pPreserve;

   while( pFirst < pLast )
   {
      Value = *pFirst;
      pPosition = pFirst;
      pPosition++;
      while( pPosition < pLast )
      {
         if( *pPosition < Value )
         {
            Value = *pPosition;
            pPreserve = pPosition;
         }
         pPosition++;
      }

      if( !(*pFirst == Value) )
      {
         *pPreserve = *pFirst;
         *pFirst = Value;
      }
      pFirst++;
   }
}


/*
PURPOSE: 
   Template function for finding lowest index in array of elements that either matches the value
   or is after the value that is just after the nearest value below the value that is serached for.
RETURN:
   tINDEX
*/
template<class tINDEX, class tVALUE>
tINDEX LowestIndex( tINDEX First, tINDEX Last, tVALUE Value ) 
{
   int iSize;
   int iSection;
   tINDEX Position;

   iSize = Last - First;
   while( 0 < iSize )
   {
      iSection = iSize / 2;
      Position = First;
      Position += iSection;
      if( *Position < Value )
      {
         First = ++Position;
         iSize -= iSection + 1;
      }
      else
      {
         iSize = iSection;
      }
   }

   return First;
}

/*
PURPOSE: 
   Returns true if a value exist in array
RETURN:
   bool
*/
template<class tINDEX, class tVALUE>
__forceinline bool BinarySearch( tINDEX First, tINDEX Last, const tVALUE& Value )
{
   _ASSERT( First <= Last );
   return !(Value < *LowestIndex( First, Last, Value ) );
}
/*
class CCriticalSection
{
public:
	void Lock() {EnterCriticalSection(&m_sec);}
	void Unlock() {LeaveCriticalSection(&m_sec);}
	void Init() {InitializeCriticalSection(&m_sec);}
	void Term() {DeleteCriticalSection(&m_sec);}
	CRITICAL_SECTION m_sec;
};

class CAutoCriticalSection
{
public:
	void Lock() {EnterCriticalSection(&m_sec);}
	void Unlock() {LeaveCriticalSection(&m_sec);}
	CAutoCriticalSection() {InitializeCriticalSection(&m_sec);}
	~CAutoCriticalSection() {DeleteCriticalSection(&m_sec);}
	CRITICAL_SECTION m_sec;
};

class CFakeCriticalSection
{
public:
	void Lock() {}
	void Unlock() {}
	void Init() {}
	void Term() {}
};
*/


}; // namespace gd

#endif

