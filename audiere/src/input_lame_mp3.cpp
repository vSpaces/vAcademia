#include "stdafx.h"
#include <string.h>
#include "input_lame_mp3.h"
#include "utility.h"
#include "debug.h"
#include "Mp3Func.h"

#ifndef Min
#define Min(A, B)       ((A) < (B) ? (A) : (B))
#endif

namespace audiere 
{
	MP3LameInputStream::MP3LameInputStream():MP_VECTOR_INIT(m_frame_sizes),MP_VECTOR_INIT(m_frame_offsets) {
    m_eof = false;	

    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;
	
	m_context = 0;

    m_input_position = 0;
    m_input_length = 0;
    m_decode_buffer = 0;
    m_first_frame = true;

    m_seekable = false;
    m_length = 0;
    m_position = 0;

#if	AUDIERE_BUILD_H
	m_file = 0;	
	m_next_context = 0;
	m_next_decode_buffer = 0;
	m_next_input_position = 0;
	m_next_input_length = 0;
	m_update_data_size = 0;
#endif
	memset(&mp3data, 0, sizeof(mp3data_struct));
	skip = 0;
	m_parse_only = false;
	num_samples_read = 0;
	count_samples_carefully = true;
	m_FileSize = 0;
	m_next_eof = false;
  }

  
  MP3LameInputStream::~MP3LameInputStream() {    
	MP_DELETE_ARR( m_decode_buffer);	
	MP_DELETE(m_next_context);
#if AUDIERE_BUILD_H

	int size = m_frame_sizes.size();
	for (int i = 0; i < size; i++)
	{
		TLameFrameSize *pFrameSize = m_frame_sizes[ i];
		if ( pFrameSize)
		{			
			MP_DELETE(pFrameSize);
		}
	}
	m_frame_sizes.clear();
	m_frame_offsets.clear();
	
	MP_DELETE_ARR(m_next_decode_buffer);
#endif
  }

int  MP3LameInputStream::decodeFirstFrame()
{
	/* set the defaults from info incase we cannot determine them from file */
	lame_set_num_samples( gfp, UINT_MAX );
	
	unsigned char buf[100];
	int     ret;
	int     len, aid_header;
	short int pcm_l[MP3LAME_FRAME_SIZE], pcm_r[MP3LAME_FRAME_SIZE];
	int freeformat = 0;
	memset(&mp3data, 0, sizeof(mp3data_struct));
	lame_decode_init();
	len = 4;
	if (CMp3Func::fread(buf, 1, len, m_file) != len)
		return -1;      /* failed */
	
	if (buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3') 
	{
		ADR_LOG("ID3v2 found. Be aware that the ID3 tag is currently lost when transcoding.\n");
		
		len = 6;
		if (CMp3Func::fread(&buf, 1, len, m_file) != len)
			return -1;      /* failed */
		buf[2] &= 127; buf[3] &= 127; buf[4] &= 127; buf[5] &= 127;
		
		len = (((((buf[2] << 7) + buf[3]) << 7) + buf[4]) << 7) + buf[5];
		CMp3Func::fskip(m_file, len, SEEK_CUR);
		len = 4;
		if (CMp3Func::fread(&buf, 1, len, m_file) != len)
			return -1;      /* failed */
	}

	aid_header = CMp3Func::check_aid(buf);
	if (aid_header) 
	{
		if (CMp3Func::fread(&buf, 1, 2, m_file) != 2)
			return -1;  /* failed */
		aid_header = (unsigned char) buf[0] + 256 * (unsigned char) buf[1];
		ADR_LOG("Album ID found.  length " + aid_header);

		/* skip rest of AID, except for 6 bytes we have already read */
		CMp3Func::fskip(m_file, aid_header - 6, SEEK_CUR);

		/* read 4 more bytes to set up buffer for MP3 header check */
		if (CMp3Func::fread(&buf, 1, len, m_file) != len)
			return -1;      /* failed */
	}

	len = 4;
	while (!CMp3Func::is_syncword_mp123(buf)) {
		int     i;
		for (i = 0; i < len - 1; i++)
			buf[i] = buf[i + 1];
		if (CMp3Func::fread(buf + len - 1, 1, 1, m_file) != 1)
			return -1;  /* failed */
	}

	if ((buf[2] & 0xf0)==0) 
	{
		ADR_LOG("Input file is freeformat.\n");
		freeformat = 1;
	}

	ret = lame_decode1_headersB(buf, len, pcm_l, pcm_r, &mp3data,&enc_delay,&enc_padding);
	if (-1 == ret)
		return -1;

	/* repeat until we decode a valid mp3 header.  */
	while (!mp3data.header_parsed) 
	{
		len = CMp3Func::fread(buf, 1, sizeof(buf), m_file);
		if (len != sizeof(buf))
			return -1;
		ret = lame_decode1_headersB(buf, len, pcm_l, pcm_r, &mp3data,&enc_delay,&enc_padding);
		if (-1 == ret)
			return -1;
	}

	if (mp3data.bitrate==0 && !freeformat) 
	{
		ADR_LOG("Input file is freeformat.\n");
		return decodeFirstFrame();
	}

	if (mp3data.totalframes > 0) 
	{
		/* mpglib found a Xing VBR header and computed nsamp & totalframes */
	}
	else {
		/* set as unknown.  Later, we will take a guess based on file size ant bitrate */
		mp3data.nsamp = UINT_MAX;
	}

	if (m_first_frame) 
	{
		m_channel_count = mp3data.stereo;
		m_sample_rate = mp3data.samplerate;
		m_sample_format = SF_S16;
#if AUDIERE_RELEASE_H
		m_header_size = m_file->tell();
#endif
	} 
	return 0;
}

  bool
  MP3LameInputStream::initialize(FilePtr file) {
    m_file = file;

	m_seekable = m_file->seek(0, File::END);
    readID3v1Tags();
    readID3v2Tags();
	m_file->seek(0, File::BEGIN);

    m_eof = false;	

	// initialize libmp3lame
	gfp = lame_init();
	if (gfp == NULL) 
	{
		ADR_LOG("fatal error during initialization\n");
		return false;
	}

	m_input_position = 0;
    m_input_length = 0;
	
	lame_set_decode_only(gfp, 1);
	lame_set_findReplayGain(gfp,1);
	
	if(decodeFirstFrame() == -1)
	{
		return false;
	}

	if (!initLameDecoder())
	{
		return false;
	}

#if AUDIERE_BUILD_H	
	m_next_decode_buffer = MP_NEW_ARR(u8, MPAUDEC_MAX_AUDIO_FRAME_SIZE);
	if (!m_next_decode_buffer)
		return false;	
#endif    
	m_decode_buffer = MP_NEW_ARR(u8, MPAUDEC_MAX_AUDIO_FRAME_SIZE);
    if (!m_decode_buffer)
        return false;

    m_first_frame = true;
	m_parse_only = false;
	
	m_next_context = MP_NEW(MPAuDecContext);
	memset(m_next_context, 0, sizeof(MPAuDecContext));

	/*if (m_seekable && m_parse_only) 
	{
		m_length = 0;
      // Scan the file to determine the length.
#if	AUDIERE_BUILD_H
		while ( !m_next_eof) 
		{
			if (!decodeNextFrame(m_parse_only, true))
				return false;

			if ( m_next_input_length < INPUT_BUFFER_SIZE)
			{
				int x = 0;
			}

			if ( !m_next_eof)
				{
				//assert( m_context->frame_size == 1152);
				TLameFrameSize *pFrameSize = new TLameFrameSize();
				pFrameSize->frame_size = m_next_context->frame_size;
				pFrameSize->total_size = m_length;
				m_frame_sizes.push_back( pFrameSize);
			}
	
			int frame_ffset = m_file->tell() -
				(m_next_input_length - m_next_input_position) -
				m_next_context->coded_frame_size;
			m_frame_offsets.push_back(frame_offset);
			m_length += m_next_context->frame_size;
		}
		m_update_data_size = m_length;
/*#else
		m_context->m_parse_only = 1;
		while (!m_eof) 
		{
			if (!decodeFrame())
				return false;
			if (!m_eof)
	#if AUDIERE_BUILD_F
			{
				//assert( m_context->frame_size == 1152);
				TLameFrameSize *pFrameSize = new TLameFrameSize();
				pFrameSize->frame_size = m_context->frame_size;
				pFrameSize->total_size = m_length;
				m_frame_sizes.push_back( pFrameSize);
			}
	#else
			m_frame_sizes.push_back(m_context->frame_size);
	#endif
			int frame_offset = m_file->tell() -
                             (m_input_length - m_input_position) -
                             m_context->coded_frame_size;
			m_frame_offsets.push_back(frame_offset);
			m_length += m_context->frame_size;
		}
#endif
		reset();
    }*/
	return true;
    // this should fill in the audio format if it isn't set already
   // return decodeFrame();
  }
 
//////////////////////////////////////////////////////////////////////////

bool MP3LameInputStream::initLameDecoder()
{
	if( -1 == lame_set_num_channels(gfp, mp3data.stereo ) ) 
	{
		ADR_LOG("Unsupported number of channels: "	+ mp3data.stereo );
		return false;
	}
	lame_set_in_samplerate(gfp, mp3data.samplerate);
	lame_set_num_samples(gfp, mp3data.nsamp);

	if (lame_get_num_samples(gfp) == UINT_MAX) 
	{
		m_FileSize = CMp3Func::getFileSize(m_file); /* try to figure out num_samples */
		if (m_FileSize >= 0) 
		{
			if (mp3data.bitrate > 0) 
			{
				double  totalseconds = (m_FileSize * 8.0 / (1000.0 * mp3data.bitrate));
				unsigned long tmp_num_samples =		
					(unsigned long) (totalseconds * lame_get_in_samplerate( gfp ));
				lame_set_num_samples(gfp, tmp_num_samples);
				mp3data.nsamp = tmp_num_samples;
				m_length = mp3data.nsamp;		
			}
			else 
			{
				lame_set_num_samples(gfp, (unsigned long)(m_FileSize / (2 * lame_get_num_channels( gfp ))) );
			}
		}
	}
	int i = lame_init_params(gfp);
	if (i < 0) 
	{
		ADR_LOG("fatal error during initialization\n");
		return false;
	}
	if (!lame_get_decode_only(gfp)) 
	{
		return false;
	}
	
	if (enc_delay>-1) 
		skip = enc_delay + 528+1;
	else 
		skip = lame_get_encoder_delay(gfp) + 528 + 1;
	return true;
}

  bool
  MP3LameInputStream::isSeekable() {
    return m_seekable;
  }

  int
  MP3LameInputStream::getPosition() {
     return m_position;
  }

  void
  MP3LameInputStream::setPosition(int position) {
	  if ( !m_seekable || position > m_length)
      return;
    int scan_position = 0;
    int target_frame = 0;
	m_mutex.lock();
    int frame_count = m_frame_sizes.size();
	m_mutex.unlock();

    if ( frame_count == 0)
	{
		/*if(!mp3data.nsamp)
		{	
			return;
		}
		double filePos = (m_FileSize * position)/mp3data.nsamp;
		m_file->seek((int)filePos, File::BEGIN);*/
		return;
	}

#if AUDIERE_BUILD_F
	target_frame = position / MP3LAME_FRAME_SIZE;
	if ( target_frame < frame_count - 1)
	{
		TLameFrameSize *pFrameSize = m_frame_sizes[ target_frame];
		while ( position <= pFrameSize->total_size && target_frame >= 1)
		{
			target_frame--;
			pFrameSize = m_frame_sizes[ target_frame];
		}
	}
	else
		target_frame = frame_count - 1;


	TLameFrameSize *pFirstFrameSize = m_frame_sizes[ target_frame];	
	
	scan_position = pFirstFrameSize->total_size;

	while ( target_frame < frame_count) {
		TLameFrameSize *pFrameSize = m_frame_sizes[ target_frame];
		int frame_size = pFrameSize->frame_size;
		if ( position <= scan_position + frame_size)
			break;
		else {
			scan_position += frame_size;
			target_frame++;
		}
	}

#else

    while (target_frame < frame_count) {
      int frame_size = m_frame_sizes[target_frame];
      if (position <= scan_position + frame_size)
        break;
      else {
        scan_position += frame_size;
        target_frame++;.
      }
    }

#endif
    // foobar2000's MP3 input plugin decodes and throws away the 10 frames
    // before the target frame whenever possible, presumably to ensure correct
    // output when jumping into the middle of a stream.  So we'll do that here.
#if AUDIERE_BUILD_F
    const int MAX_FRAME_DEPENDENCY = 10;
#else
	const int MAX_FRAME_DEPENDENCY = 10;
#endif
    target_frame = std::max(0, target_frame - MAX_FRAME_DEPENDENCY);
    reset();

#if AUDIERE_BUILD_H
	m_mutex.lock();
	int frame_offset = m_frame_offsets[ target_frame];
	m_mutex.unlock();
	m_file->seek(frame_offset, File::BEGIN);
#else
    m_file->seek(m_frame_offsets[target_frame], File::BEGIN);
#endif

    int i;
#if AUDIERE_BUILD_F
	m_position += (( TLameFrameSize *) m_frame_sizes[ target_frame])->total_size;
#else
    for (i = 0; i < target_frame; i++) {
      m_position += m_frame_sizes[i];
    }
#endif
    if (!decodeFrame(false) || m_eof) {
      reset();
      return;
    }
    int frames_to_consume = position - m_position; // PCM frames now
    if (frames_to_consume > 0) {      
	  u8 *buf = MP_NEW_ARR(u8, frames_to_consume * GetFrameSize(this));
      doRead(frames_to_consume, buf);      
	  MP_DELETE_ARR_UNSAFE(buf);
    }
  }



#if	AUDIERE_BUILD_H
  int 
  MP3LameInputStream::updateFileInfo( int loaded_bytes, bool bLoaded){
	int frame_count = m_frame_sizes.size();
	TLameFrameSize *pFrameSize = m_frame_sizes[ frame_count - 1];
	int length = pFrameSize->total_size + pFrameSize->frame_size;
	
	if ( m_update_data_size == loaded_bytes)
		return -2;

	if ( !bLoaded && loaded_bytes - m_file->getTotalReadBytes() < 65536)
		return -1;

	m_next_context->parse_only = 1;
	m_next_eof = false;

	if ( m_update_data_size == -1)
		return 1;
	
	bool bFirstNextFrame = true;

	while ( !m_next_eof) 
	{
		if ( !decodeNextFrame(m_parse_only))
			return 2;

		if ( m_next_input_length < INPUT_BUFFER_SIZE)
		{
			int x = 0;
		}

		if ( !bFirstNextFrame)
		{
			if ( !m_next_eof)
			{				
				TLameFrameSize *pFrameSize = MP_NEW( TLameFrameSize);
				pFrameSize->frame_size = m_next_context->frame_size;
				pFrameSize->total_size = m_length;
				m_frame_sizes.push_back( pFrameSize);
			}
			int frame_offset = m_file->getTotalReadBytes() -
				(m_next_input_length - m_next_input_position) -
				m_next_context->coded_frame_size;
			m_frame_offsets.push_back(frame_offset);
			m_length += m_next_context->frame_size;
		}
		else
		{
			if ( m_next_eof)
				return 3;
			bFirstNextFrame = false;
			m_mutex.lock();
			m_length -= m_next_context->frame_size;
			m_frame_offsets.pop_back();
			m_mutex.unlock();

			if ( !m_next_eof)
			{				
				TLameFrameSize *pFrameSize = MP_NEW( TLameFrameSize);
				pFrameSize->frame_size = m_next_context->frame_size;
				pFrameSize->total_size = m_length;
				m_frame_sizes.push_back( pFrameSize);
			}
			int frame_offset = m_file->getTotalReadBytes() -
				(m_next_input_length - m_next_input_position) -
				m_next_context->coded_frame_size;
			m_frame_offsets.push_back(frame_offset);
			m_length += m_next_context->frame_size;
		}
	}

	m_next_buffer.clear();
	//mpaudec_clear(m_next_context);
	//mpaudec_init(m_next_context);

	m_next_input_position = 0;
	m_next_input_length = 0;	

	m_eof = false;
	m_update_data_size = loaded_bytes;

	return 0;
  }
#endif

  int
  MP3LameInputStream::getLength() {
    return m_length;
  }

#if AUDIERE_BUILD_H
  int
  MP3LameInputStream::getSize() {    
    return 0;
  }  
#endif

  void
  MP3LameInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate = m_sample_rate;
    sample_format = m_sample_format;
  }

  
int MP3LameInputStream::doRead(int frame_count, void* samples) 
{
    ADR_GUARD("MP3LameInputStream::doRead");
	
	const int frame_size = mp3data.stereo * GetSampleSize(m_sample_format);
		
    int frames_read = 0;
    u8* out = (u8*)samples;

    while (frames_read < frame_count) 
	{
		// no more samples?  ask the MP3 for more
		if (m_buffer.getSize() < frame_size) 
		{
			if (!decodeFrame(false) || m_eof) 
			{
				// done decoding?
				return frames_read;
			}

			// if the buffer is still empty, we are done
			if (m_buffer.getSize() < frame_size)
			{
				return frames_read;
			}
		 }
		const int frames_left = frame_count - frames_read;
		const int frames_to_read = std::min (
				frames_left,
				m_buffer.getSize() / frame_size);
		m_buffer.read(out, frames_to_read * frame_size);
		out += frames_to_read * frame_size;
		frames_read += frames_to_read;
		m_position += frames_to_read;
	}
    return frames_read;
}


