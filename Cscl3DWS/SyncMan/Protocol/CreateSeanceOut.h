#pragma once

namespace SyncManProtocol
{
	class CCreateSeanceOut : public CCommonPacketOut
	{
	public:
		CCreateSeanceOut();
		CCreateSeanceOut(unsigned int auIDSeanceRmmlObject, unsigned int auLogRecordID,  const wchar_t *aName,  const wchar_t *aMembers,  unsigned long aDate);
		CCreateSeanceOut(unsigned int auIDSeanceRmmlObject, const wchar_t *aURL);
	};
};