// CXMLExArchive.cpp: implementation of the CXMLArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "..\Natura3D.h"
#include "XMLArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLExArchive::CXMLExArchive(ifile* file, bool write) :  m_pStream(file), gd_xml_stream::CXMLArchive<CXMLFileWrapper>(&m_pStream, write?eWrite:eRead)
{

}

CXMLExArchive::~CXMLExArchive()
{
	//delete m_pStream;
}

bool CXMLExArchive::open( ifile* apifile)
{
	if( !apifile)	return false;
	m_pStream.SetIFile( apifile);
	return true;
}

void CXMLExArchive::close( )
{
	//ATLASSERT( FALSE);
}

bool CXMLExArchive::has_children()
{
	return HasChildren();
}

bool CXMLExArchive::has_next_children()
{
	return HasNextChildren();
}

bool CXMLExArchive::is_end_tag()
{
	return IsEndTag();
}

bool CXMLExArchive::is_next_end_tag()
{
	return IsNextEndTag();
}

void CXMLExArchive::new_child( TCHAR* pszName )
{
	NewChild( pszName);
}

void CXMLExArchive::end_item()
{
	EndItem(gd_xml_stream::EndNode);
}

void CXMLExArchive::end()
{
	EndItem(gd_xml_stream::End);
}

void CXMLExArchive::new_sibling( TCHAR* pszName )
{
	NewSibling( pszName);
}

void CXMLExArchive::set_header( TCHAR* pszText )
{
	SetHeader( pszText);
}

void CXMLExArchive::set_attribute( TCHAR* pszName, TCHAR* pszValue )
{
	SetAttribute( pszName, pszValue);
}

void CXMLExArchive::set_value( TCHAR* pszValue )
{
	SetValue( pszValue);
}

void CXMLExArchive::set_binary_value( BYTE* pbValue, int iLength )
{
	SetBinaryValue( pbValue, iLength);
}

HRESULT CXMLExArchive::skip_header()
{
	return SkipHeader();
}

unsigned CXMLExArchive::get_section()
{
	return GetSection();
}

int  CXMLExArchive::get_name( TCHAR* pszBuffer, int iMaxLength )
{
	return GetName( pszBuffer, iMaxLength);
}

LPCTSTR CXMLExArchive::get_name()
{
	m_Name = (const TCHAR*)GetName();
	return (LPCTSTR)m_Name;
}

LPCTSTR CXMLExArchive::get_parent_name()
{
	m_ParentName = GetParentName();
	return (LPCTSTR)m_ParentName;
}

int  CXMLExArchive::get_next_name( TCHAR* pszBuffer, int iMaxLength )
{
	return GetNextName( pszBuffer, iMaxLength);
}

LPCTSTR CXMLExArchive::get_next_name()
{
	m_NextName = GetNextName();
	return (LPCTSTR)m_NextName;
}

int  CXMLExArchive::get_value( TCHAR* pszBuffer, int iMaxLength )
{
	return GetValue( pszBuffer, iMaxLength);
}

LPCTSTR CXMLExArchive::get_value()
{
	m_Value = GetValue();
	return (LPCTSTR)m_Value;
}

int  CXMLExArchive::get_binary_value( TCHAR* pszName, BYTE* pBuffer, int iLength )
{
	return GetBinaryValue( pszName, pBuffer, iLength);
}

unsigned CXMLExArchive::get_attribute_count()
{
	return GetAttributeCount();
}

LPCTSTR CXMLExArchive::get_attribute_name( unsigned uIndex )
{
	m_AttributeName = (const TCHAR*)GetAttributeName( uIndex);
	return (LPCTSTR)m_AttributeName;
}

int  CXMLExArchive::get_attribute( TCHAR* pszName, TCHAR* pszBuffer, int iMaxLength )
{
	//return GetAttribute( pszName, pszBuffer, iMaxLength);
	ATLASSERT(FALSE);
	return 0;
}

LPCTSTR CXMLExArchive::get_attribute( TCHAR* pszName )
{
	//m_Attribute = GetAttribute( pszName);
	ATLASSERT( FALSE);
	return (LPCTSTR)m_Attribute;
}

int  CXMLExArchive::get_attribute( unsigned uIndex, TCHAR* pszBuffer, int iMaxLength )
{
	ATLASSERT( FALSE);
	//return GetAttribute( uIndex, pszBuffer, iMaxLength);
	return 0;
}

LPCTSTR CXMLExArchive::get_attribute( unsigned uIndex )
{
	m_Attribute = GetAttribute(uIndex);
	return (LPCTSTR)m_Attribute;
}

int  CXMLExArchive::find_attribute( TCHAR* pszName )
{
	return FindAttribute( pszName);
}

unsigned CXMLExArchive::make_attribute_index( const TCHAR ** ppszIndex, unsigned uCount )
{
	return MakeAttributeIndex( ppszIndex, uCount);
}

bool CXMLExArchive::read_next()
{
	return ReadNext();
}

bool CXMLExArchive::read_next( TCHAR* pszName )
{
	return ReadNext( pszName);
}

bool CXMLExArchive::pre_read_next()
{
	return PreReadNext();
}

LPCTSTR CXMLExArchive::pop_node()
{
	m_PopNode = GetName();
	return m_AttributeName.GetBuffer();
}

//////////////////////////////////////////////////////////////////////////
CXMLFileWrapper::CXMLFileWrapper(ifile* file)
{
	m_pFile = file;
}

CXMLFileWrapper::~CXMLFileWrapper()
{
}

CXMLFileWrapper::SetIFile(ifile* apifile)
{
	ATLASSERT( apifile);
	if( apifile)
		m_pFile = apifile;
}

// wrapper functions
UINT	CXMLFileWrapper::Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	int	count = 0;
	if( m_pFile)
	{
		count = m_pFile->read((unsigned char*)pszBuffer, uCount);
	}
	if( puCount)
	{
		*puCount = count;
	}
	return count;
}

void	CXMLFileWrapper::Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	if( m_pFile)
	{
		m_pFile->write(pszBuffer, uCount, puCount);
	}
	else
	{
		if( puCount)
		{
			*puCount = 0;
		}
	}
}