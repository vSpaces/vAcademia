#pragma once

#ifndef __RECORDEDITOR_H_
#define __RECORDEDITOR_H_

#ifdef RECORDEDITOR_EXPORTS
#define RECORDEDITOR_API __declspec(dllexport)
#else
#define RECORDEDITOR_API __declspec(dllimport)
#endif

#ifdef WIN32
#define RECORDEDITOR_NO_VTABLE __declspec(novtable)
#else
#define RECORDEDITOR_NO_VTABLE
#endif

namespace re
{
	struct IRecordEditor
	{
		virtual bool GetConnectionState()=0;
	};

	struct IRecordEditorMessageHandler
	{
		virtual int HandleMessage( BYTE aType, BYTE* aData, int aDataSize, DWORD aCheckSum)=0;
	};
}

#endif
