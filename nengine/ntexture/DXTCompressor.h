
#pragma once

bool CompressImage(void* inData, void** outData, unsigned short width, unsigned short height, 
				   char channelCount, bool isDirectOrder, bool isMipmapRequired, unsigned int& size);				  