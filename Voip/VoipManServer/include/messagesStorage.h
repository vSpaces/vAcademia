#pragma once

typedef struct _VoipMessage
{
	unsigned char type;
	unsigned char* data;
	int			  dataSize;

	_VoipMessage(BYTE aType, const BYTE FAR *aData, int aDataSize)
	{
		type = aType;
		data = (unsigned char*)malloc( aDataSize);
		if( data)
		{
			memcpy( data, aData, aDataSize);
			dataSize = aDataSize;
		}
	}

	~_VoipMessage()
	{
		if( data)
			free( data);
		data = NULL;
	}

} VoipMessage;