#ifndef INPUT_MP3_H
#define INPUT_MP3_H

#include "mpaudec/mpaudec.h"
#include "audiere.h"
#include "basic_source.h"
#include "types.h"
#include "utility.h"

namespace audiere {

  struct TFrameSize
  {
	  int frame_size;
	  int total_size;
  };

  class MP3InputStream : public BasicSource {
  public:
    MP3InputStream();	

	void destroy() { MP_DELETE_THIS; };

    ~MP3InputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int doRead(int frame_count, void* samples);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
#if AUDIERE_BUILD_H
	int  ADR_CALL getSize();	
#endif
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

#if	AUDIERE_BUILD_H
	int ADR_CALL updateFileInfo( int loaded_bytes, bool bLoaded);
	FileFormat ADR_CALL getFileType(){ return FF_MP3;};
#endif

#if AUDIERE_RELEASE_H
	int ADR_CALL getHeaderSize();	
#endif

  private:
    const char* getGenre(u8 code);
	std::string getString(u8* buffer, int maxlen);
    void readID3v1Tags();
    void readID3v2Tags();
    bool decodeFrame();
#if AUDIERE_BUILD_H
	bool decodeNextFrame( bool bInit = false);
#endif

    FilePtr m_file;
    bool m_eof;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    MPAuDecContext* m_context;
    
    QueueBuffer m_buffer;

#if	AUDIERE_BUILD_H
	bool m_next_eof;
	MPAuDecContext* m_next_context;
	QueueBuffer m_next_buffer;
#endif

    enum { INPUT_BUFFER_SIZE = 4096 };
    u8 m_input_buffer[INPUT_BUFFER_SIZE];
    int m_input_position;
    int m_input_length;
    u8* m_decode_buffer;
    bool m_first_frame;

    bool m_seekable;
    int m_length;
    int m_position;
#if AUDIERE_BUILD_F    
	MP_VECTOR<TFrameSize *> m_frame_sizes;
#else
    MP_VECTOR<int> m_frame_sizes;
#endif    
	MP_VECTOR<int> m_frame_offsets;
#if	AUDIERE_BUILD_H	
	u8 m_next_input_buffer[INPUT_BUFFER_SIZE];
	int m_next_input_position;
	int m_next_input_length;
	int m_update_data_size;
	u8* m_next_decode_buffer;
#endif
#if AUDIERE_RELEASE_H
	int m_header_size;
#endif
#if AUDIERE_RELEASE_O	
	// from format chunk
	int m_max_channel_count;
	int m_max_sample_rate;
#endif
  };

}

#endif
