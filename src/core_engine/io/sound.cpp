#include "sound.h"

char* sound::load_wav(const char *file_name, int &channels, int &sample_rate, int &samples_per_second, int &size)
{
	std::ifstream in(file_name, std::ios::binary);
	char tmp_buffer[4];
	in.read(tmp_buffer, 4);
	if(strncmp(tmp_buffer, "RIFF", 4) != 0)
	{
		std::cout << "Not a valid wav: RIFF) " << tmp_buffer[0] << tmp_buffer[1] << tmp_buffer[2] << tmp_buffer[3] << tmp_buffer[4] << '\n';
		return 0;
	}
    
	in.read(tmp_buffer, 4);
	int B_size = convert_to_int(tmp_buffer, 4);

	in.read(tmp_buffer, 4);
	if(strncmp(tmp_buffer, "WAVE", 4) != 0)
    {
		std::cout << "Not a valid wav: WAVE " << tmp_buffer[0] << tmp_buffer[1] << tmp_buffer[2] << tmp_buffer[3] << tmp_buffer[4] << '\n';
		return 0;
	}
    
	in.read(tmp_buffer, 4);
	if(strncmp(tmp_buffer, "fmt ", 4) != 0)
    {
		std::cout << "Not a valid wav: fmt " << tmp_buffer[0] << tmp_buffer[1] << tmp_buffer[2] << tmp_buffer[3] << tmp_buffer[4] << '\n';
		return 0;
	}
    
	in.read(tmp_buffer, 4);
	int fmtSize = convert_to_int(tmp_buffer, 4);
	if(fmtSize != 16)
    {
		std::cout << "Only PCM allowed, no compressed wav" << fmtSize << '\n';
		return 0;
	}
    
	in.read(tmp_buffer, 2);
	int pcm = convert_to_int(tmp_buffer, 2);
	if(pcm != 1)
    {
		std::cout << "Only PCM allowed, no compressed wav\n";
		return 0;
	}
    
	in.read(tmp_buffer, 2);
	int Channels = convert_to_int(tmp_buffer, 2);
    
	in.read(tmp_buffer, 4);
	int Samples = convert_to_int(tmp_buffer, 4);
    
	in.read(tmp_buffer, 4);
	int byte_rate = convert_to_int(tmp_buffer, 4);
    
	in.read(tmp_buffer, 2);
	int block_align = convert_to_int(tmp_buffer, 2);
    
	in.read(tmp_buffer, 2);
	int bits_per_sample = convert_to_int(tmp_buffer, 2);
    
	in.read(tmp_buffer, 4);
	if(strncmp(tmp_buffer, "data", 4) != 0)
    {
		std::cout << "This is not a valid WAV file (4) " << tmp_buffer[0] << tmp_buffer[1] << tmp_buffer[2] << tmp_buffer[3] << '\n';
        //	return 0;
	}
    
	in.read(tmp_buffer, 4);
	int data_size = convert_to_int(tmp_buffer, 4);
	if(data_size <= 0)
    {
		std::cout << "This is not a valid WAV file (4) " << data_size << '\n';
		return 0;
	}
    
	char* data = new char[data_size];
	in.read(data, data_size);

	channels = Channels;
	sample_rate = bits_per_sample;
	samples_per_second = Samples;
	size = data_size;
    
	return data;
}

int sound::convert_to_int(const char *data, int length)
{
	int a = 0;
	if (!big_endian)
    {
        for (int i = 0; i < length; i++)
            ((char *) &a)[i] = data[i];
    }
	else
    {
        for (int i = 0; i < length; i++)
            ((char *) &a) [sizeof(int) - i - 1] = data[i];
    }

	return a;
}

bool sound::is_big_endian()
{
	int a = 1;
	return (((unsigned char*)&a)[0] == 0);
}

