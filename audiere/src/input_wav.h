#ifndef INPUT_WAV_H
#define INPUT_WAV_H


#include "audiere.h"
#include "basic_source.h"
#include "types.h"


namespace audiere {

  class WAVInputStream : public BasicSource {
  public:
    WAVInputStream();

	void destroy() { MP_DELETE_THIS; };

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int doRead(int frame_count, void* buffer);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
#if AUDIERE_BUILD_H
	int  ADR_CALL getSize();	
#endif
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();
#if AUDIERE_BUILD_H
	int	 ADR_CALL updateFileInfo(int loaded_bytes, bool bLoaded);
	FileFormat ADR_CALL getFileType(){ return FF_WAV;};
#endif

#if AUDIERE_RELEASE_H
	/**
	* @return  size of header of file
	*/
	int	 ADR_CALL getHeaderSize(){ return 44;};	
#endif

  private:
    bool findFormatChunk();
    bool findDataChunk();
    bool skipBytes(int size);

  private:
    FilePtr m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    // from data chunk
    int m_data_chunk_location; // bytes
    int m_data_chunk_length;   // in frames

#if AUDIERE_BUILD_H
	int m_data_length;         //
#endif

    int m_frames_left_in_chunk;
  };

}


#endif
