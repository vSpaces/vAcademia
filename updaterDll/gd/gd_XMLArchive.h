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

/*! \file gd_XMLArchive.h
    \brief has template class with functionality to read or store XML data


*/

#if !defined(AFX_XMLARCHIVE_H__664C0238_A109_453B_9341_6AF4614AD628__INCLUDED_)
#define AFX_XMLARCHIVE_H__664C0238_A109_453B_9341_6AF4614AD628__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _GD_XML_ARCHIVE_H_
   #define _GD_XML_ARCHIVE_H_
#endif

#ifndef _HRESULT_DEFINED
	typedef long HRESULT;
	#define S_OK                                   ((HRESULT)0x00000000L)
	#define S_FALSE                                ((HRESULT)0x00000001L)
	#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
	#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#endif

//#include <malloc.h>

#ifndef _GD_STRINGS_H_
   #include "gd_Strings.h"
#endif

#ifndef _GD_STRING_H_
   #include "gd_String.h"
#endif

#ifndef _GD_LISTTMPL_
   #include "gd_listtmpl.h"
#endif

#define _U(x) L##x

#ifdef WIN32
#ifdef _DEBUG
//! check string for valid string buffer
#define _CHECK_S( string ) _ASSERT( ::IsBadStringPtr( string, (UINT)-1 ) == FALSE )
#define _CHECK( condition ) _ASSERT( condition )
#endif
#endif

#ifndef _CHECK_S
#define _CHECK_S( string )
#endif

#ifndef _CHECK
#define _CHECK( condition )
#endif

#define _xmlarchive_NullCharacterFound          0x0010
#define _xmlarchive_NullCharacterBeforeEndNode  0x0011
#define _xmlarchive_EndingDoesNotMatchParent    0x0012
#define _xmlarchive_XMLHasEndedBeforeEndOfTags  0x0013

/*! \brief namespace for streaming xml
*/
namespace gd_xml_stream
{                                      

#pragma warning( disable : 4244 )

/*! \brief text item used to replace characters when converting text
*/
struct TextText
{
   const wchar_t* pszFrom;
   const wchar_t* pszTo;
};

/*! \brief text collection with text items used when specific textparts should be replaced
*/
struct Texts
{
   UINT uMaxCode;
   UINT uCount;
   TextText* pTextText;
};

/*! constant values for converting to specific html tags
*/
enum Convert 
{  
   eXML = 0x0001,
   eSpace = 0x0002,  //!< mark if we should change multiple spaces to &nbsp;
   eWWW = 0x0004,    //!< insert links when http:// or www. is found
};


//! copy xml text to string buffer (convert XML tags to normal text)
extern void CopyXMLText( const wchar_t* pszIn, gd::CString* pstringText, wchar_t chEnd = 0 );
//! copy xml text to text buffer (converts XML tags to normal characters)
extern int  CopyXMLText( const wchar_t* pszIn, wchar_t* pszOut, UINT uMaxLength, wchar_t chEnd = 0 );
//! calculate lenght for xml formated text in normal character format
extern UINT XMLToTextLength( const wchar_t* pszIn, wchar_t chEnd = 0 );
//! check if string has any character that need to be converted to xml entity
extern bool HasXML( const wchar_t* pszXML );
//! check if string has any character that need to be converted to xml entity
extern bool HasHTML( const wchar_t* pszText );
//! convert text to XML string
extern void TextToXML( const wchar_t* pszIn, gd::CString* pstringXML );
//! return text converterd to xml as string object
extern gd::CString TextToXML( const wchar_t* pszIn );
//! calculate length needed to store normal text as XML
extern UINT TextToXMLLength( const wchar_t* pszIn );
//! convert text to XML formated text (felxible because the sent object that has information 
//! about special formating)
extern void TextToXMLEx( const wchar_t* pszIn, Texts* pTexts, gd::CString* pstringXML, UINT uFlags = 0 );
//! convert XML text into string buffer as normal text
extern void XMLToText( const wchar_t* pszIn, gd::CString* pstringXML );
//! convert XML to normal text and return resulting string
extern gd::CString XMLToText( const wchar_t* pszIn );
//! convert text to string that has valid HTML characters
extern void TextToHTML( const wchar_t* pszIn, gd::CString* pstringXML );
//! replace XML characters ( <>"& )
extern void ReplaceXML( wchar_t* pszXML, wchar_t chReplace );
//! convert binary to hexstring
extern void BinToHex( const BYTE* pbValue, wchar_t* pszHex, UINT uLength );
//! convert hex string to binary
extern int  HexToBin( const wchar_t* pszHex, BYTE* pbValue, UINT uLength, wchar_t chEnd );
//! calculate the length for binary buffer needed to store
extern UINT HexToBinLength( const wchar_t* pszHex, wchar_t chEnd );

/*! \class CXMLItem
    \brief item use for implicit convertion of normal C++ datatypes to CXMLArchive

Helper class use for convertion of normal C++ types into the XML archive object.
This class simplifies to write serialize functionality in C++. This is mainly because 
implicit type convertion.
*/
class CXMLItem
{
public:union Data;
public:
   enum Type
   {
	   eTNULL	= 1,
	   eTI2	= 2,
	   eTI4	= 3,
	   eTR4	= 4,
	   eTR8	= 5,
	   eTSTR	= 8,
	   eTBOOL	= 11,
	   eTI1	= 16,
	   eTUI1	= 17,
	   eTUI2	= 18,
	   eTUI4	= 19,
	   eTI8	= 20,
	   eTUI8	= 21,
   };

public:
   CXMLItem() {}
   CXMLItem( int iVal ) { m_sType = eTI4; m_V.valLong = iVal; }

   CXMLItem( char chVal ) { m_sType = eTI1; m_V.valChar = chVal; }
   CXMLItem( unsigned char uchVal ) { m_sType = eTUI1; m_V.valUChar = uchVal; }
   CXMLItem( short sVal ) { m_sType = eTI2; m_V.valShort = sVal; }
   CXMLItem( unsigned short usVal ) { m_sType = eTUI2; m_V.valUShort = usVal; }
   CXMLItem( long lVal ) { m_sType = eTI4; m_V.valLong = lVal; }
   CXMLItem( unsigned long ulVal ) { m_sType = eTUI4; m_V.valULong = ulVal; }
   CXMLItem( float fVal ) { m_sType = eTR4; m_V.valFloat = fVal; }
   CXMLItem( double dVal ) { m_sType = eTR8; m_V.valDouble = dVal; }
   CXMLItem( const wchar_t* pszValue ) { m_sType = eTSTR; m_V.valpsz = pszValue; }
   CXMLItem( const gd::CString& string ) { m_sType = eTSTR; m_V.valpsz = (const LPCWSTR)string; }

