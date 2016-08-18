#pragma once

struct IStreamListener
{
	virtual void onSamplesReceived(void* buffer, unsigned int sample_count, int frame_size, int channel_count) = 0;
	virtual void destroy() = 0;
};
