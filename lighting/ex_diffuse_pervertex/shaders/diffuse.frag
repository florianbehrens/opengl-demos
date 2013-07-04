#version 330

in vec3 color;

layout (location = 0) out vec4 fragColor;

void main()
{
    // Use color interpolated form vertex shader
    fragColor = vec4( color, 1.0 );
}