   operator char() { return m_V.valChar; }
   operator unsigned char() { return m_V.valUChar; }
   operator short() { return m_V.valShort; }
   operator unsigned  short() { return m_V.valUShort; }
   operator long() { return m_V.valLong; }
   operator unsigned long() { return m_V.valULong; }
   operator float() { return m_V.valFloat; }
   operator double() { return m_V.valDouble; }
   operator int() { return m_V.valLong; }
   operator const wchar_t*() { return m_V.valpsz; }

// get/set
public:
   //! return type
   short GetType() const { return m_sType; }
   //! return data item (union)
   Data GetData() const { return m_V; }
   //! set internal type
   void SetType( Type type ) { m_sType = (short)type; }

// Attribute
public:
   short m_sType;
   union Data
   {
      char valChar;
      unsigned char valUChar;
      short valShort;
      unsigned short valUShort;
      long valLong;
      unsigned long valULong;
      float valFloat;
      double valDouble;
      const wchar_t* valpsz;
   } m_V;
};

//! value that sets next item that is managed by the CXMLArchive when streaming data
enum Operation
{
   Child       = 0x0020,
   Attribute   = 0x0040,
   Sibling     = 0x0080,
};

//! value that sets end operation for CXMLArchive when streaming data
enum EndOperation
{
   EndNode     = 1,
   End         = 2,
};

/**
 * \brief helper object for adding attributes to xml.

 Simple class used to add attributes to CXMLArchive.

 \code
CXMLArchive<CFile> ar( &file, CXMLArchive<CFile>::eWrite );
ar << Child << "DOCUMENT" 
      << Child << "ITEM" << attribute( "ATT1", "Attribute text" ) << attribute( "ATT1", 100 ) << EndNode
   << End;
 \endcode

 */
struct attribute
{
   attribute( const wchar_t* pszName, const CXMLItem& Item ) : m_pszName( pszName ), m_Item( Item ) {}
   const wchar_t* m_pszName;    
   CXMLItem m_Item;
};


/*! \class CXMLArchive
    \brief use this template class to save or load data in XML format

\par
XML is a flexible and readable way to store data. It is also a standard.
The CXMLArchive class makes it more easy to store data in this format. It's very fast. It
doesn't check for valid XML (just the basic stuff). This means that you need to
work on XML'files that ar valid.
\p
In order to use CXMLArchive here are the steps to follow.<br>
1: You need to create a class that with functions for reading and writing. The CXMLArchive is done
to be general and work in different situations. Minimizing the need of other components makes it
easier to use. The method used for reading should look like this<br>
<b>UINT Read( void* pBuf, UINT uCount, UINT* puCount );</b><br>
And the method for writing <br>
<b>void Write( const void* pBuf, UINT uCount, UINT* puCount );</b><br>
<br>
2: The CXMLArchive process informaiton in blocks. It is possible to specifiy the size of the block.
The reason why CXMLArchive processes information this way is to enable larger files for processing. 
If one XML file is very large and application needs to allocate data to store all information, 
then there could be memory problems if the file is to big. Default block size is 4096 bytes. Specify 
the block size in the constructor.<br>
<br>
3: Writing information is could be done by streaming information to the CXMLArchive object. There 
are one helper class called CXMLItem. The purpose for this class it to create implicit convertion
of information. Code is more readable to stream information with the stream operator "<<".<br>
The attribute struct is a helper object for adding attributes when information is streamed.<br>
It is possible to add information using methods. When the buffer runs out of space it calls the
write operation and writes information to that. This needs to handle it (e.g. writing information to
disk).<br>
<br>
4: When you read information from one XML file it is possible to use stream operators. This is not
recomended though. XML format is very flexible and if you hardcode where information should exist
then much of the advantage is lost. Using methods is better.<br>
It is possible to looka ahead for one item using the CXMLArchive. This is often what you need in order
to process information. If you need to look further then you shold store elements in the application. 
This can't be done in CXMLArchive.
<br>
<br>
FAQ: 
Q: How do I create xml-archive object for storing data<br>
A: First you need a stream object that can stream the data to persistent storage.
   The stream object has to have two functions. The format for these two functions
   is 

\code
   void Write( const void* pBuf, UINT uCount, UINT* puCount );
   UINT Read( void* pBuf, UINT uCount, UINT* puCount );\endcode
<br>
   Then you could create the XMLStream object for storing data.<br>
   EXAMPLE: (sample on how to use MFC CFile object with CXMLArchive)
\code
   CFile file;
   file.Open( _U("one.xml"), CFile::modeWrite|CFile::modeCreate );
   CXMLArchive<CFile> Archive( &file, CXMLArchive<CFile>::eWrite ); \endcode
<br>
Q: I need to store som data in a node called "DATA" ?<br>
A: EXAMPLE: \code
   int i;
   CFile file;
   file.Open( _U("one.xml"), CFile::modeWrite|CFile::modeCreate );
   CXMLArchive<CFile> Archive( &file, CXMLArchive<CFile>::eWrite );
   i = 1000;

   // you can do like this
   Archive.AddChild( "DATA" );
   Archive.SetAttribute( "NUMBER", NULL );
   Archive.SetItem( (long)i );

   // or like this
   Archive << Child << "DATA" << Attribute << (long)i;\endcode
<br>
Q: I have a node called data and want to extract the text from attribute "VAR"<br>
A: EXAMPLE: 
\code
   char pszBuffer[100];
   Archive.GetAttribute( "VAR", pszBuffer, 100 );\endcode
<br>
Q: I have a node called data and want to extract the long value from attribute "VAR"<br>
A: EXAMPLE: 
\code
   long lValue;
   CXMLItem item( (long)0 );
   Archive.GetItem( "VAR", item );
   lValue = Item;\endcode
<br>
Q: Can I loop through all attributes in a node <br>
A: Yes it is possible to loop the attributes for active node<br>
   EXAMPLE: 
\code
   gd::CString sValue, sName;
   unsigned uCount = Archive.GetAttributeCount();
   for( unsigned u = 0; u < uCount; u++ ) {
      sName = Archive.GetAttributeName( u );
      sValue = Archive.GetAttribute( u );
      ...
   }\endcode
<br>

Q: I need to stream some data, could you give me an example on how to do that<br>
A: EXAMPLE: 
\code
   CFile file;
   file.Open( _U("one.xml"), CFile::modeWrite|CFile::modeCreate );

   CXMLArchive<CFile> Archive( &file, CXMLArchive<CFile>::eWrite );

   Archive << Child << _U("ROOT");
   Archive << Child << _U("DATA") << Attribute << _U("CHAR") << (char)'A' 
                                  << Attribute << _U("SHORT") << (short)100
                                  << Attribute << _U("LONG") << (long)12345678
                                  << Attribute << _U("DOUBLE") << (double)1234.00998877 << EndNode;
	Archive << Child << _U("TEXT") << _U("This is a sample how you can serialize data like the ")
                                     _U("MFC CArchive class");
   Archive << End;\endcode
<br>

Q: One element has a lot of attributes. Is there a way of speeding upp work against attributes?<br>
A: Yes. You can create one attribute index and search in that index to se if the attribute exists.<br>
EXAMPLE: 
\code
   CString stringAttribute;
   CFileText File( eFUTF8 );
   CXMLArchive<CFileText> ar( &File, eRead );
   // ...
   // ...
   File.Open( "myfile.xml", uFlags );
   wchar_t const * ppszAttriute[10];

   ar.SkipHeader();

   while( ar.ReadNext() == true )
   {
      unsigned uCount = ar.MakeAttributeIndex( ppszAttriute, 10 );
      stringAttribute.Empty();
      for( unsigned u = 0; u < uCount; u++ )
      {
         ar.ReadAttributeName( ppszAttriute[u], &stringAttribute );
         // ...
      }
   }\endcode
*/
template<class STREAM>
class CXMLArchive  
{
public:

public:
   enum State
   {
      // OBJECT
      eInternalBuffer      = 0x0001,
      eRead                = 0x0002,
      eWrite               = 0x0004,
      ePreRead             = 0x0008,
	  eFormat			   = 0x0010,
   };

protected:
   enum TagState
   {
      // CURRENT TAG
      eName                = 0x0001,   //!< name has been set
      eValue               = 0x0002,   //!< value has been set
      eHasChild            = 0x0004,   //!< node has childs
      eNeedAttributeValue  = 0x0008,   //!< attribute need a value
      eHasName             = 0x0010,   //!< has name
      eEndTag              = 0x0100,   //!< marks that this is a endtag	
      eCDATA               = 0x1000,

      eChildNext           = 0x0020,   //!< next item is a child item
      eAttributeNext       = 0x0040,   //!< next item is a attribute
      eSiblingNext         = 0x0080,   //!< next item should be a sibling
      eNextMask            = 0x00e0,

      eEndNode             = 1,        //!< end current node
      eEnd                 = 2,        //!< end all nodes
   };


public:
   CXMLArchive( STREAM* pSTREAM, UINT uState = eRead, UINT uBufferSize = 4096, wchar_t* pszBuffer = NULL );
	virtual ~CXMLArchive();

// Operator
public:
   CXMLArchive& operator<<(Operation operation) {
      NextItem( operation ); return *this; }
   CXMLArchive& operator<<(EndOperation operation) {
      EndItem( operation ); return *this; }
   CXMLArchive& operator<<(const CXMLItem& item) {
      SetItem( item ); return *this; }
   CXMLArchive& operator<<(const attribute& a); 
   //! stream char value
   CXMLArchive& operator>>(char& ch) {
      CXMLItem item; item.SetType( CXMLItem::eTI1 ); GetItem( NULL, item ); 
      ch = item; return *this; }
   //! stream unsigned char value
   CXMLArchive& operator>>(unsigned char& ch) {
      CXMLItem item; item.SetType( CXMLItem::eTUI1 ); GetItem( NULL, item ); 
      ch = item; return *this; }
   //! stream short value
   CXMLArchive& operator>>(short& s) {
      CXMLItem item; item.SetType( CXMLItem::eTI2 ); GetItem( NULL, item ); 
      s = item; return *this; }
   //! stream unsigned short value
   CXMLArchive& operator>>(unsigned short& s) {
      CXMLItem item; item.SetType( CXMLItem::eTUI2 ); GetItem( NULL, item ); 
      s = item; return *this; }
   //! stream long value
   CXMLArchive& operator>>(long& l) {
      CXMLItem item; item.SetType( CXMLItem::eTI4 ); GetItem( NULL, item ); 
      l = item; return *this; }
   //! stream unsigned long value
   CXMLArchive& operator>>(unsigned long& l) {
      CXMLItem item; item.SetType( CXMLItem::eTUI4 ); GetItem( NULL, item ); 
      l = item; return *this; }
   //! stream float value
   CXMLArchive& operator>>(float& f) {
      CXMLItem item; item.SetType( CXMLItem::eTR4 ); GetItem( NULL, item ); 
      f = item; return *this; }
   //! stream double value
   CXMLArchive& operator>>(double& d) {
      CXMLItem item; item.SetType( CXMLItem::eTR8 ); GetItem( NULL, item ); 
      d = item; return *this; }
   //! stream character buffer
   CXMLArchive& operator>>(wchar_t*& psz) {
      CXMLItem item; item.SetType( CXMLItem::eTSTR ); GetItem( NULL, item ); 
      psz = item; return *this; }

// get/set
public:
   const wchar_t* GetBuffer() {
      return (const wchar_t*)m_stringBuffer; }
   STREAM* GetStream() {
      return m_pSTREAM; }
   //! returns the depth level for xml tree that current state is in
   UINT GetDepth() {
      return m_stringsStack.GetCount(); }
   //! Return mumber of parents to current item
   UINT GetParentCount();
   // Return string number of current tag
   short GetTagLine(){ return m_TagLine; }

// Operations
public:
   //! ask if curren node had children
   bool HasChildren();
   bool HasNextChildren();
   //! check if the read tag is end of another tag
   bool IsEndTag();
   bool IsNextEndTag();
   //! Insert new child node (WRITE)
   void NewChild( const wchar_t* pszName );
   //! Add sibling to current node (WRITE)
   void NewSibling( const wchar_t* pszName );
   //! Specifiy the type of item next SetItem will add (WRITE)
   void NextItem( Operation operation );
   //! End current or all nodes
   void EndItem( EndOperation operation );
   //! set header for xml file
   void SetHeader( const wchar_t* pszText );
   //! set attribute for node
   void SetAttribute( const wchar_t* pszName, const wchar_t* pszValue );
   //! set value for node
   void SetValue( const wchar_t* pszValue );
   //! set value but convert raw data to hex
   void SetBinaryValue( const BYTE* pbValue, int iLength );
   //! set item in node (what type depends on state for class
   void SetItem( const CXMLItem& item );

