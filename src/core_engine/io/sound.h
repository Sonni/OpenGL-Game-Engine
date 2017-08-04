#ifndef __Project_MMORPG__Sound__
#define __Project_MMORPG__Sound__
#include <iostream>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include "../math/math_lib.h"

class sound
{
public:
    sound();
    ~sound();
    unsigned int load(const char* file_name, float min_gain = 0, float max_gain = 1.0);
    void delete_sound(unsigned int id);
    
    void play(unsigned int id) const;
    void play(unsigned int id, const vec3f &pos) const;
    void play(unsigned int id, const vec3f &pos, const vec3f &velocity) const;
    void play(unsigned int id, const vec3f &pos, const vec3f &velocity, float gain, bool loop) const;
    
    void stop(unsigned int id) const;
    void pause(unsigned int id) const;
    
    void rewind(unsigned int id) const;
    
    void set_listener(const vec3f &pos, const vec3f &orientation) const;

private:

    struct data
    {
        unsigned int source_id;
        unsigned int buffer_id;
        char* buffer;
        data(unsigned int si, unsigned int bi, char* buff) :
                source_id(si),
                buffer_id(bi),
                buffer(buff)
        {}
    };

    std::vector<data> datas;
    ALCcontext* context;
    ALCdevice* device;
    bool big_endian;

    char* load_wav(const char *file_name, int &channels, int &sample_rate, int &samples_per_second, int &size);
    int convert_to_int(const char *data, int length);
    bool is_big_endian();

};

#endif /* defined(__Project_MMORPG__Sound__) */
