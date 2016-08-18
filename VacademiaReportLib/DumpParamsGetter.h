#pragma once

#include "VacademiaReportLib.h"

class VACADEMIAREPORT_API CDumpParamsGetter
{
public:
	CDumpParamsGetter(void);
	virtual ~CDumpParamsGetter(void);
	BOOL get( WCHAR *keyName, WCHAR **sValue, WCHAR *sDefaultValue);

private:
	CWComString m_sFileName;
};
