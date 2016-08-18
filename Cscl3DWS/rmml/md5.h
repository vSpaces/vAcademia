#include <windef.h>
#include <stdio.h>
#include <Windows.h>

#include <stdlib.h>
#include <conio.h>
#include <io.h>

#ifdef __alpha  
typedef unsigned int UINT4;  
#else  
typedef unsigned long int UINT4;  
#endif

#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89  
#define MD5_INIT_STATE_2 0x98badcfe  
#define MD5_INIT_STATE_3 0x10325476  

void MD5Init(void);  
void MD5Update(unsigned char *bug, unsigned int len);  
void MD5Final(char* cReturnStr, uint nReturnStrSize);  
void Transform(UINT4 *buf, UINT4 *in);  
void GetMD5(BYTE* pBuf, uint nLength, char* cReturnStr, uint nReturnStrSize);  
