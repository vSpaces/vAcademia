
#pragma once

#define MAX_PACK_TASKS		250

unsigned char GetPackSubdivisionCount(unsigned int originalSize);
unsigned int GetPartSize(unsigned char partCount, unsigned int originalSize, unsigned char partID);