   //! skip header section 
   HRESULT SkipHeader();
   //! return section index (how many parents including the node itself) current node has
   unsigned GetSection();
   //! get name for node
   int  GetName( wchar_t* pszBuffer, int iMaxLength ) const;
   //! get name for node in string object
   gd::CString GetName() const;
   // return parent name if one exist
   gd::CString GetParentName() const;
   //! get name for node
   int  GetNextName( wchar_t* pszBuffer, int iMaxLength ) const;
   //! get name for next node in string object
   gd::CString GetNextName() const;
   //! return value for node
   int  GetValue( wchar_t* pszBuffer, int iMaxLength ) const;
   //! get value for node in string object
   gd::CString GetValue() const;
   //! get text after the end tag in string object
   gd::CString GetTextAfterEndTag() const;
   //! return value as binary for node or attribute value (should be in hex form in XML file)
   int  GetBinaryValue( const wchar_t* pszName, BYTE* pBuffer, int iLength ) const;
   //! return number of attributes at current node
   unsigned GetAttributeCount() const;
   //! reads attributename from string pointer.
   static void ReadAttributeName( const wchar_t* pszNameStart, gd::CString* pstringName );
   //! get the name for attribute
   gd::CString GetAttributeName( unsigned uIndex ) const;
   //! get value or attribute from node
   int  GetAttribute( const wchar_t* pszName, wchar_t* pszBuffer, int iMaxLength ) const;
   gd::CString GetAttribute( const wchar_t* pszName ) const;
   //! get the attribute value for attribute at specified index in active node
   int  GetAttribute( unsigned uIndex, wchar_t* pszBuffer, int iMaxLength ) const;
   gd::CString GetAttribute( unsigned uIndex ) const;
   //! find attribute name in node
   int  FindAttribute( const wchar_t* pszName ) const;
   //! set string pointers to start of attribute names in buffer. Pointers are not valid when a new node has been read
   unsigned MakeAttributeIndex( wchar_t const ** ppszIndex, unsigned uCount );
   //! return item as specified type
   HRESULT GetItem( const wchar_t* pszName, CXMLItem& item );
   //! read next node
   bool ReadNext();
   //! read next node
   bool ReadNext( const wchar_t* pszName );
   //! read the next item in order to check what it is. The normal read is not affected by this.
   bool PreReadNext();
   //! remove endnode marker from internal data and return that nodename in string object
   gd::CString PopNode();

// CHECKING
   //! Compare current element name with specified
   bool CompareName( const wchar_t* pszName );

   //! end the node (WRITE)
   static void EndNode( CXMLArchive* pThis );
   //! end all nodes (WRITE)
   void End();

   //! Is this the last node
   bool IsEnd() const;
   //! flush the buffer (READ/WRITE)
   virtual void Flush();

   virtual void Write();
   virtual bool Read();

   // FOUND ERROR ??
   // Function used to check for errors
   bool ValidateActiveTag();
   gd::CString GetActiveTagText();

protected:
   void _Out( wchar_t ch );
   wchar_t _In();
   void _SetValue( const unsigned char* pValue, int iLength );
   int _GetName( wchar_t* pszBuffer, int iMaxLength, const gd::CString* pstringNode ) const;
   gd::CString _GetName( const gd::CString* pstringNode ) const;
   int _GetAttribute( bool bText, const wchar_t* pszName, BYTE* pBuffer, int iMaxLength ) const;
   // copy attribute at position in buffer for current node (position has to point at the " character for attribute)
   int _CopyValue( bool bText, const wchar_t* pszPosition, BYTE* pBuffer, int iMaxLength ) const;
   int _GetAttributeSize( bool bText, const wchar_t* pszName ) const;
   int _GetAttributeSize( bool bText, unsigned uIndex ) const;
   int _FindAttribute( unsigned uIndex ) const;   
   void _SkipTag();
   bool _CompareText( wchar_t* pszText, int iLength );
   static bool _CompareText( CXMLArchive* pThis, wchar_t* pszText, int iLength );

// Attributes
public:
   // STATE
   UINT m_uState;                //!< state for CXMLArchive
   UINT m_uTagState;             //!< state for active tag
   UINT m_uTagStateNext;         //!< state for next active tag (when pre reading)
   short m_sError;               //!< holds the internal error if there is one
   short m_Line;			 // holds string number read from buffer
   short m_TagLine;			 // holds current item string number

   // STREAM
   STREAM* m_pSTREAM;            //!< stream object data is streamed to or from

   // DATA
   gd::CString m_stringBuffer;   //!< internal buffer used for temporary storage
   gd::CString m_stringNext;     //!< Used when we need to read next item in order to check what's comming
   gd::CString m_stringInFront;  //!< characters that has been read but not processed
   gd::CStrings m_stringsStack;  //!< strings for holding the nested tag level
   gd::CBlocks<UINT> m_BState;   //!< holds the state for nested tags

