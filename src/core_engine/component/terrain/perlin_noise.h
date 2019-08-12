#ifndef INC_3D_ENGINE_PERLIN_NOISE_H
#define INC_3D_ENGINE_PERLIN_NOISE_H


#include "../../math/math.h"



class perlin_noise
{
public:
    perlin_noise();
    perlin_noise(int grid_x, int grid_z, int vertex_count, int seed);

    float get_height(int x, int z) const;

private:
    const float AMPLITUDE = 20.0f;
    const int OCTAVES = 5;
    const float ROUGHNESS = 0.3f;

    int seed;
    int x_offset, z_offset;

    float get_interpolated_noise(float x, float z) const;
    float interpolate(float a, float b, float blend) const;
    float get_smooth_noice(int x, int z) const;
    float get_noise(int x, int z) const;
};


#endif //INC_3D_ENGINE_PERLIN_NOISE_H
