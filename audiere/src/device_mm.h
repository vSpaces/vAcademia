#ifndef DEVICE_MM_H
#define DEVICE_MM_H

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include <windows.h>
#include <mmsystem.h>
#include <queue>
#include "internal.h"
#include "device_mixer.h"
#include "utility.h"


namespace audiere {

  class MMAudioDevice : public MixerDevice
  {
  public:
    static MMAudioDevice* create(const ParameterList& parameters);

	void destroy() { MP_DELETE_THIS; };

  private:
    MMAudioDevice(HWAVEOUT device, int rate);
    ~MMAudioDevice();

  public:
    void ADR_CALL update();
    const char* ADR_CALL getName();

  private:
    // 16 buffers of 1000 frames is 4000 frames at 44100 Hz is about
    // 364 milliseconds of audio
    enum {
      BUFFER_COUNT  = 16,
      BUFFER_LENGTH = 1000 * 4, // 1000 samples, 4000 bytes
    };

    HWAVEOUT m_device;

    int m_current_buffer;
    WAVEHDR m_buffers[BUFFER_COUNT];
    u8 m_samples[BUFFER_COUNT * BUFFER_LENGTH];

    friend class MMOutputStream;
  };

}


#endif