   // BUFFER
   UINT   m_uBufferSize;         //!< size for buffer that has temporary storage
   wchar_t* m_pszStart;            //!< start for buffer
   wchar_t* m_pszPosition;         //!< current position in buffer
   wchar_t* m_pszMax;              //!< last position in buffer that is valid
   wchar_t* m_pszStop;             //!< marks the end and stop read after this
   UINT   m_uScan;               //!< marks position in tag that is scaned

// access
public:
   short& Error() {
      return m_sError; }
};

template<class STREAM>
CXMLArchive<STREAM>& CXMLArchive<STREAM>::operator<<(const attribute& a) {
   _CHECK( (m_uTagState & eNextMask) == 0 );
   SetAttribute( a.m_pszName, NULL );
   SetItem( a.m_Item ); return *this; 
}

template<class STREAM>
__forceinline void CXMLArchive<STREAM>::_Out( wchar_t ch ) {
	if( m_pszPosition + 1 > m_pszMax) Flush();
   *m_pszPosition = ch;
   m_pszPosition++;
}

template<class STREAM>
wchar_t CXMLArchive<STREAM>::_In() {
   wchar_t ch;
   ch = *m_pszPosition;
   if(ch==13) m_Line++;
   m_pszPosition++;
	if( m_pszPosition >= m_pszMax ) Flush();
   return ch; 
}


/////////////////////////////////////////////////
// construct
template<class STREAM>
CXMLArchive<STREAM>::CXMLArchive( STREAM* pSTREAM, UINT uState, UINT uBufferSize, wchar_t* pszBuffer )
{
   _CHECK( pSTREAM != NULL );
   _CHECK( uBufferSize >= 64 ); 

   m_uState          = uState;
   m_uTagState       = 0;
   m_uTagStateNext   = 0;
   m_sError          = 0;           // No error yet...
   m_Line			 = 0;
   m_TagLine		 = 0;
   m_uBufferSize     = uBufferSize;
   m_pSTREAM         = pSTREAM;

   m_pszStart = pszBuffer;
   if( m_pszStart == NULL ) 
   {
      m_uState |= eInternalBuffer;
      m_pszStart = MP_NEW_ARR( wchar_t, uBufferSize);
   }

   // Read or write flag should be set
   _CHECK( ((uState & (eWrite|eRead)) != 0) && ((uState & (eWrite|eRead)) <= eWrite) );
   if( uState & eWrite )
   {
      m_pszMax = m_pszStart + uBufferSize;
   }
   else
   {
      m_pszMax = m_pszStart;
   }
   m_pszPosition = m_pszStart;
   m_pszStop = (wchar_t*)-1;
}

/////////////////////////////////////////////////
// destruct
template<class STREAM>
CXMLArchive<STREAM>::~CXMLArchive()
{
   Flush();
   if( m_uState & eInternalBuffer )
   {
      MP_DELETE_ARR( m_pszStart);
   }
}

/*---------------------------------------------------------------------------------*/ /**
 * Return number of parent elements for active
 * \return number of parent elements for the active element
 */
template<class STREAM>
UINT CXMLArchive<STREAM>::GetParentCount()
{
   unsigned uParents = m_stringsStack.GetCount();
   if( uParents && CompareName( m_stringsStack.GetString( m_stringsStack.GetCount() - 1 ) ) == true )
   {
      uParents--;
   }

   return uParents;
}

/////////////////////////////////////////////////
//! check if current node had children
template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::HasChildren() {
   return ( m_uTagState & eHasChild ) ? true : false; }

template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::HasNextChildren() {
   return ( m_uTagStateNext & eHasChild ) ? true : false; }

/////////////////////////////////////////////////
//! check if it is a end tag
template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::IsEndTag() {
   return ( m_uTagState & eEndTag ) ? true : false; }

template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::IsNextEndTag() {
   return ( m_uTagStateNext & eEndTag ) ? true : false; }

/////////////////////////////////////////////////
//! add a new child node
template<class STREAM>
void CXMLArchive<STREAM>::NewChild( const wchar_t* pszName /*!< name of node that is added*/ )
{
   _CHECK( !(m_uTagState & eNeedAttributeValue) );
   _CHECK( m_uState & eWrite );
//   _CHECK_S( pszName );

   if( m_uTagState != 0 )
   {
      // if name for previus node has been set but no value, then end the tag before adding child
      if( m_uTagState & eHasName && !(m_uTagState & eValue) ) m_stringBuffer += ">";
      m_uTagState |= eHasChild;
      m_uTagState &= ~(eNextMask|eHasName);
      m_BState.Push( &m_uTagState );
      Write();
   }

	if(m_uState & eFormat){
		m_stringBuffer+="\n";
		int iDepth=m_stringsStack.GetCount();
		for(int ii=0; ii<iDepth; ii++) m_stringBuffer+="\t";
	}
   m_uTagState = eName|eHasName;
   m_stringBuffer += "<";
   m_stringBuffer += pszName;
   m_stringsStack.Push( pszName );
}

/*! create new sibling for node. This means that the created node has the same parent as
   the prevoius node. */
template<class STREAM>
void CXMLArchive<STREAM>::NewSibling( const wchar_t* pszName /*!< name of the sibling*/ )
{
   _CHECK( m_uState & eWrite );

   if( pszName == NULL )
   {
      EndNode( this );
      return;
   }

//   _CHECK_S( pszName );

   if( m_uTagState != 0 )
   {
      // if previous tag had a value then add ending tag with name
      if( m_uTagState & eValue )
      {
		if(m_uState & eFormat){
			m_stringBuffer+="\n";
			int iDepth=m_stringsStack.GetCount()-1;
			for(int ii=0; ii<iDepth; ii++) m_stringBuffer+="\t";
		}
         m_stringBuffer << L"</" << m_stringsStack.Pop() << L">";
      }
      else
      {
         // no value so just end tag with end marker
         m_stringsStack.Pop();
         m_stringBuffer << L"/>";
      }

      Write();
   }

   m_uTagState = eName|eHasName;
   m_stringBuffer += "<";
   m_stringBuffer += pszName;
   m_stringsStack.Push( pszName );
}

/////////////////////////////////////////////////
//! inform what type next item in stream will be for object
template<class STREAM>
__forceinline void CXMLArchive<STREAM>::NextItem( Operation operation /*!< the next type of operation*/ ) {
   _CHECK( (m_uTagState & eNextMask) == 0 );
   m_uTagState |= operation;
}

/////////////////////////////////////////////////
//! end current or all nodes
template<class STREAM>
void CXMLArchive<STREAM>::EndItem( EndOperation operation /*!< type of end operation*/ )
{
   if( operation == eEndNode ) EndNode( this );
   else End();
}

/////////////////////////////////////////////////
//! set header text for xml file
template<class STREAM>
void CXMLArchive<STREAM>::SetHeader( const wchar_t* pszText /*!< header text */ )
{
   _CHECK( m_uState & eWrite );
   _CHECK_S( pszText );
   _CHECK( m_uTagState == 0 );
   m_stringBuffer << L"<?xml " << pszText << L"?>";
}

/////////////////////////////////////////////////
//! set attribute name and value
template<class STREAM>
void CXMLArchive<STREAM>::SetAttribute( const wchar_t* pszName, /*!< name for attribute*/ 
                                        const wchar_t* pszValue /*!< value for attribute */ )
{
   _CHECK( m_uState & eWrite );
//   _CHECK_S( pszName );
   _CHECK( m_uTagState & eName );
   _CHECK( !(m_uTagState & eValue) );

   m_stringBuffer << L" " << pszName;
   if( pszValue != NULL )
   {
//      _CHECK_S( pszValue );
      m_stringBuffer.Add( L"=\"" );
      TextToXML( pszValue, &m_stringBuffer );
      m_stringBuffer.Add( L"\"" );
   }
   else
   {
      m_uTagState |= eNeedAttributeValue;
   }
}

/////////////////////////////////////////////////
//! set value for attribute or value for tag
template<class STREAM>
__forceinline void CXMLArchive<STREAM>::SetValue( const wchar_t* pszValue ) {
   _SetValue( (const unsigned char*)pszValue, -1 ); 
}

template<class STREAM>
__forceinline void CXMLArchive<STREAM>::SetBinaryValue( const BYTE* pbValue, int iLength ) {
   _CHECK( iLength >= 0 );
   _SetValue( pbValue, iLength ); 
}


template<class STREAM>
void CXMLArchive<STREAM>::_SetValue( const unsigned char* pValue, int iLength )
{
   _CHECK( m_uState & eWrite );
   _CHECK( m_uTagState & eName );
   _CHECK( !(m_uTagState & eNextMask) );
   _CHECK( !(m_uTagState & eValue) );

   if( m_uTagState & eNeedAttributeValue )
   {
      m_stringBuffer << L"=\"";
   }
   else
   {
      m_stringBuffer << L">";
		if(m_uState & eFormat){
			m_stringBuffer+="\n";
			int iDepth=m_stringsStack.GetCount();
			for(int ii=0; ii<iDepth; ii++) m_stringBuffer+="\t";
		}
   }

   if( iLength == -1 )
   {
      _CHECK_S( (const wchar_t*)pValue );
      TextToXML( (const wchar_t*)pValue, &m_stringBuffer );
   }
   else
   {
      BinToHex( pValue, m_stringBuffer.GetBufferEnd( iLength * 2 + 1 ), iLength );
      m_stringBuffer.GetBufferSetLength( m_stringBuffer.GetLength() + iLength * 2 );
   }

   if( m_uTagState & eNeedAttributeValue )
   {
      m_uTagState &= ~eNeedAttributeValue;
      m_stringBuffer << L"\"";
   }
   else
   {
      m_uTagState |= eValue;
   }
}

#define _LONG_MAX 20  //!< max size for buffer when long value is set
#define _DOUBLE_MAX  40 //!< max size for double value when double is set
/////////////////////////////////////////////////
//! set value for attribute, value, child node or sibling
template<class STREAM>
void CXMLArchive<STREAM>::SetItem( const CXMLItem& item /*!< item that is set to stream */ )
{
   _CHECK( m_uState & eWrite );
   if( m_uTagState & eNextMask )
   {
      // it has to be a name if child, sibling or attribute has been set
      _CHECK( item.GetType() == CXMLItem::eTSTR );
      if( m_uTagState & eChildNext ) NewChild( item.GetData().valpsz );
      else if( m_uTagState & eSiblingNext ) NewSibling( item.GetData().valpsz );
      else SetAttribute( item.GetData().valpsz, NULL );

      m_uTagState &= ~eNextMask;
      return;
   }

   _CHECK( m_uTagState & eName );
   _CHECK( !(m_uTagState & eValue) );

   union Var { long l; unsigned long ul; double d; } V;

   if( m_uTagState & eNeedAttributeValue )
   {
      m_stringBuffer << L"=\"";
   }
   else
   {
      m_stringBuffer << L">";
   }

   switch( item.GetType() )
   {
   case CXMLItem::eTUI1 :
      V.ul = item.GetData().valUChar;
      goto u4;
   case CXMLItem::eTUI2 :
      V.ul = item.GetData().valUShort;
      goto u4;
   case CXMLItem::eTUI4 :
      V.ul = item.GetData().valULong;
      goto u4;
   case CXMLItem::eTI1 :
      V.ul = item.GetData().valChar;
      goto u4;
   case CXMLItem::eTI2 :
      V.l = item.GetData().valShort;
      goto i4;
   case CXMLItem::eTI4 :
      V.l = item.GetData().valLong;
      goto i4;
   case CXMLItem::eTR4 :
      V.d = item.GetData().valFloat;
      goto r8;
   case CXMLItem::eTR8 :
      V.d = item.GetData().valDouble;
      goto r8;
   case CXMLItem::eTSTR :
      goto str;

   default:
      _CHECK(FALSE);

   }

u4:
   _ultow( V.ul, m_stringBuffer.GetBufferEnd( _LONG_MAX ), 10 );
   m_stringBuffer.ReleaseBufferFromEnd();
   goto exit;
i4:
   _ltow( V.l, m_stringBuffer.GetBufferEnd( _LONG_MAX ), 10 );
   m_stringBuffer.ReleaseBufferFromEnd();
   goto exit;
r8:
//#ifndef _UNICODE
//   _gcvt( V.d, _DOUBLE_MAX - 2, m_stringBuffer.GetBufferEnd( _DOUBLE_MAX ) );
//   m_stringBuffer.ReleaseBufferFromEnd();
//#else
   char pbszBuffer[_DOUBLE_MAX];
   _gcvt( V.d, _DOUBLE_MAX - 2, pbszBuffer );
   m_stringBuffer += pbszBuffer;
//#endif
   goto exit;

str:
   TextToXML( item.GetData().valpsz, &m_stringBuffer );

exit:
   if( m_uTagState & eNeedAttributeValue )
   {
      m_uTagState &= ~eNeedAttributeValue;
      m_stringBuffer << L"\"";
   }
   else
   {
      m_uTagState |= eValue;
   }
}


template<class STREAM>
HRESULT CXMLArchive<STREAM>::SkipHeader()
{
   wchar_t ch;
   UINT uLength;
   bool bBreak;
	
   if( (int)m_pszPosition >= (int)m_pszStop ) Flush();

   while( !((ch = _In()) == -1) ) 
   {
      if( (ch > L' ') && (ch != L'<') ) return E_FAIL;
      
      if( ch == L'<' ) break;

      if( ch == L'\0' ) return S_OK;
   }

   do
   {
	   if(ch == 13) 
		   m_Line++;
      if( ch == L'<' )
      {
         bBreak = false;
         // start of tag has been found. Now we need to check next character. If this is a ? or !
         // then skip this node.
         ch = 0;
   	   if( m_pszPosition >= m_pszMax )
         {
            // we can't go backwards when reading. and here we check in front of the current position.
            // because of this we decrease the buffer that is read to and save space for one character 
            // at start.
            ch = L'<';    // set ch "hard". This is later checked in order to se if we have moved in buffer.
            *m_pszStart = ch;
            m_pszStart++;
            m_uBufferSize--;
            Flush();
         }

         if( *m_pszPosition != L'?' && *m_pszPosition != L'!' )
         {
            // This is not a header. Start scaning and stop checking characters that is skiped
            bBreak = true;
         }

         if( ch != 0 )
         {
            m_pszStart--;
            m_uBufferSize++;
         }

         if( bBreak == true )
         {
            m_pszPosition--;
            break;
         }
      }
   } while( !((ch = _In()) == -1) );

   uLength = m_pszMax - m_pszPosition;
   memmove( m_pszStart, m_pszPosition, uLength * sizeof(wchar_t) );
   m_pszMax -= (m_pszPosition - m_pszStart); 
   m_pszStop = m_pszMax - 1;
   m_pszPosition = m_pszStart;
   *m_pszMax = L'\0';

   return S_OK;
}

/////////////////////////////////////////////////
//! return level
template<class STREAM>
__forceinline unsigned CXMLArchive<STREAM>::GetSection() {
   return m_stringsStack.GetCount();
}

/////////////////////////////////////////////////
//! return the node name
template<class STREAM>
__forceinline int CXMLArchive<STREAM>::GetName( wchar_t* pszBuffer, int iMaxLength ) const {
   return _GetName( pszBuffer, iMaxLength, &m_stringBuffer );
}

/////////////////////////////////////////////////
//! return parent node name if one exist
template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetParentName() const {
   if( m_stringsStack.GetCount() > 1 ) {
      return gd::CString( m_stringsStack.GetString( m_stringsStack.GetCount() - 2 ) ); 
   }
   return gd::CString();
}

/////////////////////////////////////////////////
//! return the node name that has been preread. this is for looking forward in xml file
template<class STREAM>
__forceinline int CXMLArchive<STREAM>::GetNextName( wchar_t* pszBuffer, int iMaxLength ) const {
   return _GetName( pszBuffer, iMaxLength, &m_stringNext );
}

template<class STREAM>
int CXMLArchive<STREAM>::_GetName( wchar_t* pszBuffer, int iMaxLength, const gd::CString* pstringNode ) const
{
   int    iLength;      // name length
   wchar_t* pszPosition;
   wchar_t* pszEnd;

   pszPosition = pstringNode->GetBuffer();

   if( pszBuffer == NULL ) pszEnd = (wchar_t*)-1;
   else pszEnd = pszPosition + iMaxLength - 1;

   while( *pszPosition >= L'0' && pszPosition != pszEnd ) pszPosition++;

   iLength = pszPosition - pstringNode->GetBuffer();
   if( pszBuffer != 0 )
   {
      memcpy( pszBuffer, pstringNode->GetBuffer(), iLength * sizeof(wchar_t) );
      pszBuffer[iLength] = 0;
   }

   return iLength;
}

/////////////////////////////////////////////////
//! return the node name
template<class STREAM>
__forceinline gd::CString CXMLArchive<STREAM>::GetName() const {
   return _GetName( &m_stringBuffer );
}

/////////////////////////////////////////////////
//! return the next node name
template<class STREAM>
__forceinline gd::CString CXMLArchive<STREAM>::GetNextName() const {
   return _GetName( &m_stringNext );
}

template<class STREAM>
gd::CString CXMLArchive<STREAM>::_GetName( const gd::CString* pstringNode ) const
{
   wchar_t* pszPosition;
   gd::CString stringName( L"" );

   pszPosition = pstringNode->GetBuffer();
   if( pszPosition == NULL ) return stringName;

   while( *pszPosition >= L'0' )
   {
      stringName.FastAdd( *pszPosition );
      pszPosition++;
   }

   return stringName;
}

/////////////////////////////////////////////////
//! get value from active node
//! returns number of characters that was copied
template<class STREAM>
int CXMLArchive<STREAM>::GetValue( wchar_t* pszBuffer, int iMaxLength ) const
{
   wchar_t* pszPosition;
   wchar_t* pszCheck;

   pszPosition = m_stringBuffer.GetBuffer();
   // find value
   while( *pszPosition != 0 && *pszPosition != L'>' ) pszPosition++;

   if( *pszPosition != L'>' ) return -1;
   pszCheck = pszPosition - 2;
   while( *pszCheck <= L' ' ) pszCheck--;
   if( *pszCheck == L'/' )
   {
      return -1;
   }
   
   pszPosition++;

   return CopyXMLText( pszPosition, pszBuffer, iMaxLength );
}

template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetValue() const
{
   wchar_t* pszPosition;
   wchar_t* pszCheck;
   gd::CString stringValue;

   pszPosition = m_stringBuffer.GetBuffer();
   // find value
   while( *pszPosition != 0 && *pszPosition != L'>' ) pszPosition++;
   if( *pszPosition != L'>' ) return stringValue;

   pszCheck = pszPosition - 2;
   while( *pszCheck <= L' ' ) pszCheck--;
   if( *pszCheck == L'/' )
   {
      return stringValue;
   }

   pszPosition++;

   if( !(m_uTagState & eCDATA) )
   {
      CopyXMLText( pszPosition, &stringValue );
   }
   else
   {
      stringValue = pszPosition;
   }
   
   return stringValue;
}

template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetTextAfterEndTag() const
{
	gd::CString stringText;

	if(*m_pszPosition == L'>'){ // end tag or <../>
		wchar_t* pwc=m_pszPosition;
		pwc++;
		while(*pwc != L'<'){
			if(pwc == m_pszMax) return stringText;
			pwc++;
		}
		pwc--;
		stringText.Add(m_pszPosition+1,pwc-m_pszPosition);
	}else{ // has no children
		wchar_t* pwc=m_pszPosition;
		while(*pwc != L'>'){
			if(pwc == m_pszStart) return stringText;
			pwc--;
		}
		pwc++;
		stringText.Add(pwc,m_pszPosition-pwc);
	}
	return stringText;
}


/////////////////////////////////////////////////
//! return value as binary (value should be in hex format in the XML file)
template<class STREAM>
__forceinline int CXMLArchive<STREAM>::GetBinaryValue( const wchar_t* pszName, BYTE* pBuffer, int iLength ) const {
   return _GetAttribute( false, pszName, pBuffer, iLength );
}

/////////////////////////////////////////////////
//! return number of attribtues in active node
template<class STREAM>
unsigned CXMLArchive<STREAM>::GetAttributeCount() const {
   unsigned uCount;
   const wchar_t* pszPosition;

   // count " characters and divide it by two in order to get how many attributes the node has.
   uCount = 0;
   pszPosition = m_stringBuffer.GetBuffer();
   while( *pszPosition != 0 && *pszPosition != L'>' ) 
   {
      if( *pszPosition == L'\"' ) uCount++;
      pszPosition++;
   }

   return ( uCount >> 1 );
}

/////////////////////////////////////////////////
//! read attribute name from sent string pointer. String pointer needs to point at start of 
//! attribute.
template<class STREAM>
__forceinline void CXMLArchive<STREAM>::ReadAttributeName( const wchar_t* pszNameStart, gd::CString* pstringName ) {
   pstringName->Single();
   while( (*pszNameStart > L' ') && (*pszNameStart != L'=') )
   {
      pstringName->FastAdd( *pszNameStart );
      pszNameStart++;
   }
}

/////////////////////////////////////////////////
//! get attribute name for specified index
template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetAttributeName( unsigned uIndex ) const {
   int iPosition;
   const wchar_t* pszStart;
   const wchar_t* pszPosition;
   gd::CString string;

   iPosition = _FindAttribute( uIndex );
   if( iPosition != -1 )
   {
      string.Single();
      pszStart = m_stringBuffer.GetBuffer();
      pszPosition = pszStart + iPosition;
      // go back to "="
      while( (pszPosition != pszStart) && (*pszPosition != L'=') ) pszPosition--;
      pszPosition--;
      // go back to first character
      while( (pszPosition != pszStart) && (*pszPosition < L' ') ) pszPosition--;
      // go back to start av name
      while( (pszPosition != pszStart) && ((*pszPosition > L' ') && (*pszPosition != L'"'))  ) pszPosition--;
      pszPosition++;

      // start copy
      while( (*pszPosition > L' ') && (*pszPosition != L'=') )
      {
         string.FastAdd( *pszPosition );
         pszPosition++;
      }
   }

   return string;
}

/////////////////////////////////////////////////
//! return text for attribute
template<class STREAM>
__forceinline int CXMLArchive<STREAM>::GetAttribute( const wchar_t* pszName, wchar_t* pszBuffer, int iMaxLength ) const {
   return _GetAttribute( true, pszName, (BYTE*)pszBuffer, iMaxLength );
}

template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetAttribute( const wchar_t* pszName ) const
{
   gd::CString string;

   int iLength = _GetAttributeSize( true, pszName );
   if( iLength > 0 )
   {
      iLength++;
      _GetAttribute( true, pszName, (BYTE*)string.GetBuffer( iLength ), iLength );
      string.ReleaseBuffer();
   }

   return string;
}

/////////////////////////////////////////////////
//! copy value for attribute at specified index
template<class STREAM>
int CXMLArchive<STREAM>::GetAttribute( unsigned uIndex, wchar_t* pszBuffer, int iMaxLength ) const
{
   iPosition = _FindAttribute( uIndex );
   if( iPosition != -1 )
   {
      return _CopyValue( true, m_stringBuffer.GetBuffer() + iPosition, pszBuffer, iMaxLength );
   }
   return -1;
}

/////////////////////////////////////////////////
//! get value as CString for attribute at specified index
template<class STREAM>
gd::CString CXMLArchive<STREAM>::GetAttribute( unsigned uIndex ) const 
{
   int iPosition;
   gd::CString string;

   iPosition = _FindAttribute( uIndex );
   if( iPosition != -1 )
   {
      int iLength = _GetAttributeSize( true, uIndex ) + 1;
      _CopyValue( true, m_stringBuffer.GetBuffer() + iPosition, (BYTE*)string.GetBuffer( iLength + 1 ), iLength );
      string.ReleaseBuffer();
   }

   return string;
}


/////////////////////////////////////////////////
//! get value for attribute and return length for value that is copied.
//! returns -1 if attribute was not found
template<class STREAM>
int CXMLArchive<STREAM>::_GetAttribute( bool bText, const wchar_t* pszName, BYTE* pBuffer, int iMaxLength ) const
{
   _CHECK( iMaxLength > 0 );

   int iPosition;
   wchar_t chEnd;
   const wchar_t* pszPosition;

   if( pszName != NULL )
   {
      iPosition = FindAttribute( pszName );
      if( iPosition == -1 ) return -1;

      pszPosition = m_stringBuffer.GetBuffer() + iPosition;
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == 0 ) return -1;
   }
   else
   {
      pszPosition = m_stringBuffer.GetBuffer();
      while( *pszPosition != 0 && *pszPosition != _U('>') ) pszPosition++;

      if( *pszPosition == 0 ) return -1;
      pszPosition++;
      chEnd = 0;
   }