  void
  MP3LameInputStream::reset() {
    ADR_GUARD("MP3LameInputStream::reset");

    m_file->seek(0, File::BEGIN);
    m_eof = false;
	m_next_eof = false;

    m_buffer.clear();

	memset(&mp3data, 0, sizeof(mp3data_struct));
	
	if(decodeFirstFrame() == -1)
	{
		return;
	}

	if (!initLameDecoder())
	{
		return;
	}

	m_input_position = 0;
    m_input_length = 0;
    m_position = 0;
	m_parse_only = false;
	memset(m_next_context, 0, sizeof(MPAuDecContext));
  }

int MP3LameInputStream::lame_decode_fromfile(short pcm_l[], short pcm_r[], int &read_len, bool parse_only, bool bInit) 
{
	int ret = 0, len = 0;
	read_len = 0;
	unsigned char buf[INPUT_BUFFER_SIZE];
	/* first see if we still have data buffered in the decoder: */
	if(!parse_only)
    	ret = lame_decode1_headers(buf, len, pcm_l, pcm_r, &mp3data);
	
	if (ret != 0) 
		return ret;
	/* read until we get a valid output frame */
	while (1) 
	{
		len = CMp3Func::fread(buf, 1, INPUT_BUFFER_SIZE, m_file);
		read_len += len; 
		if (len == 0) 
		{
			/* we are done reading the file, but check for buffered data */
			if (!parse_only)
                ret = lame_decode1_headers(buf, len, pcm_l, pcm_r, &mp3data);
			/*else
				ret = -1;*/

			if (ret <= 0) 
			{//error release memory
				/*if (parse_only && !m_next_eof)
					lame_decode_exit(); /* release mp3decoder memory */
				if (!parse_only && !m_eof)
					lame_decode_exit(); /* release mp3decoder memory */
				return -1;  /* done with file */
			}
			break;
		}
		if (parse_only)
		{
			/*ret = lame_decode1_headers_parseonly(buf, len, pcm_l, pcm_r, &mp3data);
			if (m_first_frame)
			{
				m_next_input_position = mp3data.coded_frame_size*2;
				m_first_frame = false;
			}
			else
				m_next_input_position += mp3data.coded_frame_size;*/
		}
		else
			ret = lame_decode1_headers(buf, len, pcm_l, pcm_r, &mp3data);
		if (ret == -1) 
		{
			lame_decode_exit();  /* release mp3decoder memory */
			return -1;
		}
		if (ret >0) 
			break;
	}
	return ret;
}

int MP3LameInputStream::get_audio_common(short buffer[2][MP3LAME_FRAME_SIZE], int &len, bool parse_only, bool bInit)
{
	int     num_channels = lame_get_num_channels( gfp );
	int     samples_read;
	int     framesize;
	int     samples_to_read;
	unsigned int remaining, tmp_num_samples;

	samples_to_read = framesize = lame_get_framesize(gfp);
	assert(framesize <= MP3LAME_FRAME_SIZE);

	tmp_num_samples = lame_get_num_samples( gfp );

	if (count_samples_carefully) 
	{
		remaining = tmp_num_samples - Min(tmp_num_samples, num_samples_read);
		if (remaining < framesize && 0 != tmp_num_samples)
			samples_to_read = remaining;
	}

	samples_read = lame_decode_fromfile(buffer[0], buffer[1], len, parse_only, bInit);
	
	/*
	* samples_read < 0:  error, probably EOF
	* samples_read = 0:  not possible with lame_decode_fromfile() ???
	* samples_read > 0:  number of output samples
	*/

	if (samples_read < 0) 
	{
		memset(buffer, 0, sizeof(**buffer) * 2 * MP3LAME_FRAME_SIZE);
	}

	if (tmp_num_samples != UINT_MAX)
		num_samples_read += samples_read;
	
	return samples_read;
}



bool MP3LameInputStream::decodeFrame(bool parse_only) 
{
	short int Buffer[2][MP3LAME_FRAME_SIZE];
	int iread = get_audio_common(Buffer, m_next_input_length, parse_only); /* read in 'iread' samples */

	if (iread < 0)
	{
		m_eof = true;
		return true;
	}
	else if (iread == 0)
	{
		ADR_LOG("Not possible with lame_decode_from_file\n");
		return false;
	}

	if (lame_get_num_channels(gfp) != mp3data.stereo)
	{	
		ADR_LOG("Error: number of channels has changed and not supported\n");
		return false;
	}
	if (lame_get_in_samplerate(gfp) != mp3data.samplerate)
	{
		ADR_LOG("Error: sample frequency has changed and not supported\n");
		return false;
	}	

	mp3data.framenum += iread / mp3data.framesize;
	int i;
	skip -= (i = skip < iread ? skip : iread); /* 'i' samples are to skip in this frame */
	if (!parse_only)
	{
		for (; i < iread; i++) 
		{
			m_buffer.write((char *) &Buffer[0][i], sizeof(short));
			if (mp3data.stereo == 2)
			{
				m_buffer.write((char *) &Buffer[1][i], sizeof(short));
			}
		}
	}
	return true;
}

#if AUDIERE_BUILD_H
bool MP3LameInputStream::decodeNextFrame(bool parse_only, bool bInit) 
{
	short int Buffer[2][MP3LAME_FRAME_SIZE];
	if (m_next_input_position == m_next_input_length) 
	{
		m_next_input_position = 0;
	}
	
	m_next_input_length = 0;
	int iread = get_audio_common(Buffer, m_next_input_length, parse_only, bInit); 
	if (iread < 0)
	{
		m_next_eof = true;
		return true;
	}
	else if (iread == 0)
	{
		ADR_LOG("Not possible with lame_decode_from_file\n");
		return false;
	}
	if (lame_get_num_channels(gfp) != mp3data.stereo)
	{	
		ADR_LOG("Error: number of channels has changed and not supported\n");
		return false;
	}
	if (lame_get_in_samplerate(gfp) != mp3data.samplerate)
	{
		ADR_LOG("Error: sample frequency has changed and not supported\n");
		return false;
	}	
	
	mp3data.framenum += iread / mp3data.framesize;
	int i;
	skip -= (i = skip < iread ? skip : iread); 
	if (!parse_only)
	{
		for (; i < iread; i++) 
		{
			m_buffer.write((char *) &Buffer[0][i], sizeof(short));
			if (mp3data.stereo == 2)
			{
				m_buffer.write((char *) &Buffer[1][i], sizeof(short));
			}
		}
	}
	else
	{
		m_next_context->frame_size = mp3data.framesize;
		m_next_context->coded_frame_size = mp3data.coded_frame_size;
	}
	return true;
}
#endif



