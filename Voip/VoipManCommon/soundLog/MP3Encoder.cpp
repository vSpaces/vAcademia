#include "StdAfx.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(_WIN32)
#include <fcntl.h>
#include <windows.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "MP3Encoder.h"

#include "Frame.h"
#include "FrameManager.h"
#include "RecordState.h"
#include <shlobj.h>
#include <io.h>
#include <crtdbg.h>
#include <atldef.h>

#define frameSize 1152

CMP3Encoder::CMP3Encoder( CComString aUserName):
CThread( 0,
		NoAutoDeleteThread,
		CThread::NormalPriority,	
		CComString ("CMP3EncoderThread_") + aUserName)
{
	ATLTRACE( "CMP3Encoder");
	m_sUserName = aUserName;
	frameCount = 0;
	currentFrame = 0;
	gf = NULL;
	outfile = NULL;
	infile = NULL;
	input_format = sf_unknown;
	pcmbitwidth = 16;
	pcmswapbytes = 0;
	count_samples_carefully = 0;
	num_samples_read = 0;
	bEmpty = FALSE;
	MP_NEW_P( frameManager_, CFrameManager, CFrameManager::EQUAL_BY_SIZE);
	frameManager = frameManager_;
	frameManager->setFrameSize( frameSize);
	MP_NEW_P( frameManager2_, CFrameManager, CFrameManager::EQUAL_BY_SIZE);		
	frameManager2 = frameManager2_;
	frameManager2->setFrameSize( frameSize);
	bFirst = TRUE;
	bTerminating = FALSE;
	bTerminated	= FALSE;
#ifdef _DEBUG
	sampleFrameManager = NULL;
#endif
	blockNumber = 8;
	bRunSaver = FALSE;
	recordState = NULL;
	bParamsSetted = FALSE;

	m_iBitBrate = 8;
	m_iChannelMode = MONO;
	m_iQuality = 7;
	m_iSampleRate = 16000;

	Resume();
}

CMP3Encoder::~CMP3Encoder()
{				
	ATLTRACE( "~CMP3Encoder");
	destroy();
	lameClose();
}

//////////////////////////////////////////////////////////////////////////

int CMP3Encoder::addData( void *data, int length)
{	
	CWaitAndSignal m( updateStateMutex);
	if ( recordState == NULL || !recordState->getRecordState())
		return 0;	

	int total_size = 0;

	CFrameManager *pFrameManager = NULL;

	if ( bFirst)	
		pFrameManager = frameManager;
	else
		pFrameManager = frameManager2;
		
	pFrameManager->updateFrames( data, length, TRUE);

	if ( isMP3FrameReady())
	{
		bFirst = !bFirst;
		startSaving();
	}
	total_size = pFrameManager->getTotalSize();	
	return total_size;
}

void CMP3Encoder::removeAllData()
{
	CWaitAndSignal m( updateStateMutex);
	frameManager2->removeAllFrames();
	frameManager->removeAllFrames();	
}

void CMP3Encoder::destroy()
{
	Terminating();
	int k = 0;
	while( !bTerminated)
	{
		k++;
		if ( k > 20)
		{
			Terminate();
			break;
		}
		Sleep( 100);
	}
	updateStateMutex.Wait();
	if( recordState)
		MP_DELETE( recordState);
	if( frameManager2)
		MP_DELETE( frameManager2);	
	if( frameManager)
		MP_DELETE( frameManager);	
	updateStateMutex.Signal();	
}

BOOL CMP3Encoder::isBlockReady()
{
	return frameManager->isBlockReady();
}

