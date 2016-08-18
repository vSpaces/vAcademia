// CXMLExArchive.h: interface for the CXMLArchive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLARCHIVE_H__F1D158CC_38EF_4CDB_B6F7_86524ADF6C1E__INCLUDED_)
#define AFX_XMLARCHIVE_H__F1D158CC_38EF_4CDB_B6F7_86524ADF6C1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gd/gd_XMLArchive.h"
#include "../natura3d/include/n3dguids.h"
#include "../natura3d/include/n3dfile.h"
#include "../natura3d/include/n3dxml.h"
#include "FileEx.h"

using namespace natura3d;

class CXMLFileWrapper
{
	ifile* m_pFile;
public:
	CXMLFileWrapper(ifile* file);
	virtual ~CXMLFileWrapper();

	virtual SetIFile(ifile* apifile);

// wrapper functions
public:
	UINT		Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	void		Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
};

//////////////////////////////////////////////////////////////////////////
class CXMLExArchive  : public gd_xml_stream::CXMLArchive<CXMLFileWrapper>/*, 
						public ixml*/
{
	CXMLFileWrapper	m_pStream;

public:
	/*
	IMPLEMENT_RELEASE
	BEGIN_INTERFACE_IMPLEMENTATION
		IMPLEMENT_INTERFACE( ixml)
	END_INTERFACE_IMPLEMENTATION
	N3DIMPLEMENT_INTERFACE( ixml)
*/
	CXMLExArchive(ifile* file, bool write=false);
	virtual ~CXMLExArchive();

	// реализация natura3d::ixml
public:
	bool open( ifile* apifile);
	void close( );
	//! ask if curren node had children
	bool has_children();
	bool has_next_children();
	//! check if the read tag is end of another tag
	bool is_end_tag();
	bool is_next_end_tag();
	//! Insert new child node (WRITE)
	void new_child( TCHAR* pszName );
	//??
	virtual	void end_item( );
	virtual	void end( );
	//! Add sibling to current node (WRITE)
	void new_sibling( TCHAR* pszName );
	//! Specifiy the type of item next set_item will add (WRITE)
	//	void next_item( Operation operation );
	//! End current or all nodes
	//	void end_item( EndOperation operation );
	//! set header for xml file
	void set_header( TCHAR* pszText );
	//! set attribute for node
	void set_attribute( TCHAR* pszName, TCHAR* pszValue );
	//! set value for node
	void set_value( TCHAR* pszValue );
	//! set value but convert raw data to hex
	void set_binary_value( BYTE* pbValue, int iLength );
	//! set item in node (what type depends on state for class
	//	void set_item( CXMLItem& item );

	//! skip header section
	HRESULT skip_header();
	//! return section index (how many parents including the node itself) current node has
	unsigned get_section();
	//! get name for node
	int  get_name( TCHAR* pszBuffer, int iMaxLength );
	//! get name for node in string object
	LPCTSTR get_name();
	// return parent name if one exist
	LPCTSTR get_parent_name();
	//! get name for node
	int  get_next_name( TCHAR* pszBuffer, int iMaxLength );
	//! get name for next node in string object
	LPCTSTR get_next_name();
	//! return value for node
	int  get_value( TCHAR* pszBuffer, int iMaxLength );
	//! get value for node in string object
	LPCTSTR get_value();
	//! return value as binary for node or attribute value (should be in hex form in XML file)
	int  get_binary_value( TCHAR* pszName, BYTE* pBuffer, int iLength );
	//! return number of attributes at current node
	unsigned get_attribute_count();
	//! reads attributename from string pointer.
	//	static void read_attribute_name( TCHAR* pszNameStart, gd::CString* pstringName );
	//! get the name for attribute
	LPCTSTR get_attribute_name( unsigned uIndex );
	//! get value or attribute from node
	int  get_attribute( TCHAR* pszName, TCHAR* pszBuffer, int iMaxLength );
	LPCTSTR get_attribute( TCHAR* pszName );
	//! get the attribute value for attribute at specified index in active node
	int  get_attribute( unsigned uIndex, TCHAR* pszBuffer, int iMaxLength );
	LPCTSTR get_attribute( unsigned uIndex );
	//! find attribute name in node
	int  find_attribute( TCHAR* pszName );
	//! set string pointers to start of attribute names in buffer. Pointers are not valid when a new node has been read
	unsigned make_attribute_index( const TCHAR ** ppszIndex, unsigned uCount );
	//! return item as specified type
	//	HRESULT get_item( TCHAR* pszName, CXMLItem& item );
	//! read next node
	bool read_next();
	//! read next node
	bool read_next( TCHAR* pszName );
	//! read the next item in order to check what it is. The normal read is not affected by this.
	bool pre_read_next();
	//! remove endnode marker from internal data and return that nodename in string object
	LPCTSTR pop_node();
private:
	CComString	m_Name, m_ParentName, m_NextName, m_Value, m_Attribute, m_AttributeName, m_PopNode;
};

#endif // !defined(AFX_XMLARCHIVE_H__F1D158CC_38EF_4CDB_B6F7_86524ADF6C1E__INCLUDED_)