  const char* MP3LameInputStream::getGenre(u8 code) {
    const char* genres[] = {
      // From Appendix A.3 at http://www.id3.org/id3v2-00.txt and

      "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
      "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other",
      "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",
      "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
      "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
      "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
      "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
      "Soul", "Punk", "Space", "Meditative", "Instrumental Pop",
      "Instrumental Rock", "Ethnic", "Gothic", "Darkwave",
      "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
      "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40",
      "Christian Rap", "Pop/Funk", "Jungle", "Native American",
      "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes",
      "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka",
      "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk-Rock",
      "National Folk", "Swing", "Fast Fusion", "Bebob", "Latin", "Revival",
      "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
      "Progressive Rock", "Psychedelic Rock", "Symphonic Rock",
      "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",
      "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata",
      "Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire",
      "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
      "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet", "Punk Rock",
      "Drum Solo", "Acapella", "Euro-House", "Dance Hall",
      
      // http://lame.sourceforge.net/doc/html/id3.html

      "Goa", "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie",
      "BritPop", "Negerpunk", "Polsk Punk", "Beat", "Christian Gangsta",
      "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
      "Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime",
      "JPop", "SynthPop",
    };
    const int genre_count = sizeof(genres) / sizeof(*genres);

    return (code < genre_count ? genres[code] : "");
  }


