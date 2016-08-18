#pragma once

namespace SyncManProtocol
{
	class CTempPacketIn
	{
		BYTE*	dataBuffer;
		int		dataSize;
		public:
			CTempPacketIn(BYTE *aData, int aDataSize)
			{
				ATLASSERT( aData != NULL);
				ATLASSERT( aDataSize != 0);
				dataBuffer = (BYTE*)malloc( aDataSize);
				if( dataBuffer != NULL)
				{
					memcpy( dataBuffer, aData, aDataSize);
					dataSize = aDataSize;
				}
				else
				{
					dataSize = 0;
				}
			}
			~CTempPacketIn()
			{
				if( dataBuffer != NULL)
				{
					free(dataBuffer);
					dataBuffer = NULL;
				}
				dataSize = 0;
			}

			BYTE*	GetDataBuffer()
			{
				return dataBuffer;
			}

			int		GetDataSize()
			{
				return dataSize;
			}
	};
};