   return _CopyValue( bText, pszPosition, pBuffer, iMaxLength );
}

/////////////////////////////////////////////////
//! copy value that pszPosition points at into buffer
//! pszPosition has to point at a valid value !!
//! returns number of copied characters to buffer
template<class STREAM>
int CXMLArchive<STREAM>::_CopyValue( bool bText, const wchar_t* pszPosition, BYTE* pBuffer, int iMaxLength ) const
{
   wchar_t chEnd = 0;

   if( *pszPosition == _U('"') )
   {
      pszPosition++;
      chEnd = _U('"');
   }

   if( bText == true )
   {
      return CopyXMLText( pszPosition, (wchar_t*)pBuffer, iMaxLength, chEnd );
   }

   return HexToBin( pszPosition, pBuffer, iMaxLength, chEnd );
}

template<class STREAM>
int CXMLArchive<STREAM>::_GetAttributeSize( bool bText, const wchar_t* pszName ) const
{
   int iPosition;
   wchar_t chEnd;
   const wchar_t* pszPosition;

   if( pszName != NULL )
   {
      iPosition = FindAttribute( pszName );
      if( iPosition == -1 ) return -1;

      pszPosition = m_stringBuffer.GetBuffer() + iPosition;
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == 0 ) return -1;
      pszPosition++;
      chEnd = _U('"');
   }
   else
   {
      pszPosition = m_stringBuffer.GetBuffer();
      while( *pszPosition != 0 && *pszPosition != _U('>') ) pszPosition++;

      if( *pszPosition == 0 ) return -1;
      pszPosition++;
      chEnd = 0;
   }

   if( bText == true )
   {
      return XMLToTextLength( pszPosition, chEnd );
   }

   return HexToBinLength( pszPosition, chEnd );
}

