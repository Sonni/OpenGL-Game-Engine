#version 330 core

layout(location = 0) in vec3 position;
layout(location = 0) in vec2 texcoords;

uniform mat4 model;

out vec2 tex_coord;


void main()
{
    gl_Position =vec4(position.x, position.y, 0.0, 1.0);
    tex_coord = vec2((position.x + 1.0) / 2.0, (position.y + 1.0) / 2.0);
    //tex_coord = texcoords;
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord;

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen;
uniform sampler2D myTexture;
const int NUM_SAMPLES = 100;

out vec4 out_color;



void main()
{

    /*if (lightPositionOnScreen.x > -1.0 && lightPositionOnScreen.x < 1.0 &&
        lightPositionOnScreen.y > -1.0 && lightPositionOnScreen.y < 1.0) {*/

     vec2 deltaTextCoord = vec2(tex_coord - (lightPositionOnScreen.xy+1)/2);
        vec2 textCoo = tex_coord;
        deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
        float illuminationDecay = 1.0;


        vec4 color = texture(myTexture, tex_coord);
        for (int i=0; i < NUM_SAMPLES; i++)
        {
            textCoo -= deltaTextCoord;
            vec4 samples = vec4(0.0, 0.0, 0.0, 1.0);

            if (textCoo.x > -1.0 && textCoo.x < 1.0 &&
            textCoo.y > -1.0 && textCoo.y < 1.0) {
                samples = texture(myTexture, textCoo);
                samples *= illuminationDecay * weight;

                color += samples;

                illuminationDecay *= decay;

            }

        }

        color *= exposure;
        out_color = color;
   /* }
    else
    {
        out_color = vec4(0.4, 0.4, 0.4, 0.0);
    }*/


    /*
    int Samples = 128;
    float Intensity = 0.025, Decay = 0.96875;
    vec2 TexCoord = tex_coord, Direction = vec2(0.5) - TexCoord;
    Direction /= Samples;
    vec3 Color = texture(myTexture, TexCoord).rgb;

    for(int Sample = 0; Sample < Samples; Sample++)
    {
        Color += texture(myTexture, TexCoord).rgb * Intensity;
        Intensity *= Decay;
        TexCoord += Direction;
    }

    out_color = vec4(Color, 1.0);*/


}
