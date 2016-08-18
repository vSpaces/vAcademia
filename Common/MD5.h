#ifndef  _MD5_H
#define  _MD5_H

namespace  md5
{
	typedef struct
	{
		unsigned long int total[ 2 ];
		unsigned long int state[ 4 ];
		unsigned char buffer[ 64 ];
	}
	md5_context;

	void md5_starts( md5_context *ctx );
	void md5_update( md5_context *ctx, unsigned char *input, unsigned long int length );
	void md5_finish( md5_context *ctx, unsigned char digest[16 ] );

	int CalculateMD5( unsigned char *sValue, char **encodingValue, int aBufferSize);
	std::string MakeSaltedHash(std::string str);
	extern const int MD5_BUFFERSIZE;
}

#endif  /* md5_.h */