/*---------------------------------------------------------------------------------*/ /**
 * Calculate attribute length
 * \param bText true if attribute is text or false if it is hex
 * \param uIndex index for attrbute that lenght is calculated for
 * \return attribute lenght i characters or -1 if attribute was't found
 */
template<class STREAM>
int CXMLArchive<STREAM>::_GetAttributeSize( bool bText, unsigned uIndex ) const
{
   int iPosition;
   const wchar_t* pszPosition;

   iPosition = _FindAttribute( uIndex );
   if( iPosition == -1 ) return -1;

   pszPosition = m_stringBuffer.GetBuffer() + iPosition;
   pszPosition++;

   if( bText == true )
   {
      return XMLToTextLength( pszPosition, _U('"') );
   }

   return HexToBinLength( pszPosition, _U('"') );
}


template<class STREAM>
int CXMLArchive<STREAM>::_FindAttribute( unsigned uIndex ) const {
   const wchar_t* pszPosition;
   pszPosition = m_stringBuffer;

   uIndex <<= 1;

   while( *pszPosition != 0 && *pszPosition != _U('>') ) 
   {
      if( *pszPosition == _U('"') ) 
      {
         if( uIndex == 0 )  return pszPosition - m_stringBuffer.GetBuffer();
         uIndex--;
      }
      pszPosition++;
   }

   return -1;
}


/////////////////////////////////////////////////
//! find attribute name and return index for name in string or -1 if not found
template<class STREAM>
int CXMLArchive<STREAM>::FindAttribute( const wchar_t* pszName ) const
{
   _CHECK_S( pszName );

   const wchar_t* pszPosition;
   const wchar_t* psz1;
   const wchar_t* psz2;

   pszPosition = m_stringBuffer;

   // move past name
   while( *pszPosition != 0 && *pszPosition > _U(' ') ) pszPosition++;
   if( *pszPosition == 0 || *pszPosition == _U('>') ) return -1;

   while( true )
   {
      // Move to name
      while( *pszPosition != 0 && *pszPosition <= _U(' ') ) pszPosition++;
      if( *pszPosition == 0 || *pszPosition == _U('>') ) return -1;
      if( *pszPosition == *pszName )
      {
         psz1 = pszName;
         psz2 = pszPosition;
         while( *psz1 == *psz2 )
         {
            psz1++;
            psz2++;
         }

         // check if attribute was found
         if( *psz1 == 0 && ((*psz2 == _U('=')) || (*psz2 <= _U(' '))) )
         {
            return pszPosition - m_stringBuffer.GetBuffer();
         }

         pszPosition = psz2;
      }

      // pass value for attribute
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == _U('"') ) pszPosition++;
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == 0 ) return -1;
      pszPosition++;
   }

   return -1;
}

/////////////////////////////////////////////////
//! Create index for attribute possitions in buffer. Each time Archive class reads one element
//! it stores data for the element to the internal buffer. This function finds start address
//! of each attribute and store the pointer to the string string pointer.
template<class STREAM>
unsigned CXMLArchive<STREAM>::MakeAttributeIndex( wchar_t const **  ppszIndex, unsigned uCount )
{
   unsigned uIndex;
   const wchar_t* pszPosition;

   pszPosition = m_stringBuffer;

   // move past name
   while( *pszPosition != 0 && *pszPosition > _U(' ') ) pszPosition++;
   if( *pszPosition == 0 || *pszPosition == _U('>') ) return 0;

   uIndex = 0;
   while( uCount )
   {
      uCount--;

      // Move to name
      while( *pszPosition != 0 && *pszPosition <= _U(' ') ) pszPosition++;
      if( *pszPosition == 0 || *pszPosition == _U('>') ) return uIndex;

      ppszIndex[uIndex] = pszPosition;
      uIndex++;

      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == _U('"') ) pszPosition++;
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
      if( *pszPosition == 0 ) return uIndex;

      pszPosition++;
   }

   return uIndex;
}

/////////////////////////////////////////////////
//! read attribute value into item object. item object has tha type that value is converted
//! to. but it has no value yet.
template<class STREAM>
HRESULT CXMLArchive<STREAM>::GetItem( const wchar_t* pszName,/*!< name of the attribute that value is read from*/
                                      CXMLItem& item /*! reference to item that gets value */ )
{
   int    iPosition;
   wchar_t  chSave;
   wchar_t* pszEnd;
   const wchar_t* pszPosition;
   union Var { long l; unsigned long ul; double d; } V;

   // find value that should be scaned
   if( pszName == NULL )
   {
      pszPosition = m_stringBuffer.GetText( m_uScan );
      while( *pszPosition != 0 && *pszPosition != _U('>') && *pszPosition != _U('"') ) pszPosition++;
      chSave = *pszPosition;
   }
   else
   {
      iPosition = FindAttribute( pszName );
      if( iPosition == -1 ) return E_FAIL;
      pszPosition = m_stringBuffer.GetBuffer() + iPosition;
      while( *pszPosition != 0 && *pszPosition != _U('"') ) pszPosition++;
   }

   if( *pszPosition != 0 ) pszPosition++;
   if( *pszPosition == 0 ) return E_FAIL;

   switch( item.GetType() )
   {
   case CXMLItem::eTUI1 :
   case CXMLItem::eTUI2 :
   case CXMLItem::eTUI4 :
      goto u4;
   case CXMLItem::eTI1 :
   case CXMLItem::eTI2 :
   case CXMLItem::eTI4 :
      goto i4;
   case CXMLItem::eTR4 :
   case CXMLItem::eTR8 :
      goto r8;
   case CXMLItem::eTSTR :
      goto exit;
   }

u4:
   V.ul = _tcstoul( pszPosition, &pszEnd, 10  );
   goto exit;
i4:
   V.l = _ttol( pszPosition );
   goto exit;
r8: 
   V.d = _tcstod( pszPosition, &pszEnd );

exit:

   switch( item.GetType() )
   {
   case CXMLItem::eTUI1 :  item.m_V.valUChar = (unsigned char)V.ul; break;
   case CXMLItem::eTUI2 :  item.m_V.valUShort = (unsigned short)V.ul; break;
   case CXMLItem::eTUI4 :  item.m_V.valULong = V.ul; break;
   case CXMLItem::eTI1 :   item.m_V.valChar = (char)V.l; break;
   case CXMLItem::eTI2 :   item.m_V.valShort = (short)V.l; break;
   case CXMLItem::eTI4 :   item.m_V.valLong = V.l; break;
   case CXMLItem::eTR4 :   item.m_V.valFloat = (float)V.d; break;
   case CXMLItem::eTR8 :   item.m_V.valDouble = V.d; break;
   case CXMLItem::eTSTR :  item.m_V.valpsz = pszPosition; break;
   default: _ASSERT(FALSE);
   }

   // move to next value
   if( pszName != NULL )
   {
      while( *pszPosition != 0 && *pszPosition != L'"' ) pszPosition++;
      if( *pszPosition != 0 ) pszPosition++;
      m_uScan = pszPosition - m_stringBuffer.GetBuffer();
   }

   return 0;
}

/////////////////////////////////////////////////
//! read next item in XML text
template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::ReadNext() {
   return Read();
}

/////////////////////////////////////////////////
//! read next specified element in XML text
template<class STREAM>
bool CXMLArchive<STREAM>::ReadNext( const wchar_t* pszName ) {
   unsigned uLength = _tcslen( pszName );
   while( Read() )
   {
      if( IsEndTag() ) continue;
      if( _tcsncmp( pszName, m_stringBuffer, uLength ) == 0 && m_stringBuffer[uLength] <= L' ' ) return true;
   }

   return false;
}

/////////////////////////////////////////////////
//! read next item in XML text
template<class STREAM>
__forceinline bool CXMLArchive<STREAM>::PreReadNext() {
   m_uState |= ePreRead;
   return Read();
}

/////////////////////////////////////////////////
//! pop nodename from internal data of CXMLArchive
template<class STREAM>
gd::CString CXMLArchive<STREAM>::PopNode() {
   _ASSERT( m_stringsStack.GetCount() );
   gd::CString string = m_stringsStack.Pop();
   return string;
}

/*---------------------------------------------------------------------------------*/ /**
 * Compare active element name to specified
 * \param pszName name that is compared with active element name
 * \return true if names are the same or false if they are not
 */
template<class STREAM>
bool CXMLArchive<STREAM>::CompareName( const wchar_t* pszName )
{
   const wchar_t* pszPosition, * pszStart;
   pszStart = pszPosition = m_stringBuffer;

   if( *pszPosition != *pszName ) return false;

   while( *pszPosition >= L'0' ) pszPosition++;

   return( _tcsncmp( pszName, pszStart, pszPosition - pszStart ) == 0 );
}


