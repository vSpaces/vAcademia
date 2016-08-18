#pragma once

struct IAudioStreamMixer
{
	virtual void onUpdate() = 0;
	virtual void onSamplesReceived() = 0;
};