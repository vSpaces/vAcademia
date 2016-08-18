#include "StdAfx.h"
#include "InternalLogWriter.h"

CInternalLogWriter::CInternalLogWriter()
{
	m_pLogWriter = NULL;
}

ILogWriter*	CInternalLogWriter::GetILogWriter()
{
	return m_pLogWriter;
}

void CInternalLogWriter::WriteLPCSTR(const char * str, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLPCSTR(str, logType);
}


void CInternalLogWriter::WriteLnLPCSTR(const char * str, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLnLPCSTR(str, logType);
}

void CInternalLogWriter::PrintLPCSTR(const char * str, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLPCSTR(str, logType);
}

void CInternalLogWriter::PrintLnLPCSTR(const char* str, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLnLPCSTR(str, logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn(obj, logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  logType);
}

void CInternalLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->WriteLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  obj10,  logType);
}

void CInternalLogWriter::Write(CLogValue obj, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  logType);
}

void CInternalLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Write( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  obj10,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  obj5,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2, obj3,  obj4,  obj5,  obj6,  obj7,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  logType);
}

void CInternalLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->PrintLn( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  obj10,  logType);
}

void CInternalLogWriter::Print(CLogValue obj, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  logType);
}

void CInternalLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if( m_pLogWriter)	
		m_pLogWriter->Print( obj1,  obj2,  obj3,  obj4,  obj5,  obj6,  obj7,  obj8,  obj9,  obj10,  logType);
}

CInternalLogWriter::~CInternalLogWriter()
{

}
