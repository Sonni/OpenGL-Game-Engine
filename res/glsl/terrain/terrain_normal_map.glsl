#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec2 tex_coord;
out vec3 toLightVector;
//out vec3 world_pos;
//out vec3 normals;


uniform mat4 model;
uniform mat4 mvp;
uniform vec3 sun_pos;
uniform vec4 cutting_plane;


void main()
{
    vec4 world_position = model * vec4(position, 1.0);

    tex_coord = uv;
    //normals = (model * vec4(normal, 0.0)).xyz;

    gl_ClipDistance[0] = dot(world_position, cutting_plane);
    gl_Position = mvp * vec4(position, 1.0);

    toLightVector = (sun_pos-world_position.xyz);

    /*vec3 n = normalize((model * vec4(normal, 0.0)).xyz);
    vec3 t = normalize((model * vec4(tangent, 0.0)).xyz);
    t = normalize(t - dot(t, n) * n);

    vec3 biTangent = cross(t, n);
    mat3 tbn_mat = mat3(t, biTangent, n);*/
}


//-END_OF_VS-
#version 330 core

in vec2 tex_coord;
in vec3 toLightVector;

//in vec3 world_pos;
//in vec3 normals;



out vec4 out_color;

uniform sampler2D background_tex;
uniform sampler2D r_tex;
uniform sampler2D g_tex;
uniform sampler2D b_tex;
uniform sampler2D blend_tex;
uniform sampler2D normal_map;


void main()
{
    // No need to convert between tangent- and world space since our normal_map
    // is generated in world space and converted to an image
    vec4 normalMapValue = 2.0 * texture(normal_map, tex_coord, -1.0) - 1.0;

    vec3 unitNormal = normalize(normalMapValue.rgb);
    vec3 unitLightVector = normalize(toLightVector);

    float nDot1 = dot(unitNormal, unitLightVector);
    float brightness = max(nDot1,0.0);
    vec3 diffuse = brightness * vec3(0.7, 0.7, 0.7);

    vec4 blend_tex_color = texture(blend_tex, tex_coord);

    float back_tex_amount = 1 - (blend_tex_color.r + blend_tex_color.g + blend_tex_color.b);
    vec2 tiled_coord = tex_coord * 30.0;
    vec4 back_color = texture(background_tex, tiled_coord) * back_tex_amount;
    vec4 r_color = texture(r_tex,tiled_coord) * blend_tex_color.r;
    vec4 g_color = texture(g_tex,tiled_coord) * blend_tex_color.g;
    vec4 b_color = texture(b_tex,tiled_coord) * blend_tex_color.b;

    vec4 total_color = back_color + r_color + g_color + b_color;

    out_color =   total_color* vec4(diffuse,1.0) ;
}
