#include "stdafx.h"
#include "AvatarDescQueryOut.h"

using namespace MapManagerProtocol;

CAvatarDescQueryOut::CAvatarDescQueryOut(const wchar_t* apwcStartURL) :CMapServerPacketOutBase()
{
	Add( apwcStartURL);
}
