#include "perlin_noise.h"

perlin_noise::perlin_noise()
{
    srand (time(nullptr));

    m_seed = rand() % 1000000000 + 1;
}

perlin_noise::perlin_noise(int grid_x, int grid_z, int vertex_count, int seed)
{
    m_seed = seed;
    m_x_offset = grid_x * (vertex_count-1);
    m_z_offset = grid_z * (vertex_count-1);
}

float perlin_noise::get_height(int x, int z) const
{
    float total = 0;
    float d = (float) pow(2, m_OCTAVES - 1);
    for(int i = 0; i < m_OCTAVES; i++)
    {
        float freq = (float) (pow(2, i) / d);
        float amp = (float) pow(m_ROUGHNESS, i) * m_AMPLITUDE;
        total += get_interpolated_noise((x + m_x_offset) * freq, (z + m_z_offset) * freq) * amp;
    }
    return total;
}

float perlin_noise::get_interpolated_noise(float x, float z) const
{
    float frac_x = x - (int) x;
    float frac_z = z - (int) z;

    float v1 = get_smooth_noice((int) x, (int) z);
    float v2 = get_smooth_noice((int) x + 1, (int) z);
    float v3 = get_smooth_noice((int) x, (int) z + 1);
    float v4 = get_smooth_noice((int) x + 1, (int) z + 1);
    float i1 = interpolate(v1, v2, frac_x);
    float i2 = interpolate(v3, v4, frac_x);
    return interpolate(i1, i2, frac_z);
}

float perlin_noise::interpolate(float a, float b, float blend) const
{
    double theta = blend * MATH_PI;
    float f = (float) (1.0f - cos(theta)) * 0.5f;
    return a * (1.0f - f) + b * f;
}

float perlin_noise::get_smooth_noice(int x, int z) const
{
    float corners = (get_noise(x - 1, z - 1) + get_noise(x + 1, z - 1) + get_noise(x - 1, z + 1) +
                     get_noise(x + 1, z + 1)) / 16.0f;
    float sides = (get_noise(x - 1, z) + get_noise(x + 1, z) + get_noise(x, z - 1) + get_noise(x, z + 1)) / 8.0f;
    float center = get_noise(x, z) / 4.0f;
    return corners + sides + center;
}

float perlin_noise::get_noise(int x, int z) const
{
    srand (x * 52032 + z * 12413 + m_seed);

    return (float) rand() / (float) (RAND_MAX) * 2.0f - 1.0f;
}