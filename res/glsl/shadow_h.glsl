const int pcf_level = 3;
const float texels = (pcf_level * 2.0 + 1.0) * (pcf_level * 2.0 + 1.0);
const float map_size = 2048.0;

float calc_shadow(vec4 _shadow_coord, sampler2D _shadow_tex)
{
    float texel_size = 1.0 / map_size;
    float total = 0.0;

    for (int x = -pcf_level; x <= pcf_level; x++)
    {
        for (int y = -pcf_level; y <= pcf_level; y++)
        {
            float obj_nearest_light = texture(_shadow_tex, _shadow_coord.xy + vec2(x, y) * texel_size).r;
            if (_shadow_coord.z > obj_nearest_light + 0.01)
            {
                total += 1.0;
            }
        }
    }

    total /= texels;
    return 1.0 - (total * _shadow_coord.w);
}