BOOL CMP3Encoder::isMP3FrameReady()
{
	CFrameManager *pFrameManager = NULL;

	if ( bFirst)	
		pFrameManager = frameManager;
	else
		pFrameManager = frameManager2;

	if ( pFrameManager->isBlockReady())
		return (( pFrameManager->getFrameCount() / pFrameManager->getBlockFrameCount()) > blockNumber);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

int CMP3Encoder::lameInit()
{
	/* initialize libmp3lame */
	input_format = sf_unknown;
	if (( gf = lame_init()) == NULL) 
	{
		//fprintf(stderr, "fatal error during initialization\n");
		return 0;
	}

	id3tag_init( gf);
	
	lame_set_findReplayGain( gf, 1);
	
	if( MONO == lame_get_mode( gf ) ) 
		(void) lame_set_num_channels( gf, 1 );
	else 
		(void) lame_set_num_channels( gf, 2 );


	if ( lame_get_free_format( gf))
	{
		if ( lame_get_brate( gf) < 8  ||  lame_get_brate( gf) > 640 ) 
		{
			//fprintf( stderr, "For free format, specify a bitrate between 8 and 640 kbps\n");
			//fprintf( stderr, "with the -b <bitrate> option\n");
			return 0;
		}
	}

	/* set the defaults from info incase we cannot determine them from file */
	lame_set_num_samples( gf, MAX_U_32_NUM );

	return 1;
}

int CMP3Encoder::setMP3Params( int channels, int samples_per_sec, int bits_per_sample)
{
	assert( gf);
	if ( gf == NULL)
		return 0;

	int vbr = lame_get_VBR( gf);
	int mode = lame_get_mode( gf);

	char *preset_name = "56";	
	
	lame_set_brate( gf, m_iBitBrate);
	//lame_set_VBR_min_bitrate_kbps( gf, lame_get_brate( gf));
	lame_set_mode( gf, (MPEG_mode) m_iChannelMode);	
	//lame_set_VBR( gf, vbr_off);
	lame_set_quality( gf, m_iQuality);

	lame_set_out_samplerate( gf, m_iSampleRate);

	// vbr-new используется новый режим VBR
	//lame_set_VBR( gf, vbr_mtrh);
	// кодирование голоса, радио, моно и т. д.
	//lame_set_VBR_q( gf, 7);	//*/

//	lame_set_preset( gf, rtl_atoi( preset_name));
	
//	
	if ( -1 == lame_set_num_channels( gf, channels)) 
	{
		return 0;
	}

	CFrameManager *pFrameManager = NULL;
	if ( bFirst)
		pFrameManager = frameManager2;
	else
		pFrameManager = frameManager;

	assert( pFrameManager);
	if ( pFrameManager == NULL)
		return 0;
	
	(void) lame_set_in_samplerate( gf, samples_per_sec);
	pcmbitwidth = bits_per_sample;	
	(void) lame_set_num_samples( gf, pFrameManager->getTotalSize() / ( channels * (( pcmbitwidth + 7) / 8)) );

	lame_init_params( gf);
	
	frameCount = pFrameManager->getFrameCount();
				
	return 1;
}

BOOL CMP3Encoder::openMP3File( WCHAR *sFileName)
{
	if (( outfile = init_outfile( sFileName, lame_get_decode_only( gf))) == NULL) 
	{
		//fprintf( stderr, "Can't init outfile '%s'\n", sFileName);
		return FALSE;
	}
	input_format = sf_wave;
	count_samples_carefully = 1;
	//brhist = 1;
	return TRUE;
}

BOOL CMP3Encoder::openNewMP3File( WCHAR *sFileName)
{
	init_newoutfile( sFileName);
	/*if (( outfile = init_newoutfile( sFileName)) == NULL) 
	{	
		return FALSE;
	}*/	
	return TRUE;
}

int CMP3Encoder::encode_sample()
{	
	if ( !bEmpty && currentFrame >= frameCount)
		return -1;
	
	int iread, imp3;

	int frames;	

	/* read in 'iread' samples */
	iread = getAudio( Buffer);
	frames = lame_get_frameNum( gf);
	
	/* encode */
	imp3 = lame_encode_buffer_int( gf, Buffer[ 0], Buffer[ 1], iread,
		mp3buffer, sizeof( mp3buffer));

	/* was our output buffer big enough? */
	if ( imp3 < 0) {
		/*if (imp3 == -1)
		fprintf(stderr, "mp3 buffer is not big enough... \n");
		else
		fprintf(stderr, "mp3 internal error:  error code=%i\n", imp3);*/
		return -1;
	}

	if ( fwrite( mp3buffer, 1, imp3, outfile) != imp3) 
	{
		//fprintf(stderr, "Error writing mp3 output \n");
		return -1;
	}	
	currentFrame++;
	
	return imp3;
}

int CMP3Encoder::setEmptySample()
{
	bEmpty = TRUE;
	return encode_sample();
}

int CMP3Encoder::encode_flush()
{
	int imp3 = lame_encode_flush( gf, mp3buffer, sizeof( mp3buffer));
	fwrite( mp3buffer, 1, imp3, outfile);
	currentFrame = 0;
	bEmpty = FALSE;
	return imp3;
}

void CMP3Encoder::closeMP3File()
{	
	lame_mp3_tags_fid( gf, outfile);
	fclose( outfile);
}

void CMP3Encoder::lameClose()
{
	if ( gf != NULL)
	{
		lame_close( gf);
		gf = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

int CMP3Encoder::lame_set_stream_binary_mode( FILE* const fp )
{
#if   defined __EMX__
	_fsetmode ( fp, "b" );
#elif defined __BORLANDC__
	setmode   (_fileno(fp),  O_BINARY );
#elif defined __CYGWIN__
	setmode   ( fileno(fp), _O_BINARY );
#elif defined _WIN32
	_setmode  (_fileno(fp), _O_BINARY );
#endif
	return 0;
}

FILE *CMP3Encoder::init_newoutfile( wchar_t *outPath)
{
	FILE   *outf = NULL;
	BOOL bExist = TRUE;
	int index = 1;
	CWComString fileName = outPath;
	while ( true)
	{
		int res = _wfopen_s( &outf, fileName.GetBuffer(), L"r");
		if (res != 0 || outf == NULL)
		{			
			break;
		}
		if ( index == 1)
			fileName.Format( L"%s_0%d.mp3", fileName.Left( fileName.GetLength() - 4).GetBuffer(), index);		
		else if ( index < 9)
			fileName.Format( L"%s_0%d.mp3", fileName.Left( fileName.GetLength() - 7).GetBuffer(), index);
		else
			fileName.Format( L"%s_%d.mp3", fileName.Left( fileName.GetLength() - 7).GetBuffer(), index);
		fclose( outf);
		index++;
	}

	recordState->setFileName( fileName.GetBuffer());
	//if (( outf = fopen(fileName.GetBuffer(), "w")) == NULL)
	//	return NULL;
	return NULL;
}

FILE *CMP3Encoder::init_outfile( wchar_t *outPath, int decode)
{
	FILE   *outf = NULL;
#ifdef __riscos__
	char   *p;
#endif

	/* open the output file */
	if (0 == wcscmp(outPath, L"-")) 
	{
		lame_set_stream_binary_mode(outf = stdout);
	}
	else {
		int res =_wfopen_s(&outf, outPath, L"ab");
		if (res != 0 || outf == NULL)
			return NULL;
#ifdef __riscos__
		/* Assign correct file type */
		for (p = outPath; *p; p++) /* ugly, ugly to modify a string */
			switch (*p) {
			case '.':
				*p = '/';
				break;
			case '/':
				*p = '.';
				break;
			}
			SetFiletype(outPath, decode ? 0xFB1 /*WAV*/ : 0x1AD /*AMPEG*/);
#endif
	}
	return outf;
}

int CMP3Encoder::unpackReadSamples( const int samples_to_read, const int bytes_per_sample, const int swap_order, int *sample_buffer)
{
	int samples_read = 0;
	int i;
	int *op;			/* output pointer */
	unsigned char *ip = (unsigned char *) sample_buffer; /* input pointer */
	const int b = sizeof(int) * 8;

#define GA_URS_IFLOOP( ga_urs_bps ) \
	if( bytes_per_sample == ga_urs_bps ) \
	for( i = samples_read * bytes_per_sample; (i -= bytes_per_sample) >=0;)


	//samples_read = fread( sample_buffer, bytes_per_sample, 
	//	samples_to_read, pcm_in);
	if ( !this->bEmpty)
	{
		CFrameManager *pFrameManager = NULL;
		if ( bFirst)
			pFrameManager = frameManager2;
		else
			pFrameManager = frameManager;
		assert( pFrameManager);
		if ( pFrameManager == NULL)
			return -1;
		CFrame *frame = pFrameManager->getFrame( currentFrame); 
		samples_read = frame->getSize();
		memcpy( sample_buffer, frame->getData(), samples_read);	
		samples_read /= bytes_per_sample;
	}

	op = sample_buffer + samples_read;

	GA_URS_IFLOOP( 1 )
		*--op = (ip[i] ^ 0x80)<<(b-8) | 0x7f<<(b-16);/* convert from unsigned*/
	if( swap_order == 0 ) {
		GA_URS_IFLOOP( 2 )
			*--op = ip[i]<<(b-16) | ip[i+1]<<(b-8); 
		GA_URS_IFLOOP( 3 )
			*--op = ip[i]<<(b-24) | ip[i+1]<<(b-16) | ip[i+2]<<(b-8);
		GA_URS_IFLOOP( 4 )
			*--op = ip[i]<<(b-32) | ip[i+1]<<(b-24) | ip[i+2]<<(b-16) | ip[i+3] << (b-8);
	} else {
		GA_URS_IFLOOP( 2 )
			*--op = ip[i]<<(b-8) | ip[i+1]<<(b-16); 
		GA_URS_IFLOOP( 3 )
			*--op = ip[i]<<(b-8) | ip[i+1]<<(b-16) | ip[i+2]<<(b-24);
		GA_URS_IFLOOP( 4 )
			*--op = ip[i]<<(b-8) | ip[i+1]<<(b-16) | ip[i+2]<<(b-24) | ip[i+3]<<(b-32);
	}
#undef GA_URS_IFLOOP
	return( samples_read );
}

int CMP3Encoder::readSamplesPcm( int sample_buffer[ 2304], int frame_size /* unused */, int samples_to_read)
{
	int     samples_read;
	int     iswav = (input_format == sf_wave);
	int     hi_lo_order;	/* byte order of input stream */

	if (( 32 == pcmbitwidth) || (24 == pcmbitwidth) || (16 == pcmbitwidth) ) 
	{
		/* assume only recognized wav files are */
		/*  in little endian byte order */
		hi_lo_order = ( !iswav == !pcmswapbytes);
		samples_read = unpackReadSamples( samples_to_read, pcmbitwidth / 8, hi_lo_order,sample_buffer);

	} 
	else if( 8 == pcmbitwidth ) 
	{
		samples_read = unpackReadSamples( samples_to_read, 1, 0, sample_buffer);
	} 
	else 
	{
//		if( silent < 10 ) {
//			fprintf(stderr, "Only 8, 16, 24 and 32 bit input files supported \n");
//		}
		//exit(1);
		return -1;
	}	

	return samples_read;
}

int CMP3Encoder::getAudioCommon( int buffer[2][1152], short buffer16[2][1152])
{
	int     num_channels = lame_get_num_channels( this->gf);
	int     insamp[ 2 * 1152];
	short   buf_tmp16[ 2][ 1152];
	int     samples_read;
	int     framesize;
	int     samples_to_read;
	int remaining, tmp_num_samples;
	int     i;
	int     *p;

	/* 
	* NOTE: LAME can now handle arbritray size input data packets,
	* so there is no reason to read the input data in chuncks of
	* size "framesize".  EXCEPT:  the LAME graphical frame analyzer 
	* will get out of sync if we read more than framesize worth of data.
	*/

	samples_to_read = framesize = lame_get_framesize( gf);
	assert( framesize <= 1152);

	/* get num_samples */
	tmp_num_samples = lame_get_num_samples( gf);

	/* if this flag has been set, then we are carefull to read
	* exactly num_samples and no more.  This is useful for .wav and .aiff
	* files which have id3 or other tags at the end.  Note that if you
	* are using LIBSNDFILE, this is not necessary 
	*/
	if ( count_samples_carefully) 
	{
		remaining = tmp_num_samples - Min(tmp_num_samples, num_samples_read);
		if (remaining < framesize && 0 != tmp_num_samples)
			/* in case the input is a FIFO (at least it's reproducible with
			a FIFO) tmp_num_samples may be 0 and therefore remaining
			would be 0, but we need to read some samples, so don't
			change samples_to_read to the wrong value in this case */
			samples_to_read = remaining;
	}


	samples_read = readSamplesPcm( insamp, num_channels * framesize, num_channels * samples_to_read);

	p = insamp + samples_read;
	samples_read /= num_channels;

	if( buffer != NULL ) 
	{	/* output to int buffer */
		if( num_channels == 2 ) {
			for( i = samples_read; --i >= 0; ) {
				buffer[1][i] = *--p;
				buffer[0][i] = *--p;
			}
		} else if( num_channels == 1 ) {
			memset( buffer[1], 0, samples_read * sizeof(int) );
			for( i = samples_read; --i >= 0; ) {
				buffer[0][i] = *--p;
			}
		} else
			assert(0);
	} 
	else 
	{		/* convert from int; output to 16-bit buffer */
		if( num_channels == 2 ) {
			for( i = samples_read; --i >= 0; ) {
				buffer16[1][i] = *--p >> (8 * sizeof(int) - 16);
				buffer16[0][i] = *--p >> (8 * sizeof(int) - 16);
			}
		} else if( num_channels == 1 ) {
			memset( buffer16[1], 0, samples_read * sizeof(short) );
			for( i = samples_read; --i >= 0; ) {
				buffer16[0][i] = *--p >> (8 * sizeof(int) - 16);
			}
		} else
			assert(0);
	}
	

	/* LAME mp3 output 16bit -  convert to int, if necessary */
	if( input_format == sf_mp1 || input_format == sf_mp2 || 
		input_format == sf_mp3) 
	{
		if( buffer != NULL )
		{
			for( i = samples_read; --i >= 0; )
				buffer[0][i] = buf_tmp16[0][i] << (8 * sizeof(int) - 16);
			if( num_channels == 2 ) {
				for( i = samples_read; --i >= 0; )
					buffer[1][i] = buf_tmp16[1][i] << (8 * sizeof(int) - 16);
			} else if( num_channels == 1 ) {
				memset( buffer[1], 0, samples_read * sizeof(int) );
			} else
				assert(0);
		}
	}


	/* if num_samples = MAX_U_32_NUM, then it is considered infinitely long.
	Don't count the samples */
	if ( tmp_num_samples != MAX_U_32_NUM )
		num_samples_read += samples_read;

	return samples_read;
}

int CMP3Encoder::getAudio( int buffer[2][1152])
{
	if ( infile == NULL)
		return( getAudioCommon( buffer, NULL));
	return -1;
}

//////////////////////////////////////////////////////////////////////////

CFrameManager *CMP3Encoder::getFrameManager()
{
	return frameManager;
}

//////////////////////////////////////////////////////////////////////////
int CMP3Encoder::encode()
{
	if ( !openMP3File( recordState->getFileName()))
		return -1;		

	for ( int i = 0; i < frameCount; i++)
		if ( encode_sample() == -1)
			return -1;

	if ( setEmptySample() < 0)
		return -1;

	int bytes;
	if (( bytes = encode_flush()) < 0)
		return -1;

	closeMP3File();	
	ATLASSERT( _CrtCheckMemory( ));

	return bytes;
}

////////////////////////////////W//////////////////////////////////////////

BOOL CMP3Encoder::setFileName( WCHAR *sFileName)
{
	//ATLASSERT( _CrtCheckMemory( ));
	assert( sFileName);
	if ( sFileName == NULL)	
		return FALSE;

	WCHAR *lpFilePath = MP_NEW_ARR( WCHAR, MAX_PATH);
	SHGetSpecialFolderPathW(NULL, lpFilePath, CSIDL_APPDATA, true);
	wcscat_s( lpFilePath, MAX_PATH, L"\\");
	wcscat_s( lpFilePath, MAX_PATH, sFileName);
	if ( recordState == NULL)
	{
		recordState = MP_NEW( CRecordState);
	}
	recordState->setFileName( lpFilePath);
	MP_DELETE_ARR( lpFilePath);
	//ATLASSERT( _CrtCheckMemory( ));
	return openNewMP3File( recordState->getFileName());
}

void CMP3Encoder::setParams( int channels, int sampleRate, int sampleSize)
{
	if ( recordState == NULL)
		return;

	if ( bParamsSetted)
		return;
	
	ATLASSERT( _CrtCheckMemory( ));
	recordState->setChannels( channels);
	recordState->setSamples_per_sec( sampleRate);
	recordState->setBits_per_sample( sampleSize);
	recordState->setRecordState( TRUE);	
	bParamsSetted = TRUE;
}

int CMP3Encoder::setMP3FileParams( int bit_brate, int iChannelMode, int iQuality, int iSampleRate)
{
	m_iBitBrate = bit_brate;
	m_iChannelMode = iChannelMode;
	m_iQuality = iQuality;
	m_iSampleRate = iSampleRate;
	return 1;
}

BOOL CMP3Encoder::init( int channels, int sampleRate, int sampleSize, WCHAR *sFileName)
{	
	setParams( channels, sampleRate, sampleSize);
	return setFileName( sFileName);
}

BOOL CMP3Encoder::clearSoundFileLog()
{
	if ( openNewMP3File( recordState->getFileName()))
	{
		fclose( outfile);
		outfile = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL CMP3Encoder::run( BOOL isBufferSaving)
{
	assert( recordState);
	ATLASSERT( _CrtCheckMemory( ));
	if ( recordState == NULL)
		return FALSE;

	if ( !lameInit())
	{
		removeAllData();
		return FALSE;
	}

	if ( !setMP3Params( recordState->getChannels(), recordState->getSamples_per_sec(), recordState->getBits_per_sample()))
	{
		removeAllData();
		lameClose();
		return FALSE;
	}

	if ( encode() <= 0)
	{
		removeAllData();
		lameClose();
		return FALSE;
	}

	if ( !isBufferSaving)
	{
		removeAllData();
	}
	lameClose();
	return TRUE;
}

BOOL CMP3Encoder::save()
{	
	CWaitAndSignal m( updateStateMutex);
	if ( recordState != NULL)
		recordState->setRecordState( FALSE);
	CFrameManager *pFrameManager = NULL;	
	if ( bFirst)	
		pFrameManager = frameManager;
	else
		pFrameManager = frameManager2;
	bFirst = !bFirst;
	if ( pFrameManager->getFrameCount() > 0)
	{
		BOOL bSucc = run();			
		return bSucc;
	}	
	return TRUE;
}

BOOL CMP3Encoder::isRecording()
{
	CWaitAndSignal m( updateStateMutex);
	return recordState != NULL && recordState->getRecordState();
}

//////////////////////////////////////////////////////////////////////////

void CMP3Encoder::setBlockNumber( int blockNumber)
{
	this->blockNumber = blockNumber;
}

wchar_t *CMP3Encoder::getFileName()
{
	if ( recordState != NULL)
		return recordState->getFileName();
	else 
		return L"";		
}

void CMP3Encoder::Main()
{
	bTerminated = FALSE;
	while( true)
	{
		if ( bRunSaver)
		{
			updateStateMutex.Wait();
			run( TRUE);
			bRunSaver = FALSE;
			CFrameManager *pFrameManager = NULL;
			if ( bFirst)	
				pFrameManager = frameManager2;
			else
				pFrameManager = frameManager;
			pFrameManager->removeAllFrames();
			updateStateMutex.Signal();
		}
		if ( bTerminating)
			break;
		Sleep( 100);
	}
	bTerminated = TRUE;
}

void CMP3Encoder::Terminating()
{
	bTerminating = TRUE;
}

void CMP3Encoder::startSaving()
{
	bRunSaver = TRUE;
}

BOOL CMP3Encoder::getFileFullNameFromUuid( CWComString &tmpFileFullName, CWComString &tmpFileName, CWComString firstLine /* = "" */, CWComString lastLine /* = "" */)
{	
	wchar_t *szTempDir = MP_NEW_ARR( wchar_t, 255);
	HRESULT res;
	res = ::GetTempPathW( 255, szTempDir);	
	//BSTR pbstrGUID;
	UUID uuid;
	CComString sPar;

	if ( UuidCreate( &uuid) != RPC_S_OK)
	{					
		sPar.Format( "onSoundLog|%s", "generated uuid is failed");
		return FALSE;
	}//*/

	unsigned char *sUUID;
	if ( UuidToString( &uuid, &sUUID) != RPC_S_OK)
	{					
		sPar.Format( "onSoundLog|%s", "generated fileName is failed");
		return FALSE;
	}
	
	USES_CONVERSION;
	tmpFileName.Format( L"%s", A2W((char*)sUUID));
	RpcStringFree( &sUUID);
	if ( RpcStringFree( &sUUID) != RPC_S_OK)
	{
		sPar.Format( "onSoundLog|%s", "UUID String deleting is failed");		
	}

	tmpFileFullName.Format( L"%s%s%s%s.mp3", szTempDir, firstLine.GetBuffer(), tmpFileName.GetBuffer(), lastLine.GetBuffer());
	
	return TRUE;
}

BOOL CMP3Encoder::getPathFromName( char *name, wchar_t *path)
{
	if ( name == NULL)
	{
#ifdef _DEBUG
		name = "OpenH323sd.dll";
#else
		name = "OpenH323s.dll";
#endif
	}
	wchar_t lpFileName[ MAX_PATH];
	HMODULE hModule = GetModuleHandle( name);

	if ( GetModuleFileNameW( hModule, lpFileName, MAX_PATH) == 0)
	{
		int codeerror = GetLastError();
		return FALSE;
	}

	if ( path == NULL)
		path = MP_NEW_ARR( wchar_t, wcslen( lpFileName) - strlen( name) + 1);

	memset( path, 0, MAX_PATH);
	memcpy( path, lpFileName, wcslen( lpFileName) - strlen( name));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL bMP3Turn = FALSE;

void CMP3Encoder::turnOn()
{
	bMP3Turn = TRUE;
}

void CMP3Encoder::turnOff()
{
	bMP3Turn = FALSE;
}

BOOL CMP3Encoder::isTurn()
{
	return bMP3Turn;
}
