#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CObjectsDescUpdateOut : public CMapServerPacketOutBase
	{
	public:
		CObjectsDescUpdateOut(int objCount, unsigned int auRmmlQueryID, std::string& name, std::string& res, float x, float y, float z, 
			float bx, float by, float bz, float rx, float ry, float rz, float rw,
			float scalex, float scaley, float scalez, 
			unsigned int zoneID,
			std::string& className, std::wstring& userProps, int type, 
			unsigned int objectID, unsigned int bornRealityID, 
			int level, std::string& location
		);
	};
};