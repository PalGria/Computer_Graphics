#version 330

in vec2 TexCoord0;
in vec4 LightIntensity0;

out vec4 FragColor;

uniform sampler2D gTextureSampler;
uniform float red;
uniform float green;
uniform float blue;

void main()
{
    FragColor = vec4(red, green, blue, 1.0) * LightIntensity0;
}