/////////////////////////////////////////////////
//! end current node
template<class STREAM>
void CXMLArchive<STREAM>::EndNode( CXMLArchive<STREAM>* pThis )
{
   _CHECK( pThis->m_uState & eWrite );
   if( pThis->m_uTagState != 0 )
   {
      if( pThis->m_uTagState & (eValue|eHasChild) )
      {
		if(pThis->m_uState & eFormat){
			pThis->m_stringBuffer+="\n";
			int iDepth=pThis->m_stringsStack.GetCount()-1;
			for(int ii=0; ii<iDepth; ii++) pThis->m_stringBuffer+="\t";
		}
         pThis->m_stringBuffer << L"</" << pThis->m_stringsStack.Pop() << L">";
      }
      else
      {
         pThis->m_stringsStack.Pop();
         pThis->m_stringBuffer << L"/>";
      }
      pThis->Write();
      if( pThis->m_BState.GetSize() > 0 )
      {
         pThis->m_uTagState = *pThis->m_BState.Pop();
      }
      else
      {
         pThis->m_uTagState = 0;
      }
   }
}

/////////////////////////////////////////////////
//! end all tags
template<class STREAM>
void CXMLArchive<STREAM>::End()
{
   _CHECK( m_uState & eWrite );
   UINT uCount;

   uCount = m_stringsStack.GetCount();
   while( uCount != 0 )
   {
      uCount--;
      EndNode( this );
   }

   Flush();
}

/////////////////////////////////////////////////
//! check if end of stream is found
template<class STREAM>
bool CXMLArchive<STREAM>::IsEnd() const
{
   if((m_pszPosition > m_pszStop) || (*m_pszPosition == L'\0')) return true;
   return false;
}

/////////////////////////////////////////////////
//! flush buffer
/*---------------------------------------------------------------------------------*/ /**
 * Flush buffer
 \note CXMLArchive process the XML file in sections. If the XML file is very large then it
 could be trouble if there isn't enough memory. By using buffered reading and writing this isn't
 a problem.<br><br>

\verbatim
Internal members used flushing buffer:
--------------------------------------
m_pszStart:       points to start of buffer
m_uBufferSize:    holds the buffer size 
m_pszPosition:    points to current position
m_pszStop:        points to last valid position in buffer
m_pszMax:         points to last valid position in buffer 
m_stringInFront:  holds characters if there has been a preread to find specific items but 
                  internal buffer didn't hold all the characters search for.\endverbatim
 */
template<class STREAM>
void CXMLArchive<STREAM>::Flush()
{
   UINT uBytes;
   if( m_uState & eRead )
   {
      unsigned uBufferSize = m_uBufferSize;
      wchar_t* pszStart = m_pszStart;
      if( m_stringInFront.GetLength() )
      {
         wcscpy( m_pszStart, m_stringInFront );
         uBufferSize -= m_stringInFront.GetLength();
         pszStart += m_stringInFront.GetLength();
         m_stringInFront.Empty();
      }
      uBytes = 0;
      m_pSTREAM->Read( pszStart, uBufferSize * sizeof(wchar_t), &uBytes );
      m_pszMax = (wchar_t*)((UINT)pszStart + uBytes);
      if( uBytes < (uBufferSize * sizeof(wchar_t)) )
      {
	     m_pszStop = m_pszMax - 1;
		 if( uBytes != 0)
			_ASSERT( *m_pszStop < 1 || *m_pszStop > 8 ); // No crummy characters
         if( m_pszStop < m_pszStart ) m_pszStop = m_pszStart;
         if( uBytes == 0 ) *m_pszStart = L'\0';          // no read!! then set end
      }else{
	     m_pszStop = m_pszMax - 1;
      }
   }
   else
   {
      m_pSTREAM->Write( m_pszStart, (UINT)m_pszPosition - (UINT)m_pszStart, NULL );
   }

   m_pszPosition = m_pszStart;
}

/////////////////////////////////////////////////
//! read next XML tag
/*---------------------------------------------------------------------------------*/ /**
 * Read is the main workhorse parsing XML files.<br>
 Normaly you read node for node, check what type of node it is and exectues functionality 
 depending on the node. Often there is a need to check the next node in the xml file. If the
 active node contains child nodes and what that type of child is.<br>
 CXMLArchive can read one node ahead to se whats coming. This is often what you need to
 work the file. If you need to check more that one node ahead then you need to store nodes
 and add functionality for that.
 - Flow
   -# Check if normal read or if it is a prearad that should be done. The m_uState have information
      about this.
   -# Find the "<" for next element, check if it is a comment and if so skip it and continue to next
   -# Find first character after "<"
   -# Check if the read element ends previous element, If it is a end tag then compare if it is the same
      as previous. Exit function.<br>
      Add element name to name stack
   -# Read element data.<br>
      When reading element data we need to check for <![CDATA[. This complicates the parsing a lot. Because
      CXMLArchive uses buffered reading _CompareText function is used to check if CDATA is found.
   -# When element data is read check if it has ending tag. if not then this element have children.

 * \return true if successful read otherwise false
 */
template<class STREAM>
bool CXMLArchive<STREAM>::Read()
{
#define _xmlarchive_ATTRIBUTES 1
#define _xmlarchive_VALUE     0
#define _xmlarchive_COPYVALUE 2

   _CHECK( m_uState & eRead );

   unsigned uSpace = _xmlarchive_ATTRIBUTES;
   wchar_t ch;
   UINT* puState;
   const wchar_t* pszPosition;
   gd::CString* pstring;

   // If we have pre read one item then switch that string to current buffer that holds the item.
   if( m_stringNext.GetLength() > 0 )
   {
      m_uScan = 0;
      m_stringBuffer = m_stringNext;
      m_stringNext.Empty();
      m_uTagState = m_uTagStateNext;
      m_uTagStateNext = 0;
      return true;
   }

   // check if normal read or if we should preread next node in order to look ahead
   if( !(m_uState & ePreRead) )
   {
      m_uScan = 0;
      m_uTagState = 0;
      pstring = &m_stringBuffer;
      puState = &m_uTagState;
   }
   else
   {
      _ASSERT( m_stringNext.GetLength() == 0 ); // Only one preread in between of a readnext
      pstring = &m_stringNext;
      m_uState &= ~ePreRead;
      puState = &m_uTagStateNext;
   }

   if( m_pszPosition >= m_pszMax ) Flush();
   if( *m_pszPosition == L'\0' ) return false;

   
   pstring->Empty();
   pstring->Single();

   // find start of tag "<"
   for(;;)
   {
      while( *m_pszPosition && *m_pszPosition != L'<' )
      {
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
         if( *m_pszPosition == L'\0' )
         {
            if( m_stringsStack.GetCount() ) m_sError = _xmlarchive_NullCharacterFound;
            return false;
         }
      }

      if( *m_pszPosition == 0 )
      {
         if( m_stringsStack.GetCount() ) m_sError = _xmlarchive_NullCharacterFound;
         return false;
      }

      _In();

      // check if first character is !. If it is then skip it and find next tag
      if( *m_pszPosition == L'!' )
      {
         _SkipTag();
         if( m_sError != 0 )
         {
            return false;
         }
      }
      else
      {
         break;
      }
   }

   m_TagLine=m_Line;
   // * start of element tag is has been found. It's time to get values from XML element

   // find the first character
   while( ((ch = _In()) <= L' ') && (ch != L'\0') );
   if( ch == L'\0' )
   {
      m_sError = _xmlarchive_NullCharacterFound;
      if( m_stringsStack.GetCount() ) m_sError = _xmlarchive_NullCharacterBeforeEndNode;
      return false;
   }

   // Check if this is a end tag
   if( ch == L'/' )
   {
      // * it was a end tag and we pass this tag and also remove the tag name from stack

      // * Move to name
      while( *m_pszPosition && *m_pszPosition <= L' ' )
      {
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
      }

      if( *m_pszPosition == 0 )
      {
         m_sError = _xmlarchive_NullCharacterBeforeEndNode;
         return false;
      }

      while( *m_pszPosition > L' ' && *m_pszPosition != L'>' )
      {
         pstring->FastAdd( *m_pszPosition );
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
      }

      if( m_stringsStack.GetCount() == 0 || !(*pstring == m_stringsStack.GetString( m_stringsStack.GetCount() - 1 ) ) )
      {
         m_sError = _xmlarchive_EndingDoesNotMatchParent;
         return false;

#ifdef _DEBUG
         // Dump stack
         for( unsigned u = 0; u < m_stringsStack.GetCount(); u++ )
         {
   #ifdef TRACE
            TRACE( L"%s ", (const wchar_t*)m_stringsStack.GetString( u ) ); 
   #endif
         }
#endif
      }
      m_stringsStack.Pop();


      while( (*m_pszPosition != L'>') && *m_pszPosition )
      {
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
      }

      if( *m_pszPosition == 0 )
      {
         m_sError = _xmlarchive_NullCharacterFound;
         return false;
      }

      *puState |= eEndTag;
      return true;
   }

   // * read element name
   while( ch >= L'0' && ch != L'>' )
   {
      pstring->FastAdd( ch );
      ch = _In();
   }
   _CHECK( pstring->GetLength() > 0 );

   m_stringsStack.Push( *pstring );

   pstring->FastAdd( L' ' );
   pstring->FastAdd( ch );

   if( *m_pszPosition == L'<' )
   {
      //*check for "![CDATA["
      if( _CompareText( this, L"<![CDATA[", 9 ) == true )
      {
         uSpace = _xmlarchive_VALUE;
         goto start_cdata;
      }
   }

   while( *m_pszPosition && *m_pszPosition != L'<' )
   {
      if( uSpace ) pstring->FastAdd( *m_pszPosition );
      else 
      {
         if( (unsigned)*m_pszPosition > L' ' )
         {
            uSpace = _xmlarchive_COPYVALUE;
            *pstring += *m_pszPosition;
         }
      }

      // * if end tag is found then check backwards in string for end tag
      if( *m_pszPosition == L'>' )
      {
         uSpace = _xmlarchive_VALUE;
         pszPosition = pstring->GetBufferEnd( 0 ) - 2;
         while( *pszPosition <= L' ' ) pszPosition--;
         if( *pszPosition == L'/' )
         {
            m_stringsStack.Pop();
            return true;
         }
         
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
         //*check for "![CDATA["
         if( (*m_pszPosition == L'<') && (_CompareText( this, L"<![CDATA[", 9 ) == true) )
         {
start_cdata:
            *puState |= eCDATA;
            //* move to charcters
            unsigned u = 9;
            while( u ) 
            {
               u--;
			  if(*m_pszPosition==13) m_Line++;
               m_pszPosition++;
	            if( m_pszPosition >= m_pszMax ) Flush();
            }
copy_cdata:
            while( *m_pszPosition && *m_pszPosition != L']' )
            {
               *pstring += *m_pszPosition;
			   if(*m_pszPosition==13) m_Line++;
               m_pszPosition++;
	            if( m_pszPosition >= m_pszMax ) Flush();
            }

            if( *m_pszPosition == L']' )
            {
			   if(*m_pszPosition==13) m_Line++;
               m_pszPosition++;
	            if( m_pszPosition >= m_pszMax ) Flush();
               if( *m_pszPosition != L']' )
               {
                  pstring->FastAdd( L']' );
                  goto copy_cdata;
               }

			   if(*m_pszPosition==13) m_Line++;
               m_pszPosition++;
	            if( m_pszPosition >= m_pszMax ) Flush();
               if( *m_pszPosition != _U('>') )
               {
                  pstring->FastAdd( _U(']') );
                  goto copy_cdata;
               }
            }
            else if( *m_pszPosition == 0 )
            {
               m_sError = _xmlarchive_NullCharacterFound;
               return false;
            }
         }
         else
         {
            continue;
         }
      }

	  if(*m_pszPosition==13) m_Line++;
      m_pszPosition++;
	   if( m_pszPosition >= m_pszMax ) Flush();
   }

   //*check for "![CDATA["
   if( _CompareText( this, _U("<![CDATA["), 9 ) == true )
   {
      goto start_cdata;
   }

   // * check if node has children
   // * this is done by checking the first character after "<". if the character is a  "/" then
   // * there are no children for this node. but if it is another character then this node has children
   ch = 0;
   while( *m_pszPosition && (*m_pszPosition <= _U(' ') || *m_pszPosition == _U('<')) )
   {
	  if(*m_pszPosition==13) m_Line++;
      m_pszPosition++;
   	if( m_pszPosition >= m_pszMax )
      {
         // we can't go backwards when reading. and here we check in front of the current position.
         // because of this we decrease the buffer that is read to and save space for one character 
         // at start.
         ch = _U('<');    // set ch "hard". This is later checked in order to se if we have moved in buffer.
         m_pszStart++;
         m_uBufferSize--;
         Flush();
      }
   }                 

   if( *m_pszPosition == 0 )
   {
      m_sError = _xmlarchive_NullCharacterFound;
      return false;
   }

   // is the first character not "/"
   if( *m_pszPosition != _U('/') )
   {
      // set flag for marking that node has children
      *puState |= eHasChild;
   }
   else
   {
      // * there are no children for this node. move pointer past end of tag
      while( *m_pszPosition && *m_pszPosition != _U('<') )
      {
		  if(*m_pszPosition==13) m_Line++;
         m_pszPosition++;
	      if( m_pszPosition >= m_pszMax ) Flush();
      }

      if( *m_pszPosition == 0 )
      {
         m_sError = _xmlarchive_NullCharacterFound;
         return false;
      }

      m_stringsStack.Pop();
      if( ch != 0 )  // check ch to se if we have reserved space at start of buffer.
      {
         m_pszStart--;
         m_uBufferSize++;
      }
      return true;
   }

   // move backwards to start character for node
   m_pszPosition--;
   if( ch != 0 )  // check ch to se if we have reserved space at start of buffer.
   {
      m_pszStart--;
      m_uBufferSize++;
   }

   // set character that marks start of node "hard".
   if( *m_pszPosition != _U('<') ) *m_pszPosition = _U('<');

   return true;
}