sound::sound()
{
	big_endian = is_big_endian();
	device = alcOpenDevice(NULL);
	if(device == NULL)
    {
		std::cout << "couldn't open device" << std::endl;
		context = NULL;
		return;
	}
    
	context = alcCreateContext(device, NULL);
	if(context == NULL)
    {
		std::cout << "couldn't open context" << std::endl;
		return;
	}
    
	alcMakeContextCurrent(context);
}

sound::~sound()
{
	for(int i = 0; i < datas.size(); i++)
    {
        alDeleteSources(1, &datas[i].source_id);
        alDeleteBuffers(1, &datas[i].buffer_id);
        delete[] datas[i].buffer;
	}
	alcDestroyContext(context);
	alcCloseDevice(device);
}

unsigned int sound::load(const char *file_name, float min_gain, float max_gain)
{
	unsigned int sourceId;
	int channel, sampleRate, samplesPerSecond, size;
    
	alGenSources(1, &sourceId);
	char* bufferData = load_wav(file_name, channel, sampleRate, samplesPerSecond, size);
	unsigned int bufferId;
	alGenBuffers(1, &bufferId);

	int format;
	if(channel == 1)
    {
		if(sampleRate == 8)
            format = AL_FORMAT_MONO8;
		else
            format = AL_FORMAT_MONO16;
	}
    else
    {
		if(sampleRate == 8)
            format = AL_FORMAT_STEREO8;
		else
            format = AL_FORMAT_STEREO16;
	}
    
	alBufferData(bufferId, format, bufferData, size, samplesPerSecond);
	alSourcei(sourceId, AL_BUFFER, bufferId);
	alSourcef(sourceId, AL_MIN_GAIN, min_gain);
	alSourcef(sourceId, AL_MAX_GAIN, max_gain);
	alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
	datas.push_back(data(sourceId, bufferId, bufferData));
	return sourceId;
}

void sound::delete_sound(unsigned int id)
{
    for (int i = 0; i < datas.size(); i++)
    {
        if (datas[i].source_id == id)
        {
            alSourcei(datas[i].source_id, AL_BUFFER, (ALint) NULL);
            alDeleteBuffers(1, &datas[i].buffer_id);
            delete[] datas[i].buffer;
            alDeleteSources(1, &datas[i].source_id);
            datas.erase(datas.begin() + i);
            return;
        }
    }
}

void sound::play(unsigned int id) const
{
	alSourcePlay(id);
}

void sound::play(unsigned int id, const vec3f &pos) const
{
	alSource3f(id, AL_POSITION, pos.get_x(), pos.get_y(), pos.get_z());
	alSourcePlay(id);
}

void sound::play(unsigned int id, const vec3f &pos, const vec3f &velocity) const
{
	alSource3f(id, AL_POSITION, pos.get_x(), pos.get_y(), pos.get_z());
	alSource3f(id, AL_VELOCITY, velocity.get_x(), velocity.get_y(), velocity.get_z());
	alSourcePlay(id);
}

void sound::play(unsigned int id, const vec3f &pos, const vec3f &velocity, float gain, bool loop) const
{
	alSource3f(id, AL_POSITION, pos.get_x(), pos.get_y(), pos.get_z());
	alSource3f(id, AL_VELOCITY, velocity.get_x(), velocity.get_y(), velocity.get_z());
	alSourcef(id, AL_GAIN, gain);
	alSourcePlay(id);
}

void sound::stop(unsigned int id) const
{
	alSourceStop(id);
}

void sound::pause(unsigned int id) const
{
    alSourcePause(id);
}

void sound::rewind(unsigned int id) const
{
    alSourceRewind(id);
}

void sound::set_listener(const vec3f &pos, const vec3f &orientation) const
{
	alListener3f(AL_POSITION, pos.get_x(), pos.get_y(), pos.get_z());
	float f[] = {orientation.get_x(), orientation.get_y(), orientation.get_z(), 0, 1, 0};
	alListenerfv(AL_ORIENTATION, f);
}