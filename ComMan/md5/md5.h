#ifndef MD5_H
#define MD5_H 

#include <windows.h>

#ifdef __alpha
typedef unsigned int uint32;
#else
typedef unsigned long uint32;
#endif

struct MD5Context {
		uint32 buf[4];
		uint32 bits[2];
		unsigned char in[64];
};
//typedef struct MD5Context MD5_CTX;

extern void MD5Init(struct MD5Context FAR *ctx);
extern void MD5Update(struct MD5Context FAR *ctx, unsigned char const FAR *buf, unsigned len);
//extern void MD5Final(unsigned char FAR *digest, struct MD5Context FAR *ctx);
extern void MD5Transform(uint32 FAR *buf, uint32 FAR *in);


#endif /* !MD5_H */
