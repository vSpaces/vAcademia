// N3DCharacter.cpp: implementation of the CN3DCharacter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "N3DCharacter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3DCharacter::CN3DCharacter(nrm3DObject* pc)
{
	prmCharacter = pc;
}

CN3DCharacter::~CN3DCharacter()
{

}

void CN3DCharacter::update(DWORD	dwTime, void* data)
{
	if( prmCharacter)	prmCharacter->update(dwTime, data);
	C3DCharacter::update(dwTime, data);
}