  // Return a null-terminated std::string from the beginning of 'buffer'
  // up to 'maxlen' chars in length.
  std::string MP3LameInputStream::getString(u8* buffer, int maxlen) {
    char* begin = reinterpret_cast<char*>(buffer);
    int end = 0;
    for (; end < maxlen && begin[end]; ++end) {
    }
    return std::string(begin, begin + end);
  }


  void
  MP3LameInputStream::readID3v1Tags() {
    // Actually, this function reads both ID3v1 and ID3v1.1.
    if (!m_file->seek(-128, File::END)) {
      return;
    }
    u8 buffer[128];
    if (m_file->read(buffer, 128) != 128) {
      return;
    }
    // Verify that it's really an ID3 tag.
    if (memcmp(buffer + 0, "TAG", 3) != 0) {
      return;
    }
    std::string title   = getString(buffer + 3,  30);
    std::string artist  = getString(buffer + 33, 30);
    std::string album   = getString(buffer + 63, 30);
    std::string year    = getString(buffer + 93, 4);
    std::string comment = getString(buffer + 97, 30);
    std::string genre   = getGenre(buffer[127]);
    addTag("title",   title,   "ID3v1");
    addTag("artist",  artist,  "ID3v1");
    addTag("album",   album,   "ID3v1");
    addTag("year",    year,    "ID3v1");
    addTag("comment", comment, "ID3v1");
    addTag("genre",   genre,   "ID3v1");
    // This is the ID3v1.1 part.
    if (buffer[97 + 28] == 0 && buffer[97 + 29] != 0) {
      char track[20];
      sprintf(track, "%d", int(buffer[97 + 29]));
      addTag("track", track, "ID3v1.1");
    }
  }

  void
  MP3LameInputStream::readID3v2Tags() {
    // ID3v2 is super complicated.
  }

}
