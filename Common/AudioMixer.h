
class CAudioMixer
{
public:
	/*
	 * Микширует несколько звуковых буферов в выходной буфер.
	 * Предполагается, что все буферы 16битные, одной длины, с одинаковым количеством каналов.
	 * bufferSize - количество samples * channels
	 */
	static bool MixBuffers(const short** buffers, int bufferCount, int bufferSize, short* outputBuffer);
};
