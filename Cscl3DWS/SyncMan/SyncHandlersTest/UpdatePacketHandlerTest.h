// ****************************************************************
// This is free software licensed under the NUnit license. You
// may obtain a copy of the license as well as information regarding
// copyright ownership at http://nunit.org/?p=license&r=2.4.
// ****************************************************************
//;_WINDOWS;_USRDLL;undefined_EXPORTS
//Import Library:  $(OutDir)/SendAndReceiveTest.lib
#pragma once

#include "UpdatePacketOut.h"
#include "UpdatePacketIn.h"

using namespace System;
using namespace NUnit::Framework;
using namespace Protocol;

namespace SyncManProtocol
{
	[TestFixture]
	public __gc class UpdatePacketHandlerTest
	{
		int fValue1;
		int fValue2;
	private:
		void AddPropertyObject(CUpdatePacketOut *packetOut, const wchar_t *sPropName, int deepMax);
		void GetDeepProperties(syncIPropertyList *aPropertyList);
	public:
		[SetUp] void Init();

		[Test] void Test();
	};
}
