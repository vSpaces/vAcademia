#pragma once

namespace SyncManProtocol{
	class CGetUserListOut : public CCommonPacketOut
	{
	public:
		CGetUserListOut();
		CGetUserListOut(unsigned int aRealityID);
	
	};
};