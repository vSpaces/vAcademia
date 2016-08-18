#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VACADEMIAREPORT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VACADEMIAREPORT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef VACADEMIAREPORT_EXPORTS
#define VACADEMIAREPORT_API __declspec(dllexport)
#else
#define VACADEMIAREPORT_API __declspec(dllimport)
#endif

// This class is exported from the VacademiaReport.dll
class VACADEMIAREPORT_API CVacademiaReport {
public:
	CVacademiaReport(void);
	// TODO: add your methods here.
};

//extern VACADEMIAREPORT_API int nVacademiaReport;

//VACADEMIAREPORT_API int fnVacademiaReport(void);
