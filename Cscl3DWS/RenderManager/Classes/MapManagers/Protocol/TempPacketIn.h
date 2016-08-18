#pragma once

namespace MapManagerProtocol
{
	// TODO Сделать разбор пакетов мапманагера прямо по приходу
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
				free(dataBuffer);
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