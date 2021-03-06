#ifndef INPUT_MOD_H
#define INPUT_MOD_H


#include <dumb.h>
#include "audiere.h"
#include "basic_source.h"
#include "types.h"


namespace audiere {

  class MODInputStream : public BasicSource {
  public:
    MODInputStream();

	void destroy() { MP_DELETE_THIS; };

    ~MODInputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    void ADR_CALL reset();

    int doRead(int frame_count, void* buffer);

#if AUDIERE_BUILD_H
	FileFormat ADR_CALL getFileType(){ return FF_MOD;};
#endif

#if AUDIERE_RELEASE_H
	/**
	* @return  size of header of file
	*/
	int	 ADR_CALL getHeaderSize(){ return 0;};	
#endif

  private:
    DUH* openDUH();

    static void* dfs_open(const char* filename);
    static int   dfs_skip(void* f, long n);
    static int   dfs_getc(void* f);
    static long  dfs_getnc(char* ptr, long n, void* f);
    static void  dfs_close(void* f);

    static int loopCallback(void* ptr);

  private:
    FilePtr          m_file;
    DUH*             m_duh;
    DUH_SIGRENDERER* m_renderer;
  };

}


#endif
