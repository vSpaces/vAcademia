#pragma once

#include "IPadSessionBaseIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionChangeSlideIn : public CIPadSessionBaseIn
	{
	protected:
		int changeSlide;
	public:
		CIPadSessionChangeSlideIn( BYTE *aData, int aDataSize);
		~CIPadSessionChangeSlideIn();
		bool Analyse();
		unsigned int GetChangeSlide() const;
	};
};