/////////////////////////////////////////////////
//! move pointer to next tag
template<class STREAM>
void CXMLArchive<STREAM>::_SkipTag()
{
   wchar_t ch;

   // move to first character
   while( ((ch = _In()) <= _U(' ')) && (ch != _U('\0')) );

   if( ch == 0 )
   {
      m_sError = _xmlarchive_NullCharacterFound;
      return;
   }

   // check if this could be a comment
   if( *m_pszPosition == _U('-') )
   {
      _In();
      // is it a comment
      if( *m_pszPosition == _U('-') )
      {
         _In();
         // it was a comment. now find end of comment ("-->")
         while( true )
         {
            while( *m_pszPosition != _U('-') )
            {
			  if(*m_pszPosition==13) m_Line++;
               m_pszPosition++;
	            if( m_pszPosition >= m_pszMax ) 
					Flush();
				if(m_pszPosition >= m_pszMax)
					return;
//               if( m_pszStop <= m_pszPosition ) 
//				   return;
            }
            _In();                                          

            if( *m_pszPosition == _U('-') )
            {
               _In();
               if( *m_pszPosition == _U('>') )
               {
                  break;
               }
            }
         }// end while  
      }// end if
   }// end if

   // find start of tag "<"
   while( *m_pszPosition && *m_pszPosition != _U('<' ) )
   {
//      m_pszPosition++;
//      if( m_pszStop <= m_pszPosition )
      if(_In()==_U('\0')) // patched by Tandy 27.04.2005
	  {
         m_sError = _xmlarchive_XMLHasEndedBeforeEndOfTags;
         return;
      }
   }

   if( *m_pszPosition == 0 )
   {
      m_sError = _xmlarchive_NullCharacterFound;
   }
}

/////////////////////////////////////////////////
//! check the text 
//! static member because VC couldn't compile member. There was some sort
//! of optimization done and the function didn't work. It worked with no optimization.
//! Static works on all versions.
template<class STREAM>
bool CXMLArchive<STREAM>::_CompareText( CXMLArchive<STREAM>* pThis, wchar_t* pszText, int iLength )
{
   bool bOk;
   int iRead;

   if( iLength < 0 )
   {
      iLength = wcslen( pszText );
   }

   _ASSERT( pThis->m_uBufferSize > (unsigned)(iLength * 2) );

   //* check if we need to read data from file in order to get same length that is checked
   iRead = iLength - (pThis->m_pszStop - pThis->m_pszPosition);
   if( iRead > 0 )
   {
      /* && iRead > pThis->m_stringInFront.GetLength() */
      if( pThis->m_stringInFront.GetLength() == 0 )
      {
         unsigned uBytes;
         pThis->m_pSTREAM->Read( pThis->m_stringInFront.GetBuffer( iRead + 1), (iRead + 1) * sizeof(wchar_t), &uBytes );
         assert( uBytes <= ((unsigned)(iRead + 1) * sizeof(wchar_t)) );
         pThis->m_stringInFront.GetBufferSetLength( uBytes / sizeof(wchar_t) );
      }
      else
      {
         unsigned uBytes;
         unsigned uStart = pThis->m_stringInFront.GetLength();
         pThis->m_pSTREAM->Read( (pThis->m_stringInFront.GetBuffer( iRead + 1 ) + uStart), (iRead - uStart + 1) * sizeof(wchar_t), &uBytes );
         assert( uBytes <= ((unsigned)(iRead - uStart + 1) * sizeof(wchar_t)) );
         pThis->m_stringInFront.GetBufferSetLength( uStart + (uBytes / sizeof(wchar_t)) );
      }

      //* m_pszStop points to the last character
      //* when calculating "m_pszStop - m_pszPosition" you get the difference from last character
      //* here we have read characters after the last one and because of this the read count
      //* is decreased with one.
      iRead--;
   }

   //* compare name
   if( iRead < 0 )
   {
      iRead = 0;
   }

   bOk = (wcsncmp( pThis->m_pszPosition, pszText, iLength - iRead ) == 0);
   if( iRead > 0 && bOk )
   {
      bOk = (wcsncmp( pThis->m_stringInFront, &pszText[iLength - iRead], iRead ) == 0);
   }

   return bOk;
}

/////////////////////////////////////////////////
//! write string to main buffer
template<class STREAM>
void CXMLArchive<STREAM>::Write()
{
   if( m_stringBuffer.GetLength() == 0 ) return;

   if( m_stringBuffer.GetLength() >= (m_pszMax - m_pszPosition) )
   {
      unsigned uIndex = 0;
      while( uIndex < (unsigned)m_stringBuffer.GetLength() )
      {
         if( m_pszPosition >= m_pszMax )
         {
            Flush();                                        // flush to current position
         }
         *m_pszPosition = m_stringBuffer[uIndex];
         m_pszPosition++;
         uIndex++;
      }
   }
   else
   {
      memcpy( m_pszPosition, m_stringBuffer.GetBuffer(), m_stringBuffer.GetLength() * sizeof(wchar_t) );
//#ifdef _DEBUG
//      // * check crap characters
//      for( unsigned u = 0; u < (unsigned)m_stringBuffer.GetLength(); u++ )
//      {
//         if( (unsigned char)m_pszPosition[u] < _U(' ') )
//         {
//            if( m_pszPosition[u] > _U('\t') )
//            {
//               _ASSERT( m_pszPosition[u] == _U('\n') );
//            }
//            else 
//            {
//               _ASSERT( m_pszPosition[u] == _U('\t') );
//            }
//         }
//      }
//#endif
      m_pszPosition += m_stringBuffer.GetLength();
   }

   m_stringBuffer.Empty();
}

/////////////////////////////////////////////////
//! validate the active tag (don't check all XML roules but some)
template<class STREAM>
bool CXMLArchive<STREAM>::ValidateActiveTag()
{
   if( m_uTagState & eCDATA ) return true;
   if( m_stringBuffer.GetLength() == 0 ) return false;
   if( m_stringBuffer.Count( _U(">") ) != 1 ) return false;
   if( m_stringBuffer.Count( _U("<") ) != 0 ) return false;
   if( (m_stringBuffer.Count( _U("\"") ) % 2) == 1 ) return false;
   
   return true;
}


template<class STREAM>
__forceinline gd::CString CXMLArchive<STREAM>::GetActiveTagText() {
   return m_stringBuffer; }


}; // namespace gd_xml_stream

#endif // !defined(AFX_XMLARCHIVE_H__664C0238_A109_453B_9341_6AF4614AD628__INCLUDED_)
